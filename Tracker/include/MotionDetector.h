#include "vld.h"

//#ifndef _MOTIONDETECTOR_H
//#define _MOTIONDETECTOR_H
#pragma once
#include "ProbabilisticEgomotion.h"
#include "FaceTrackLib.h"
#include "FTImageAdapter.h"
#include "ProbabilisticEgomotion.h"
#include "Position.h"
class MotionDetector
{
public:
    void Init(IFTImage* colorBuffer) 
    {
        m_colorBuffer = colorBuffer;
        m_previous.SetImage(m_colorBuffer);
        IFTImage* temp = m_previous.GetImage();
        temp->Allocate(m_colorBuffer->GetWidth(), m_colorBuffer->GetHeight(), m_colorBuffer->GetFormat());
    }

    /*MotionParameters Compute3Dmotion(IPose* position) 
    {
        SaveImage();
        int width, height, ncolors;
        m_current.getSize(width, height, ncolors);
        return compute3Dmotion(m_current, m_previous, (float)position->Get(Position::ZAxis), (float)(width/2 + position->Get(Position::XAxis)), (float)(height/2 + position->Get(Position::YAxis)));
    }*/

protected:

    void SaveImage()
    {
        m_current.GetImage()->CopyTo(m_previous.GetImage(), NULL, 0, 0);
    }


    FTImageAdapter m_current;
    FTImageAdapter m_previous;
    IFTImage* m_colorBuffer;
}
//#endif