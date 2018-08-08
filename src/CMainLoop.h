/*****

@Filename  : CMainLoop.h
@Author    : inonote
@Modified  : 2018/06/29
@First ed. : 2018/06/29
@Comment   : メインループ部のヘッダファイルです。

*****/

#pragma once
#include "stdafx.h"
#include "CMusicMan.h"
#include "CChooseMusic.h"
#include "CResultWnd.h"

class CMainLoop
{
public:
	void Run(HINSTANCE hInst, HWND hMainWnd, bool bFullScreen, bool bUseTouchPanel);
	~CMainLoop();

private:

	void Draw();

	void LoadSkin(const char* SkinName);
	void LoadSE();
	bool LoadMusic(const char* FolderPath, int nLv);
	void ChooseMusic();
	void ShowResult(ResultData* pRd);

	int hBackBuffer = NULL;

	HINSTANCE m_hInstance = NULL;
	HWND m_hWnd = NULL;
	bool m_bBreak = false;
	Fps m_cFps;

	SkinData m_Skin;
	SEData m_SE;

	CChooseMusic* cchm = NULL;
	CMusicMan* cmm = NULL;
	CResultWnd* crw = NULL;

	enum CONTENT {
		CT_CHOOSEMUSIC,
		CT_PLAY,
		CT_RESULT,
		CT_SETTING
	} ;

	CONTENT ct;

	bool m_bUseTouchPanel;
};