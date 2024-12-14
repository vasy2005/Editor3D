#include <graphics.h>

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <time.h>
#include <windows.h>

using namespace std;

#include "constants.h"
#include "structs.h"
#include "globals.h"
#include "vector_math.h"
#include "object.h"
#include "draw.h"
#include "file_io.h"
#include "menu.h"
#include "input.h"

void NewWindow(Flags* flags)
{
	initwindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH); // make lines thicker to stop floodfill from flickering

	flags->windowHandle = FindWindow(NULL, WINDOW_NAME);

	// fullscreen
	SetWindowLongPtr(flags->windowHandle, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_MINIMIZEBOX);
	SetWindowPos(flags->windowHandle, NULL, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_NOZORDER);
	GetCurrentDirectory(512, flags->workingDir);

	srand(time(NULL));
}

int main()
{
	// Flags* flags = new Flags;
	NewWindow(flags);
	Menu* menu = NewMenu(flags);

	Object** objects = new Object*[flags->objectCapacity]; // init with capacity of 16, resize later
	int objectCount = 0;

	int buffer = 0;

	while (flags->windowShouldClose == false)
	{
		ProcessInput(objects, objectCount, flags, menu); // handle mouse and keyboard events
		getMouseInputRot(objects, objectCount);
		getMouseInputScale(objects, objectCount);
		getMouseInputPos(objects, objectCount);
		checkKeyPressed();

		///// DRAW HERE ///////////////////////////////////////
		if (flags->updateWindow == true) // only redraw screen if there are changes
		{
			setvisualpage(buffer);
			setactivepage(1 - buffer);

			cleardevice(); // clear previous frame

			if (selectedObject != NULL)
				drawHitBox(selectedObject);

			for (int i = 0; i < objectCount; ++i)
				DrawObject(objects[i]);

			DrawMenu(menu); // draw over objects

			setvisualpage(1 - buffer);
			buffer = 1 - buffer;

			flags->updateWindow = false;
		}
		///////////////////////////////////////////////////////

		 delay(16); // cap framerate at 60fps
	}

	delete[] objects;
	delete menu;
	delete flags;
	closegraph();
	return 0;
}