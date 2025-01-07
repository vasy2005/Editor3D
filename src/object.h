#pragma once

void ResetCamera()
{
	camera.position = { 0, 0, 0 };
	camera.rotation = { 0.5, -0.8, 0 };

	camera.direction = { -cos(camera.rotation.x) * sin(camera.rotation.y),
						 sin(camera.rotation.x),
						-cos(camera.rotation.x) * cos(camera.rotation.y) };

	camera.forward = { 0, 0, -1 };
	camera.up = { 0, 1,  0 };
	camera.right = { 1, 0,  0 };

	Rotate(camera.forward, camera.rotation);
	Rotate(camera.up, camera.rotation);
	Rotate(camera.right, camera.rotation);
}

void NewScene(Object**& objects, int& objectCount, Flags*& flags)
{
	int flag = MessageBox(flags->windowHandle, "Unsaved changes will be lost. Proceed?", "Warning!", MB_YESNO | MB_ICONEXCLAMATION);

	if (flag == IDYES)
	{
		flags->objectCapacity = 16;
		delete[] objects;
		objects = new Object * [16];
		objectCount = 0;

		ResetCamera();

		flags->updateWindow = true;
	}

	selectedObject = NULL;
}

void DeleteObject(Object*& object, Object**& objects, int& objectCount)
{
	if (object == NULL)
	{
		MessageBox(flags->windowHandle, "No object selected for deletion.", "Warning!", MB_OK | MB_ICONWARNING);
		return;
	}

	int pos = -1;

	for (int i = 0; i < objectCount; i++)
	{
		if (objects[i] == object)
		{
			delete objects[i];
			pos = i;
		}

		if (pos != -1 && i + 1 < objectCount)
			objects[i] = objects[i + 1];
	}

	objectCount--;

	selectedObject = NULL;
	flags->updateWindow = true;
}

void CopyObject(Object* object)
{
	if(object == NULL)
	{
		MessageBox(flags->windowHandle, "No object selected for copying.", "Warning!", MB_OK | MB_ICONWARNING);
		return;
	}

	if(copiedObject == NULL)
	{
		flags->updateWindow = true; // ca sa apara paste prima data
		copiedObject = new Object;
	}

	*copiedObject = *object;
}

void PasteObject(Object* object)
{
	if (object == NULL)
	{
		MessageBox(flags->windowHandle, "There is no object copied in the clipboard.", "Warning!", MB_OK | MB_ICONWARNING);
		return;
	}

	if (objectCount + 1 > flags->objectCapacity) // resize objects if too many are added
	{
		flags->objectCapacity *= 2;
		Object** temp = new Object * [flags->objectCapacity];
		memcpy(temp, objects, sizeof(Object*) * objectCount);
		delete[] objects;
		objects = temp;
	}

	objectCount++;

	Object* newObj = objects[objectCount - 1] = new Object;

	newObj->vertices = new Vector3[copiedObject->vertexCount];
	memcpy(newObj->vertices, copiedObject->vertices, sizeof(Vector3) * copiedObject->vertexCount);

	newObj->indices = new int* [copiedObject->indexCount];
	for (int i = 0; i < copiedObject->indexCount; i++)
	{
		newObj->indices[i] = new int[3];

		newObj->indices[i][0] = copiedObject->indices[i][0];
		newObj->indices[i][1] = copiedObject->indices[i][1];
		newObj->indices[i][2] = copiedObject->indices[i][2];
	}

	newObj->uv = copiedObject->uv; // not a good idea but it works

	newObj->realVertices = new Vector3[copiedObject->vertexCount];
	memcpy(newObj->realVertices, copiedObject->realVertices, sizeof(Vector3) * copiedObject->vertexCount);

	newObj->vertexCount = copiedObject->vertexCount;
	newObj->indexCount = copiedObject->indexCount;

	newObj->position = copiedObject->position;
	newObj->scale = copiedObject->scale;
	newObj->rotation = copiedObject->rotation;

	newObj->color = copiedObject->color;

	newObj->ad = new bool*[copiedObject->vertexCount];
	for (int i = 0; i < copiedObject->vertexCount; i++)
	{
		newObj->ad[i] = new bool[copiedObject->vertexCount];

		for (int j = 0; j < copiedObject->vertexCount; j++)
			newObj->ad[i][j] = copiedObject->ad[i][j];
	}

	if (copiedObject->texture != NULL)
	{
		newObj->texture = new unsigned char[24 + 4 * copiedObject->textureW * copiedObject->textureH];
		memcpy(newObj->texture, copiedObject->texture, 24 + 4 * copiedObject->textureW * copiedObject->textureH);
	}
	else
		newObj->texture = NULL;

	newObj->textureW = copiedObject->textureW;
	newObj->textureH = copiedObject->textureH;

	strcpy(newObj->texturePath, copiedObject->texturePath);
}


