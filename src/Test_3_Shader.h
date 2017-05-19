// shader testi

#pragma once
#include "Program.h"

class MyShaderCallBack : public video::IShaderConstantSetCallBack
{
public:
	MyShaderCallBack() : WorldViewProjID(-1), TransWorldID(-1), InvWorldID(-1), PositionID(-1),
						ColorID(-1), TextureID(-1), FirstUpdate(true)
	{
	}

	void OnSetConstants(video::IMaterialRendererServices* services,
			s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		// get shader constants id.

		if (FirstUpdate)
		{
			WorldViewProjID = services->getVertexShaderConstantID("mWorldViewProj");
			TransWorldID = services->getVertexShaderConstantID("mTransWorld");
			InvWorldID = services->getVertexShaderConstantID("mInvWorld");
			PositionID = services->getVertexShaderConstantID("mLightPos");
			ColorID = services->getVertexShaderConstantID("mLightColor");

			// Textures ID are important only for OpenGL interface.
			if(driver->getDriverType() == video::EDT_OPENGL)
				TextureID = services->getVertexShaderConstantID("myTexture");

			FirstUpdate = false;
		}

		// set inverted world matrix
		// if we are using highlevel shaders (the user can select this when
		// starting the program), we must set the constants by name.

		core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
		invWorld.makeInverse();

		services->setVertexShaderConstant(InvWorldID, invWorld.pointer(), 16);

		// set clip matrix
		core::matrix4 worldViewProj;
		worldViewProj = driver->getTransform(video::ETS_PROJECTION);
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);

		services->setVertexShaderConstant(WorldViewProjID, worldViewProj.pointer(), 16);

		// set camera position
		core::vector3df pos = device->getSceneManager()->getActiveCamera()->getAbsolutePosition();

		services->setVertexShaderConstant(PositionID, reinterpret_cast<f32*>(&pos), 3);

		// set light color
		video::SColorf col(0.8f,5.0f,1.0f,0.0f);

		services->setVertexShaderConstant(ColorID, reinterpret_cast<f32*>(&col), 4);

		// set transposed world matrix
		core::matrix4 world = driver->getTransform(video::ETS_WORLD);
		world = world.getTransposed();

		services->setVertexShaderConstant(TransWorldID, world.pointer(), 16);

		// set texture, for textures you can use both an int and a float setPixelShaderConstant interfaces (You need it only for an OpenGL driver).
		s32 TextureLayerID = 0;
		services->setPixelShaderConstant(TextureID, &TextureLayerID, 1);
	}

private:
	s32 WorldViewProjID;
	s32 TransWorldID;
	s32 InvWorldID;
	s32 PositionID;
	s32 ColorID;
	s32 TextureID;

	bool FirstUpdate;
} test1_shaderCallback;


//------------------------------------------------------------------------

class Test_3 : public Program
{
protected:
	Model ninja;
public:

	void createScene()
	{
		ninja.load("test1/ninja.b3d");
		ninja.getAnimatedNode()->setMaterialFlag(video::EMF_LIGHTING, false);
		ninja.getAnimatedNode()->setFrameLoop(206, 250); // idle
		ninja.getAnimatedNode()->setAnimationSpeed(15);
		ninja.getAnimatedNode()->setTransitionTime(0.2f);
		ninja.getAnimatedNode()->setScale(vector3df(0.1f, 0.1f, 0.1f));
		ninja.getAnimatedNode()->setPosition(vector3df(0,0,0));
		ninja.useShader(ninja.loadShader("opengl", &test1_shaderCallback));

		setFPSCamera();
		camera->setPosition(vector3df(0, 1, -1));
		//sceneManager->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
	}

	void clearScene()
	{
		sceneManager->clear();
		guienv->clear();
	}

	void render(f32 frameDeltaTime)
	{
		if(input.IsKeyDown(irr::KEY_ESCAPE))
			running=false; //device->closeDevice();

		ninja.getAnimatedNode()->animateJoints();

		sceneManager->drawAll();
		guienv->drawAll();

	}

};
