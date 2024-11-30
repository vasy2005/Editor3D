#include <graphics.h>

#include <iostream>
#include <cmath>
#include <random>
#include <time.h>

using namespace std;



///// CONSTANTS ///////////////////////////////////////////////////////////////////////////////////
#define windowWidth 1152
#define windowHeight 648

#define PI  3.141592f
#define PHI 1.618033f
#define INF 1000000005
///////////////////////////////////////////////////////////////////////////////////////////////////





///// STRUCTURES //////////////////////////////////////////////////////////////////////////////////
struct Vector3
{
	double x, y, z;
};

struct Vector2
{
	double x, y;
};

struct Color
{
	int r, g, b, a;
};

struct Object
{
	Vector3* vertices;
	int*     indices; // triangles

	int vertexCount;
	int indexCount;

	Vector2 hitBox[2];

	Vector3 position;
	Vector3 scale;
	Vector3 rotation;

	Color color;
};
///////////////////////////////////////////////////////////////////////////////////////////////////





///// OBJECT CREATION FUNCTIONS ///////////////////////////////////////////////////////////////////
Object* NewCube(Vector3 position, Vector3 scale, Vector3 rotation, Color color)
{
	Object* object = new Object;

	object->vertexCount = 8;
	object->vertices = new Vector3[8]{ {-0.5f, -0.5f, +0.5f},
									   {-0.5f, +0.5f, +0.5f},
									   {+0.5f, +0.5f, +0.5f},
									   {+0.5f, -0.5f, +0.5f},

									   {-0.5f, -0.5f, -0.5f},
									   {-0.5f, +0.5f, -0.5f},
									   {+0.5f, +0.5f, -0.5f},
									   {+0.5f, -0.5f, -0.5f} };

	object->indexCount = 36;
	object->indices = new Vector3[36]{ 0, 4, 7},
								   0, 7, 3,
								   
		                           2, 1, 0, 
								   3, 2, 0, 
								   
		                           0, 1, 5, 
								   0, 5, 4, 
								   
		                           4, 5, 6, 
								   4, 6, 7, 
								   
		                           6, 2, 3, 
								   7, 6, 3, 
								   
		                           6, 5, 1, 
								   2, 6, 1 };

	object->position = position;
	object->scale    = scale;
	object->rotation = rotation;
	object->color    = color;

	return object;
}

Object* NewIcosahedron(Vector3 position, Vector3 scale, Vector3 rotation, Color color)
{
	Object* object = new Object;

	object->vertexCount = 12;
	object->vertices = new Vector3[12]{ { 0.0f, -PHI, -1.0f },
									    { 0.0f, +PHI, -1.0f },
									    { 0.0f, -PHI, +1.0f },
									    { 0.0f, +PHI, +1.0f },

									    {-1.0f,  0.0f, -PHI },
									    {+1.0f,  0.0f, -PHI },
									    {-1.0f,  0.0f, +PHI },
									    {+1.0f,  0.0f, +PHI },

									    { -PHI, -1.0f, 0.0f },
									    { +PHI, -1.0f, 0.0f },
									    { -PHI, +1.0f, 0.0f },
									    { +PHI, +1.0f, 0.0f } };

	object->indexCount = 60;
	object->indices = new int[60] { 0,  4,  5,
									0,  5,  9,
									0,  9,  2,
									0,  2,  8,
									0,  8,  4,

									5,  4,  1,
								   11,  5,  1,
									5, 11,  9,
									7,  9, 11,
									9,  7,  2,
									7,  6,  2,
									2,  6,  8,
								   10,  8,  6,
								    4,  8, 10,
									1,  4, 10,

									3,  1, 10,
									3, 11,  1,
									3,  7, 11,
									3,  6,  7,
									3, 10,  6 };

	object->position = position;
	object->scale    = scale;
	object->rotation = rotation;
	object->color    = color;

	return object;
}

