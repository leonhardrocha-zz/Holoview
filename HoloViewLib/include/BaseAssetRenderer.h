#ifndef _BASE_ASSET_RENDERER_H_
#define _BASE_ASSET_RENDERER_H_

class BaseAssetRenderer
{
public:
	virtual void Initialize()=0;
	virtual void Render()=0;
};

#endif