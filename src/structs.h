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

	Vector3* realVertices;

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

	bool showRedDot = 1;

	char cwd[512]; // folderul in care a fost deschisa aplicatia. cand dai open la un fiser se schimba folderul
};
Flags* flags = new Flags;