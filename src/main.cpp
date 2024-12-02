#include <graphics.h>

#include <iostream>
#include <cmath>
#include <random>
#include <time.h>
#include <windows.h>

using namespace std;



///// CONSTANTS ///////////////////////////////////////////////////////////////////////////////////
#define windowWidth 1152
#define windowHeight 648

#define PI  3.141592f
#define PHI 1.618033f
#define INF 1000000005
#define EPS 0.01
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
	int**     indices; // triangles

	int vertexCount;
	int indexCount;

	Vector2 hitBox[2];

	Vector3 position;
	Vector3 scale;
	Vector3 rotation;

	bool** ad; //matrice de adiacenta a grafului

	Color color;
};
///////////////////////////////////////////////////////////////////////////////////////////////////

int selectedVertice = -1; Object* selectedObject; //select point to edit

Object* NewCube(Vector3 position, Vector3 scale, Vector3 rotation, Color color);
Object* NewIcosahedron(Vector3 position, Vector3 scale, Vector3 rotation, Color color);
Object* NewNObject(int n, Vector3 position, Vector3 scale, Vector3 rotation, Color color);
void Translate(Vector3& vector, Vector3 offset);
void Scale(Vector3& vector, Vector3 scale);
void Transform(double transform_mat[3][3], Vector3& vector);
void Rotate(Vector3& vector, Vector3 rotation);
Vector3 SurfaceNormal(Vector3 first, Vector3 second, Vector3 third);
Vector3 cross(Vector3, Vector3);
double dot(Vector3, Vector3);
void DrawLine(Vector3 first, Vector3 second);
void DrawTriangle(int[3], Object*, Object* orig);
void DrawObject(Object* object);
void drawHitBox(Object* object);
void getMouseInputRot(Object* object[], int n);
void getMouseInputScale(Object* object[], int n);
void getMouseInputPos(Object* object[], int n);
void checkKeyPressed();
void deleteVertex(int, Object*);

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

		//selectedVertice = -1;
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
		checkKeyPressed();
		selectedVertice = -1;


		buffer = 1 - buffer;                                                                        // double buffering

		//Sleep(16); // wait 16 milliseconds between frames. should make the app run at around 60fps
	}

	delete object[0];
	delete object[1];
	closegraph();
	getch();
	return 0;
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

	object->indexCount = 12;
	const int cnt = object->indexCount;
	int aux[12][3] = { {0, 4, 7},
		{0, 7, 3},

		{2, 1, 0},
		{3, 2, 0},

		{0, 1, 5},
		{0, 5, 4},

		{4, 5, 6},
		{4, 6, 7},

		{6, 2, 3},
		{7, 6, 3},

		{6, 5, 1},
		{2, 6, 1}
	};
	object->indices = new int* [cnt]; //create indices
	object->ad = new bool* [object->vertexCount]; //create graph
	for (int i = 0; i < cnt; ++i)
	{
		object->indices[i] = new int[3];
		for (int j = 0; j < 3; ++j)
			object->indices[i][j] = aux[i][j];
	}

	for (int i = 0; i < object->vertexCount; ++i)
	{
		object->ad[i] = new bool[object->vertexCount];
		for (int j = 0; j < object->vertexCount; ++j)
			object->ad[i][j] = 0;
	}

	for (int i = 0; i < cnt; ++i)
	{
		int a = object->indices[i][0];
		int b = object->indices[i][1];
		int c = object->indices[i][2];

		object->ad[a][b] = object->ad[b][a] = 1;
		object->ad[a][c] = object->ad[c][a] = 1;
		object->ad[b][c] = object->ad[c][b] = 1;
	}

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
	object->indexCount = 20;
	const int cnt = object->indexCount;
	int aux[20][3] = {{0, 4, 5},
		{ 0,  5,  9 },
		{ 0,  9,  2 },
		{ 0,  2,  8 },
		{0, 8, 4},

		{ 5,  4,  1 },
		{ 11,  5,  1 },
		{5, 11,  9},
		{7,  9, 11},
		{9,  7,  2},
		{7,  6,  2},
		{2,  6,  8},
		{10,  8,  6},
		{4,  8, 10},
		{1,  4, 10},

		{3,  1, 10},
		{3, 11,  1},
		{3,  7, 11},
		{3,  6,  7},
		{3, 10,  6}
	};
	object->indices = new int* [100]; //create indices
	object->ad = new bool* [object->vertexCount]; //create graph
	for (int i = 0; i < cnt; ++i)
	{
		object->indices[i] = new int[3];
		for (int j = 0; j < 3; ++j)
			object->indices[i][j] = aux[i][j];
	}

	for (int i = 0; i < object->vertexCount; ++i)
	{
		object->ad[i] = new bool[object->vertexCount];
		for (int j = 0; j < object->vertexCount; ++j)
			object->ad[i][j] = 0;
	}

	for (int i = 0; i < cnt; ++i)
	{
		int a = object->indices[i][0];
		int b = object->indices[i][1];
		int c = object->indices[i][2];

		object->ad[a][b] = object->ad[b][a] = 1;
		object->ad[a][c] = object->ad[c][a] = 1;
		object->ad[b][c] = object->ad[c][b] = 1;
	}

	object->position = position;
	object->scale    = scale;
	object->rotation = rotation;
	object->color    = color;

	return object;
}

