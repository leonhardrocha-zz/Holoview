#include "osgLib.h"
#ifndef _OSG_EXTENSION_H
#define _OSG_EXTENSION_H
#include "TiltedScreen.h"
#include "MatrixExtension.h"
#include "ICallable.h"
#include "ViewerArgs.h"




namespace OsgExtension
{
    struct Slave : osg::View::Slave
    {
        Slave(bool useMastersSceneData=true): 
            osg::View::Slave(useMastersSceneData) {}

        Slave(osg::Camera* camera, const osg::Matrixd& projectionOffset, const osg::Matrixd& viewOffset, bool useMastersSceneData=true):
            osg::View::Slave(camera, projectionOffset, viewOffset, useMastersSceneData) {}

        Slave(osg::Camera* camera, 
            const osg::Matrixd& projectionOffset, const osg::Matrixd& viewOffset,
            const osg::Matrixd& postProjectionOffset, const osg::Matrixd& postViewOffset,bool useMastersSceneData=true):
            osg::View::Slave(camera, projectionOffset, viewOffset, useMastersSceneData), _postProjectionOffset(postProjectionOffset) , _postViewOffset(postViewOffset){}

        Slave(const Slave& rhs) :
            osg::View::Slave(rhs),
            _postProjectionOffset(rhs._postProjectionOffset),
            _postViewOffset(rhs._postViewOffset) {}

        virtual ~Slave() {}

        Slave& operator = (const Slave& rhs)
        {
            _camera = rhs._camera;
            _projectionOffset = rhs._projectionOffset;
            _viewOffset = rhs._viewOffset;
            _postProjectionOffset = rhs._postProjectionOffset;
            _postViewOffset = rhs._postViewOffset;
            _useMastersSceneData = rhs._useMastersSceneData;
            _updateSlaveCallback = rhs._updateSlaveCallback;
            return *this;
        }


        struct UpdateSlaveCallback : public virtual osg::Referenced
        {
            virtual void updateSlave(osg::View& view, osg::View::Slave& slave) = 0;
        };

        void updateSlave(osg::View& view)
        {
            if (_updateSlaveCallback.valid()) _updateSlaveCallback->updateSlave(view, *this);
            else updateSlaveImplementation(view);
        }

        osg::Matrixd                        _postProjectionOffset;
        osg::Matrixd                        _postViewOffset;
    };

    class View : public osgViewer::View
    {
    public :

        View() : osgViewer::View() { _extslaves.clear(); }

        View(const View& view, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) : osgViewer::View(view, copyop), _extslaves(view._extslaves) {  }

        ~View() { _extslaves.clear();}

        bool View::addSlave(osg::Camera* camera, const osg::Matrix& projectionOffset, const osg::Matrix& viewOffset,
                            const osg::Matrix& postProjectionOffset = osg::Matrix::identity(), const osg::Matrix& postViewOffset = osg::Matrix::identity(), bool useMastersSceneData = true)
        {
            unsigned int i = _extslaves.size();
            OsgExtension::Slave slave(camera, projectionOffset, viewOffset, postProjectionOffset, postViewOffset, useMastersSceneData);
            _extslaves.push_back(slave);
            _extslaves[i].updateSlave(*this);

            return true;
        }

        bool removeSlave(unsigned int pos)
        {
            if (pos >= _extslaves.size()) return false;

            _extslaves[pos]._camera->setView(0);
            _extslaves[pos]._camera->setCullCallback(0);

            _extslaves.erase(_extslaves.begin()+pos);

            return true;
        }

        View* asView() { return this; }

        unsigned int getNumSlaves() const { return static_cast<unsigned int>(_extslaves.size()); }

        View::Slave * View::findSlaveForCamera(osg::Camera* camera)
        {
            unsigned int i = findSlaveIndexForCamera(camera);

            if (i >= getNumSlaves()) return (NULL);

            return &(_extslaves[i]);
        }

