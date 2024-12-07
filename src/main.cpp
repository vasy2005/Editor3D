#include <graphics.h>

#include "structs.h"
#include "input.h"
#include "draw.h"

using namespace std;



int main()
{
	srand(time(NULL));

	initwindow(windowWidth, windowHeight, "Editor3D");
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH); // make lines thicker to stop floodfill from flickering

	Flags* flags = new Flags;
	flags->windowHandle = FindWindow(NULL, "Editor3D");
	ShowWindow(flags->windowHandle, SW_MAXIMIZE);

	Object** objects = new Object*[flags->objectCapacity]; // init with capacity of 16, resize later
	int objectCount = 0;


	while (flags->windowShouldClose == false)
	{
		ProcessInput(objects, objectCount, flags); // handle mouse and keyboard events
		getMouseInputRot(objects, objectCount);
		getMouseInputScale(objects, objectCount);
		getMouseInputPos(objects, objectCount);
		checkKeyPressed();
		selectedVertice = -1;


		///// DRAW HERE ///////////////////////////////////////
		swapbuffers(); // double buffering
		cleardevice(); // clear previous frame

		for (int i = 0; i < objectCount; ++i)
		{
			DrawObject(objects[i]);
			drawHitBox(objects[i]);
		}
		///////////////////////////////////////////////////////


		// delay(16); // cap framerate at 60fps
	}


	delete flags;
	delete[] objects;
	closegraph();
	return 0;
}