double r = 2;
/*Object* NewNObject(int n, Vector3 position, Vector3 scale, Vector3 rotation, Color color)
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
}*/

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

Vector3 cross(Vector3 A, Vector3 B)
{
	return { A.y * B.z - A.z * B.y, -A.x * B.z + A.z * B.x, A.x * B.y - A.y * B.x };
}

double dot(Vector3 A, Vector3 B)
{
	return A.x * B.x + A.y * B.y + A.z * B.z;
}
///////////////////////////////////////////////////////////////////////////////////////////////////





///// DRAW FUNCTIONS //////////////////////////////////////////////////////////////////////////////
void DrawLine(Vector3 first, Vector3 second)
{
	line(first.x, first.y, second.x, second.y);
}

void DrawTriangle(int indice[3], Object* object, Object* orig)
{
	Vector3* first = &object->vertices[indice[0]];
	Vector3* second = &object->vertices[indice[1]];
	Vector3* third = &object->vertices[indice[2]];

	Vector3 normal = SurfaceNormal(*first, *second, *third);
	Normalize(normal);

	// backface culling. don't draw triangles the camera can't see. increases performance.
	if (normal.z < 0) // TODO: works for now because camera faces negative z axis. will have to check which dir the camera is facing once it can move around.
		return;
		

	double average = 255 * (normal.x + normal.y + normal.z) / 3;

	int r = object->color.r - average; if (r < 0) r = 0; if (r > 255) r = 255;
	int g = object->color.g - average; if (g < 0) g = 0; if (g > 255) g = 255;
	int b = object->color.b - average; if (b < 0) b = 0; if (b > 255) b = 255;

	int final_color = RGB(r, g, b);
	setcolor(final_color);

	DrawLine(*first, *second);
	DrawLine(*second, *third);
	DrawLine(*third, *first);


	Vector3 center = { (first->x + second->x + third->x)/3, (first->y + second->y + third->y)/3, 0}; // z might matter later?
	//final_color += rand() % 100;
	setfillstyle(SOLID_FILL, final_color);
	floodfill(center.x, center.y, final_color);
	//int aux[6] = {first->x, first->y, second->x, second->y, third->x, third->y};
	//fillpoly(3, aux);

	//Vertex select
	setcolor(RED);
	setfillstyle(SOLID_FILL, RED);

	Vector3 points[] = { *first, *second, *third };
	
	for (int i = 0; i < 3; ++i)
	{
		double rad = 6;
		if (abs(points[i].x - mousex()) <= rad && abs(points[i].y - mousey()) <= rad)
		{
			//selected point
			rad = 10;
			selectedVertice = indice[i]; selectedObject = orig;
		}

	//Object HitBox
	for (int i = 0; i < 3; ++i)
	{
		Vector3* crt = &object->vertices[indice[i]];
		object->hitBox[0].x = min(object->hitBox[0].x, crt->x);
		object->hitBox[0].y = min(object->hitBox[0].y, crt->y);
		object->hitBox[1].x = max(object->hitBox[1].x, crt->x);
		object->hitBox[1].y = max(object->hitBox[1].y, crt->y);
	}
		circle(points[i].x, points[i].y, rad);
		floodfill(points[i].x, points[i].y, RED);
	}
}

void DrawObject(Object* object)
{
	//copy object
	Object *crt = new Object;
	crt->vertexCount = object->vertexCount;
	crt->indexCount = object->indexCount;
	crt->position = object->position; crt->rotation = object->rotation;
	crt->scale = object->scale; crt->color = object->color;
	crt->vertices = new Vector3[object->vertexCount];
	memcpy(crt->vertices, object->vertices, sizeof(Vector3) * object->vertexCount);

	crt->indices = new int* [object->indexCount];
	for (int i = 0; i < object->indexCount; ++i)
	{
		crt->indices[i] = new int[3];
		memcpy(crt->indices[i], object->indices[i], sizeof(int[3]));
	}

	crt->hitBox[0] = { INF, INF };
	crt->hitBox[1] = { -1, -1 };

	//object->rotation.x = object->rotation.x - floor(object->rotation.x);
	//object->rotation.y = object->rotation.y - floor(object->rotation.y);
	//object->rotation.z = object->rotation.z - floor(object->rotation.z);

	//transform object
	for (int i = 0; i < object->vertexCount; i++)
	{
		Rotate(object->vertices[i], object->rotation);
		Rotate(crt->vertices[i], crt->rotation);
		Scale(crt->vertices[i], crt->scale);
		Translate(crt->vertices[i], crt->position);
		//Rotate(copy_vertices[i], object->rotation);
		//Scale(copy_vertices[i], object->scale);
		//Translate(copy_vertices[i], object->position);
		

		// move (0, 0) to center of screen
		//Translate(object->vertices[i], {windowWidth/2, windowHeight/2, 0});
	}

	for (int i = 0; i < crt->indexCount; i ++)
		DrawTriangle(crt->indices[i], crt, object);

	object->hitBox[0] = crt->hitBox[0];
	object->hitBox[1] = crt->hitBox[1];

	for (int i = 0; i < crt->indexCount; i++)
		delete[] crt->indices[i];
	delete[] crt->vertices;
	delete crt;

	object->rotation = { 0, 0, 0 };
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
	object[crtS]->scale = {object[crtS]->scale.x + lg , object[crtS]->scale.y + lg , object[crtS]->scale.z + lg};
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
	if (GetKeyState('Q') & 0x8000)
	{
		if (pressed) return;
		pressed = 1;
		if (selectedVertice != -1)
			deleteVertex(selectedVertice, selectedObject);
		return;
	}
	
	pressed = 0;
}

