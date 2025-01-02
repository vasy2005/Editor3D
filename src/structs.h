#pragma once

struct Vector2
{
	double x, y;
};

struct Vector3
{
	double x, y, z;
};

struct Vector4
{
	double x, y, z, w;
};

struct Color
{
	int r, g, b, a;
};

struct UV
{
	Vector2 f, s, t;
};

struct Object
{
	Vector3* vertices;
	int** indices; // triangles

	UV* uv;

	Vector3* realVertices;

	int vertexCount;
	int indexCount;

	Vector2 hitBox[2];

	Vector3 position;
	Vector3 scale;
	Vector3 rotation;

	Vector3 realPosition;

	

	bool** ad; //matrice de adiacenta a grafului

	Color color;

	char* texture = NULL;
	int textureW;
	int textureH;
};
Object** objects;





struct Flags
{
	HWND windowHandle = NULL;
	bool windowShouldClose = false;
	bool updateWindow = true;

	int objectCapacity = 16;

	int oldMouseX = -1;
	int oldMouseY = -1;
	bool pressedCreate = false;
	bool pressedLeftClick = false;

	int selectedButton = -1;
	int buttonAnimation;

	bool showRedDot = 1;

	char workingDir[512]; // folderul in care a fost deschisa aplicatia. cand dai open la un fiser se schimba folderul

	bool xray = false;
};

struct Camera
{
	Vector3 position = { 0, 0, +10 };
	Vector3 rotation = { 0, 0,   0 };

	Vector3 forward = { 0, 0, -1 };
	Vector3 up      = { 0, 1,  0 };
	Vector3 right   = { 1, 0,  0 };

	Vector3 direction = { 0, 0, -1 }; // true camera direction relative to the world origin
};