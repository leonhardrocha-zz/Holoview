#include "vld.h"

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _TRACKERHELPER_H
#define _TRACKERHELPER_H

#pragma warning(disable:4786)
#include <FaceTrackLib.h>



namespace std
{
    int lround(float d)
    {
        return static_cast<LONG>(d + 0.5);
    }
}
namespace TrackerHelper
{
    static std::string ErrorMessageMap[15] =
    {
        "Success",
        "The face tracking models loaded by the tracking engine have incorrect format",
        "Passed input image is invalid",
        "Face tracking fails due to face detection errors",
        "Face tracking fails due to errors in tracking individual face parts",
        "Face tracking fails due to inability of the Neural Network to find nose, mouth corners and eyes",
        "Uninitialized face tracker is used",
        "A file path to the face model files is invalid or when the model files could not be located",
        "Face tracking worked but later evaluation found that the quality of the results was poor",
        "The passed camera configuration is invalid",
        "The passed 3D hint vectors contain invalid values (for example out of range)",
        "The system cannot find the head area in the passed data based on passed 3D hint vectors or region of interest rectangle",
        "The user ID of the subject being tracked is switched or lost so we should call StartTracking on next call for tracking face",
        "Kinect DLL failed to load",
        "Kinect sensor was not detected in the system "
    };
    FT_VECTOR3D AddFtVector3d(const FT_VECTOR3D& a, const FT_VECTOR3D& b)
    {
        FT_VECTOR3D c;
        c.x = a.x + b.x;
        c.y = a.y + b.y;
        c.z = a.z + b.z;
        return c;
    }

    FT_VECTOR3D SubFtVector3d(const FT_VECTOR3D& a, const FT_VECTOR3D& b)
    {
        FT_VECTOR3D c;
        c.x = a.x - b.x;
        c.y = a.y - b.y;
        c.z = a.z - b.z;
        return c;
    }

    UINT32 GetErrorColor(HRESULT trackingStatus)
     {
        switch (trackingStatus)
        {
            case FT_ERROR_FACE_DETECTOR_FAILED:
                return 0x000000;
            case FT_ERROR_AAM_FAILED:
                return 0x00F0F0;
            case FT_ERROR_NN_FAILED:
                return 0x00FFFF;
            case FT_ERROR_INVALID_3DHINT:
                return 0x0F0F00;
            case FT_ERROR_EVAL_FAILED:
                return 0xFFFF00;
            case FT_ERROR_HEAD_SEARCH_FAILED:
                return 0x0F0000;
            case FT_ERROR_USER_LOST:
                return 0xFF0000;
            default:
                return 0x00FF00;
        }
     }


    bool IsFTErrorSevere(HRESULT trackingStatus)
    {
    switch (trackingStatus)
    {
        case FT_ERROR_FACE_DETECTOR_FAILED:
        case FT_ERROR_AAM_FAILED:
        case FT_ERROR_NN_FAILED:
        case FT_ERROR_INVALID_3DHINT:
        case FT_ERROR_EVAL_FAILED:
        case FT_ERROR_HEAD_SEARCH_FAILED:
        case FT_ERROR_USER_LOST:
            return false;
        default:
            return true;
    }
    return true;
    }

    std::string GetFTErrorMessage(HRESULT trackingStatus)
    {
        for (int i = 0; i < 15; i++)
        {
            HRESULT error = MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, i+1);
            if (trackingStatus == error)
            {
                return ErrorMessageMap[i];
            }
        }

        return ErrorMessageMap[0];
    }

