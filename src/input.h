#pragma once

#include <graphics.h>
#include <windows.h>
#include "structs.h"
#include "object.h"
#include "file_io.h"
#include "globals.h"
#include "menu.h"

#include <iostream>

using namespace std;



void Quit(Flags*& flags)
{
	// enervant
	//int flag = MessageBox(flags->windowHandle, "Are you sure you want to exit?", "Warning!", MB_YESNO | MB_ICONEXCLAMATION);

	//if (flag == IDYES)
		flags->windowShouldClose = true;
}

bool IsPressed(int key)
{
	return (GetAsyncKeyState(key) & 0x8000);
}

void ProcessInput(Object**& objects, int& objectCount, Flags*& flags, Menu*& menu)
{
	int mouseX = mousex();
	int mouseY = mousey();



	// close the app by pressing ESC
	if (IsPressed(VK_ESCAPE))
		Quit(flags);



	// new scene with CTRL + N
	if (IsPressed(VK_CONTROL) && IsPressed('N'))
		NewScene(objects, objectCount, flags);

	// save files with CTRL + S
	if (IsPressed(VK_CONTROL) && IsPressed('S'))
		Save(objects, objectCount, flags);

	// open files with CTRL + O
	if (IsPressed(VK_CONTROL) && IsPressed('O'))
		Open(objects, objectCount, flags);



	// create new objects with keyboard shortcuts
	if (IsPressed(VK_SPACE))
	{
		if (flags->pressedSpace == false)
		{
			if (objectCount + 1 > flags->objectCapacity) // resize objects if too many are added
			{
				flags->objectCapacity *= 2;
				Object** temp = new Object * [flags->objectCapacity];
				memcpy(temp, objects, sizeof(Object*) * objectCount);
				delete[] objects;
				objects = temp;
			}


			int oldObjectCount = objectCount;


			int random = -1;
			if (IsPressed('R')) // create a random object SPACE + R
				random = rand() % 2;



			if (IsPressed('1') || random == 0) // create cube SPACE + 1
				objects[objectCount++] = NewCube({ (float)(mouseX), (float)(mouseY), 0 }, { 100, 100, 100 }, { 0, 0, 0 }, { 255, 255, 255 });

			else if (IsPressed('2') || random == 1) // create icosahedron SPACE + 2
				objects[objectCount++] = NewIcosahedron({ (float)(mouseX), (float)(mouseY), 0 }, { 50,  50,  50 }, { 0, 0, 0 }, { 255, 255, 255 });



			if (oldObjectCount != objectCount) // modified
			{
				if (random != -1)
					RandomizeObjectProperties(objects[objectCount - 1]);

				// flags->selectedObject = objects[objectCount - 1];
				flags->updateWindow = true;
				flags->pressedSpace = true;
			}
		}
	}
	else
		flags->pressedSpace = false;

	// handle the button click event after the animation plays out
	if (flags->selectedButton != -1)
	{
		if (flags->buttonAnimation == 0)
		{
			Sleep(250);
			menu->buttons[flags->selectedButton].pressed = false;
		}

		if (flags->buttonAnimation == 2)
		{
			switch (flags->selectedButton)
			{
				case 0: // close
					Quit(flags);
					break;

				case 1: // new
					NewScene(objects, objectCount, flags);
					break;

				case 2: // save
					Save(objects, objectCount, flags);
					break;

				case 3: // open
					Open(objects, objectCount, flags);
					break;

				case 4:
					CHOOSECOLOR dialogue = {0};
					COLORREF result      = WHITE;
					COLORREF custom[16];

					dialogue.lStructSize  = sizeof(dialogue);
					dialogue.Flags        = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
					dialogue.hwndOwner    = flags->windowHandle;
					dialogue.rgbResult    = result;
					dialogue.lpCustColors = custom;

					int error = ChooseColor(&dialogue);

					if (error != 0)
					{
						if (selectedObject != NULL)
							selectedObject->color = {GetRValue(dialogue.rgbResult), GetGValue(dialogue.rgbResult), GetBValue(dialogue.rgbResult)};
					}

					break;
			}

			flags->selectedButton = -1;
		}

		flags->buttonAnimation++;
	}

	// check button pressed
	if (IsPressed(VK_LBUTTON))
	{
		if (flags->pressedLeftClick == false)
		{
			for (int i = 0; i < menu->buttonCount; i++)
			{
				int left   = menu->buttons[i].center.x - menu->buttons[i].width/2;
				int right  = menu->buttons[i].center.x + menu->buttons[i].width/2;
				int top    = menu->buttons[i].center.y - menu->buttons[i].height/2;
				int bottom = menu->buttons[i].center.y + menu->buttons[i].height/2;

				if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom)
				{
					flags->buttonAnimation = 0;
					flags->selectedButton = i;
					menu->buttons[i].pressed = true;
					break;
				}
			}

			flags->pressedLeftClick = true;
		}
	}
	else
		flags->pressedLeftClick = false;

	// this is bad. need to update only if object actually changes
	if (IsPressed(VK_LBUTTON) || IsPressed(VK_RBUTTON) || IsPressed(VK_MBUTTON) || flags->oldMouseX != mouseX || flags->oldMouseY != mouseY || IsPressed('Q'))
		flags->updateWindow = true;

	flags->oldMouseX = mouseX;
	flags->oldMouseY = mouseY;
}



