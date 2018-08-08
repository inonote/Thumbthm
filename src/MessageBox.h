#pragma once
#include"stdafx.h"

int DxMesageBox(const char* Message, const char* sTitle, int nType, int hGraph, Fps& fps, bool bSmallFont = false);

void CalcPos_TextCenter(POINT *out, const RECT *in, const char *text, int FontHandle);