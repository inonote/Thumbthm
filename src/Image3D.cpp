#include "stdafx.h"

void DrawGraph3DEx(float x, float y, float z, float z2, float sx, float sy, int GrHandle, int TransFlag, uchar r, uchar g, uchar b, uchar a) {
	VERTEX_3D Vertex[6];
	Vertex[0].pos.x = x;
	Vertex[0].pos.y = y;
	Vertex[0].pos.z = z;
	Vertex[0].u = 0.0F;
	Vertex[0].v = 0.0F;

	Vertex[1].pos.x = x + sx;
	Vertex[1].pos.y = y;
	Vertex[1].pos.z = z;
	Vertex[1].u = 1.0F;
	Vertex[1].v = 0.0F;

	Vertex[2].pos.x = x;
	Vertex[2].pos.y = y + sy;
	Vertex[2].pos.z = z2;
	Vertex[2].u = 0.0F;
	Vertex[2].v = 1.0F;

	Vertex[3].pos.x = x + sx;
	Vertex[3].pos.y = y + sy;
	Vertex[3].pos.z = z2;
	Vertex[3].u = 1.0F;
	Vertex[3].v = 1.0F;

	Vertex[4].pos.x = x;
	Vertex[4].pos.y = y + sy;
	Vertex[4].pos.z = z2;
	Vertex[4].u = 0.0F;
	Vertex[4].v = 1.0F;

	Vertex[5].pos.x = x + sx;
	Vertex[5].pos.y = y;
	Vertex[5].pos.z = z;
	Vertex[5].u = 1.0F;
	Vertex[5].v = 0.0F;

	// ãPìx
	Vertex[0].r = r;
	Vertex[1].r = r;
	Vertex[2].r = r;
	Vertex[3].r = r;
	Vertex[4].r = r;
	Vertex[5].r = r;
	Vertex[0].g = g;
	Vertex[1].g = g;
	Vertex[2].g = g;
	Vertex[3].g = g;
	Vertex[4].g = g;
	Vertex[5].g = g;
	Vertex[0].b = b;
	Vertex[1].b = b;
	Vertex[2].b = b;
	Vertex[3].b = b;
	Vertex[4].b = b;
	Vertex[5].b = b;
	Vertex[0].a = a;
	Vertex[1].a = a;
	Vertex[2].a = a;
	Vertex[3].a = a;
	Vertex[4].a = a;
	Vertex[5].a = a;

	DrawPolygon3D(Vertex, 2, GrHandle, TransFlag);
}

void DrawGraph3DEx(float x, float y, float z, float z2, float sx, float sy, int GrHandle, int TransFlag) {
	VERTEX_3D Vertex[6];
	Vertex[0].pos.x = x;
	Vertex[0].pos.y = y;
	Vertex[0].pos.z = z;
	Vertex[0].u = 0.0F;
	Vertex[0].v = 0.0F;

	Vertex[1].pos.x = x + sx;
	Vertex[1].pos.y = y;
	Vertex[1].pos.z = z;
	Vertex[1].u = 1.0F;
	Vertex[1].v = 0.0F;

	Vertex[2].pos.x = x;
	Vertex[2].pos.y = y + sy;
	Vertex[2].pos.z = z2;
	Vertex[2].u = 0.0F;
	Vertex[2].v = 1.0F;

	Vertex[3].pos.x = x + sx;
	Vertex[3].pos.y = y + sy;
	Vertex[3].pos.z = z2;
	Vertex[3].u = 1.0F;
	Vertex[3].v = 1.0F;

	Vertex[4].pos.x = x;
	Vertex[4].pos.y = y + sy;
	Vertex[4].pos.z = z2;
	Vertex[4].u = 0.0F;
	Vertex[4].v = 1.0F;

	Vertex[5].pos.x = x + sx;
	Vertex[5].pos.y = y;
	Vertex[5].pos.z = z;
	Vertex[5].u = 1.0F;
	Vertex[5].v = 0.0F;

	// ãPìx
	Vertex[0].r = Vertex[0].g = Vertex[0].b = 255;
	Vertex[1].r = Vertex[1].g = Vertex[1].b = 255;
	Vertex[2].r = Vertex[2].g = Vertex[2].b = 255;
	Vertex[3].r = Vertex[3].g = Vertex[3].b = 255;
	Vertex[4].r = Vertex[4].g = Vertex[4].b = 255;
	Vertex[5].r = Vertex[5].g = Vertex[5].b = 255;

	// a Ç‡ç≈ëÂíl
	Vertex[0].a = 255;
	Vertex[1].a = 255;
	Vertex[2].a = 255;
	Vertex[3].a = 255;
	Vertex[4].a = 255;
	Vertex[5].a = 255;

	DrawPolygon3D(Vertex, 2, GrHandle, TransFlag);
}

