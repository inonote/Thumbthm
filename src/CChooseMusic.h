#pragma once
#include "stdafx.h"
#include "CAnimation.h"
#include "CFileList.h"
#include "CKeyInput.h"

class CChooseMusic
{
public:
	CChooseMusic(const SkinData* pSkin, const SEData* pSE);
	~CChooseMusic();

	void MainLoop(int hGraph);
	int GetIndex();
	int GetLevel();
	string* GetName();

private:
	void Draw(int hGraph);
	void Update();
	void GetMusicInfo();

	const SkinData* m_pSkin;
	const SEData* m_pSE;

	CFileList* m_pCfl = NULL;

	static int m_nMusicLevel;		//曲のレベル
	static int m_nMusicIndex;		//選択中の曲のインデックス
	int m_nMusicMax;				//曲の数
	int m_nMusicMax_Visible;		//見えている曲の数
	
	CKeyInput m_Cki;

	//楽曲データー
	struct {
		string sTitle;
		string sBand;
		int nBPM;
		string sLyrics;
		string sMusic;
		string sArranged;
		int hImg;
		int nLv;
	} m_MusicInfo;

	//描画関係
	int m_hFont_StatusBar;
	int m_hFont_MusicList;
	int m_hFont_MusicList_S;
	int m_hFont_MusicInfo;

	int m_hBackBuffer;
	CAnimation m_CAnim_Wnd;		//ウィンドウのアニメーション
	CAnimation m_CAnim_List;	//曲一覧のアニメーション
	bool m_anim_List_Way;

	int m_Ret;


};