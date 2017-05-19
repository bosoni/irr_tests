// RTT testi
// lataa skenen, näyttää RT:n vasemmas yläkulmassa
// SPACE näyttää RT:n kokoruudulla
#pragma once
#include "Program.h"

class Test_2 : public Program
{
protected:	
	ScreenQuad *quad;
	scene::ICameraSceneNode* fixedCam;
	video::ITexture* rt;

public:

	void createScene()
	{
		Model map;
		map.load("test1/map1.irr");

		setFPSCamera();


		// create render target
		if (driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
		{
			rt = driver->addRenderTargetTexture(core::dimension2d<u32>(256,256), "RTT1");
			fixedCam = sceneManager->addCameraSceneNode(0, core::vector3df(5,5,-30), core::vector3df(0,0,0));

			quad=new ScreenQuad(driver);
			quad->setTexture(rt);
		}

		printf("\n\nPress SPACE to see fs RT");

	}

	void clearScene()
	{
		sceneManager->clear();
		guienv->clear();
	}

	void render(f32 frameDeltaTime)
	{
		bool pressed=false;
		float SPD=1;

		if(input.IsKeyDown(irr::KEY_ESCAPE))
			running=false; //device->closeDevice();

		// rt kameran liikutus
		fixedCam->setPosition( vector3df(fixedCam->getPosition().X,
			fixedCam->getPosition().Y,
			fixedCam->getPosition().Z+frameDeltaTime*5));

		// set render target texture
		driver->setRenderTarget(rt, true, true, video::SColor(255,0,0,255));

		// set fixed camera as active camera
		sceneManager->setActiveCamera(fixedCam);

		// draw whole scene into render buffer
		sceneManager->drawAll();

		// set back old render target
		// The buffer might have been distorted, so clear it
		driver->setRenderTarget(0, true, true, 0);

		// make the cube visible and set the user controlled camera as active one
		//obj->setVisible(true);
		sceneManager->setActiveCamera(camera);


		// näytä RT kokoruudulla jos painetaan välilyöntiä
		if(input.IsKeyDown(irr::KEY_SPACE))
		{
			// quadin materiaali on rt joten rendaus toimii
			quad->render();
		}
		else
		{
			// ensin rendataan skene ruudulle
			sceneManager->drawAll();

			// sitten rendataan  rt  yläkulmaa
			driver->draw2DImage(rt, core::position2d<s32>(0,0),
				core::rect<s32>(0,0,256,256), 0,
				video::SColor(255,255,255,255), true);
		}


	}
};
