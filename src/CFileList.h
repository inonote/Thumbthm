#pragma once
#include "stdafx.h"

//vector <string> Çåpè≥
class CFileList : public vector <string>
{
public:
	CFileList(const char* path, DWORD dwFileAttributes);
	CFileList(const char* path);
};