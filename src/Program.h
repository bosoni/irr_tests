#pragma once
#include <iostream>
#include <vector>
#include <irrlicht.h>
#include "EventReceiver.h"
#include "CMeshCombiner.h"
#include "ScreenQuad.h"
#include "Model.h"

#define LIMIT_FPS
#define PROG_NAME L"Test"
#define _OVERRIDE_  //override
#ifdef _IRR_WINDOWS_
	#pragma comment(lib, "Irrlicht.lib")
	//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
#ifdef _IRR_ANDROID_PLATFORM_
	#include <android_native_app_glue.h>
	#include "android_tools.h"
	const stringc DATA_DIR="media/";
#else
	const stringc DATA_DIR="../media/";
#endif

/*
TODO pois jos ei tarvii
#ifdef _IRR_WINDOWS_
	#include <hash_map>
	using namespace stdext;
#endif
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2
	// gcc 4.2 and greater uses tr1::unordered_set
	// see http://gcc.gnu.org/gcc-4.3/changes.html
	#include <tr1/unordered_map>
	#define hash_map std::tr1::unordered_map
#endif
*/

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

E_DRIVER_TYPE driverType;
IrrlichtDevice *device = 0;
IVideoDriver *driver = 0;
ISceneManager *sceneManager = 0;
IFileSystem *fileSystem = 0;

class Program
{
protected:	
	int screenWidth, screenHeight;

	MyEventReceiver input;
	ICameraSceneNode *camera;

	IGUIEnvironment *guienv;
	IGUIFont *font;

	int lastFPS;
	bool running;

	const static bool fullScreen=false, VSync=true;

public:
	Program()
	{
		lastFPS = -1;
		running=true;
	}

#ifndef _IRR_ANDROID_PLATFORM_
	// kutsu vain kerran main():sta
	bool init(E_DRIVER_TYPE type, int w, int h)
	{
		if(device==0)
		{
			screenWidth=w; screenHeight=h;

			driverType=type;
			device = createDevice(driverType, dimension2d<u32>(w, h), 32, fullScreen, false, VSync, &input);

			if (!device)
			{
				device->getLogger()->log("Cannot create device.");
				return false;
			}

			device->setWindowCaption(PROG_NAME);

			driver = device->getVideoDriver();
			sceneManager = device->getSceneManager();
			guienv = device->getGUIEnvironment();
			fileSystem = device->getFileSystem();

			IGUISkin* skin = guienv->getSkin();
			if(screenWidth<500)
				font = guienv->getFont(DATA_DIR + "font/Arial.png");
			else
				font = guienv->getFont(DATA_DIR + "font/bigfont.png");
			if (font) skin->setFont(font);
		}
	
		return true;
	}
#else
	// kutsu vain kerran main():sta
	bool init(E_DRIVER_TYPE type, int w, int h, android_app* app)
	{
		if(device==0)
		{
			driverType=type;

			// create device
			SIrrlichtCreationParameters param;
			param.DriverType = type;				  // android:glEsVersion in AndroidManifest.xml should be "0x00020000"
			param.WindowSize = dimension2d<u32>(w,h); // using 0,0 it will automatically set it to the maximal size
			param.PrivateData = app;
			param.Bits = 24;
			param.ZBufferBits = 16;
			param.AntiAlias  = 0;
			// Logging is written to a file. So your application should disable all logging when you distribute your
			// application or it can fill up that file over time.
#ifndef _DEBUG
			param.LoggingLevel = ELL_NONE;	
#endif
			device = createDeviceEx(param);
			device->sleep(1000, false);
			if(!device) 
			{
				device->getLogger()->log("Cannot create device.");
				return false;
			}

			device->setEventReceiver(&input);
			driver = device->getVideoDriver();
			sceneManager = device->getSceneManager();
			guienv = device->getGUIEnvironment();

			/* Access to the Android native window. You often need this when accessing NDK functions like we are doing here.
			   Note that windowWidth/windowHeight have already subtracted things like the taskbar which your device might have,
			   so you get the real size of your render-window.
			*/
			ANativeWindow* nativeWindow = static_cast<ANativeWindow*>(driver->getExposedVideoData().OGLESAndroid.Window);
			int32_t windowWidth = ANativeWindow_getWidth(app->window);
			int32_t windowHeight = ANativeWindow_getHeight(app->window);

			screenWidth=windowWidth; 
			screenHeight=windowHeight;

			/* Get display metrics. We are accessing the Java functions of the JVM directly in this case as there is no NDK function for that yet.
			   Checkout android_tools.cpp if you want to know how that is done. */
			irr::android::SDisplayMetrics displayMetrics;
			memset(&displayMetrics, 0, sizeof displayMetrics);
			irr::android::getDisplayMetrics(app, displayMetrics);

			/* For troubleshooting you can use the Irrlicht logger.
			   The Irrlicht logging messages are send to the Android logging system using the tag "Irrlicht". 
			   They stay in a file there, so you can check them even after running your app.
			   You can watch them with the command: "adb logcat Irrlicht:V *:S"
			   (this means Irrlicht _V_erbose and all other messages _S_ilent).
			   Clean the logging file with: "adb logcat -c".
			   See http://developer.android.com/tools/debugging/debugging-log.html for more advanced log options.
			*/
			char strDisplay[1000];
			sprintf(strDisplay, "Window size:(%d/%d)\nDisplay size:(%d/%d)", windowWidth, windowHeight, displayMetrics.widthPixels, displayMetrics.heightPixels);
			device->getLogger()->log(strDisplay);
			
			fileSystem = device->getFileSystem();

			// The Android assets file-system does not know which sub-directories it has (blame google).
			// So we have to add all sub-directories in assets manually. Otherwise we could still open the files, 
			// but existFile checks will fail (which are for example needed by getFont).
			for ( u32 i=0; i < fileSystem->getFileArchiveCount(); ++i )
			{
				IFileArchive* archive = fileSystem->getFileArchive(i);
				if ( archive->getType() == EFAT_ANDROID_ASSET )
				{
					archive->addDirectoryToFileList(DATA_DIR);
					archive->addDirectoryToFileList(DATA_DIR+"font/");
					archive->addDirectoryToFileList(DATA_DIR+"test1/");
					archive->addDirectoryToFileList(DATA_DIR+"textures/");
					break;
				}
			}

			IGUISkin* skin = guienv->getSkin();
			if(screenWidth<500)
				font = guienv->getFont(DATA_DIR + "font/Arial.png");
			else
				font = guienv->getFont(DATA_DIR + "font/bigfont.png");
			if (font) skin->setFont(font);
		}
		return true;
	}
#endif

