#include "osgLib.h"
#ifndef _HOLOCAMERAVIEW_H
#define _HOLOCAMERAVIEW_H

class HoloCameraView : public osg::CameraView
{
public:
    HoloCameraView(osg::View* view) : osg::CameraView() 
    {
         view->getCamera()->getProjectionMatrixAsPerspective(_fieldOfView, m_aspectRatio, m_zNear, m_zFar);
    }

    virtual bool CameraView::computeLocalToWorldMatrix(Matrix& matrix,NodeVisitor* nv) const
    {
        if (_referenceFrame==RELATIVE_RF)
        {
            matrix.preMultTranslate(_position);
            matrix.preMultRotate(_attitude);
        }
        else // absolute
        {
            matrix.makeRotate(_attitude);
            matrix.postMultTranslate(_position);
        }
        return true;
    }

protected:
    double  m_aspectRatio, m_zNear, m_zFar;
};
#endif