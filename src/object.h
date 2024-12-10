#pragma once

#include "constants.h"
#include "structs.h"
#include "vector_math.h"
#include "globals.h"

using namespace std;



void NewScene(Object**& objects, int& objectCount, Flags*& flags)
{
	int flag = MessageBox(flags->windowHandle, "Unsaved changes will be lost. Proceed?", "Warning!", MB_YESNO | MB_ICONEXCLAMATION);

	if (flag == IDYES)
	{
		flags->objectCapacity = 16;
		delete[] objects;
		objects = new Object * [16];
		objectCount = 0;

		flags->updateWindow = true;
	}

	selectedObject = NULL;
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

	for (int i = 0; i < neighborsCounter - 1; ++i)
		for (int j = i + 1; j < neighborsCounter; ++j)
			if (angle[i] > angle[j])
			{
				swap(angle[i], angle[j]);
				swap(neighbors[i], neighbors[j]);
			}

	// Add new triangles
	for (int i = 1; i < neighborsCounter - 1; ++i)
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

void RandomizeObjectProperties(Object* object)
{
	float scale = (float)(rand() % 90 + 10);

	object->scale = { scale, scale, scale };
	object->rotation = { PI / 180 * (rand() % 360), PI / 180 * (rand() % 360), PI / 180 * (rand() % 360) };
	object->color = { rand() % 256, rand() % 256, rand() % 256 };
}



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
	object->realVertices = new Vector3[8];
	memcpy(object->realVertices, object->vertices, sizeof(Vector3) * object->vertexCount);

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
	object->scale = scale;
	object->rotation = rotation;
	object->color = color;

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
	object->realVertices = new Vector3[12];
	memcpy(object->realVertices, object->vertices, sizeof(Vector3) * object->vertexCount);

	object->indexCount = 20;
	const int cnt = object->indexCount;
	int aux[20][3] = { {  0,  4,  5 },
		               {  0,  5,  9 },
		               {  0,  9,  2 },
		               {  0,  2,  8 },
		               {  0,  8,  4 },
		               	 
		               {  5,  4,  1 },
		               { 11,  5,  1 },
		               {  5, 11,  9 },
		               {  7,  9, 11 },
		               {  9,  7,  2 },
		               {  7,  6,  2 },
		               {  2,  6,  8 },
		               { 10,  8,  6 },
		               {  4,  8, 10 },
		               {  1,  4, 10 },
		               	 
		               {  3,  1, 10 },
		               {  3, 11,  1 },
		               {  3,  7, 11 },
		               {  3,  6,  7 },
		               {  3, 10,  6 } };
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
	object->scale = scale;
	object->rotation = rotation;
	object->color = color;

	return object;
}

/*
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
}*/