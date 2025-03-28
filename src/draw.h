#pragma once

void DrawLine(Vector3 first, Vector3 second)
{
	line(first.x, first.y, second.x, second.y);
}

void DrawTriangle(int indice[3], Object* object)
{
	Vector3* first = &object->realVertices[indice[0]];
	Vector3* second = &object->realVertices[indice[1]];
	Vector3* third = &object->realVertices[indice[2]];

	Vector3 normal = SurfaceNormal(*first, *second, *third);
	Normalize(normal);

	// backface culling. don't draw triangles the camera can't see. increases performance.

	if (normal.z < 0) // TODO: works for now because camera faces negative z axis. will have to check which dir the camera is facing once it can move around.
		if (flags->xray == false)
			return;
		else
			setlinestyle(4, 0, 3);


	double average = 255 * (normal.x + normal.y + normal.z) / 3;

	int r = object->color.r - average; if (r < 0) r = 0; if (r > 255) r = 255;
	int g = object->color.g - average; if (g < 0) g = 0; if (g > 255) g = 255;
	int b = object->color.b - average; if (b < 0) b = 0; if (b > 255) b = 255;

	int final_color = RGB(r, g, b);

	if (flags->xray == false)
		setcolor(final_color);
	else
	{
		if(normal.z < 0)
			setcolor(RGB(127, 127, 127));
		else
			setcolor(RGB(255, 255, 255));
	}

	//DrawLine(*first,  *second);
	//DrawLine(*second, *third);
	//DrawLine(*third,  *first);


	Vector3 center = { (first->x + second->x + third->x) / 3, (first->y + second->y + third->y) / 3, 0 }; // z might matter later?
	//final_color += rand() % 100;
	setfillstyle(SOLID_FILL, final_color);
	
	if(flags->xray == false)
	{
		int aux[6] = { first->x, first->y, second->x, second->y, third->x, third->y };
		fillpoly(3, aux);
	}
	else
	{
		DrawLine(*first,  *second);
		DrawLine(*second, *third);
		DrawLine(*third,  *first);
	}

	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	//Vertex select
	setcolor(RED);
	setfillstyle(SOLID_FILL, RED);

	Vector3 points[] = { *first, *second, *third };

	for (int i = 0; i < 3; ++i)
	{
		//Object HitBox
		for (int j = 0; j < 3; ++j)
		{
			Vector3* crt = &object->realVertices[indice[j]];
			object->hitBox[0].x = min(object->hitBox[0].x, crt->x);
			object->hitBox[0].y = min(object->hitBox[0].y, crt->y);
			object->hitBox[1].x = max(object->hitBox[1].x, crt->x);
			object->hitBox[1].y = max(object->hitBox[1].y, crt->y);
		}

		if (flags->showRedDot)
		{
			double rad;
			if (selectedVertice == indice[i])
				rad = 10;
			else
				rad = 6;

			circle(points[i].x, points[i].y, rad);
			floodfill(points[i].x, points[i].y, RED);
		}
	}
}

void DrawObject(Object* object)
{
	object->hitBox[0] = { INF, INF };
	object->hitBox[1] = { -1, -1 };

	//object->rotation.x = object->rotation.x - floor(object->rotation.x);
	//object->rotation.y = object->rotation.y - floor(object->rotation.y);
	//object->rotation.z = object->rotation.z - floor(object->rotation.z);

	//transform object
	for (int i = 0; i < object->vertexCount; i++)
	{
		object->realVertices[i] = object->vertices[i];

		///// LOCAL COORD TO WORLD COORD /////////////////////////////////////
		Rotate(object->vertices[i], object->rotation);
		Rotate(object->realVertices[i], object->rotation);
		Scale(object->realVertices[i], object->scale);
		Translate(object->realVertices[i], object->position);
		//////////////////////////////////////////////////////////

		// WORLD COORD TO CAMEARA RELATIVE COORD
		ViewMatrix(object->realVertices[i]);

		Perspective(object->realVertices[i]);

		// nu mai desena obiectele care sunt prea aproape de camera sau in spatele camerei
		// TODO nici alea prea indepartate nu ar trebuii desenate
		// momentan nu merge cand intorci camera la 180 de grade
		if (object->realVertices[i].z < -1200)
			return;

		// TODO
		// ar trebuii si o copie a centrului obiectelor trecut prin matrici
		// si sa tinem vectorul de obiecte sortat crescator dupa valoarea finala a lui z
		// intai trebuie desenate obiectele mai din spate dupa cele apropiate
		// ar trebuii resortat dupa o rotire a camerei cred

		// artificially center objects
		Translate(object->realVertices[i], {WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 0});
	}

	for (int i = 0; i < object->indexCount; i++)
		DrawTriangle(object->indices[i], object);

	object->rotation = { 0, 0, 0 };
}

void drawHitBox(Object* object)
{
	Vector2 first = object->hitBox[0];
	Vector2 second = object->hitBox[1];

	setcolor(WHITE);
	rectangle(first.x, first.y, second.x, second.y);
}