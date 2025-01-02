#pragma once

Object* selectedObject;
int selectedVertice = -1; //select point to edit
double rad = 6; //red circle radius
double verticeMoved = 0;

Flags* flags = new Flags;
Camera camera;

int objectCount = 0;
Object* addVerticeObject = 0;

int pos[INDICEMAX]; //pos of triangles facing the camera
int posLen = 0;
bool highlighted[INDICEMAX];

char* canvas;
//up, down, left, right

Object* copiedObject = NULL;