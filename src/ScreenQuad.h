#pragma once

#include <iostream>
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class ScreenQuad 
{
public:
	ScreenQuad(IVideoDriver *xy)
	{
		vd = xy;

		const SColor white(255, 255, 255, 255);
		vertices[0].Pos = vector3df(-1, 1, 0);
		vertices[0].TCoords = vector2df(0, 0);
		vertices[0].Color = white;
		vertices[1].Pos = vector3df(1, 1, 0);
		vertices[1].TCoords = vector2df(1, 0);
		vertices[1].Color = white;
		vertices[2].Pos = vector3df(1, -1, 0);
		vertices[2].TCoords = vector2df(1, 1);
		vertices[2].Color = white;
		vertices[3].Pos = vector3df(-1, -1, 0);
		vertices[3].TCoords = vector2df(0, 1);
		vertices[3].Color = white;
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 3;
		indices[3] = 2;

		mat.Lighting = false;
		mat.ZBuffer = video::ECFN_ALWAYS;
		mat.ZWriteEnable = false;
		//  mat.SkipArrays = ESA_COLOR | ESA_NORMAL;

		for(u32 c = 0; c < MATERIAL_MAX_TEXTURES; c++)
		{
			mat.TextureLayer[c].TextureWrapU = video::ETC_CLAMP_TO_EDGE;
			mat.TextureLayer[c].TextureWrapV = video::ETC_CLAMP_TO_EDGE;
		}
	}

	SMaterial& getMaterial() { return mat; }

	//Set the texture to render with the quad
	void setTexture(ITexture* __restrict tex, u32 layer = 0)
	{
		mat.TextureLayer[layer].Texture = tex;
	}

	ITexture* getTexture(u32 layer = 0) { return mat.TextureLayer[layer].Texture; }

	void setMaterialType(E_MATERIAL_TYPE mt) { mat.MaterialType = mt; }

	void render(bool setRTToFrameBuff = true)
	{
		if(setRTToFrameBuff)
			vd->setRenderTarget(video::ERT_FRAME_BUFFER);

		sq_dorender();
	}

	void render(ITexture* rt)
	{
		vd->setRenderTarget(rt);
		sq_dorender();
	}

protected:
	S3DVertex vertices[4];
	u16 indices[4];
	SMaterial mat;
	IVideoDriver* vd;

	void sq_dorender()
	{
		//glColor3ub(255, 255, 255);
		vd->setMaterial(mat);
		vd->setTransform(ETS_WORLD, core::IdentityMatrix);
		vd->setTransform(ETS_VIEW, core::IdentityMatrix);
		vd->setTransform(ETS_PROJECTION, core::IdentityMatrix);
		vd->drawVertexPrimitiveList(vertices, 4, indices, 2, EVT_STANDARD, EPT_TRIANGLE_STRIP);
	}
};
