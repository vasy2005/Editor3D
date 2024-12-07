#pragma once

#include <fstream>
#include <cstring>
#include <windows.h>
#include "structs.h"

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

}

void Save(Object**& objects, int& objectCount, Flags*& flags)
{
	OPENFILENAME dialogue = { 0 };
	char filePath[512] = "";

	dialogue.lStructSize = sizeof(dialogue);
	dialogue.hwndOwner = flags->windowHandle;
	dialogue.lpstrFilter = "3D Scene File (*.tri)\0*.tri\0";
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
	Object** temp = new Object * [objectCount];
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

}

void Open(Object**& objects, int& objectCount, Flags*& flags)
{
	OPENFILENAME dialogue = { 0 };
	char filePath[512] = "";

	dialogue.lStructSize = sizeof(dialogue);
	dialogue.hwndOwner = flags->windowHandle;
	dialogue.lpstrFilter = "3D Scene File (*.tri)\0*.tri\0";
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