#pragma once
#include "stdafx.h"


void DrawGraph3DEx(float x, float y, float z, float z2, float sx, float sy, int GrHandle, int TransFlag, uchar r, uchar g, uchar b, uchar a);
void DrawGraph3DEx(float x, float y, float z, float z2, float sx, float sy, int GrHandle, int TransFlag);
void DrawGraph3DEx(float x, float y, float z, float sx, float sy, int GrHandle, int TransFlag);
void DrawGraph3DEx(float x, float y, float z, float z2, int GrHandle, int TransFlag);
void DrawGraph3DEx(float x, float y, float z, int GrHandle, int TransFlag);
void DrawGraph3DEx(BOXF (&box)[4], int GrHandle, int TransFlag);

BOXF BoxGet(float x, float y, float z);