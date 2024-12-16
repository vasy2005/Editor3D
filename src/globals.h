#pragma once

Object* selectedObject;
int selectedVertice = -1; //select point to edit
double rad = 6; //red circle radius
double verticeMoved = 0;

Flags* flags = new Flags;
Camera camera;

int objectCount = 0;