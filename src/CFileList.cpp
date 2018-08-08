#include "stdafx.h"
#include "CFileList.h"

CFileList::CFileList(const char* path, DWORD dwFileAttributes)
{
	this->clear();

	WIN32_FIND_DATA find_dir_data;
	string sPath = path;
	sPath += "*";
	HANDLE hFind = FindFirstFile(sPath.c_str(), &find_dir_data);
	do {
		if (hFind != INVALID_HANDLE_VALUE) {
			if (strcmp(find_dir_data.cFileName, ".") &&
				strcmp(find_dir_data.cFileName, "..")
				) {
				if (find_dir_data.dwFileAttributes & dwFileAttributes) {
					if (find_dir_data.cFileName[0]!='.')
						this->push_back(string(find_dir_data.cFileName));
				}
			}
		}
	} while (FindNextFile(hFind, &find_dir_data));
}

CFileList::CFileList(const char* path) {
	CFileList(path, 0xFFFFFFFF);
}
