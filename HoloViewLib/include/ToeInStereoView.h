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

#include "Matrices.h"
// ToeInStereoView window

class ToeInStereoView : public DualStereoView
{

public:
	//	Constructor / Destructor.
	ToeInStereoView();
	virtual ~ToeInStereoView() {};
	virtual void SetupView();
	virtual void SetViewAngles(float pitch, float yaw, float roll);
	Util::Matrix4 leftViewVectors;
	Util::Matrix4 rightViewVectors;
// Overrides
protected:
	virtual void Init();
	//	Main OpenGL functions.
	virtual void RenderLeftView();
	virtual void RenderRightView();
};

#endif
