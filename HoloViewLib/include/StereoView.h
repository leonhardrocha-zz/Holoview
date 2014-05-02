#ifndef _STEREO_VIEW_H_
#define _STEREO_VIEW_H_

#include "BaseStereoView.h"
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

// StereoView window

class StereoView : public BaseStereoView
{
	friend class DualStereoView;
	friend class ToeInStereoView;
public:
	//	Constructor / Destructor.
	StereoView();
	virtual ~StereoView() {};
	virtual void SetupView();
	virtual void SetupWindow();
	virtual void SetupScene();
// Overrides
protected:
	//	Main OpenGL functions.
	virtual void RenderScene();
	virtual void RenderStereoView();
	virtual void RenderLeftView();
	virtual void RenderRightView();
	virtual void ResetLeftView();
	virtual void ResetRightView();
	virtual void DoOpenGLDraw();
	virtual void DoOpenGLResize(int nWidth, int nHeight);
	void SetRenderer(BaseAssetRenderer &newInstance);
};

#endif