        unsigned int View::findSlaveIndexForCamera(osg::Camera* camera) const
        {
            if (_camera == camera) return _extslaves.size();

            for(unsigned int i=0; i<_extslaves.size(); ++i)
            {
                if (_extslaves[i]._camera == camera) return (i);
            }

            return _extslaves.size();
        }

        OsgExtension::Slave& getSlave(unsigned int pos) { return _extslaves[pos]; }
        const OsgExtension::Slave& getSlave(unsigned int pos) const { return _extslaves[pos]; }

        typedef std::vector<OsgExtension::Slave>      ExtSlaves; 

        ExtSlaves                          _extslaves;
    };

    struct SlaveCallback : public osg::View::Slave::UpdateSlaveCallback
    {
    public:

        virtual void updateSlave(osg::View& view, osg::View::Slave& slave)
        {
            osg::Camera* slaveCamera = slave._camera.get();
            osg::Camera* viewCamera = view.getCamera();
        
            if (!slaveCamera|| !viewCamera) return;

            if (slaveCamera->getReferenceFrame()==osg::Transform::RELATIVE_RF)
            {
                osg::Matrix viewMatrix = viewCamera->getViewMatrix();
                osg::Matrix projMatrix = viewCamera->getProjectionMatrix();
                slaveCamera->setProjectionMatrix(slave._projectionOffset);
                slaveCamera->setViewMatrix(viewMatrix * slave._viewOffset);
            }
            slaveCamera->inheritCullSettings(*viewCamera, slaveCamera->getInheritanceMask());
        }
    };
    
    class ExtViewer : public osgViewer::Viewer
    {
    public:
        ExtViewer() : osgViewer::Viewer() {} 
    protected:
        osgViewer::Viewer _viewer;
    };

    class ExtCompositeViewer : public osgViewer::CompositeViewer
    {
    public:
        ExtCompositeViewer() : osgViewer::CompositeViewer() {} 

        void addView(ExtViewer* view) { _viewer.addView(view); };
        void removeView(ExtViewer* view) { _viewer.removeView(view); };
    protected:
        osgViewer::CompositeViewer _viewer;
    };
   
    class Camera : public osg::Camera
    {
    public:
        osg::Matrix getPreProjectionMatrix() { return m_postProjectionMatrix; }
        osg::Matrix getPreViewMatrix() { return m_postViewMatrix; }
        void setPreProjectionMatrix(osg::Matrix matrix) { m_postProjectionMatrix = matrix; }
        void setPreViewMatrix(osg::Matrix matrix) { m_postViewMatrix = matrix; }
    protected:
        osg::Matrix m_postProjectionMatrix;
        osg::Matrix m_postViewMatrix;
    };

