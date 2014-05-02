#ifndef _TOEIN_STEREO_VIEW_H_
#define _TOEIN_STEREO_VIEW_H_

#include "DualStereoView.h"
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <IL/il.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

// ToeInStereoView window

class ToeInStereoView : public DualStereoView
{

public:
	//	Constructor / Destructor.
	ToeInStereoView();
	virtual ~ToeInStereoView() {};
	virtual void SetupView();

// Overrides
protected:
	//	Main OpenGL functions.
	virtual void RenderLeftView();
	virtual void RenderRightView();
	float CameraRotationAngle;
	float CameraRotationShift[3];
	float leftViewAngles[4];
	float rightViewAngles[4];
};

#endif