	void setFPSCamera()
	{
		device->getCursorControl()->setVisible(false);
		SKeyMap keyMap[8];
		keyMap[0].Action = EKA_MOVE_FORWARD;
		keyMap[0].KeyCode = KEY_KEY_W;
		keyMap[1].Action = EKA_MOVE_BACKWARD;
		keyMap[1].KeyCode = KEY_KEY_S;
		keyMap[2].Action = EKA_STRAFE_LEFT;
		keyMap[2].KeyCode = KEY_KEY_A;
		keyMap[3].Action = EKA_STRAFE_RIGHT;
		keyMap[3].KeyCode = KEY_KEY_D;
		camera = sceneManager->addCameraSceneNodeFPS(0, 100, 0.02f, -1, keyMap, 4);
		camera->setPosition(vector3df(0,2,-1));
		camera->setNearValue(0.1f);
		camera->setFarValue(1000);
		//camera->setTarget(vector3df(0,0,0));
	}

	void showFPS()
	{
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw tmp(PROG_NAME);
			tmp += L"    ";
			tmp += driver->getName();
			tmp += L"    Poly: ";
			tmp += driver->getPrimitiveCountDrawn();
			tmp += L"    FPS: ";
			tmp += fps;
			device->setWindowCaption(tmp.c_str());
			lastFPS = fps;
		}
	}

	void run()
	{
		u32 then = device->getTimer()->getTime();
		bool firstTime=true;

		while(running && device->run())
		{
			if(device->isWindowActive())
			{
				// Work out a frame delta time.
				const u32 now = device->getTimer()->getTime();
				const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
				then = now;

				driver->beginScene(true, true, SColor(255,50,50,150));
				if(firstTime)
				{
					firstTime=false;
					render(0);
				}
				else
					render(frameDeltaTime);

				driver->endScene();

#ifdef _DEBUG
#ifndef _IRR_ANDROID_PLATFORM_
				showFPS();
#endif
#endif

				// limit fps (about 60):
#ifdef LIMIT_FPS
				irr::ITimer* timer = device->getTimer(); 
				irr::u32 timeThisFrame = timer->getTime(); 
				while( (timer->getTime() - timeThisFrame) <= 16 ) timer->tick(); 
#endif
			}
			device->yield();

		}

		clearScene();
	}

	virtual void render(f32 frameDeltaTime) _OVERRIDE_
	{
	}

	virtual void createScene() _OVERRIDE_
	{
	}

	virtual void clearScene() _OVERRIDE_
	{
		sceneManager->clear();
		guienv->clear();
	}

	IMetaTriangleSelector *createTriangleSelectors()
	{
		// Create a meta triangle selector to hold several triangle selectors.
		IMetaTriangleSelector *meta = sceneManager->createMetaTriangleSelector();
		array<ISceneNode *> nodes;
		sceneManager->getSceneNodesFromType(ESNT_ANY, nodes); // Find all nodes

		for (u32 i=0; i < nodes.size(); ++i)
		{
			ISceneNode *node = nodes[i];
			ITriangleSelector *selector = 0;
			switch(node->getType())
			{
			case scene::ESNT_ANIMATED_MESH:
				// Because the selector won't animate with the mesh,
				// and is only being used for camera collision, we'll just use an approximate
				// bounding box instead of ((scene::IAnimatedMeshSceneNode*)node)->getMesh(0)
				selector = sceneManager->createTriangleSelectorFromBoundingBox(node);
				node->setTriangleSelector(selector);
				break;

			case scene::ESNT_MESH:
				selector = sceneManager->createTriangleSelector(((IMeshSceneNode*)node)->getMesh(), node);
				node->setTriangleSelector(selector);
				break;

			case scene::ESNT_TERRAIN:
				selector = sceneManager->createTerrainTriangleSelector((ITerrainSceneNode*)node);
				break;

			case scene::ESNT_OCTREE:
				selector = sceneManager->createOctreeTriangleSelector(((IMeshSceneNode*)node)->getMesh(), node);
				break;

			default:
				// Don't create a selector for this node type
				break;
			}

			if(selector)
			{
				// Add it to the meta selector, which will take a reference to it
				meta->addTriangleSelector(selector);
				// And drop my reference to it, so that the meta selector owns it.
				selector->drop();
			}
		}
		return meta;
	}


	 
	void draw2DImage(irr::video::ITexture* texture, irr::core::rect<irr::s32> sourceRect, const core::matrix4& mat, bool useAlphaChannel, irr::video::SColor color, bool useFiltering/* = true*/, bool usePremultipliedAlpha/* = false*/) 
	{
		  //irr::video::SMaterial material;
		irr::video::SMaterial& material = driver->getMaterial2D();
 
		/*
		if(usePremultipliedAlpha)
		{
			color.color = PremultiplyAlpha(color.color);
		}
		*/

		// Store and clear the world matrix
		irr::core::matrix4 oldWorldMat = driver->getTransform(irr::video::ETS_WORLD);
		// Store and clear the projection matrix
		irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
		// Store and clear the view matrix
		irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
 
		core::matrix4 m;
		// this fixes some problems with pixel exact rendering, but also breaks nice texturing
		driver->setTransform(irr::video::ETS_WORLD, mat);
 
		// adjust the view such that pixel center aligns with texels
		// Otherwise, subpixel artifacts will occur
		if(driver->getDriverType() == video::EDT_DIRECT3D9)
			m.setTranslation(core::vector3df(-0.5f,-0.5f,0));
		driver->setTransform(irr::video::ETS_VIEW, m);
 
		const core::dimension2d<u32>& renderTargetSize = driver->getCurrentRenderTargetSize();
		m.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0, 1.0);
		m.setTranslation(core::vector3df(-1,1,0));
		driver->setTransform(irr::video::ETS_PROJECTION, m);
 
		// Find the positions of corners
		irr::core::vector3df corner[4];
 
		f32 halfWidth = (f32)(sourceRect.getWidth())*0.5f;
		f32 halfHeight = (f32)(sourceRect.getHeight())*0.5f;
		corner[0] = irr::core::vector3df(0.0f - halfWidth, 0.0f - halfHeight, 0.0f);
		corner[1] = irr::core::vector3df(0.0f + halfWidth, 0.0f - halfHeight, 0.0f);
		corner[2] = irr::core::vector3df(0.0f - halfWidth, 0.0f + halfHeight, 0.0f);
		corner[3] = irr::core::vector3df(0.0f + halfWidth, 0.0f + halfHeight, 0.0f);
 
 
		// Find the uv coordinates of the sourceRect
		irr::core::vector2df uvCorner[4];
		uvCorner[0] = irr::core::vector2df((f32)sourceRect.UpperLeftCorner.X,(f32)sourceRect.UpperLeftCorner.Y);
		uvCorner[1] = irr::core::vector2df((f32)sourceRect.LowerRightCorner.X,(f32)sourceRect.UpperLeftCorner.Y);
		uvCorner[2] = irr::core::vector2df((f32)sourceRect.UpperLeftCorner.X,(f32)sourceRect.LowerRightCorner.Y);
		uvCorner[3] = irr::core::vector2df((f32)sourceRect.LowerRightCorner.X,(f32)sourceRect.LowerRightCorner.Y);
		const irr::core::dimension2d<u32>& texSize = texture->getOriginalSize();
		for (int x = 0; x < 4; x++) 
		{
			float uvX = uvCorner[x].X/(float)texSize.Width;
			float uvY = uvCorner[x].Y/(float)texSize.Height;
			uvCorner[x] = irr::core::vector2df(uvX,uvY);
		}
 
		// Vertices for the image
		irr::video::S3DVertex vertices[4];
		irr::u16 indices[6] = { 0, 1, 2, 3 ,2 ,1 };
 
		for (int x = 0; x < 4; x++) 
		{
			vertices[x].Pos = corner[x];
			vertices[x].TCoords = uvCorner[x];
			vertices[x].Color = color;
		}
 
		//driver->enableMaterial2D(true);
		material.Lighting = false;
		material.ZWriteEnable = false;
		material.ZBuffer = false;
		material.BackfaceCulling = false;
		//material.AntiAliasing = true;
		material.TextureLayer[0].Texture = texture;
		// Fix dark border appearance when texture scaled down with bilinear or trilinear filter on
		if(uvCorner[0].X >= 0.0f && uvCorner[3].X <= 1.0f)
			material.TextureLayer[0].TextureWrapU = video::ETC_CLAMP_TO_EDGE;
		else
			material.TextureLayer[0].TextureWrapU = video::ETC_REPEAT;
		if(uvCorner[0].Y >= 0.0f && uvCorner[3].Y <= 1.0f)
			material.TextureLayer[0].TextureWrapV = video::ETC_CLAMP_TO_EDGE;
		else
			material.TextureLayer[0].TextureWrapV = video::ETC_REPEAT;
 
		if(usePremultipliedAlpha)
			material.MaterialTypeParam = irr::video::pack_textureBlendFunc(irr::video::EBF_ONE, irr::video::EBF_ONE_MINUS_SRC_ALPHA, irr::video::EMFN_MODULATE_1X, irr::video::EAS_TEXTURE | irr::video::EAS_VERTEX_COLOR);
		else
			material.MaterialTypeParam = irr::video::pack_textureBlendFunc(irr::video::EBF_SRC_ALPHA, irr::video::EBF_ONE_MINUS_SRC_ALPHA, irr::video::EMFN_MODULATE_1X, irr::video::EAS_TEXTURE | irr::video::EAS_VERTEX_COLOR);
 
		if (useAlphaChannel)
			material.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
		else
			material.MaterialType = irr::video::EMT_SOLID;
 
		material.TextureLayer[0].BilinearFilter = false;
		material.TextureLayer[0].TrilinearFilter = useFiltering;
 
		driver->setMaterial(material);
		//driver->draw2DVertexPrimitiveList(&vertices[0],4,&indices[0],2);
		driver->drawIndexedTriangleList(&vertices[0],4,&indices[0],2);
 
		//driver->enableMaterial2D(false);
 
		// Restore projection and view matrices
		driver->setTransform(irr::video::ETS_WORLD,oldWorldMat);
		driver->setTransform(irr::video::ETS_PROJECTION,oldProjMat);
		driver->setTransform(irr::video::ETS_VIEW,oldViewMat);
	}

};

