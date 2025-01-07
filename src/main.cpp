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
#include "texture.h"
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

	objects = new Object*[flags->objectCapacity]; // init with capacity of 16, resize later

	AddNewObject(0, 1);

	canvas = new unsigned char[24 + 4 * WINDOW_WIDTH * WINDOW_HEIGHT];

	ResetCamera();


	int buffer = 0;

	while (flags->windowShouldClose == false)
	{
		stopwatch = clock(); // din time.h

		oldSelectedObject = selectedObject;

		bool test;

		
		ProcessInput(objects, objectCount, flags, menu); // handle mouse and keyboard events
		
		// cout << "1: " << (selectedObject == NULL) << '\n';
		// cout << flags->drawTooltip << ' ' << (selectedObject != NULL) << 1 << '\n';
		if (mousey() > 85 && flags->grabbedFloatWindow == false)
		{
			
			getMouseInputRot(objects, objectCount); 
			
			getMouseInputScale(objects, objectCount);
			getMouseInputPos(objects, objectCount); 

			checkKeyPressed();

		}
		// cout << flags->drawTooltip << ' ' << (selectedObject != NULL) << 2 << '\n';

		if (selectedObject != NULL)
		{
			if (oldSelectedObject != selectedObject) // reopen float window
				flags->floatWindowClosed = false;
		}
		else
			flags->floatWindowClosed = true;

		// ascunde paste cand nu e nimic in clipboard
		menu->buttons[8].disabled = (copiedObject == NULL);

		// ascunde copy, delete, texture si color cand nu este nici un obiect selectat
		menu->buttons[4].disabled = menu->buttons[6].disabled 
			= menu->buttons[7].disabled = menu->buttons[9].disabled = (selectedObject == NULL);

		///// DRAW HERE ///////////////////////////////////////
		if (flags->updateWindow == true) // only redraw screen if there are changes
		{
			//cout << flags->drawTooltip << ' ' << (selectedObject != NULL) <<1<< '\n';
			setvisualpage(buffer);
			setactivepage(1 - buffer);

			cleardevice(); // clear previous frame
			ClearFrame();

			//CalculatePlane();
			DrawPlane();

			if (selectedObject != NULL)
				drawHitBox(selectedObject);

			for (int i = 0; i < objectCount; ++i)
				CalculateRealCoords(objects[i]);

			HighlightObject();
			sortObjectsByZ(objects);


			for (int i = 0; i < objectCount; ++i)
				DrawObject(objects[i]);

			DrawMenu(menu); // draw over objects
			//if (selectedObject != NULL && selectedObject->texture != NULL)
			//{
			//	putimage(0, 0, selectedObject->texture, COPY_PUT);
			//	cout << selectedObject->textureW << ' ' << selectedObject->textureH << '\n';
			//}

			setvisualpage(1 - buffer);
			buffer = 1 - buffer;

			flags->updateWindow = false;

			// cout << camera.rotation.x << ' ' << camera.rotation.y << ' ' << camera.rotation.z << '\n';
			// cout << "drawing...\n";

			// cout << "3: " << (selectedObject == NULL) << '\n';
			// cout << floatWindow.pivotX << ' ' << floatWindow.pivotY << '\n';
			//cout << flags->drawTooltip << ' ' << (selectedObject != NULL) <<2<< '\n';
		}
		///////////////////////////////////////////////////////


		 delay(16); // cap framerate at 60fps
	}

	FreeIconsInMemory();
	delete[] canvas;
	delete[] objects;
	delete menu;
	delete flags;
	closegraph();
	return 0;
}