double r = 2;
Object* NewNObject(int n, Vector3 position, Vector3 scale, Vector3 rotation, Color color)
{
	Object* object = new Object;

	object->vertexCount = n;
	object->vertices = new Vector3[n];
	for (int i = 0; i < n; ++i)
	{
		Vector3 crt;
		crt.x = double(rand() % int(r*100)) / 100.0;
		crt.y = double(rand() % int(r*100)) / 100.0;
		crt.z = sqrt(r*r - crt.x * crt.x - crt.y * crt.y);
		if (crt.x * crt.x + crt.y * crt.y + crt.z*crt.z != r*r)
		{
			i--;
			continue;
		}

		object->vertices[i] = crt;
		cout << crt.x << ' ' << crt.y << ' ' << crt.z << '\n';
	}

	object->indexCount = 0;
	object->indices = new int[n * n * n * 3];
	for (int i = 0; i < n; ++i)
		for (int j = i+1; j < n; ++j)
			for (int k = j+1; k < n; ++k)
			{
				object->indices[object->indexCount++] = i;
				object->indices[object->indexCount++] = j;
				object->indices[object->indexCount++] = k;
			}

	object->position = position;
	object->scale = scale;
	object->rotation = rotation;
	object->color = color;

	return object;
}

///////////////////////////////////////////////////////////////////////////////////////////////////







///// VECTOR MATH /////////////////////////////////////////////////////////////////////////////////
void Translate(Vector3& vector, Vector3 offset)
{
	vector = { vector.x + offset.x, vector.y + offset.y, vector.z + offset.z };
}

void Scale(Vector3& vector, Vector3 scale)
{
	vector = { vector.x * scale.x, vector.y * scale.y, vector.z * scale.z };
}

void Transform(double transform_mat[3][3], Vector3& vector)
{
	double vector_mat[3][1] = { {vector.x}, {vector.y}, {vector.z} };
	double result_mat[3][1] = { {0}, {0}, {0} };

	// matrix multiplication algorithm
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 1; j++)
			for (int k = 0; k < 3; k++)
				result_mat[i][j] += transform_mat[i][k] * vector_mat[k][j];

	vector = { result_mat[0][0], result_mat[1][0], result_mat[2][0] };
}

void Rotate(Vector3& vector, Vector3 rotation)
{
	double rcos, rsin;

	rcos = cos(rotation.x);
	rsin = sin(rotation.x);
	double rotateX[3][3] = { { 1,  0,      0  },
							{ 0, rcos, -rsin },
							{ 0, rsin,  rcos } };

	rcos = cos(rotation.y);
	rsin = sin(rotation.y);
	double rotateY[3][3] = { {  rcos,  0,  rsin },
							{    0,   1,   0   },
							{ -rsin,  0,  rcos } };

	rcos = cos(rotation.z);
	rsin = sin(rotation.z);
	double rotateZ[3][3] = { { rcos, -rsin, 0 },
							{ rsin,  rcos, 0 },
							{  0,      0,  1 } };

	Transform(rotateX, vector);
	Transform(rotateY, vector);
	Transform(rotateZ, vector);
}

void Normalize(Vector3& vector)
{
	double length = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	vector = { vector.x / length, vector.y / length, vector.z / length };
}

Vector3 SurfaceNormal(Vector3 first, Vector3 second, Vector3 third) // 3d perpendicular on a triangle
{
	Vector3 A = { second.x - first.x, second.y - first.y, second.z - first.z };
	Vector3 B = {  third.x - first.x,  third.y - first.y,  third.z - first.z };

	return { A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x };
}
///////////////////////////////////////////////////////////////////////////////////////////////////





///// DRAW FUNCTIONS //////////////////////////////////////////////////////////////////////////////
void DrawLine(Vector3 first, Vector3 second)
{
	line(first.x, first.y, second.x, second.y);
}