    template<class matrix_type, class value_type>
    bool _myclampProjectionMatrix(matrix_type& projection, double& znear, double& zfar, value_type delta_span)
    {
        double epsilon = 1e-6;
        if (zfar<znear-epsilon)
        {
            if (zfar != -FLT_MAX || znear != FLT_MAX)
            {
                OSG_INFO<<"_clampProjectionMatrix not applied, invalid depth range, znear = "<<znear<<"  zfar = "<<zfar<<std::endl;
            }
            return false;
        }

        if (zfar<znear+epsilon)
        {
            // znear and zfar are too close together and could cause divide by zero problems
            // late on in the clamping code, so move the znear and zfar apart.
            double average = (znear+zfar)*0.5;
            znear = average-epsilon;
            zfar = average+epsilon;
            // OSG_INFO << "_clampProjectionMatrix widening znear and zfar to "<<znear<<" "<<zfar<<std::endl;
        }

        if (fabs(projection(0,3))<epsilon  && fabs(projection(1,3))<epsilon  && fabs(projection(2,3))<epsilon )
        {
            // OSG_INFO << "Orthographic matrix before clamping"<<projection<<std::endl;

            //value_type delta_span = (zfar-znear)*0.02;
            if (delta_span<1.0) delta_span = 1.0;
            value_type desired_znear = znear - delta_span;
            value_type desired_zfar = zfar + delta_span;

            // assign the clamped values back to the computed values.
            znear = desired_znear;
            zfar = desired_zfar;

            projection(2,2)=-2.0f/(desired_zfar-desired_znear);
            projection(3,2)=-(desired_zfar+desired_znear)/(desired_zfar-desired_znear);

            // OSG_INFO << "Orthographic matrix after clamping "<<projection<<std::endl;
        }
        else
        {
            // OSG_INFO << "Persepective matrix before clamping"<<projection<<std::endl;

            //std::cout << "_computed_znear"<<_computed_znear<<std::endl;
            //std::cout << "_computed_zfar"<<_computed_zfar<<std::endl;

            //value_type zfarPushRatio = 1.02;
            //value_type znearPullRatio = 0.98;

            //znearPullRatio = 0.99;

            /*value_type desired_znear = znear * znearPullRatio;
            value_type desired_zfar = zfar * zfarPushRatio;*/

            value_type desired_znear = znear + delta_span;
            value_type desired_zfar = zfar + delta_span;

            // near plane clamping.
            /*double min_near_plane = zfar*nearFarRatio;
            if (desired_znear<min_near_plane) desired_znear=min_near_plane;*/

            // assign the clamped values back to the computed values.
            znear = desired_znear;
            zfar = desired_zfar;

            value_type trans_near_plane = (-desired_znear*projection(2,2)+projection(3,2))/(-desired_znear*projection(2,3)+projection(3,3));
            value_type trans_far_plane = (-desired_zfar*projection(2,2)+projection(3,2))/(-desired_zfar*projection(2,3)+projection(3,3));

            value_type ratio = fabs(2.0/(trans_near_plane-trans_far_plane));
            value_type center = -(trans_near_plane+trans_far_plane)/2.0;

            projection.postMult(osg::Matrix(1.0f,0.0f,0.0f,0.0f,
                                            0.0f,1.0f,0.0f,0.0f,
                                            0.0f,0.0f,ratio,0.0f,
                                            0.0f,0.0f,center*ratio,1.0f));

            // OSG_INFO << "Persepective matrix after clamping"<<projection<<std::endl;
        }
        return true;
    }

    struct MyClampProjectionMatrixCallback : public osg::CullSettings::ClampProjectionMatrixCallback
    {
        MyClampProjectionMatrixCallback(double deltaSpan) : _deltaSpan(deltaSpan)
        {}

        virtual bool clampProjectionMatrixImplementation(osg::Matrixf& projection, double& znear, double& zfar) const
        {
            return _myclampProjectionMatrix( projection, znear, zfar, _deltaSpan );
        };
        virtual bool clampProjectionMatrixImplementation(osg::Matrixd& projection, double& znear, double& zfar) const
        {
            return _myclampProjectionMatrix( projection, znear, zfar, _deltaSpan );
        };

        double _deltaSpan;
    };

    class ViewUpdateHandler : public osgGA::GUIEventHandler, public ICallable
    {
    public:
        virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*) 
        { 
            Call();
            return osgGA::GUIEventHandler::handle(ea,aa);
        }

        virtual void SetCallback(ICallback callback, void* instance=NULL, IArgs* args=NULL)
        {
            m_callback = callback;
            m_instance = instance;
            m_callbackArgs = args;
        };

        virtual void Call() { if (m_callback) { (*m_callback)(m_instance, m_callbackArgs); } else throw new std::exception("Callback is not set or NULL");};

        virtual ICallback GetCallback() { return m_callback;};
        virtual IArgs* GetArgs() { return m_callbackArgs;};
        virtual void* GetInstance() { return m_instance;};
    protected:

        ICallback               m_callback;
        void*                   m_instance;
        IArgs*                  m_callbackArgs;
        osg::ref_ptr<DualScreenViewer> m_parent;
    };
}
#endif