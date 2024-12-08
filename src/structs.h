#pragma once

#include <windows.h>

using namespace std;



struct Vector2
{
	double x, y;
};

struct Vector3
{
	double x, y, z;
};

struct Color
{
	int r, g, b, a;
};

struct Object
{
	Vector3* vertices;
	int** indices; // triangles

	int vertexCount;
	int indexCount;

	Vector2 hitBox[2];

	Vector3 position;
	Vector3 scale;
	Vector3 rotation;

	bool** ad; //matrice de adiacenta a grafului

	Color color;
};





struct Flags
{
	HWND windowHandle = NULL;
	bool windowShouldClose = false;
	bool updateWindow = true;

	int objectCapacity = 16;

	int oldMouseX = -1;
	int oldMouseY = -1;
	bool pressedSpace = false;
	bool pressedLeftClick = false;

	int selectedButton = -1;
	int buttonAnimation;

	char cwd[512]; // folderul in care a fost deschisa aplicatia. cand dai open la un fiser se schimba folderul
};





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
	Button buttons[5];
	int buttonCount = sizeof(buttons)/sizeof(Button);
};