void DrawTriangle(Vector3 first, Vector3 second, Vector3 third, Color color, Object *object)
{
	Vector3 normal = SurfaceNormal(first, second, third);
	Normalize(normal);

	// backface culling. don't draw triangles the camera can't see. increases performance.
	if (normal.z < 0) // TODO: works for now because camera faces negative z axis. will have to check which dir the camera is facing once it can move around.
		return;

	double average = 255 * (normal.x + normal.y + normal.z) / 3;

	int r = color.r - average; if (r < 0) r = 0; if (r > 255) r = 255;
	int g = color.g - average; if (g < 0) g = 0; if (g > 255) g = 255;
	int b = color.b - average; if (b < 0) b = 0; if (b > 255) b = 255;

	int final_color = RGB(r, g, b);
	setcolor(final_color);

	DrawLine(first, second);
	DrawLine(second, third);
	DrawLine(third, first);

	object->hitBox[0].x = min(object->hitBox[0].x, first.x);
	object->hitBox[0].y = min(object->hitBox[0].y, first.y);
	object->hitBox[1].x = max(object->hitBox[1].x, first.x);
	object->hitBox[1].y = max(object->hitBox[1].y, first.y);

	object->hitBox[0].x = min(object->hitBox[0].x, second.x);
	object->hitBox[0].y = min(object->hitBox[0].y, second.y);
	object->hitBox[1].x = max(object->hitBox[1].x, second.x);
	object->hitBox[1].y = max(object->hitBox[1].y, second.y);

	object->hitBox[0].x = min(object->hitBox[0].x, third.x);
	object->hitBox[0].y = min(object->hitBox[0].y, third.y);
	object->hitBox[1].x = max(object->hitBox[1].x, third.x);
	object->hitBox[1].y = max(object->hitBox[1].y, third.y);


	Vector3 center = { (first.x + second.x + third.x)/3, (first.y + second.y + third.y)/3, 0}; // z might matter later?

	setfillstyle(SOLID_FILL, final_color);
	int points[6] = { first.x, first.y, second.x, second.y, third.x, third.y };
	fillpoly(3, points);
}

void DrawObject(Object* object)
{
	Vector3* copy_vertices = new Vector3[object->vertexCount];
	memcpy(copy_vertices, object->vertices, sizeof(Vector3) * object->vertexCount);

	int* copy_indices = new int[object->indexCount];
	memcpy(copy_indices, object->indices, sizeof(int) * object->indexCount);

	object->hitBox[0] = { INF, INF };
	object->hitBox[1] = { -1, -1 };

	//object->rotation.x = object->rotation.x - floor(object->rotation.x);
	//object->rotation.y = object->rotation.y - floor(object->rotation.y);
	//object->rotation.z = object->rotation.z - floor(object->rotation.z);

	for (int i = 0; i < object->vertexCount; i++)
	{
		Rotate(object->vertices[i], object->rotation);
		Rotate(copy_vertices[i], object->rotation);
		Scale(copy_vertices[i], object->scale);
		Translate(copy_vertices[i], object->position);
		

		// move (0, 0) to center of screen
		//Translate(copy_vertices[i], {windowWidth/2, windowHeight/2, 0});
	}

	for (int i = 0; i < object->indexCount; i += 3)
		DrawTriangle(copy_vertices[copy_indices[i]], copy_vertices[copy_indices[i + 1]], copy_vertices[copy_indices[i + 2]], object->color, object);

	object->rotation = { 0, 0, 0 };

	delete[] copy_vertices;
	delete[] copy_indices;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void drawHitBox(Object* object)
{
	Vector2 first = object->hitBox[0];
	Vector2 second = object->hitBox[1];

	setcolor(WHITE);
	rectangle(first.x, first.y, second.x, second.y);
}


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


	object[crt]->rotation = {object[crt]->rotation.x - (y - inity) / 180, object[crt]->rotation.y + (x - initx) / 180, object[crt]->rotation.z};
	initx = x; inity = y;
}

