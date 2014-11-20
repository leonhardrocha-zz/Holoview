#ifdef _MSC_VER
#pragma once
#endif

#include "ITracker.h"
#include "ICallable.h"
#include "TrackerBase.h"
#include "TrackerManager.h"
#include "EggAvatar.h"

class KinectTracker :  public TrackerBase
{ 
public:
    KinectTracker() : TrackerBase() { if(Init()) Start(); }
    ~KinectTracker() { if (Stop()) Release(); }
    virtual void                PaintEvent(void* message);
    bool                        FitToWindow;
    bool                        GetPosition(double& x, double& y, double& z);
    virtual void                UpdateVideo();
    virtual bool                SetWindowHandler(void* handler);
    bool                        AddVideoUpdateCallback(const Callback& callback);
    TrackerManager*             GetManager() { return m_pTrackerManager; }
protected:
    TrackerManager*             m_pTrackerManager;
    bool                        IsShowingVideo;
    bool                        IsShowingAvatar;
    bool                        IsShowingMask;
    std::vector<Callback>       VideoCallbacks;
    static void                 UpdateVideoCallback(void* instance);
    EggAvatar*                  GetEggAvatar() { return(&m_eggAvatar); }
    bool                        PaintVideo();
    bool                        UpdateWindow();
    double                      GetXCenterFace()    { return(m_XCenterFace);};
    double                      GetYCenterFace()    { return(m_YCenterFace);};
    bool                        ShowVideo(int width, int height, int originX, int originY);
    bool                        ResizeVideo(int width, int height, int originX, int originY);
    bool                        ShowEggAvatar(int width, int height, int originX, int originY);
    bool                        ShowTrackingResult();
    void                        SetCenterOfImage(const RECT& faceRect);
    HDC                         m_hdc;
    HWND                        m_hWnd;
    double                      m_XCenterFace;
    double                      m_YCenterFace;
    KinectFaceTracker*          m_pFaceTracker;
    EggAvatar                   m_eggAvatar;
    UINT32                      m_maskColor;
    IFTImage*                   m_pImageBuffer;
    IFTImage*                   m_pVideoBuffer;
private:
    virtual bool                do_init();
    virtual bool                do_start();
    virtual bool                do_stop();
    virtual bool                do_release();
    virtual void                do_trackEvent(void* message);
};