/*
* Etape:
* 1. Creare puncte rosii pe varfuri
* 2. Punctele cresc cand cursorul e pe ele
* 3. Miscare varfuri xOy (cu click stanga)
* 4. Miscare varfuri OZ (cu click dreapta)
* 5. Stergere puncte (cu grafuri)
*	- stergere varf din vertices
*   - stergere indices incidente
*   - creare noi indices cu graf
*   - stergere din graf
* 6. Creare puncte noi
* 7. Legare puncte, combinare obiecte
*/

void getMouseInputRot(Object* object[], int n)
{
	static bool mouse = 0;
	static double initx, inity;
	static int crt;
	double x = mousex(), y = mousey();
	if (ismouseclick(WM_RBUTTONUP))
	{
		mouse = 0;
		clearmouseclick(WM_RBUTTONUP);
		return;
	}

	if (ismouseclick(WM_RBUTTONDOWN))
	{
		for (int i = 0; i < n; ++i)
		{
			Vector2 first = object[i]->hitBox[0];
			Vector2 second = object[i]->hitBox[1];
			if (x >= first.x && x <= second.x && y >= first.y && y <= second.y)
			{
				crt = i;
				initx = x;
				inity = y;
				mouse = 1;
				break;
			}
		}
		clearmouseclick(WM_RBUTTONDOWN);
		return;
	}

	if (!mouse) return;


	object[crt]->rotation = { object[crt]->rotation.x - (y - inity) / 180, object[crt]->rotation.y + (x - initx) / 180, object[crt]->rotation.z };
	initx = x; inity = y;
}

void getMouseInputScale(Object* object[], int n)
{
	static double initxS, inityS;
	static int crtS;
	static bool mouseS = 0;

	if (ismouseclick(WM_MBUTTONUP))
	{
		mouseS = 0;
		clearmouseclick(WM_MBUTTONUP);
		clearmouseclick(WM_MBUTTONDOWN);
		return;
	}

	double x = mousex(), y = mousey();

	if (ismouseclick(WM_MBUTTONDOWN))
	{
		for (int i = 0; i < n; ++i)
		{
			Vector2 first = object[i]->hitBox[0];
			Vector2 second = object[i]->hitBox[1];
			if (x >= first.x && x <= second.x && y >= first.y && y <= second.y)
			{
				crtS = i;
				initxS = x;
				inityS = y;
				mouseS = 1;
				break;
			}
		}
		clearmouseclick(WM_MBUTTONDOWN);
		return;
	}

	if (!mouseS) return;

	int a = x - initxS;
	int b = y - inityS;
	double lg = b;
	object[crtS]->scale = { object[crtS]->scale.x + lg , object[crtS]->scale.y + lg , object[crtS]->scale.z + lg };
	initxS = x; inityS = y;
}

void getMouseInputPos(Object* object[], int n)
{
	static int dx, dy;
	static bool mouseP = 0;

	if (ismouseclick(WM_LBUTTONUP))
	{
		mouseP = 0;
		clearmouseclick(WM_LBUTTONUP);
		clearmouseclick(WM_LBUTTONDOWN);
		selectedVertice = -1;
		return;
	}

	double x = mousex(), y = mousey();
	if (ismouseclick(WM_LBUTTONDOWN))
	{
		if (selectedVertice != -1)
		{
			selectedObject->vertices[selectedVertice].x = (x - selectedObject->position.x) / selectedObject->scale.x;
			selectedObject->vertices[selectedVertice].y = (y - selectedObject->position.y) / selectedObject->scale.y;
			return;
		}

		for (int i = 0; i < n; ++i)
		{
			Vector2 first = object[i]->hitBox[0];
			Vector2 second = object[i]->hitBox[1];
			if (x >= first.x && x <= second.x && y >= first.y && y <= second.y)
			{
				if (!mouseP)
				{
					mouseP = 1;
					dx = x - object[i]->position.x;
					dy = y - object[i]->position.y;
				}
				object[i]->position.x = x - dx;
				object[i]->position.y = y - dy;
				break;
			}
		}

	}

}

void checkKeyPressed()
{
	bool static pressed = 0;
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		if (pressed) return;
		pressed = 1;
		if (selectedVertice != -1)
			deleteVertex(selectedVertice, selectedObject);
		return;
	}

	pressed = 0;
}