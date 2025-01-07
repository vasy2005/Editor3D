#pragma once

struct Button
{
	Vector2 center;
	int width  = 50;
	int height = 50;

	char text[256] = "";
	char imagePath[256] = "";
	char pressedImagePath[256] = "";

	int textColor   = RGB(184, 211, 255);
	int backColor   = RGB(47, 99, 185);
	int shadowColor = RGB(23, 46, 164);

	bool pressed  = false;
	bool disabled = false;
};

struct Menu
{
	Button buttons[12];
	int buttonCount = sizeof(buttons) / sizeof(Button);
};

void LoadIconsInMemory(Menu* menu)
{
	// readimage e prea ineficient pt ca tre sa ia de fiecare data imaginile butoanelor de pe disc. injumatateste frameratul
	// le citesc o singura data si le pastrez in RAM/ memoria programului si folosesc functia putimage() ca sa fie mai rapid

	int w, h;
	for (int i = 0; i < 12; i++)
	{
		icons[i] = NULL;
		icons_press[i] = NULL;

		int width  = menu->buttons[i].width;
		int height = menu->buttons[i].height;

		int x = menu->buttons[i].center.x - width /2;
		int y = menu->buttons[i].center.y - height/2;

		readimagefile(menu->buttons[i].imagePath, 0, 0, width - 1, height - 1);
		icons[i] = new unsigned char[24 + width * height * 4];
		getimage(0, 0, width - 1, height - 1, icons[i]);

		readimagefile(menu->buttons[i].pressedImagePath, 0, 0, width - 1, height - 1);
		icons_press[i] = new unsigned char[24 + width * height * 4];
		getimage(0, 0, width - 1, height - 1, icons_press[i]);
	}
}

void FreeIconsInMemory()
{
	for (int i = 0; i < 12; i++)
	{
		delete[] icons[i];
		delete[] icons_press[i];
	}
}