void deleteVertex(int vertex, Object* object)
{
	// Remove triangles containing the vertex
	for (int i = 0; i < object->indexCount; ++i)
	{
		int a = object->indices[i][0];
		int b = object->indices[i][1];
		int c = object->indices[i][2];
		if (a == vertex || b == vertex || c == vertex) 
		{
			for (int j = i; j < object->indexCount - 1; ++j)
			{
				object->indices[j][0] = object->indices[j + 1][0];
				object->indices[j][1] = object->indices[j + 1][1];
				object->indices[j][2] = object->indices[j + 1][2];
			}
			object->indexCount--;
			i--; 
		}
	}

	// Collect neighbors
	int neighborsCounter = 0;;
	for (int i = 0; i < object->vertexCount; ++i)
		if (object->ad[vertex][i])
			neighborsCounter++;
	int* neighbors = new int[neighborsCounter];
	for (int i = 0, j = 0; i < object->vertexCount; ++i)
		if (object->ad[vertex][i])
			neighbors[j++] = i;

	// Compute centroid
	Vector3 centroid = { 0, 0, 0 };
	for (int i = 0; i < neighborsCounter; ++i)
	{
		centroid.x += object->vertices[neighbors[i]].x;
		centroid.y += object->vertices[neighbors[i]].y;
		centroid.z += object->vertices[neighbors[i]].z;
	}
	centroid.x /= neighborsCounter;
	centroid.y /= neighborsCounter;
	centroid.z /= neighborsCounter;

	// Sort neighbors counterclockwise
	//calculate the orthogonal frame
	Vector3 normal = SurfaceNormal(object->vertices[neighbors[0]], object->vertices[neighbors[1]], object->vertices[neighbors[2]]);
	Normalize(normal);

	Vector3 u = (fabs(normal.x) > 0.9) ? Vector3{ 0, 1, 0 } : Vector3{ 1, 0, 0 };
	u = cross(normal, u); Normalize(u);
	Vector3 v = cross(normal, u);

	double* angle = new double[neighborsCounter];

	for (int i = 0; i < neighborsCounter; ++i)
	{
		Vector3 v = object->vertices[neighbors[i]];
		Vector3 r = { v.x - centroid.x, v.y - centroid.y, v.z - centroid.z };

		angle[i] = atan2(r.y, r.x);
	}

	for (int i = 0; i < neighborsCounter-1; ++i)
		for (int j = i+1; j < neighborsCounter; ++j)
				if (angle[i] > angle[j])
				{
					swap(angle[i], angle[j]);
					swap(neighbors[i], neighbors[j]);
				}

	// Add new triangles
	for (int i = 1; i < neighborsCounter-1; ++i) 
	{
		object->indices[object->indexCount][0] = neighbors[0];
		object->indices[object->indexCount][1] = neighbors[i];
		object->indices[object->indexCount][2] = neighbors[i + 1];
		object->indexCount++;
	}

	// Shift vertices array
	for (int i = vertex; i < object->vertexCount - 1; ++i) {
		object->vertices[i] = object->vertices[i + 1];
	}
	object->vertexCount--;

	// Update indices to reflect vertex shift
	for (int i = 0; i < object->indexCount; ++i)
		for (int j = 0; j < 3; ++j)
			if (object->indices[i][j] > vertex) 
				object->indices[i][j]--;
	
	//Update ad matrix
	for (int i = 0; i < object->vertexCount; ++i)
		for (int j = 0; j < object->vertexCount; ++j)
			object->ad[i][j] = 0;

	for (int i = 0; i < object->indexCount; ++i)
	{
		int a = object->indices[i][0];
		int b = object->indices[i][1];
		int c = object->indices[i][2];

		object->ad[a][b] = object->ad[b][a] = 1;
		object->ad[a][c] = object->ad[c][a] = 1;
		object->ad[b][c] = object->ad[c][b] = 1;
	}

	delete[] neighbors;
	delete[] angle;
}






