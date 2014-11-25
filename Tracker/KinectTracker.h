#ifdef _MSC_VER
#pragma once
#endif

#include "ITracker.h"
#include "ICallable.h"
#include "TrackerManager.h"
#include "EggAvatar.h"

class KinectTracker :  public TrackerManager
{    

public:
    KinectTracker(ITracker* parent=NULL, bool initialize = true, bool start = true);
    ~KinectTracker();
    bool                        Init();
    bool                        Start();
    virtual void                PaintEvent(void* message);
    virtual void                TrackEvent(void* message);
    bool                        IsShowingVideo;
    bool                        IsShowingAvatar;
    bool                        IsRunning;
    bool                        IsInitialized;
    bool                        FitToWindow;
    bool                        GetPosition(double& x, double& y, double& z);
    virtual void                UpdateVideo();
    virtual bool                SetWindowHandler(void* handler);
    bool                        AddVideoUpdateCallback(const Callback& callback);
protected:
    std::vector<Callback>       VideoCallbacks;
    static void                 UpdateVideoCallback(void* instance);
    EggAvatar*                  GetEggAvatar() { return(&m_eggAvatar); }
    bool                        PaintWindow();
    bool                        UpdateWindow();
    double                      GetXCenterFace()    { return(m_XCenterFace);};
    double                      GetYCenterFace()    { return(m_YCenterFace);};
    bool                        ShowVideo(int width, int height, int originX, int originY);
    bool                        ShowEggAvatar(int width, int height, int originX, int originY);
    bool                        ShowTrackingResult();
    void                        SetCenterOfImage(const RECT& faceRect);
private:
    HDC                         m_hdc;
    HWND                        m_hWnd;
    double                      m_XCenterFace;
    double                      m_YCenterFace;
    KinectFaceTracker*          m_pFaceTracker;
    EggAvatar                   m_eggAvatar;
    UINT32                      m_maskColor;
    IFTImage*                   m_pImageBuffer;
    IFTImage*                   m_pVideoBuffer;




};