void getMouseInputScale(Object* object[], int n)
{
	static bool mouseS = 0; 
	static double initxS, inityS; 
	static int crtS;

	double x = mousex(), y = mousey();
	if (ismouseclick(WM_MBUTTONUP))
	{
		mouseS = 0;
		clearmouseclick(WM_MBUTTONUP);
		return;
	}

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
	object[crtS]->scale = {object[crtS]->scale.x + lg , object[crtS]->scale.y + lg , object[crtS]->scale.z + lg};
	initxS = x; inityS = y;
}

void getMouseInputPos(Object* object[], int n)
{
	static int dx, dy;
	static bool mouseP = 0;

	double x = mousex(), y = mousey();
	if (ismouseclick(WM_LBUTTONDOWN))
	{
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
	if (ismouseclick(WM_LBUTTONUP))
	{
		mouseP = 0;
		clearmouseclick(WM_LBUTTONDOWN);
		clearmouseclick(WM_LBUTTONUP);
	}
}

// asdjkfakdbfhabdsghabdghbjasjdfbasdf
// comentariu 2
// comentariu 3

int main()
{
	srand(time(NULL));
	initwindow(windowWidth, windowHeight);
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH); // make lines thicker to stop floodfill from flickering

	const int n = 2; Object* object[n];
	object[0] = NewIcosahedron({windowWidth/4, windowWidth/3, 0}, {100, 100, 100}, {0, 0, 0}, {45, 130, 220});
	object[1] = NewCube({ windowWidth / 4*3, windowWidth / 3, 0}, {100, 100, 100}, {PI / 4, PI / 4, 0}, {33, 200,  60});
	//object[1] = NewNObject(4, { windowWidth / 4 * 3, windowWidth / 3, 0 }, { 100, 100, 100 }, { 0,0, 0 }, { 45, 130, 220 });
	//object[1] = New30Object({ windowWidth / 4 * 3, windowWidth / 3, 0 }, { 100, 100, 100 }, { 0,0, 0 }, { 45, 130, 220 });

	int buffer = 0;                                                                                 // double buffering (removes flickering)

	while (1)
	{
		setactivepage(buffer);                                                                      // double buffering
		setvisualpage(1 - buffer);                                                                  // double buffering

		///// DRAW HERE ///////////////////////////////////////
		cleardevice(); // clear previous frame

		for (int i = 0; i < n; ++i)
		{
			DrawObject(object[i]);
			drawHitBox(object[i]);
		}
		
		//object1->rotation = {object1->rotation.x + 0.05f, object1->rotation.y + 0.05f, object1->rotation.z + 0.00f};
		//object2->rotation = {object2->rotation.x + 0.05f, object2->rotation.y + 0.05f, object2->rotation.z + 0.00f};
		///////////////////////////////////////////////////////

		getMouseInputRot(object, n);
		getMouseInputScale(object, n);
		getMouseInputPos(object, n);


		buffer = 1 - buffer;                                                                        // double buffering

		//Sleep(16); // wait 16 milliseconds between frames. should make the app run at around 60fps
	}

	delete object[0];
	delete object[1];
	closegraph();
	getch();
	return 0;
}





