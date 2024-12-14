#pragma once

struct Button
{
	Vector2 center;
	int width, height;

	char text[256] = "";
	char imagePath[256] = "";
	char pressedImagePath[256] = "";

	int textColor   = RGB(184, 211, 255);
	int backColor   = RGB(47, 99, 185);
	int shadowColor = RGB(23, 46, 164);

	bool pressed = false;
};

struct Menu
{
	Button buttons[6];
	int buttonCount = sizeof(buttons) / sizeof(Button);
};

Menu* NewMenu(Flags* flags)
{
	Menu* menu = new Menu;

	// close
	menu->buttons[0].center = {1875, 35};
	menu->buttons[0].width  = 50;
	menu->buttons[0].height = 50;
	char path[512]; strcpy(path, flags->workingDir); strcat(path, "\\icons\\close.bmp");
	strcpy(menu->buttons[0].imagePath, path);
	strcpy(path, flags->workingDir); strcat(path, "\\icons\\closePressed.bmp");
	strcpy(menu->buttons[0].pressedImagePath, path);

	// new
	menu->buttons[1].center = {100, 35};
	menu->buttons[1].width  = 150;
	menu->buttons[1].height =  50;
	strcpy(menu->buttons[1].text, "NEW");

	// save
	menu->buttons[2].center = {275, 35};
	menu->buttons[2].width  = 150;
	menu->buttons[2].height =  50;
	strcpy(menu->buttons[2].text, "SAVE");

	// load
	menu->buttons[3].center = {450, 35};
	menu->buttons[3].width  = 150;
	menu->buttons[3].height =  50;
	strcpy(menu->buttons[3].text, "OPEN");

	// change color
	menu->buttons[4].center = { 1050, 35 };
	menu->buttons[4].width = 150;
	menu->buttons[4].height = 50;
	strcpy(menu->buttons[4].text, "COLOR");

	// x-ray
	menu->buttons[5].center = { 1300, 35 };
	menu->buttons[5].width = 150;
	menu->buttons[5].height = 50;
	strcpy(menu->buttons[5].text, "X-RAY");

	return menu;
}

void DrawButton(Button& button)
{
	int shadowOffset = 5;
	int buttonOffset = 0;

	if (button.pressed == true)
		buttonOffset = 5;

	// shadow
	setcolor(button.shadowColor);
	setfillstyle(SOLID_FILL, button.shadowColor);

	rectangle(button.center.x - button.width/2 + shadowOffset, button.center.y - button.height/2 + shadowOffset,
		      button.center.x + button.width/2 + shadowOffset, button.center.y + button.height/2 + shadowOffset);
	floodfill(button.center.x, button.center.y, button.shadowColor);

	// background
	if (button.pressed == false)
	{
		setcolor(button.backColor);
		setfillstyle(SOLID_FILL, button.backColor);

		rectangle(buttonOffset + button.center.x - button.width / 2, buttonOffset + button.center.y - button.height / 2,
				  buttonOffset + button.center.x + button.width / 2, buttonOffset + button.center.y + button.height / 2);
		floodfill(button.center.x, button.center.y, button.backColor);
	}

	// text
	if(button.pressed == false)
		setbkcolor(button.backColor);
	else
		setbkcolor(button.shadowColor);

	setcolor(button.textColor);
	outtextxy(buttonOffset + button.center.x - 6 * strlen(button.text), buttonOffset + button.center.y - 10, button.text); // kinda centers text?
	setbkcolor(RGB(0, 0, 0));

	// image
	if (strcmp(button.imagePath, "") != 0)
	{
		if(button.pressed == false)
			readimagefile(button.imagePath, buttonOffset + button.center.x - button.width/2, buttonOffset + button.center.y - button.height/2,
											buttonOffset + button.center.x + button.width/2, buttonOffset + button.center.y + button.height/2);
		else
			readimagefile(button.pressedImagePath, buttonOffset + button.center.x - button.width / 2, buttonOffset + button.center.y - button.height / 2,
		                                   		   buttonOffset + button.center.x + button.width / 2, buttonOffset + button.center.y + button.height / 2);
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
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	// buttons
	for (int i = 0; i < menu->buttonCount; i++)
		DrawButton(menu->buttons[i]);
	DrawAxis();
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

	quad->scale = { 100, 100, 100 };
	quad->rotation = { 0, 0, 0 };
	quad->color = { 255, 255, 255 };

	int count = 10;

	double center = count * 100 / 2;

	for(int z = 0; z < count; z++)
		for (int x = 0; x < count; x++)
		{
			quad->position = { (double)x * 100 - center, 0, -(double)z * 100 + center };

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

				if (quad->realVertices[i].z < -1200)
				{
					cull = true;
					break;
				}

				Translate(quad->realVertices[i], { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0 });
			}

			if (cull)
				break;

			setcolor(RGB(255, 255, 255));
			DrawLine(quad->realVertices[0], quad->realVertices[1]);
			DrawLine(quad->realVertices[1], quad->realVertices[3]);
			DrawLine(quad->realVertices[3], quad->realVertices[2]);
			DrawLine(quad->realVertices[2], quad->realVertices[0]);
		}

	delete quad;
}