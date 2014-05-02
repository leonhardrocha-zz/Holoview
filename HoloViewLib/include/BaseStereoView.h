#ifndef _BASE_STEREO_VIEW_H_
#define _BASE_STEREO_VIEW_H_

#include "BaseAssetRenderer.h"

class BaseStereoView
{

public:

// Attributes
public:

// Operations
public:
// Overrides
	virtual void SetupWindow() = 0;
	virtual void SetupView() = 0;
	virtual void SetupScene() = 0;

protected:
	virtual void RenderStereoView() = 0;
	virtual void RenderLeftView() = 0;
	virtual void RenderRightView() = 0;
	virtual void RenderScene() = 0;
	virtual void SetRenderer(BaseAssetRenderer &newInstance) = 0;
	// Generated message map functions
protected:
	int windowWidth;
	int windowHeight;
	int windowOffsetX;
	int windowOffsetY;
	int viewWidth;
	int viewHeight;
	int viewOffsetX;
	int viewOffsetY;

	double fovy;                                          //field of view in y-axis
	double aspect;  //screen aspect ratio
	double nearZ;                                        //near clipping plane
	double farZ;                                        //far clipping plane
	double IOD;
	float CameraPosition[3];
	float LookAtPosition[3];
	float LightPosition[3];
	BaseAssetRenderer *assetRenderer;
};

#endif