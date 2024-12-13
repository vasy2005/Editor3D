#include <graphics.h>


#include "structs.h"
#include "input.h"
#include "draw.h"
#include "menu.h"

#include <iostream>

using namespace std;


int main()
{
	srand(time(NULL));

	initwindow(windowWidth, windowHeight, "Editor3D");
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH); // make lines thicker to stop floodfill from flickering

	
	flags->windowHandle = FindWindow(NULL, "Editor3D");

	// fullscreen
	SetWindowLongPtr(flags->windowHandle, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_MINIMIZEBOX);
	SetWindowPos(flags->windowHandle, NULL, 0, 0, 1920, 1080, SWP_NOZORDER);
	GetCurrentDirectory(512, flags->cwd);

	Object** objects = new Object*[flags->objectCapacity]; // init with capacity of 16, resize later
	int objectCount = 0;

	Menu* menu = InitMenu(flags);

	int buffer = 0;

	while (flags->windowShouldClose == false)
	{
		ProcessInput(objects, objectCount, flags, menu); // handle mouse and keyboard events
		getMouseInputRot(objects, objectCount);
		getMouseInputScale(objects, objectCount);
		getMouseInputPos(objects, objectCount);
		checkKeyPressed();


		///// DRAW HERE ///////////////////////////////////////
		if (flags->updateWindow == true) // only redraw screen if there are changes (keyboard or mouse input detected)
		{
			//cout << "TEST";
			flags->updateWindow = false;
			setvisualpage(buffer);
			setactivepage(1 - buffer);

			cleardevice(); // clear previous frame

			if (selectedObject != NULL)
				drawHitBox(selectedObject);

			for (int i = 0; i < objectCount; ++i)
				DrawObject(objects[i]);

			DrawMenu(menu); // draw over objects
			DrawAxis();

			setvisualpage(1 - buffer);
			buffer = 1 - buffer;

			cout << "drawing...\n";
		}
		///////////////////////////////////////////////////////


		 delay(16); // cap framerate at 60fps
	}


	delete flags;
	delete[] objects;
	closegraph();
	return 0;
}