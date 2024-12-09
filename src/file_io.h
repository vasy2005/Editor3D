#pragma once

#include <fstream>
#include <cstring>
#include <windows.h>
#include "structs.h"
#include "vector_math.h"

#include <iostream>

using namespace std;



void SaveAsTri(char filePath[], Object**& objects, int& objectCount, Flags*& flags)
{
	ofstream fout(filePath);

	fout << "# Number of objects\n";
	fout << objectCount << "\n\n";

	for (int i = 0; i < objectCount; i++)
	{
		fout << "========== Object " << i << " ==========\n";

		fout << "# Vertices\n";
		fout << objects[i]->vertexCount << '\n';
		for (int j = 0; j < objects[i]->vertexCount; j++)
			fout << objects[i]->vertices[j].x << ' ' << objects[i]->vertices[j].y << ' ' << objects[i]->vertices[j].z << '\n';

		fout << "\n# Triangles\n";
		fout << objects[i]->indexCount << '\n';
		for (int j = 0; j < objects[i]->indexCount; j++)
			fout << objects[i]->indices[j][0] << ' ' << objects[i]->indices[j][1] << ' ' << objects[i]->indices[j][2] << '\n';

		fout << "\n# Position\n";
		fout << objects[i]->position.x << ' ' << objects[i]->position.y << ' ' << objects[i]->position.z << '\n';

		fout << "\n# Scale\n";
		fout << objects[i]->scale.x << ' ' << objects[i]->scale.y << ' ' << objects[i]->scale.z << '\n';

		fout << "\n# Rotation\n";
		fout << objects[i]->rotation.x << ' ' << objects[i]->rotation.y << ' ' << objects[i]->rotation.z << '\n';

		fout << "\n# Color\n";
		fout << objects[i]->color.r << ' ' << objects[i]->color.g << ' ' << objects[i]->color.b << ' ' << objects[i]->color.a << '\n';

		fout << "\n# Hitbox\n";
		fout << objects[i]->hitBox[0].x << ' ' << objects[i]->hitBox[0].y << '\n';
		fout << objects[i]->hitBox[1].x << ' ' << objects[i]->hitBox[1].y << '\n';

		fout << "\n# Graph\n";
		for (int j = 0; j < objects[i]->vertexCount; j++)
		{
			for (int k = 0; k < objects[i]->vertexCount; k++)
				fout << objects[i]->ad[j][k] << ' ';
			fout << '\n';
		}

		fout << '\n';
	}

	fout.close();
}

