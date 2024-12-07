#pragma once

#include <graphics.h>
#include "constants.h"
#include "structs.h"
#include "vector_math.h"
#include "globals.h"

using namespace std;



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


	Vector3 center = { (first->x + second->x + third->x) / 3, (first->y + second->y + third->y) / 3, 0 }; // z might matter later?
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
	Object* crt = new Object;
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

	for (int i = 0; i < crt->indexCount; i++)
		DrawTriangle(crt->indices[i], crt, object);

	object->hitBox[0] = crt->hitBox[0];
	object->hitBox[1] = crt->hitBox[1];

	for (int i = 0; i < crt->indexCount; i++)
		delete[] crt->indices[i];
	delete[] crt->vertices;
	delete crt;

	object->rotation = { 0, 0, 0 };
}

void drawHitBox(Object* object)
{
	Vector2 first = object->hitBox[0];
	Vector2 second = object->hitBox[1];

	setcolor(WHITE);
	rectangle(first.x, first.y, second.x, second.y);
}