#if 0
    void ParseCmdString(PWSTR lpCmdLine)
{
    const WCHAR KEY_DEPTH[]                                 = L"-Depth";
    const WCHAR KEY_COLOR[]                                 = L"-Color";
    const WCHAR KEY_NEAR_MODE[]                             = L"-NearMode";
    const WCHAR KEY_DEFAULT_DISTANCE_MODE[]                 = L"-DefaultDistanceMode";
    const WCHAR KEY_SEATED_SKELETON_MODE[]                  = L"-SeatedSkeleton";

    const WCHAR STR_NUI_IMAGE_TYPE_DEPTH[]                  = L"DEPTH";
    const WCHAR STR_NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX[] = L"PLAYERID";
    const WCHAR STR_NUI_IMAGE_TYPE_COLOR[]                  = L"RGB";
    const WCHAR STR_NUI_IMAGE_TYPE_COLOR_YUV[]              = L"YUV";

    const WCHAR STR_NUI_IMAGE_RESOLUTION_80x60[]            = L"80x60";
    const WCHAR STR_NUI_IMAGE_RESOLUTION_320x240[]          = L"320x240";
    const WCHAR STR_NUI_IMAGE_RESOLUTION_640x480[]          = L"640x480";
    const WCHAR STR_NUI_IMAGE_RESOLUTION_1280x960[]         = L"1280x960";

    enum TOKEN_ENUM
    {
        TOKEN_ERROR,
        TOKEN_DEPTH,
        TOKEN_COLOR,
        TOKEN_NEARMODE,
        TOKEN_DEFAULTDISTANCEMODE,
        TOKEN_SEATEDSKELETON
    }; 

    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(lpCmdLine, &argc);

    for(int i = 0; i < argc; i++)
    {
        NUI_IMAGE_TYPE* pType = NULL;
        NUI_IMAGE_RESOLUTION* pRes = NULL;

        TOKEN_ENUM tokenType = TOKEN_ERROR; 
        PWCHAR context = NULL;
        PWCHAR token = wcstok_s(argv[i], L":", &context);
        if(0 == wcsncmp(token, KEY_DEPTH, ARRAYSIZE(KEY_DEPTH)))
        {
            tokenType = TOKEN_DEPTH;
            pType = &m_depthType;
            pRes = &m_depthRes;
        }
        else if(0 == wcsncmp(token, KEY_COLOR, ARRAYSIZE(KEY_COLOR)))
        {
            tokenType = TOKEN_COLOR;
            pType = &m_colorType;
            pRes = &m_colorRes;
        }
        else if(0 == wcsncmp(token, KEY_NEAR_MODE, ARRAYSIZE(KEY_NEAR_MODE)))
        {
            tokenType = TOKEN_NEARMODE;
            m_bNearMode = TRUE;
        }
        else if(0 == wcsncmp(token, KEY_DEFAULT_DISTANCE_MODE, ARRAYSIZE(KEY_DEFAULT_DISTANCE_MODE)))
        {
            tokenType = TOKEN_DEFAULTDISTANCEMODE;
            m_bNearMode = FALSE;
        }
        else if(0 == wcsncmp(token, KEY_SEATED_SKELETON_MODE, ARRAYSIZE(KEY_SEATED_SKELETON_MODE)))
        {
            tokenType = TOKEN_SEATEDSKELETON;
            m_bSeatedSkeletonMode = TRUE;
        }

        if(tokenType == TOKEN_DEPTH || tokenType == TOKEN_COLOR)
        {
            _ASSERT(pType != NULL && pRes != NULL);

            while((token = wcstok_s(NULL, L":", &context)) != NULL)
            {
                if(0 == wcsncmp(token, STR_NUI_IMAGE_TYPE_DEPTH, ARRAYSIZE(STR_NUI_IMAGE_TYPE_DEPTH)))
                {
                    *pType = NUI_IMAGE_TYPE_DEPTH;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, ARRAYSIZE(STR_NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX)))
                {
                    *pType = NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_TYPE_COLOR, ARRAYSIZE(STR_NUI_IMAGE_TYPE_COLOR)))
                {
                    *pType = NUI_IMAGE_TYPE_COLOR;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_TYPE_COLOR_YUV, ARRAYSIZE(STR_NUI_IMAGE_TYPE_COLOR_YUV)))
                {
                    *pType = NUI_IMAGE_TYPE_COLOR_YUV;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_RESOLUTION_80x60, ARRAYSIZE(STR_NUI_IMAGE_RESOLUTION_80x60)))
                {
                    *pRes = NUI_IMAGE_RESOLUTION_80x60;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_RESOLUTION_320x240, ARRAYSIZE(STR_NUI_IMAGE_RESOLUTION_320x240)))
                {
                    *pRes = NUI_IMAGE_RESOLUTION_320x240;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_RESOLUTION_640x480, ARRAYSIZE(STR_NUI_IMAGE_RESOLUTION_640x480)))
                {
                    *pRes = NUI_IMAGE_RESOLUTION_640x480;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_RESOLUTION_1280x960, ARRAYSIZE(STR_NUI_IMAGE_RESOLUTION_1280x960)))
                {
                    *pRes = NUI_IMAGE_RESOLUTION_1280x960;
                }
            }
        }
    }

    if(argv) LocalFree(argv);
}
#endif
}


// Face Tracking Error Codes
//#define FT_FACILITY                             0xFAC   // FT facility error code               // Face Tracking facility error code
//#define FT_ERROR_INVALID_MODELS                 MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 1)    // Returned when the face tracking models loaded by the tracking engine have incorrect format
//#define FT_ERROR_INVALID_INPUT_IMAGE            MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 2)    // Returned when passed input image is invalid
//#define FT_ERROR_FACE_DETECTOR_FAILED           MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 3)    // Returned when face tracking fails due to face detection errors
//#define FT_ERROR_AAM_FAILED                     MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 4)    // Returned when face tracking fails due to errors in tracking individual face parts
//#define FT_ERROR_NN_FAILED                      MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 5)    // Returned when face tracking fails due to inability of the Neural Network to find nose, mouth corners and eyes
//#define FT_ERROR_UNINITIALIZED                  MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 6)    // Returned when uninitialized face tracker is used
//#define FT_ERROR_INVALID_MODEL_PATH             MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 7)    // Returned when a file path to the face model files is invalid or when the model files could not be located
//#define FT_ERROR_EVAL_FAILED                    MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 8)    // Returned when face tracking worked but later evaluation found that the quality of the results was poor
//#define FT_ERROR_INVALID_CAMERA_CONFIG          MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 9)    // Returned when the passed camera configuration is invalid
//#define FT_ERROR_INVALID_3DHINT                 MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 10)   // Returned when the passed 3D hint vectors contain invalid values (for example out of range)
//#define FT_ERROR_HEAD_SEARCH_FAILED             MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 11)   // Returned when the system cannot find the head area in the passed data based on passed 3D hint vectors or region of interest rectangle
//#define FT_ERROR_USER_LOST                      MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 12)   // Returned when the user ID of the subject being tracked is switched or lost so we should call StartTracking on next call for tracking face
//#define FT_ERROR_KINECT_DLL_FAILED              MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 13)   // Returned when Kinect DLL failed to load
//#define FT_ERROR_KINECT_NOT_CONNECTED           MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 14)   // Returned when Kinect sensor was not detected in the system 

#endif