//Object* NewSphere(Vector3 position, Vector3 scale, Vector3 rotation, Color color)
//{
//	Object* object = NewIcosahedron({ 0, 0, 0 }, { 1, 1, 1 }, { 0, 0, 0 }, { 255, 255, 255 });
//
//	int oldVertexCount = object->vertexCount;
//	int oldIndexCount = object->indexCount;
//
//	// I SHOULD REALLY ONLY RESIZE ONCE
//	Vector3* temp = new Vector3[object->vertexCount + 42];
//	for (int i = 0; i < oldVertexCount; i++)
//		temp[i] = object->vertices[i];
//	delete[] object->vertices;
//	object->vertices = temp;
//
//	int* temp2 = new int[object->indexCount * 4];
//	for (int i = 0; i < oldIndexCount; i++)
//		temp2[i] = object->indices[i];
//	delete[] object->indices;
//	object->indices = temp2;
//
//	for (int i = 0; i < oldIndexCount; i += 3)
//	{
//		int oldIndexA = object->indices[i];
//		int oldIndexB = object->indices[i + 1];
//		int oldIndexC = object->indices[i + 2];
//
//		Vector3 A = object->vertices[object->indices[i]];
//		Vector3 B = object->vertices[object->indices[i + 1]];
//		Vector3 C = object->vertices[object->indices[i + 2]];
//
//		Vector3 new1 = { (A.x + B.x) / 2, (A.y + B.y) / 2, (A.z + B.z) / 2 };
//		Vector3 new2 = { (B.x + C.x) / 2, (B.y + C.y) / 2, (B.z + C.z) / 2 };
//		Vector3 new3 = { (C.x + A.x) / 2, (C.y + A.y) / 2, (C.z + A.z) / 2 };
//
//		Normalize(A);
//		Normalize(B);
//		Normalize(C);
//
//		Normalize(new1);
//		Normalize(new2);
//		Normalize(new3);
//
//		int newIndex3;
//		int newIndex2;
//		int newIndex1;
//
//		bool found1 = false;
//		bool found2 = false;
//		bool found3 = false;
//		// doubleing point imprecision??
//		for (int j = oldVertexCount; j < object->vertexCount; j++)
//			if (object->vertices[j].x == new1.x && object->vertices[j].y == new1.y && object->vertices[j].z == new1.z)
//			{
//				found1 = true;
//				newIndex1 = j;
//				//break;
//			}
//			else if (object->vertices[j].x == new2.x && object->vertices[j].y == new2.y && object->vertices[j].z == new2.z)
//			{
//				found2 = true;
//				newIndex2 = j;
//				//break;
//			}
//			else if (object->vertices[j].x == new3.x && object->vertices[j].y == new3.y && object->vertices[j].z == new3.z)
//			{
//				found3 = true;
//				newIndex3 = j;
//				//break;
//			}
//
//		if (found1 == false)
//		{
//			object->vertexCount++;
//			object->vertices[object->vertexCount - 1] = new1;
//			newIndex1 = object->vertexCount - 1;
//		}
//
//		if (found2 == false)
//		{
//			object->vertexCount++;
//			object->vertices[object->vertexCount - 1] = new2;
//			newIndex2 = object->vertexCount - 1;
//		}
//
//		if (found3 == false)
//		{
//			object->vertexCount++;
//			object->vertices[object->vertexCount - 1] = new3;
//			newIndex3 = object->vertexCount - 1;
//		}
//
//		//if (found1 == false)
//		//{
//		//	object->vertexCount++;
//		//	object->vertices[object->vertexCount - 1] = new3;
//		//}
//
//		//if (found2 == false)
//		//{
//		//	object->vertexCount++; found the error?
//		//	object->vertices[object->vertexCount - 2] = new2;
//		//}										   ^~~~~~
//
//		//if (found3 == false)
//		//{
//		//	object->vertexCount++;
//		//	object->vertices[object->vertexCount - 3] = new1;
//		//}
//
//		object->indexCount += 9;
//
//		// change old one
//		object->indices[i] = oldIndexA;
//		object->indices[i + 1] = newIndex3;
//		object->indices[i + 2] = newIndex1;
//
//		// new ones
//		object->indices[object->indexCount - 7] = newIndex1;
//		object->indices[object->indexCount - 8] = newIndex3;
//		object->indices[object->indexCount - 9] = newIndex2;
//
//		object->indices[object->indexCount - 6] = newIndex1;
//		object->indices[object->indexCount - 5] = newIndex2;
//		object->indices[object->indexCount - 4] = oldIndexB;
//
//		object->indices[object->indexCount - 3] = newIndex3;
//		object->indices[object->indexCount - 2] = oldIndexC;
//		object->indices[object->indexCount - 1] = newIndex2;
//	}
//
//	object->position = position;
//	object->scale = scale;
//	object->rotation = rotation;
//	object->color = color;
//
//	return object;
//}