void SortVerticesCounterclockwise(int neighbors[], int neighborsCounter, Object* object, Vector3 centroid)
{
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

	delete[] angle;
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
	SortVerticesCounterclockwise(neighbors, neighborsCounter, object, centroid);

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
}


void RandomizeObjectProperties(Object* object)
{
	float scale = (float)(rand() % 90 + 10);

	object->scale = { scale, scale, scale };
	object->rotation = { PI / 180 * (rand() % 360), PI / 180 * (rand() % 360), PI / 180 * (rand() % 360) };
	object->color = { rand() % 256, rand() % 256, rand() % 256 };
}


void sortObjectsByZ(Object**& objects)
{
	int i, j;

	for (i = 0; i < objectCount; ++i)
		for (j = i + 1; j < objectCount; ++j)
			if (objects[i]->realPosition.z < objects[j]->realPosition.z)
				swap(objects[i], objects[j]);
}


Object* NewCube(Vector3 position, Vector3 scale, Vector3 rotation, Color color)
{
	Object* object = new Object;

	object->vertexCount = 8;
	object->vertices = new Vector3[VERTEXMAX]{ {-0.5f, -0.5f, +0.5f},
											   {-0.5f, +0.5f, +0.5f},
											   {+0.5f, +0.5f, +0.5f},
											   {+0.5f, -0.5f, +0.5f},
											   
											   {-0.5f, -0.5f, -0.5f},
											   {-0.5f, +0.5f, -0.5f},
											   {+0.5f, +0.5f, -0.5f},
											   {+0.5f, -0.5f, -0.5f} };

	object->realVertices = new Vector3[VERTEXMAX];
	memcpy(object->realVertices, object->vertices, sizeof(Vector3) * object->vertexCount);

	object->indexCount = 12;
	const int cnt = object->indexCount;
	int aux[12][3] = { {0, 2, 1}, // front
					   {0, 3, 2},
					   
					   {4, 5, 6}, // back
					   {4, 6, 7},
					   
					   {3, 6, 2}, // right
					   {3, 7, 6},
					   
					   {0, 1, 5}, // left
					   {0, 5, 4},
					   
					   {1, 6, 5}, // top
					   {1, 2, 6},
					   
					   {0, 4, 7}, // bottom
					   {0, 7, 3} };

	object->indices = new int* [INDICEMAX]; //create indices
	object->ad = new bool* [VERTEXMAX]; //create graph
	for (int i = 0; i < cnt; ++i)
	{
		object->indices[i] = new int[3];
		for (int j = 0; j < 3; ++j)
			object->indices[i][j] = aux[i][j];
	}

	for (int i = 0; i < object->vertexCount; ++i)
	{
		object->ad[i] = new bool[VERTEXMAX];
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

	object->uv = new UV[12] { { {0.25, 0.33}, {0.00, 0.33}, {0.00, 0.66} },
				              { {0.25, 0.66}, {0.25, 0.33}, {0.00, 0.66} },
				              
				              { {0.50, 0.33}, {0.75, 0.33}, {0.50, 0.66} },
				              { {0.75, 0.33}, {0.75, 0.66}, {0.50, 0.66} },
				              
				              { {0.50, 0.33}, {0.25, 0.33}, {0.25, 0.66} },
				              { {0.50, 0.66}, {0.50, 0.33}, {0.25, 0.66} },
				              
				              { {0.75, 0.33}, {1.00, 0.33}, {0.75, 0.66} },
				              { {1.00, 0.33}, {1.00, 0.66}, {0.75, 0.66} },
				              
				              { {0.50, 0.00}, {0.25, 0.00}, {0.25, 0.33} },
				              { {0.50, 0.33}, {0.50, 0.00}, {0.25, 0.33} },
				              
				              { {0.25, 0.66}, {0.50, 0.66}, {0.25, 1.00} },
				              { {0.50, 0.66}, {0.50, 1.00}, {0.25, 1.00} } };

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
	object->vertices = new Vector3[VERTEXMAX]{ { 0.0f, -PHI, -1.0f },
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
	object->realVertices = new Vector3[VERTEXMAX];
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
	object->indices = new int* [INDICEMAX]; //create indices
	object->ad = new bool* [VERTEXMAX]; //create graph
	for (int i = 0; i < cnt; ++i)
	{
		object->indices[i] = new int[3];
		for (int j = 0; j < 3; ++j)
			object->indices[i][j] = aux[i][j];
	}

	for (int i = 0; i < object->vertexCount; ++i)
	{
		object->ad[i] = new bool[VERTEXMAX];
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

void HighlightObject()
{
	if (!objectCount) return;

	for (int i = 0; i < INDICEMAX; ++i)
		highlighted[i] = 0;

	int minim = INF;
	Object* object;
		object = selectedObject;
		if (!object)
		{
			addVerticeObject = 0;
			return;
		}

		for (int i = 0; i < object->indexCount; ++i)
		{
			int aux[3];
			aux[0] = object->indices[i][0];
			aux[1] = object->indices[i][1];
			aux[2] = object->indices[i][2];

			Vector3 v1 = object->realVertices[aux[0]];
			Vector3 v2 = object->realVertices[aux[1]];
			Vector3 v3 = object->realVertices[aux[2]];

			Vector3 center = { (v1.x + v2.x + v3.x) / 3.0, (v1.y + v2.y + v3.y) / 3.0, (v1.z + v2.z + v3.z) / 3.0 };

			if (minim > center.z)
			{
				minim = center.z;
				addVerticeObject = object;
				posLen = 0;
				pos[posLen++] = i;
			}
		}
	 
	object = addVerticeObject;

	for (int i = 0; i < object->indexCount; ++i)
	{
		int k;
		for (k = 0; k < posLen; k++)
		{
			Vector3 normal1 = SurfaceNormal(object->vertices[object->indices[i][0]], object->vertices[object->indices[i][1]], object->vertices[object->indices[i][2]]);
			Normalize(normal1);

			Vector3 normal2 = SurfaceNormal(object->vertices[object->indices[pos[0]][0]], object->vertices[object->indices[pos[0]][1]], object->vertices[object->indices[pos[0]][2]]);
			Normalize(normal2);

			//daca normal1 = normal2 atunci cele 2 triunghiuri apartin aceleiasi fete (teoretic? idk merge)

			if (abs(normal1.x-normal2.x) < EPS && abs(normal1.y - normal2.y) < EPS && abs(normal1.z - normal2.z) < EPS)
				break;
		}
		if (k != posLen)
			pos[posLen++] = i;
	}
	
	for (int i = 0; i < posLen; ++i)
		highlighted[pos[i]] = 1;
}

void createVertex()
{
	Object* obj = addVerticeObject;
	

	//delete obj->indices[obj->pos]
	//insert noul vertex (center)
	//insert cele 3 triunghiuri noi
	//actualizeaza ad[][]

	//insert center (new Vertex)

	//vertices[] = vector de varfuri distincte din cele posLen triunghiuri
	int vertices[VERTEXMAX]; int vLen = 0;
	for (int i = 0; i < posLen; ++i)
	{
		int v[3];
		v[0] = obj->indices[pos[i]][0];
		v[1] = obj->indices[pos[i]][1];
		v[2] = obj->indices[pos[i]][2];

		for (int j = 0; j < 3; ++j)
		{
			int k;
			for (k = 0; k < vLen; ++k)
				if (obj->vertices[vertices[k]].x == obj->vertices[v[j]].x &&
					obj->vertices[vertices[k]].y == obj->vertices[v[j]].y &&
					obj->vertices[vertices[k]].z == obj->vertices[v[j]].z)
						break;
			if (k == vLen)
				vertices[vLen++] = v[j];
		}
	}

	//calculam centrul
	Vector3 center = { 0, 0, 0 };
	for (int i = 0; i < vLen; ++i)
	{
		center.x += obj->vertices[vertices[i]].x;
		center.y += obj->vertices[vertices[i]].y;
		center.z += obj->vertices[vertices[i]].z;
	}
	center.x /= vLen * 1.0;
	center.y /= vLen * 1.0;
	center.z /= vLen * 1.0;

	//inseram centrul in obj->vertices[]
	obj->vertices[obj->vertexCount++] = center;
	int newVertex = obj->vertexCount - 1;

	//insert new indices created by newVertex
	SortVerticesCounterclockwise(vertices, vLen, obj, center);
	for (int i = 0; i < vLen-1; ++i)
	{
		obj->indices[obj->indexCount] = new int[3];
		obj->indices[obj->indexCount][0] = vertices[i];
		obj->indices[obj->indexCount][1] = vertices[i+1];
		obj->indices[obj->indexCount][2] = newVertex;
		obj->indexCount++;
	}

	obj->indices[obj->indexCount] = new int[3];
	obj->indices[obj->indexCount][0] = vertices[vLen-1];
	obj->indices[obj->indexCount][1] = vertices[0];
	obj->indices[obj->indexCount][2] = newVertex;
	obj->indexCount++;

	//delete obj->indices[obj->pos]
	/*for (int i = 0; i < posLen; ++i)
		for (int j = i+1; j < posLen; ++j)
			if (pos[i] < pos[j])
				swap(pos[i], pos[j]);

	for (int i = 0; i < posLen; ++i)
	{
		for (int j = pos[i]; j < obj->indexCount - 1; ++j)
			for (int k = 0; k < 3; ++k)
				obj->indices[j][k] = obj->indices[j + 1][k];

		obj->indexCount--;
	}*/
	//nu merge din ceva motiv, se poate si fara

	//update adjacency matrix
	//Update ad matrix
	for (int i = 0; i < obj->vertexCount; ++i)
		for (int j = 0; j < obj->vertexCount; ++j)
		{
			if (i == obj->vertexCount - 1)
				obj->ad[i] = new bool[VERTEXMAX];
			obj->ad[i][j] = 0;
		}

	for (int i = 0; i < obj->indexCount; ++i)
	{
		int a = obj->indices[i][0];
		int b = obj->indices[i][1];
		int c = obj->indices[i][2];

		obj->ad[a][b] = obj->ad[b][a] = 1;
		obj->ad[a][c] = obj->ad[c][a] = 1;
		obj->ad[b][c] = obj->ad[c][b] = 1;
	}

	flags->updateWindow = 1;
}

void deleteObject()
{
	int i;
	for (i = 0; i < objectCount; ++i)
		if (objects[i] == addVerticeObject)
			break;

	delete objects[i];
	for (; i < objectCount - 1; ++i)
		objects[i] = objects[i + 1];

	objectCount --;
	flags->updateWindow = 1;
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