void SaveAsObj(char filePath[], Object**& objects, int& objectCount, Flags*& flags)
{
	// .obj files store color information separetely in a .mtl file
	char mtlName[512];
	strcpy(mtlName, filePath);
	while (strchr(mtlName, '\\'))
		strcpy(mtlName, strchr(mtlName, '\\') + 1);
	strcpy(mtlName + strlen(mtlName) - 4, ".mtl");

	char mtlPath[512];
	strcpy(mtlPath, filePath);
	strcpy(mtlPath + strlen(mtlPath) - 4, ".mtl");



	///// OBJ ////////////////////////////////////////////////////////////////////////////////////////
	ofstream obj(filePath);
	obj << "mtllib " << mtlName << '\n';
	

	int counter = 0;

	for (int i = 0; i < objectCount; i++)
	{
		obj << "o Object" << i << '\n';

		for (int j = 0; j < objects[i]->vertexCount; j++)
		{
			Vector3 vec = objects[i]->vertices[j];
			Rotate(vec, objects[i]->rotation);
			Scale(vec, objects[i]->scale);
			Translate(vec, {objects[i]->position.x - windowWidth/2, objects[i]->position.y - windowHeight/2, objects[i]->position.z});
			
			obj << "v " << vec.x << ' ' << -vec.y << ' ' << vec.z << '\n';
		}

		obj << "usemtl Material" << i << '\n';

		for (int j = 0; j < objects[i]->indexCount; j++)
			obj << "f " << objects[i]->indices[j][0] + 1 + counter << ' ' << objects[i]->indices[j][1] + 1 + counter << ' ' << objects[i]->indices[j][2] + 1 + counter << '\n';

		counter += objects[i]->vertexCount;
	}

	obj.close();
	//////////////////////////////////////////////////////////////////////////////////////////////////



	///// MTL ////////////////////////////////////////////////////////////////////////////////////////
	ofstream mtl(mtlPath);

	for (int i = 0; i < objectCount; i++)
	{
		mtl << "newmtl Material" << i << '\n';
		mtl << "Kd " << objects[i]->color.r/255.0 << ' ' << objects[i]->color.g/255.0 << ' ' << objects[i]->color.b/255.0 << '\n';
	}

	mtl.close();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

void Save(Object**& objects, int& objectCount, Flags*& flags)
{
	OPENFILENAME dialogue = { 0 };
	char filePath[512] = "";

	dialogue.lStructSize = sizeof(dialogue);
	dialogue.hwndOwner = flags->windowHandle;
	dialogue.lpstrFilter = "All Files (*.*)\0*.*\0Tridimensional File (*.tri)\0*.tri\0Wavefront File (*.obj)\0*.obj\0";
	dialogue.lpstrFile = filePath;
	dialogue.nMaxFile = 512;
	dialogue.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	dialogue.lpstrDefExt = "tri";

	int error = GetSaveFileName(&dialogue); // open the dialogue window

	if (error != 0) // success
	{
		// do something with the file path
		if(strstr(filePath, ".tri"))
			SaveAsTri(filePath, objects, objectCount, flags);

		else if (strstr(filePath, ".obj"))
			SaveAsObj(filePath, objects, objectCount, flags);
		// else
		// TODO display a warning message that the file extension is not accepted
	}
}




void SkipFileLines(ifstream& fin, int lineCount)
{
	for (int i = 0; i < lineCount; i++)
		fin.ignore(512, '\n');
}

void OpenTri(char filePath[], Object**& objects, int& objectCount, Flags*& flags)
{
	ifstream fin(filePath);

	SkipFileLines(fin, 1); // skip comment lines
	fin >> objectCount;
	SkipFileLines(fin, 2); // leftover '\n' after last read numeric value. skip extra line because of that

	// resize objects
	Object** temp = new Object*[objectCount];
	delete[] objects;
	objects = temp;

	for (int i = 0; i < objectCount; i++)
	{
		objects[i] = new Object;

		SkipFileLines(fin, 2);
		fin >> objects[i]->vertexCount;
		objects[i]->vertices = new Vector3[objects[i]->vertexCount];
		for (int j = 0; j < objects[i]->vertexCount; j++)
			fin >> objects[i]->vertices[j].x >> objects[i]->vertices[j].y >> objects[i]->vertices[j].z;

		SkipFileLines(fin, 3);
		fin >> objects[i]->indexCount;
		objects[i]->indices = new int* [objects[i]->indexCount];
		for (int j = 0; j < objects[i]->indexCount; j++)
		{
			objects[i]->indices[j] = new int[3];
			fin >> objects[i]->indices[j][0] >> objects[i]->indices[j][1] >> objects[i]->indices[j][2];
		}

		SkipFileLines(fin, 3);
		fin >> objects[i]->position.x >> objects[i]->position.y >> objects[i]->position.z;

		SkipFileLines(fin, 3);
		fin >> objects[i]->scale.x >> objects[i]->scale.y >> objects[i]->scale.z;

		SkipFileLines(fin, 3);
		fin >> objects[i]->rotation.x >> objects[i]->rotation.y >> objects[i]->rotation.z;

		SkipFileLines(fin, 3);
		fin >> objects[i]->color.r >> objects[i]->color.g >> objects[i]->color.b >> objects[i]->color.a;

		SkipFileLines(fin, 3);
		fin >> objects[i]->hitBox[0].x >> objects[i]->hitBox[0].y;
		fin >> objects[i]->hitBox[1].x >> objects[i]->hitBox[1].y;

		SkipFileLines(fin, 3);
		objects[i]->ad = new bool* [objects[i]->vertexCount];
		for (int j = 0; j < objects[i]->vertexCount; j++)
		{
			objects[i]->ad[j] = new bool[objects[i]->vertexCount];

			for (int k = 0; k < objects[i]->vertexCount; k++)
				fin >> objects[i]->ad[j][k];
		}

		SkipFileLines(fin, 2);
	}

	fin.close();
}

void OpenObj(char filePath[], Object**& objects, int& objectCount, Flags*& flags)
{
	// .obj files store color information separetely in a .mtl file
	char mtlName[512];
	strcpy(mtlName, filePath);
	while (strchr(mtlName, '\\'))
		strcpy(mtlName, strchr(mtlName, '\\') + 1);
	strcpy(mtlName + strlen(mtlName) - 4, ".mtl");

	char mtlPath[512];
	strcpy(mtlPath, filePath);
	strcpy(mtlPath + strlen(mtlPath) - 4, ".mtl");



	///// OBJ ////////////////////////////////////////////////////////////////////////////////////////
	ifstream obj(filePath);

	delete[] objects;
	flags->objectCapacity = 16;
	objects = new Object*[flags->objectCapacity];
	objectCount = 0;

	Object* current = NULL;

	int counter = 0;

	while (1)
	{
		char type[256]; obj >> type;

		if (type[0] == NULL)
		{
			break;
		}
		else if (strcmp(type, "o") == 0)
		{
			if(objectCount > 0)
				counter += objects[objectCount - 1]->vertexCount;

			objectCount++;
			if (objectCount > flags->objectCapacity)
			{
				flags->objectCapacity *= 2;
				Object** temp = new Object*[flags->objectCapacity];
				memcpy(temp, objects, sizeof(Object*) * objectCount);
				delete[] objects;
				objects = temp;
			}

			current = objects[objectCount - 1] = new Object;

			current->position = {windowWidth/2, windowHeight/2, 0};
			current->scale = {  20,  20,  20 };
			current->color = { 255, 255, 255 };

			current->vertexCount = 0;
			current->vertices = NULL;
			current->indexCount  = 0;
			current->indices  = NULL;

			SkipFileLines(obj, 1);
		}
		else if (strcmp(type, "v") == 0)
		{
			Vector3 vec;
			obj >> vec.x >> vec.y >> vec.z;

			current->vertexCount++;
			Vector3* temp = new Vector3[current->vertexCount];
			if (current->vertices != NULL)
			{
				memcpy(temp, current->vertices, (current->vertexCount - 1) * sizeof(Vector3));
				delete[] current->vertices;
			}
			current->vertices = temp;

			current->vertices[current->vertexCount - 1] = {vec.x, -vec.y, vec.z};

			obj.ignore(1); // skip '\n'
		}
		else if (strcmp(type, "f") == 0)
		{
			int first, second, third;
			obj >> first;
			obj.ignore(512, ' ');
			obj >> second;
			obj.ignore(512, ' ');

			bool stop = false;

			while (stop == false)
			{
				obj >> third;

				current->indexCount++;
				int** temp = new int* [current->indexCount];
				if (current->indices != NULL)
				{
					for (int i = 0; i < current->indexCount - 1; i++)
					{
						temp[i] = new int[3];

						temp[i][0] = current->indices[i][0];
						temp[i][1] = current->indices[i][1];
						temp[i][2] = current->indices[i][2];
					}
					delete[] current->indices;
				}
				current->indices = temp;

				current->indices[current->indexCount - 1] = new int[3];

				current->indices[current->indexCount - 1][0] = first  - counter - 1;
				current->indices[current->indexCount - 1][1] = third  - counter - 1;
				current->indices[current->indexCount - 1][2] = second - counter - 1;

				char c[1];
				while (obj.read(c, 1)) // dont skip whitespace
				{
					if (c[0] == ' ')
						break;

					if (c[0] == '\n')
					{
						stop = true;
						break;
					}
				}

				second = third;
			}
		}
		else
			SkipFileLines(obj, 1);
	}

	// TODO
	for (int i = 0; i < objectCount; i++)
	{
		current = objects[i];

		current->ad = new bool* [current->vertexCount];
		for (int i = 0; i < current->vertexCount; i++)
		{
			current->ad[i] = new bool[current->vertexCount];

			for (int j = 0; j < current->vertexCount; j++)
				current->ad[i][j] = 0;
		}
	}

	obj.close();
	//////////////////////////////////////////////////////////////////////////////////////////////////



	///// MTL ////////////////////////////////////////////////////////////////////////////////////////
	ifstream mtl(mtlPath); // TODO what if there's not mtl file?

	int index = -1;

	while (1)
	{
		char type[256]; mtl >> type;

		if (type[0] == NULL)
		{
			break;
		}
		else if (strcmp(type, "newmtl") == 0)
		{
			index++;
			mtl.ignore(512, '\n');
		}
		else if (strcmp(type, "Kd") == 0)
		{
			double r, g, b;
			mtl >> r >> g >> b;

			r *= 255;
			g *= 255;
			b *= 255;

			objects[index]->color = { (int)r, (int)g, (int)b };

			mtl.ignore(1);
		}
		else if(type[0] != '\n')
			SkipFileLines(mtl, 1);
	}

	mtl.close();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

void Open(Object**& objects, int& objectCount, Flags*& flags)
{
	OPENFILENAME dialogue = { 0 };
	char filePath[512] = "";

	dialogue.lStructSize = sizeof(dialogue);
	dialogue.hwndOwner = flags->windowHandle;
	dialogue.lpstrFilter = "All Files (*.*)\0*.*\0Tridimensional File (*.tri)\0*.tri\0Wavefront File (*.obj)\0*.obj\0";
	dialogue.lpstrFile = filePath;
	dialogue.nMaxFile = 512;
	dialogue.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	dialogue.lpstrDefExt = "tri";

	int error = GetOpenFileName(&dialogue); // open the dialogue window

	if (error != 0) // success
	{
		// do something with the file path
		if (strstr(filePath, ".tri"))
			OpenTri(filePath, objects, objectCount, flags);

		else if (strstr(filePath, ".obj"))
			OpenObj(filePath, objects, objectCount, flags);
	}
}