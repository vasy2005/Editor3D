#pragma once

bool IsPressed(int key)
{
	return (GetAsyncKeyState(key) & 0x8000);
}

void CameraControls()
{
	int mouseX = mousex();
	int mouseY = mousey();

	///// ROTATION ///////////////////////////////////////////////////////////////////////////////////
	if (IsPressed(VK_CONTROL) && IsPressed(VK_RBUTTON))
	{
		double rotationSpeed = 1.0 / 1000;

		double difX = (flags->oldMouseX - mouseX) * rotationSpeed;
		double difY = (flags->oldMouseY - mouseY) * rotationSpeed;

		if (difX != 0 || difY != 0)
		{
			camera.rotation = { camera.rotation.x + difY, camera.rotation.y + difX, camera.rotation.z };

			camera.direction = {-cos(camera.rotation.x) * sin(camera.rotation.y),
								 sin(camera.rotation.x),
								-cos(camera.rotation.x) * cos(camera.rotation.y) };

			camera.forward = { 0, 0, -1 };
			camera.up      = { 0, 1,  0 };
			camera.right   = { 1, 0,  0 };

			Rotate(camera.forward, camera.rotation);
			Rotate(camera.up,      camera.rotation);
			Rotate(camera.right,   camera.rotation);

			flags->updateWindow = true;
		}
	}

	///// MOVEMENT ///////////////////////////////////////////////////////////////////////////////////
	Vector3 oldCameraPos = camera.position;
	double movementSpeed = 50;

	// front and back
	if (IsPressed('W'))
		camera.position = { camera.position.x, camera.position.y, camera.position.z + movementSpeed };

	if (IsPressed('S'))
		camera.position = { camera.position.x, camera.position.y, camera.position.z - movementSpeed };


	// right and left
	if (IsPressed('D'))
		camera.position = { camera.position.x + movementSpeed, camera.position.y, camera.position.z };

	if (IsPressed('A'))
		camera.position = { camera.position.x - movementSpeed, camera.position.y, camera.position.z };


	// up and down
	if (IsPressed(VK_LSHIFT))
		camera.position = { camera.position.x, camera.position.y + movementSpeed, camera.position.z };

	if (IsPressed(VK_SPACE))
		camera.position = { camera.position.x, camera.position.y - movementSpeed, camera.position.z };


	// update
	if (oldCameraPos.x != camera.position.x || oldCameraPos.y != camera.position.y || oldCameraPos.z != camera.position.z)
		flags->updateWindow = true;
}

void CheckButtonPress(Object**& objects, int& objectCount, Flags*& flags, Menu*& menu)
{
	int mouseX = mousex();
	int mouseY = mousey();

	// handle the button click event after the animation plays out
	if (flags->selectedButton != -1)
	{
		if (flags->buttonAnimation == 0)
		{
			Sleep(250);
			menu->buttons[flags->selectedButton].pressed = false;
			flags->updateWindow = true;
		}

		if (flags->buttonAnimation == 2)
		{
			switch (flags->selectedButton)
			{
				case 0: // close
					{
						int flag = MessageBox(flags->windowHandle, "Are you sure you want to exit?", "Warning!", MB_YESNO | MB_ICONEXCLAMATION);

						if (flag == IDYES)
							flags->windowShouldClose = true;
					}
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
				{
					CHOOSECOLOR dialogue = { 0 };
					COLORREF result = WHITE;
					COLORREF custom[16];

					dialogue.lStructSize = sizeof(dialogue);
					dialogue.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
					dialogue.hwndOwner = flags->windowHandle;
					dialogue.rgbResult = result;
					dialogue.lpCustColors = custom;

					int error = ChooseColor(&dialogue);

					if (error != 0)
					{
						if (selectedObject != NULL)
							selectedObject->color = { GetRValue(dialogue.rgbResult), GetGValue(dialogue.rgbResult), GetBValue(dialogue.rgbResult) };
						flags->updateWindow = true;
					}
				}
					break;

				case 5:
					flags->xray = !flags->xray;
					flags->updateWindow = true;
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
				int left = menu->buttons[i].center.x - menu->buttons[i].width / 2;
				int right = menu->buttons[i].center.x + menu->buttons[i].width / 2;
				int top = menu->buttons[i].center.y - menu->buttons[i].height / 2;
				int bottom = menu->buttons[i].center.y + menu->buttons[i].height / 2;

				if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom)
				{
					flags->buttonAnimation = 0;
					flags->selectedButton = i;
					menu->buttons[i].pressed = true;
					flags->updateWindow = true;
					break;
				}
			}

			flags->pressedLeftClick = true;
		}
	}
	else
		flags->pressedLeftClick = false;
}