Menu* NewMenu(Flags* flags)
{
	Menu* menu = new Menu;

	// close
	menu->buttons[0].center = {1875, 35};
	menu->buttons[0].backColor   = RGB(244, 54, 54);
	menu->buttons[0].shadowColor = RGB(206, 30, 30);
	char path[512]; strcpy(path, flags->workingDir); strcat(path, "\\icons\\close.bmp");
	strcpy(menu->buttons[0].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\close_press.bmp");
	strcpy(menu->buttons[0].pressedImagePath, path);

	// new
	menu->buttons[1].center = {50, 35};
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\new.bmp");
	strcpy(menu->buttons[1].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\new_press.bmp");
	strcpy(menu->buttons[1].pressedImagePath, path);

	// save
	menu->buttons[2].center = {125, 35};
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\save.bmp");
	strcpy(menu->buttons[2].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\save_press.bmp");
	strcpy(menu->buttons[2].pressedImagePath, path);

	// load
	menu->buttons[3].center = {200, 35};
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\load.bmp");
	strcpy(menu->buttons[3].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\load_press.bmp");
	strcpy(menu->buttons[3].pressedImagePath, path);

	// change color
	menu->buttons[4].center = { 630, 35 };
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\color.bmp");
	strcpy(menu->buttons[4].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\color_press.bmp");
	strcpy(menu->buttons[4].pressedImagePath, path);
	menu->buttons[4].disabled = true;

	// x-ray
	menu->buttons[5].center = { 780, 35 };
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\xray.bmp");
	strcpy(menu->buttons[5].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\xray_press.bmp");
	strcpy(menu->buttons[5].pressedImagePath, path);

	// texture
	menu->buttons[6].center = { 705, 35 };
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\texture.bmp");
	strcpy(menu->buttons[6].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\texture_press.bmp");
	strcpy(menu->buttons[6].pressedImagePath, path);
	menu->buttons[6].disabled = true;

	// copy
	menu->buttons[7].center = { 340, 35 };
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\copy.bmp");
	strcpy(menu->buttons[7].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\copy_press.bmp");
	strcpy(menu->buttons[7].pressedImagePath, path);
	
	// paste
	menu->buttons[8].center = { 415, 35 };
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\paste.bmp");
	strcpy(menu->buttons[8].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\paste_press.bmp");
	strcpy(menu->buttons[8].pressedImagePath, path);

	// delete
	menu->buttons[9].center = { 490, 35 };
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\delete.bmp");
	strcpy(menu->buttons[9].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\delete_press.bmp");
	strcpy(menu->buttons[9].pressedImagePath, path);

	// cube
	menu->buttons[10].center = { 925, 35 };
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\cube.bmp");
	strcpy(menu->buttons[10].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\cube_press.bmp");
	strcpy(menu->buttons[10].pressedImagePath, path);

	// icosa
	menu->buttons[11].center = { 1000, 35 };
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\icosa.bmp");
	strcpy(menu->buttons[11].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\icosa_press.bmp");
	strcpy(menu->buttons[11].pressedImagePath, path);

	LoadIconsInMemory(menu);

	return menu;
}

void DrawButton(Button& button, int i)
{
	int shadowOffset = 5;
	int buttonOffset = 0;

	if (button.pressed || button.disabled)
		buttonOffset = 5;

	// shadow
	setcolor(button.shadowColor);
	setfillstyle(SOLID_FILL, button.shadowColor);

	rectangle(button.center.x - button.width/2 + shadowOffset, button.center.y - button.height/2 + shadowOffset,
		      button.center.x + button.width/2 + shadowOffset, button.center.y + button.height/2 + shadowOffset);
	floodfill(button.center.x, button.center.y, button.shadowColor);

	// background
	if (button.pressed == false && button.disabled == false)
	{
		setcolor(button.backColor);
		setfillstyle(SOLID_FILL, button.backColor);

		rectangle(buttonOffset + button.center.x - button.width / 2, buttonOffset + button.center.y - button.height / 2,
				  buttonOffset + button.center.x + button.width / 2, buttonOffset + button.center.y + button.height / 2);
		floodfill(button.center.x, button.center.y, button.backColor);
	}

	// text
	if(button.pressed == false && button.disabled == false)
		setbkcolor(button.backColor);
	else
		setbkcolor(button.shadowColor);

	setcolor(button.textColor);
	outtextxy(buttonOffset + button.center.x - 6 * strlen(button.text), buttonOffset + button.center.y - 10, button.text); // kinda centers text?
	setbkcolor(RGB(0, 0, 0));

	// image
	if (strcmp(button.imagePath, "") != 0)
	{
		if (button.pressed == false && button.disabled == false)
			putimage(buttonOffset + button.center.x - button.width / 2, buttonOffset + button.center.y - button.height / 2, icons[i], COPY_PUT);
		else
			putimage(buttonOffset + button.center.x - button.width / 2, buttonOffset + button.center.y - button.height / 2, icons_press[i], COPY_PUT);
	}
}


void DrawAxis()
{
	int posX = 120;
	int posY = WINDOW_HEIGHT - 120;
	int length = 60;

	// back
	setcolor(MENU_SHADOW_COLOR);
	setlinestyle(SOLID_LINE, 0, 5);
	setfillstyle(SOLID_FILL, MENU_BACK_COLOR);
	circle(posX, posY, length * 1.2);
	floodfill(posX, posY, MENU_SHADOW_COLOR);
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	// z axis
	setcolor(RGB(0, 255, 0));
	line(posX, posY, posX + camera.forward.x * length, posY - camera.forward.y * length);

	// x axis
	setcolor(RGB(255, 0, 0));
	line(posX, posY, posX + camera.right.x * length, posY - camera.right.y * length);

	// y axis
	setcolor(RGB(0, 0, 255));
	line(posX, posY, posX + camera.up.x * length, posY - camera.up.y * length);
}

void DrawPlane()
{
	// a quad is just a 2d rectangle/square drawn in 3d space
	Object* quad = new Object;

	quad->vertices = new Vector3[4]{ {-0.5, 0, +0.5},
									 {+0.5, 0, +0.5},
									 {-0.5, 0, -0.5},
									 {+0.5, 0, -0.5} };
	quad->vertexCount = 4;

	quad->indices = new int* [2];
	quad->indices[0] = new int[3] { 0, 2, 1 };
	quad->indices[1] = new int[3] { 3, 1, 2 };

	quad->indexCount = 2;

	quad->realVertices = new Vector3[4];

	double dist = 100;
	//dist *= dist / (up.y / 20);

	quad->scale = { dist, dist, dist };
	quad->rotation = { 0, 0, 0 };
	quad->color = { 255, 255, 255 };

	/*1 .. count*dist/2
	x .. camera.position.x*/
	double count = 5;

	double center = count * dist / 2;

	static int aux = 0;

	for(int z = 0; z < count; z++)
		for (int x = 0; x < count; x++)
		{
			quad->position = { ((double)(x) * dist  - center), 0, -(double)z * dist + center};

			bool cull = false;

			for (int i = 0; i < quad->vertexCount; i++)
			{
				quad->realVertices[i] = quad->vertices[i];

				///// LOCAL COORD TO WORLD COORD /////////////////////////////////////
				Rotate(quad->vertices[i], quad->rotation);
				Rotate(quad->realVertices[i], quad->rotation);
				Scale(quad->realVertices[i], quad->scale);
				Translate(quad->realVertices[i], quad->position);
				//////////////////////////////////////////////////////////

				// WORLD COORD TO CAMEARA RELATIVE COORD
				ViewMatrix(quad->realVertices[i]);

				Perspective(quad->realVertices[i]);

				if (quad->realVertices[i].z < -1100)
				{
					cull = true;
					break;
				}

				Translate(quad->realVertices[i], { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0 });
			}

			if (cull)
				continue;

			setcolor(RGB(127, 127, 127));
			DrawLine(quad->realVertices[0], quad->realVertices[1]);
			DrawLine(quad->realVertices[1], quad->realVertices[3]);
			DrawLine(quad->realVertices[3], quad->realVertices[2]);
			DrawLine(quad->realVertices[2], quad->realVertices[0]);
		}

	delete quad;
}



void DrawTooltip()
{
	int x = mousex() + 20;
	int y = mousey() + 30;

	if (x > WINDOW_WIDTH * 3. / 4)
		x -= 300;

	int width  = 250;
	int height = 90;

	setlinestyle(SOLID_LINE, 0, 5);
	setcolor(RGB(4, 3, 22));
	rectangle(x, y, x + width, y + height);
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	setfillstyle(SOLID_FILL, RGB(14, 19, 52));
	floodfill(x + width/2, y + height/2, RGB(4, 3, 22));

	setbkcolor(RGB(14, 19, 52));

	setcolor(RGB(184, 211, 255));
	settextstyle(GOTHIC_FONT, HORIZ_DIR, 0);
	setusercharsize(5, 6, 5, 3);
	outtextxy(x + 20, y + 10, flags->tooltipTitle);

	setcolor(RGB(42, 52, 102));
	outtextxy(x + 20, y + 45, flags->tooltipDescription);

	setbkcolor(RGB(0, 0, 0));
}

void CheckButtonHover(Menu* menu)
{
	int mouseX = mousex();
	int mouseY = mousey();

	int difX = flags->oldMouseX - mouseX;
	int difY = flags->oldMouseY - mouseY;

	if (difX == 0 && difY == 0)
		return;

	bool found = false;

	for (int i = 0; i < menu->buttonCount; i++)
	{
		int left   = menu->buttons[i].center.x - menu->buttons[i].width  / 2;
		int right  = menu->buttons[i].center.x + menu->buttons[i].width  / 2;
		int top    = menu->buttons[i].center.y - menu->buttons[i].height / 2;
		int bottom = menu->buttons[i].center.y + menu->buttons[i].height / 2;

		if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom)
		{
			found = true;

			switch (i)
			{
				case 0:
				{
					strcpy(flags->tooltipTitle, "Exit");
					strcpy(flags->tooltipDescription, "Keyb. Esc");
				}
				break;

				case 1:
				{
					strcpy(flags->tooltipTitle, "New Scene");
					strcpy(flags->tooltipDescription, "Keyb. Ctrl + N");
				}
				break;

				case 2:
				{
					strcpy(flags->tooltipTitle, "Save to File");
					strcpy(flags->tooltipDescription, "Keyb. Ctrl + S");
				}
				break;

				case 3:
				{
					strcpy(flags->tooltipTitle, "Open File");
					strcpy(flags->tooltipDescription, "Keyb. Ctrl + O");
				}
				break;

				case 4:
				{
					strcpy(flags->tooltipTitle, "Color");
					strcpy(flags->tooltipDescription, "Keyb. None");
				}
				break;

				case 5:
				{
					strcpy(flags->tooltipTitle, "X-Ray");
					strcpy(flags->tooltipDescription, "Keyb. None");
				}
				break;

				case 6:
				{
					strcpy(flags->tooltipTitle, "Texture");
					strcpy(flags->tooltipDescription, "Keyb. None");
				}
				break;

				case 7:
				{
					strcpy(flags->tooltipTitle, "Copy");
					strcpy(flags->tooltipDescription, "Keyb. Ctrl + C");
				}
				break;

				case 8:
				{
					strcpy(flags->tooltipTitle, "Paste");
					strcpy(flags->tooltipDescription, "Keyb. Ctrl + V");
				}
				break;

				case 9:
				{
					strcpy(flags->tooltipTitle, "Delete");
					strcpy(flags->tooltipDescription, "Keyb. Backspace");
				}
				break;

				case 10:
				{
					strcpy(flags->tooltipTitle, "Cube");
					strcpy(flags->tooltipDescription, "Keyb. C + 1");
				}
				break;

				case 11:
				{
					strcpy(flags->tooltipTitle, "Icosahedron");
					strcpy(flags->tooltipDescription, "Keyb. C + 2");
				}
				break;
			}

			flags->drawTooltip  = true;
			flags->updateWindow = true;
		}
	}

	if (found == false && flags->drawTooltip == true)
	{
		flags->drawTooltip  = false;
		flags->updateWindow = true;
	}
}



void DrawFPS()
{
	setcolor(RGB(255, 255, 255));
	settextstyle(GOTHIC_FONT, HORIZ_DIR, 0);
	setusercharsize(1, 2, 1, 2);

	int ms = (clock() - stopwatch) * 1000 / CLOCKS_PER_SEC;
	int fps = 1000 / (ms == 0 ? 1 : ms);
	char msg[512];
	sprintf(msg, "%dfps (%d ms)", fps, ms);

	outtextxy(1790, 1050, msg);
}



struct FloatWindow
{
	// top left corner
	int pivotX = 1200;
	int pivotY =  565;

	int width  = 700;
	int height = 450;

	int titleBarHeight = 45;
};

FloatWindow floatWindow;

void DrawFloatWindow()
{
	int x = floatWindow.pivotX;
	int y = floatWindow.pivotY;

	int width  = floatWindow.width;
	int height = floatWindow.height;

	setlinestyle(SOLID_LINE, 0, 5);
	setcolor(RGB(4, 3, 22));
	rectangle(x, y, x + width, y + height);
	line(x, y + floatWindow.titleBarHeight, x + width, y + floatWindow.titleBarHeight); // title bar delim
	line(x + width - 35, y + 12, x + width - 15, y + 32); // draw x
	line(x + width - 35, y + 32, x + width - 15, y + 12); // draw x

	// input fields
	rectangle(x +  60, y + 130, x + 230, y + 170);
	rectangle(x + 280, y + 130, x + 450, y + 170);
	rectangle(x + 500, y + 130, x + 670, y + 170);

	rectangle(x +  60, y + 250, x + 230, y + 290);
	rectangle(x + 280, y + 250, x + 450, y + 290);
	rectangle(x + 500, y + 250, x + 670, y + 290);

	rectangle(x +  60, y + 370, x + 230, y + 410);
	rectangle(x + 280, y + 370, x + 450, y + 410);
	rectangle(x + 500, y + 370, x + 670, y + 410);

	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	if (x + 6 >= 0) // check if outside the screen
	{
		setfillstyle(SOLID_FILL, RGB(42, 52, 102)); // flood title bar
		floodfill(x + 6, y + 6, RGB(4, 3, 22));
		setfillstyle(SOLID_FILL, RGB(14, 19, 52));
		floodfill(x + 6, y + floatWindow.titleBarHeight + 20, RGB(4, 3, 22)); // flood rest
	}
	else
	{
		setfillstyle(SOLID_FILL, RGB(42, 52, 102)); // flood title bar
		floodfill(x + width - 6, y + 6, RGB(4, 3, 22));
		setfillstyle(SOLID_FILL, RGB(14, 19, 52));
		floodfill(x + width - 6, y + floatWindow.titleBarHeight + 20, RGB(4, 3, 22)); // flood rest
	}

	// input fields
	setfillstyle(SOLID_FILL, RGB(0, 0, 0));
	floodfill(x + 60 + 6, y + 130 + 6, RGB(4, 3, 22));
	floodfill(x + 280 + 6, y + 130 + 6, RGB(4, 3, 22));
	floodfill(x + 500 + 6, y + 130 + 6, RGB(4, 3, 22));
	floodfill(x + 60 + 6, y + 250 + 6, RGB(4, 3, 22));
	floodfill(x + 280 + 6, y + 250 + 6, RGB(4, 3, 22));
	floodfill(x + 500 + 6, y + 250 + 6, RGB(4, 3, 22));
	floodfill(x + 60 + 6, y + 370 + 6, RGB(4, 3, 22));
	floodfill(x + 280 + 6, y + 370 + 6, RGB(4, 3, 22));
	floodfill(x + 500 + 6, y + 370 + 6, RGB(4, 3, 22));

	setbkcolor(RGB(14, 19, 52));

	settextstyle(GOTHIC_FONT, HORIZ_DIR, 0);
	setusercharsize(5, 6, 5, 3);

	setcolor(RGB(184, 211, 255));
	outtextxy(x + 20, y + floatWindow.titleBarHeight +  30, "o Position");
	outtextxy(x + 20, y + floatWindow.titleBarHeight + 150, "o Scale");
	outtextxy(x + 20, y + floatWindow.titleBarHeight + 270, "o Rotation");

	setcolor(RGB(255, 0, 0));
	outtextxy(x + 25, y + 135, "X");
	setcolor(RGB(0, 0, 255));
	outtextxy(x + 247, y + 135, "Y");
	setcolor(RGB(0, 250, 0));
	outtextxy(x + 465, y + 135, "Z");

	setcolor(RGB(255, 0, 0));
	outtextxy(x + 25, y + 255, "X");
	setcolor(RGB(0, 0, 255));
	outtextxy(x + 247, y + 255, "Y");
	setcolor(RGB(0, 250, 0));
	outtextxy(x + 465, y + 255, "Z");

	setcolor(RGB(255, 0, 0));
	outtextxy(x + 25, y + 375, "X");
	setcolor(RGB(0, 0, 255));
	outtextxy(x + 247, y + 375, "Y");
	setcolor(RGB(0, 250, 0));
	outtextxy(x + 465, y + 375, "Z");

	setbkcolor(RGB(0, 0, 0));



	// input fields text
	char msg[512];
	setcolor(RGB(42, 52, 102));

	sprintf(msg, "%.3f", selectedObject->position.x); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 70, y + 135, msg);
	sprintf(msg, "%.3f", (selectedObject->position.y == 0 ? 0 : -selectedObject->position.y)); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 292, y + 135, msg);
	sprintf(msg, "%.3f", selectedObject->position.z); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 510, y + 135, msg);

	sprintf(msg, "%.3f", selectedObject->scale.x); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 70, y + 255, msg);
	sprintf(msg, "%.3f", selectedObject->scale.y); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 292, y + 255, msg);
	sprintf(msg, "%.3f", selectedObject->scale.z); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 510, y + 255, msg);

	sprintf(msg, "%d deg", (int)(selectedObject->realRotation.x * 180/PI)); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 70, y + 375, msg);
	sprintf(msg, "%d deg", (int)(selectedObject->realRotation.y * 180/PI)); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 292, y + 375, msg);
	sprintf(msg, "%d deg", (int)(selectedObject->realRotation.z * 180/PI)); if (strlen(msg) > 10) msg[8] = msg[9] = '.';  msg[10] = NULL;
	outtextxy(x + 510, y + 375, msg);
}

void DrawMenu(Menu* menu)
{
	// background
	int back_color = RGB(14, 19, 52);
	setcolor(back_color);
	rectangle(0, 0, 1920, 75);
	setfillstyle(SOLID_FILL, back_color);
	floodfill(5, 5, back_color);

	int back_highlight = RGB(5, 8, 32);
	setcolor(back_highlight);
	setlinestyle(SOLID_LINE, 0, 5);
	rectangle(0, 79, 1920, 84);

	int delim_color = RGB(42, 52, 102);

	// delimitator intre grupuri de butoane
	setcolor(delim_color);
	setlinestyle(SOLID_LINE, 0, 5);
	rectangle(270, 10, 275, 70);

	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	// delimitator 2
	setcolor(delim_color);
	setlinestyle(SOLID_LINE, 0, 5);
	rectangle(560, 10, 565, 70);

	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	// delimitator 3
	setcolor(delim_color);
	setlinestyle(SOLID_LINE, 0, 5);
	rectangle(850, 10, 855, 70);

	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	// buttons
	for (int i = 0; i < menu->buttonCount; i++)
		DrawButton(menu->buttons[i], i);
	DrawAxis();

	if (flags->drawTooltip == true)
		DrawTooltip();

	if(flags->floatWindowClosed == false)
		DrawFloatWindow();

	// DrawFPS();
	setcolor(RGB(255, 255, 255));
	settextstyle(GOTHIC_FONT, HORIZ_DIR, 0);
	setusercharsize(1, 2, 1, 2);

	outtextxy(1730, 1025, "Vasilache Stefan");
	outtextxy(1730, 1050, "Apetrei Ionut-Teodor");
}