void DrawGraph3DEx(float x, float y, float z, float sx, float sy, int GrHandle, int TransFlag) {
	DrawGraph3DEx(x, y, z, z, sx, sy, GrHandle, TransFlag);
}
void DrawGraph3DEx(float x, float y, float z, float z2, int GrHandle, int TransFlag) {
	float sx, sy;
	GetGraphSizeF(GrHandle, &sx, &sy);
	DrawGraph3DEx(x, y, z, z2, sx, sy, GrHandle, TransFlag);
}
void DrawGraph3DEx(float x, float y, float z, int GrHandle, int TransFlag) {
	float sx, sy;
	GetGraphSizeF(GrHandle, &sx, &sy);
	DrawGraph3DEx(x, y, z, z, sx, sy, GrHandle, TransFlag);
}

void DrawGraph3DEx(BOXF(&box)[4], int GrHandle, int TransFlag) {
	VERTEX_3D Vertex[6];
	Vertex[0].pos.x = box[0].x;
	Vertex[0].pos.y = box[0].y;
	Vertex[0].pos.z = box[0].z;
	Vertex[0].u = 0.0F;
	Vertex[0].v = 0.0F;

	Vertex[1].pos.x = box[1].x;
	Vertex[1].pos.y = box[1].y;
	Vertex[1].pos.z = box[1].z;
	Vertex[1].u = 1.0F;
	Vertex[1].v = 0.0F;

	Vertex[2].pos.x = box[3].x;
	Vertex[2].pos.y = box[3].y;
	Vertex[2].pos.z = box[3].z;
	Vertex[2].u = 0.0F;
	Vertex[2].v = 1.0F;

	Vertex[3].pos.x = box[2].x;
	Vertex[3].pos.y = box[2].y;
	Vertex[3].pos.z = box[2].z;
	Vertex[3].u = 1.0F;
	Vertex[3].v = 1.0F;

	Vertex[4].pos.x = box[3].x;
	Vertex[4].pos.y = box[3].y;
	Vertex[4].pos.z = box[3].z;
	Vertex[4].u = 0.0F;
	Vertex[4].v = 1.0F;

	Vertex[5].pos.x = box[1].x;
	Vertex[5].pos.y = box[1].y;
	Vertex[5].pos.z = box[1].z;
	Vertex[5].u = 1.0F;
	Vertex[5].v = 0.0F;

	Vertex[0].r = Vertex[0].g = Vertex[0].b = 255;
	Vertex[1].r = Vertex[1].g = Vertex[1].b = 255;
	Vertex[2].r = Vertex[2].g = Vertex[2].b = 255;
	Vertex[3].r = Vertex[3].g = Vertex[3].b = 255;
	Vertex[4].r = Vertex[4].g = Vertex[4].b = 255;
	Vertex[5].r = Vertex[5].g = Vertex[5].b = 255;
	Vertex[0].a = 255;
	Vertex[1].a = 255;
	Vertex[2].a = 255;
	Vertex[3].a = 255;
	Vertex[4].a = 255;
	Vertex[5].a = 255;

	DrawPolygon3D(Vertex, 2, GrHandle, TransFlag);
}

BOXF BoxGet(float x, float y, float z) {
	BOXF box;
	box.x = x;
	box.y = y;
	box.z = z;
	return box;
}