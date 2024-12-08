#include <graphics.h>
#include <cstring>
#include "structs.h"

using namespace std;

Menu* InitMenu(Flags* flags)
{
	Menu* menu = new Menu;

	// close
	menu->buttons[0].center = {1875, 35};
	menu->buttons[0].width  = 50;
	menu->buttons[0].height = 50;
	char path[512]; strcpy(path, flags->cwd); strcat(path, "\\icons\\close.bmp");
	strcpy(menu->buttons[0].imagePath, path);
	strcpy(path, flags->cwd); strcat(path, "\\icons\\closePressed.bmp");
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
}