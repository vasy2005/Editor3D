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

	int objectCapacity = 16;

	bool pressedSpace = false;
};