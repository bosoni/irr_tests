#pragma once
#include <irrlicht.h>

using namespace irr;
using namespace core;
FILE *logFile;

class MyEventReceiver : public IEventReceiver
{
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];

public:

	MyEventReceiver()
	{
		clearStates();
	}

	// We'll create a struct to record info on the mouse state
	struct SMouseState
	{
		core::position2di Position;
		bool Buttons[3];
		bool ButtonUp[3];
		bool ButtonDown[3];
		bool DoubleClick[3];
	} MouseState;

	void clearStates()
	{
		for(int q=0;q<3;q++) 
			MouseState.Buttons[q]=MouseState.ButtonUp[q]=MouseState.ButtonDown[q]=MouseState.DoubleClick[q]=false;

		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

	// This is the one method that we have to implement
	bool OnEvent(const SEvent& event)
	{
		// Remember the mouse state
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch(event.MouseInput.Event)
			{
			case EMIE_LMOUSE_DOUBLE_CLICK:
				MouseState.ButtonUp[0]=MouseState.ButtonDown[0]=false;
				MouseState.DoubleClick[0]=true;
				break;

			case EMIE_LMOUSE_PRESSED_DOWN:
				MouseState.Buttons[0] = true;
				MouseState.ButtonDown[0] = true;
				break;

			case EMIE_LMOUSE_LEFT_UP:
				MouseState.Buttons[0] = false;
				MouseState.ButtonUp[0] = true;
				break;

			case EMIE_MOUSE_MOVED:
				MouseState.Position.X = event.MouseInput.X;
				MouseState.Position.Y = event.MouseInput.Y;
				break;

			case EMIE_MOUSE_WHEEL:
				// TODO jos tarttee
				break;

			}
		}
		else
			if (event.EventType == irr::EET_KEY_INPUT_EVENT) // Remember whether each key is down or up
				KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

#ifndef _IRR_ANDROID_PLATFORM_
		else
			if(event.EventType == irr::EET_LOG_TEXT_EVENT)
			{
				switch(event.LogEvent.Level)
				{
				case irr::ELL_INFORMATION:
					if(logFile==0)
					{
						logFile=fopen("log.txt", "w");
					}
					fwrite(event.LogEvent.Text, 1, strlen(event.LogEvent.Text), logFile); fwrite("\n", 1, 1, logFile);
					fflush(logFile);
					//return true;
					break;

				default:
					break;
				}
			}
#endif

#ifdef _IRR_ANDROID_PLATFORM_
// We only work with the first for now, but could be up to NUMBER_OF_MULTI_TOUCHES touches.

		else
		if (event.EventType == EET_MULTI_TOUCH_EVENT)
		{
			switch ( event.MultiTouchInput.Event)
			{
				case EMTIE_PRESSED_DOWN:
					MouseState.Buttons[0] = true;
					MouseState.ButtonDown[0] = true;
					MouseState.Position.X = event.MultiTouchInput.X[0];
					MouseState.Position.Y = event.MultiTouchInput.Y[0];
					break;

				case EMTIE_LEFT_UP:
					MouseState.Buttons[0] = false;
					MouseState.ButtonUp[0] = true;
					MouseState.Position.X = event.MultiTouchInput.X[0];
					MouseState.Position.Y = event.MultiTouchInput.Y[0];
					break;

				case EMTIE_MOVED:
					MouseState.Position.X = event.MultiTouchInput.X[0];
					MouseState.Position.Y = event.MultiTouchInput.Y[0];
					break;

				default:
					break;
			}
		}
#endif

		return false;
	}

	// This is used to check whether a key is being held down
	bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

};
