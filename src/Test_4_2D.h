#pragma once
#include "Program.h"

class Test_4 : public Program
{
	ITexture *bg, *part, *pointer, *obj;
	
	// lentävän obun tiedot
	float oangle;
	int ox, oy;
	SColor ocolor;
	int WWHH; // obun lev ja kor
	float oscale;

	int destroyed;
	bool flash;
public:

	void createScene()
	{
		oscale=1;
		flash=false;
		destroyed=0;

		srand(device->getTimer()->getRealTime());

		ox=rand()%(screenWidth-100);
		oy=rand()%(screenHeight-100);
		ocolor=SColor(255, rand()%255, rand()%255, rand()%255);
		oangle=(float)(rand()%360);
		
#ifndef _IRR_ANDROID_PLATFORM_
		device->getCursorControl()->setVisible(false);
#endif

		bg = driver->getTexture(DATA_DIR + "textures/bg.jpg");
		part = driver->getTexture(DATA_DIR + "textures/particle.png");
		obj = driver->getTexture(DATA_DIR + "textures/obj.png");
		pointer = driver->getTexture(DATA_DIR + "textures/pointer.png");
		//pointer = driver->getTexture(DATA_DIR + "textures/pointer_default.png");

		/*
			lasketaan skaalausarvo obulle (mitä pienempi näyttö, sitä pienempi obu)
			1024/128 =  8 (times smaller)
		*/
		WWHH = (int)((float)screenWidth / 8.0f);
		oscale = (float)screenWidth / 1024.0f;


		// A field to show some text. Comment out stat->setText in run() if you want to see the dpi instead of the fps.
		IGUIStaticText *text = guienv->addStaticText(L"2D test", rect<s32>(1,15,300,60), false, false, 0, 0 );
		text->setOverrideColor(SColor(255, 255,255,255));
	}

	void clearScene()
	{
		sceneManager->clear();
		guienv->clear();
	}

	void render(f32 frameDeltaTime)
	{
		if(input.IsKeyDown(irr::KEY_ESCAPE))
			running=false;  //device->closeDevice();

		if(input.MouseState.Buttons[0])
		{
			if(input.MouseState.Position.X > ox-WWHH/2 && input.MouseState.Position.X < ox+WWHH/2 &&
				input.MouseState.Position.Y > oy-WWHH/2 && input.MouseState.Position.Y < oy+WWHH/2)
			{
				ox=rand()%(screenWidth-WWHH);
				oy=rand()%(screenHeight-WWHH);
				ocolor=SColor(255, rand()%255, rand()%255, rand()%255);
				oangle=(float)(rand()%360);
				destroyed++;
				flash=true;
			}

		}


		// background
		SColor bgColors[4];
		SColor bgColor=SColor(255, 100,100, 100);
		if(flash) bgColor=SColor(255, 255,255, 255);
		for(int q=0;q<4;q++) bgColors[q]=bgColor;	
		driver->draw2DImage(bg, rect<s32>(0,0,screenWidth, screenHeight),
							rect<s32>(0,0,bg->getSize().Width, bg->getSize().Width), 0,
							bgColors);
		flash=false;

		// pyörivä obu
		oangle+=frameDeltaTime * 80;
		if(oangle>=360) oangle-=360;
		matrix4 pos, rot, scale;
		pos.setTranslation(vector3df((f32)ox, (f32)oy, 0));
		rot.setRotationDegrees(vector3df(0,0,oangle));
		scale.setScale(oscale);
		draw2DImage(obj, rect<s32>(0,0,obj->getSize().Width, obj->getSize().Height),
			pos*rot*scale,
			true, ocolor, true, false);


		if(input.MouseState.Buttons[0])
			driver->draw2DImage(part, position2d<s32>(input.MouseState.Position.X-part->getSize().Width/2, input.MouseState.Position.Y-part->getSize().Height/2), 
									rect<s32>(0,0,part->getSize().Width, part->getSize().Width), 
									0, SColor(255,255,255,255), true);

#ifndef __ANDROID__
		driver->draw2DImage(pointer, position2d<s32>(input.MouseState.Position.X, input.MouseState.Position.Y), rect<s32>(0,0,pointer->getSize().Width, pointer->getSize().Width), 
			0, SColor(255,255,255,255), true);
#endif

		sceneManager->drawAll();
		guienv->drawAll();

		stringc destrtxt="Destroyed: ";
		destrtxt+=destroyed;
		font->draw(destrtxt, rect<s32>(1,35, 50,300), SColor(255,255,0,0));

		/*
		destrtxt="";
		destrtxt+=screenWidth;
		destrtxt+=" ";
		destrtxt+=screenHeight;
		destrtxt+=" ";
		destrtxt+=oscale;
		font->draw(destrtxt, rect<s32>(1,65, 50,300), SColor(255,255,0,0));
		*/
	}

};