void ProcessInput(Object**& objects, int& objectCount, Flags*& flags, Menu*& menu)
{
	int mouseX = mousex();
	int mouseY = mousey();


	CameraControls();
	CheckButtonPress(objects, objectCount, flags, menu);


	// close the app by pressing ESC
	if (IsPressed(VK_ESCAPE))
		flags->windowShouldClose = true;


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
	if (IsPressed('C'))
	{
		if (flags->pressedCreate == false)
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
			if (IsPressed('R')) // create a random object C + R
				random = rand() % 2;

			// position relative to camera
			Vector3 position;
			position.x = camera.position.x + (float)(mouseX - WINDOW_WIDTH  / 2);
			position.y = camera.position.y + (float)(mouseY - WINDOW_HEIGHT / 2);
			position.z = camera.position.z + -100;
			ViewMatrix(position);
			Translate(position, camera.position);

			if (IsPressed('1') || random == 0) // create cube C + 1
				objects[objectCount++] = NewCube(position, { 100, 100, 100 }, { 0, 0, 0 }, { 255, 255, 255 });

			else if (IsPressed('2') || random == 1) // create icosahedron C + 2
				objects[objectCount++] = NewIcosahedron(position, { 50, 50, 50 }, { 0, 0, 0 }, { 255, 255, 255 });

			if (oldObjectCount != objectCount) // modified
			{
				if (random != -1)
					RandomizeObjectProperties(objects[objectCount - 1]);

				selectedObject = objects[objectCount - 1]; // auto select newly created object
				flags->pressedCreate = true;
				flags->updateWindow  = true;
			}
		}
	}
	else
		flags->pressedCreate = false;


	if (flags->oldMouseX != mouseX || flags->oldMouseY != mouseY)
	{
		int prevSelected = selectedVertice;
		selectedVertice = -1;
		for (int i = 0; i < objectCount; ++i)
		{
			for (int j = 0; j < objects[i]->vertexCount; ++j)
			{
				Vector3 crtPoint = objects[i]->realVertices[j];
				if (abs(crtPoint.x - mouseX) <= rad && abs(crtPoint.y - mouseY) <= rad)
				{
					//selected point
					selectedVertice = j; selectedObject = objects[i];
					flags->updateWindow = 1;
				}
			}
		}
		if (prevSelected != selectedVertice)
			flags->updateWindow = 1;
	}

	if (IsPressed(VK_CONTROL) && IsPressed('T'))
	{
		flags->showRedDot = !flags->showRedDot;
		flags->updateWindow = 1;
	}


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
		flags->updateWindow = 1;
		return;
	}

	if (!mouse) return;
	else
		flags->updateWindow = 1;


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
		flags->updateWindow = 1;
		return;
	}

	if (!mouseS) return;
	else
		flags->updateWindow = 1;

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
		flags->updateWindow = 1;
		if (selectedVertice != -1)
		{
			selectedObject->vertices[selectedVertice].x = (x - 1920/2 - 5 - selectedObject->position.x) / selectedObject->scale.x;
			selectedObject->vertices[selectedVertice].y = (y - 1080/2 - 5 - selectedObject->position.y) / selectedObject->scale.y;
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

	if (mouseP)
		flags->updateWindow = 1;

}

void checkKeyPressed()
{
	bool static pressed = 0;
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		if (pressed) return;
		pressed = 1;
		if (selectedVertice != -1)
		{
			deleteVertex(selectedVertice, selectedObject);
			flags->updateWindow = 1;
		}
		return;
	}

	pressed = 0;
}