// jos k�ytt�� angelscripi�, define USESCRIPT
#undef USESCRIPT

//#define _IRR_STATIC_LIB_

#include "Program.h"

#include "Test_1_Ninja.h"
#include "Test_2_RTT.h"
#include "Test_3_Shader.h"
#include "Test_4_2D.h"

//#include "Game_Memmu.h"

#ifdef _IRR_ANDROID_PLATFORM_
#include <android_native_app_glue.h>
#include "android_tools.h"
void android_main(android_app *app)
{
	app_dummy(); // Make sure glue isn't stripped.

	//Program *prog = new Test_1(); // walking ninja, grass
	//Program *prog = new Test_2(); // rtt
	//Program *prog = new Test_3(); // shader
	//Program *prog = new Test_4(); // 2d
#else
#include <stdio.h>
int main(int argc, char **argv)
{
	Program *prog;
	
	printf("Choose test (1-4): ");
	char t=getchar();
	switch(t)
	{
	case '1':
		prog = new Test_1(); // walking ninja, grass
		break;
	case '2':
		prog = new Test_2(); // rtt
		break;
	case '3':
		prog = new Test_3(); // shader
		break;
	case '4':
		prog = new Test_4(); // 2d
		break;
	default:
		printf("test not found\n");
		return 0;
	}

#endif

	//Program *prog = new Game_Memmu();

#ifdef _IRR_ANDROID_PLATFORM_
	//if(prog->init(EDT_OGLES1, 0,0, app)==false)
	if(prog->init(EDT_OGLES2, 0,0, app)==false)
		return;
#else
	if(prog->init(EDT_OPENGL, 1024, 768)==false)
		return -1;
#endif

	prog->createScene();
	prog->run();

	// lopetus
	device->setEventReceiver(0);
	device->closeDevice();
	device->drop();
#ifndef _IRR_ANDROID_PLATFORM_
	return 0;
#endif
}
