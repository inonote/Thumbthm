/*****

@Filename  : CChooseMusic.cpp
@Author    : inonote
@Modified  : 2018/08/06
@First ed. : 不明
@Comment   : 楽曲選択画面のソースコード。

*****/

#include "stdafx.h"
#include "CChooseMusic.h"
#include "CChooseMusic.h"
#include "CAnimation.h"
#include "CFileList.h"
#include "Easing.h"
#include <math.h>

int CChooseMusic::m_nMusicIndex = 0;
int CChooseMusic::m_nMusicLevel = LV_NORMAL;

CChooseMusic::CChooseMusic(const SkinData* pSkin, const SEData* pSE)
{
	m_pSkin = pSkin;
	m_pSE = pSE;

	//曲の一覧を取得
	m_pCfl = new CFileList(APP_PATH_MUSIC, FILE_ATTRIBUTE_DIRECTORY);

	//フォントを作成
	m_hFont_MusicList = CreateFontToHandle("メイリオ", APP_CMW_LIST_FONTHEIGHT, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	m_hFont_MusicList_S = CreateFontToHandle("メイリオ", APP_CMW_LIST_FONTHEIGHT_SEL, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	m_hFont_MusicInfo = CreateFontToHandle("メイリオ", APP_CMW_INFO_FONTHEIGHT, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	m_hFont_StatusBar = CreateFontToHandle("メイリオ", APP_CMW_STAT_FONTHEIGHT, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	//描画先バッファを作成
	m_hBackBuffer = MakeScreen(APP_WNDSX, APP_WNDSY, TRUE);

	//メンバ変数を初期値にセット
	m_nMusicMax = m_pCfl->size();
	m_nMusicMax_Visible = (APP_WNDSY - APP_CMW_MARGINY * 2) / APP_CMW_LIST_ITEMHEIGHT + 1;
	m_Ret = -1;

	GetMusicInfo();
	m_CAnim_Wnd.Set(0, 1, CATYPE_ADD, SecToFrame(0.5));
	m_CAnim_List.Set(APP_FPS / 4, 1, CATYPE_ADD, SecToFrame(0.25));


	m_anim_List_Way = 0;
}

CChooseMusic::~CChooseMusic()
{
	delete m_pCfl;
	DeleteGraph(m_hBackBuffer);
	if (m_MusicInfo.hImg)
		DeleteGraph(m_MusicInfo.hImg);
	DeleteFontToHandle(m_hFont_MusicList);
	DeleteFontToHandle(m_hFont_MusicList_S);
	DeleteFontToHandle(m_hFont_MusicInfo);
	DeleteFontToHandle(m_hFont_StatusBar);
}

void CChooseMusic::MainLoop(int hGraph) {
	Draw(hGraph);
	Update();
	m_Cki.Update();

}

int CChooseMusic::GetIndex() {
	return m_Ret;
}
int CChooseMusic::GetLevel() {
	return m_nMusicLevel;
}

string* CChooseMusic::GetName() {
	if (m_Ret < 0)
		return nullptr;
	int n = m_Ret;
	m_Ret = -1;
	return &m_pCfl->at(n);
}
void CChooseMusic::Draw(int hGraph) {
	double aval = 0.0;
	int px, py, sx, sy, y, col;
	ClearDrawScreen();

	SetDrawScreen(m_hBackBuffer);
	DrawBox(0, 0, APP_WNDSX, APP_WNDSY, 0x0F0F0F, TRUE);


	//曲一覧を描画
	px = APP_CMW_MARGINX;
	py = APP_CMW_MARGINY;
	sx = APP_WNDSX - APP_CMW_INFOSX - APP_CMW_MARGINX * 2 - APP_CMW_SPACE;
	sy = APP_WNDSY - APP_CMW_MARGINY * 2;
	SetDrawArea(px, py, px + sx, py + sy);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
	DrawBox(px, py, px + sx, py + sy, 0x7F7F7F, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	aval = APP_CMW_LIST_ITEMHEIGHT - APP_CMW_LIST_ITEMHEIGHT * Easing::easeOutCubic(m_CAnim_List.GetP());
	if (m_anim_List_Way)
		aval = -aval;
	//曲のタイトルを描画

	for (int i = 0; i <= m_nMusicMax_Visible; i++) {
		DrawStringToHandle(px + APP_CMW_LIST_MARGINX, py + i * APP_CMW_LIST_ITEMHEIGHT +
			(APP_CMW_LIST_ITEMHEIGHT - APP_CMW_LIST_FONTHEIGHT) / 2 - APP_CMW_LIST_ITEMHEIGHT / 2 - aval,
			m_pCfl->at(abs((int)(i + m_nMusicIndex + (m_nMusicMax  - m_nMusicMax_Visible / 2))) % m_nMusicMax).c_str(), APP_CMW_LIST_TEXTCOL,
			m_hFont_MusicList, APP_CMW_LIST_TEXTCOL_EDGE);
	}

	//選択項目は別に描画
	{
		size_t i = m_nMusicMax_Visible / 2;
		SetDrawArea(px, py + i * APP_CMW_LIST_ITEMHEIGHT - APP_CMW_LIST_ITEMHEIGHT_SEL / 2,
			px + sx, py + i * APP_CMW_LIST_ITEMHEIGHT + APP_CMW_LIST_ITEMHEIGHT_SEL / 2);
		DrawBox(px, py + i * APP_CMW_LIST_ITEMHEIGHT - APP_CMW_LIST_ITEMHEIGHT_SEL / 2,
			px + sx, py + i * APP_CMW_LIST_ITEMHEIGHT + APP_CMW_LIST_ITEMHEIGHT_SEL / 2,
			0x7F7F7F, TRUE);

		if (m_nMusicLevel == LV_EASY)
			col = APP_LVCOL_EASY;
		else if (m_nMusicLevel == LV_NORMAL)
			col = APP_LVCOL_NORMAL;
		else if (m_nMusicLevel == LV_HARD)
			col = APP_LVCOL_HARD;
		else if (m_nMusicLevel == LV_EXPERT)
			col = APP_LVCOL_EXPERT;
		else
			col = 0x000000;
		DrawBox(px, py + i * APP_CMW_LIST_ITEMHEIGHT - APP_CMW_LIST_ITEMHEIGHT_SEL / 2,
			px + APP_CMW_INFO_MARGINX, py + i * APP_CMW_LIST_ITEMHEIGHT + APP_CMW_LIST_ITEMHEIGHT_SEL / 2,
			col, TRUE);
	}
	for (size_t i = m_nMusicMax_Visible / 2 - 1; i <= m_nMusicMax_Visible / 2 + 1; i++) {
		DrawStringToHandle(px + APP_CMW_INFO_MARGINX_SEL, py + i * APP_CMW_LIST_ITEMHEIGHT +
			(APP_CMW_LIST_ITEMHEIGHT_SEL - APP_CMW_LIST_FONTHEIGHT_SEL) / 2 - APP_CMW_LIST_ITEMHEIGHT_SEL / 2 - aval,
			m_pCfl->at(abs((int)(i + m_nMusicIndex - m_nMusicMax_Visible / 2)) % m_nMusicMax).c_str(), APP_CMW_LIST_TEXTCOL_SEL,
			m_hFont_MusicList_S, APP_CMW_LIST_TEXTCOL_EDGE);
	}


	//詳細情報表示枠を描画
	px = APP_WNDSX - APP_CMW_INFOSX - APP_CMW_MARGINX;
	py = APP_CMW_MARGINY;
	sx = APP_CMW_INFOSX;
	sy = APP_WNDSY - APP_CMW_MARGINY * 2;
	SetDrawArea(px, py, px + sx, py + sy);


	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
	DrawBox(px, py, px + sx, py + sy, 0x7F7F7F, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//サムネイル
	if (m_nMusicLevel == LV_EASY)
		col = APP_LVCOL_EASY;
	else if (m_nMusicLevel == LV_NORMAL)
		col = APP_LVCOL_NORMAL;
	else if (m_nMusicLevel == LV_HARD)
		col = APP_LVCOL_HARD;
	else if (m_nMusicLevel == LV_EXPERT)
		col = APP_LVCOL_EXPERT;
	else
		col = 0x000000;
	DrawBox(px + (sx - APP_CMW_INFO_IMGSIZEX) / 2 + 5, py + APP_CMW_INFO_MARGINY - 5,
		px + (sx - APP_CMW_INFO_IMGSIZEX) / 2 + APP_CMW_INFO_IMGSIZEX + 5,
		py + APP_CMW_INFO_MARGINY + APP_CMW_INFO_IMGSIZEY - 5,
		col, TRUE);
	DrawExtendGraph(px + (sx - APP_CMW_INFO_IMGSIZEX) / 2, py + APP_CMW_INFO_MARGINY,
		px + (sx - APP_CMW_INFO_IMGSIZEX) / 2 + APP_CMW_INFO_IMGSIZEX,
		py + APP_CMW_INFO_MARGINY + APP_CMW_INFO_IMGSIZEY,
		m_MusicInfo.hImg, FALSE);

	//曲情報を表示する
	y = py + APP_CMW_INFO_MARGINY + APP_CMW_INFO_IMGSIZEY + APP_CMW_INFO_ITEMHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX, y, "Title:", APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);
	y += APP_CMW_INFO_ITEMHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX * 2, y, m_MusicInfo.sTitle.c_str(), APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);

	y += APP_CMW_INFO_ITEMHEIGHT + APP_CMW_INFO_SPHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX, y, "Band:", APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);
	y += APP_CMW_INFO_ITEMHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX * 2, y, m_MusicInfo.sBand.c_str(), APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);

	y += APP_CMW_INFO_ITEMHEIGHT + APP_CMW_INFO_SPHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX, y, "Lyrics:", APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);
	y += APP_CMW_INFO_ITEMHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX * 2, y, m_MusicInfo.sLyrics.c_str(), APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);

	y += APP_CMW_INFO_ITEMHEIGHT + APP_CMW_INFO_SPHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX, y, "Music:", APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);
	y += APP_CMW_INFO_ITEMHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX * 2, y, m_MusicInfo.sMusic.c_str(), APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);

	y += APP_CMW_INFO_ITEMHEIGHT + APP_CMW_INFO_SPHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX, y, "Arranged:", APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);
	y += APP_CMW_INFO_ITEMHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX * 2, y, m_MusicInfo.sArranged.c_str(), APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);

	y += APP_CMW_INFO_ITEMHEIGHT + APP_CMW_INFO_SPHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX, y, "Level:", APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);
	y += APP_CMW_INFO_ITEMHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX * 2, y, to_string(m_MusicInfo.nLv).c_str(), APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);

	y += APP_CMW_INFO_ITEMHEIGHT + APP_CMW_INFO_SPHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX, y, "BPM:", APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);
	y += APP_CMW_INFO_ITEMHEIGHT;
	DrawStringToHandle(px + APP_CMW_INFO_MARGINX * 2, y, to_string(m_MusicInfo.nBPM).c_str(), APP_CMW_INFO_TEXTCOL, m_hFont_MusicInfo, APP_CMW_INFO_TEXTCOL_EDGE);

	SetDrawArea(0, 0, APP_WNDSX, APP_WNDSY);

	DrawStringToHandle(APP_CMW_MARGINX, APP_WNDSY - APP_CMW_STAT_FONTHEIGHT - 10, "↑/↓:カーソル移動  ←/→:レベル変更  Space:決定", 0xFFFFFF, m_hFont_StatusBar, 0x202020);
	
	//裏画面にコピー
	SetDrawScreen(hGraph);
	if (m_CAnim_Wnd.IsPause()) {
		DrawGraph(0, 0, m_hBackBuffer, FALSE);
	}
	else {
		aval = Easing::easeOutQuart(m_CAnim_Wnd.GetP());
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * aval);
		DrawExtendGraphF(20 - 20 * aval, 20 - 20 * aval
			, APP_WNDSX - 20 + 20 * aval, APP_WNDSY - 20 + 20 * aval,
			m_hBackBuffer, FALSE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void CChooseMusic::Update() {

	//キー入力判定
	if (m_Cki.KeyDown(KEY_INPUT_DOWN)) {
		m_nMusicIndex++;
		if (m_nMusicIndex >= m_nMusicMax)
			m_nMusicIndex = 0;
		m_CAnim_List.Set(0, 1, CATYPE_ADD, APP_FPS / 4);
		m_anim_List_Way = 1;

		GetMusicInfo();
	}
	else if (m_Cki.KeyDown(KEY_INPUT_UP)) {
		m_nMusicIndex--;
		if (m_nMusicIndex < 0)
			m_nMusicIndex = m_nMusicMax - 1;
		m_CAnim_List.Set(0, 1, CATYPE_ADD, APP_FPS / 4);
		m_anim_List_Way = 0;

		GetMusicInfo();
	}

	if (m_Cki.KeyDown(KEY_INPUT_LEFT)) {
		m_nMusicLevel = max(m_nMusicLevel - 1, LV_EASY);

		GetMusicInfo();
	}
	else if (m_Cki.KeyDown(KEY_INPUT_RIGHT)) {
		m_nMusicLevel = min(m_nMusicLevel + 1, LV_EXPERT);
		GetMusicInfo();
	}
	
	if (m_Cki.KeyDown(KEY_INPUT_SPACE)) {
		m_Ret = m_nMusicIndex;
	}

	m_CAnim_Wnd.Update();
	m_CAnim_List.Update();
}

void CChooseMusic::GetMusicInfo() {
	static bool bInit = false;
	char sFilePath[MAX_PATH + 1];
	char buf[0x301];
	char buf2[0x301];

	//その曲のIniファイルを読み込む
	sprintf_s(sFilePath, MAX_PATH, "%s%s\\Info.ini", APP_PATH_MUSIC, m_pCfl->at(m_nMusicIndex).c_str());
	GetPrivateProfileString("Main", "Title", "", buf, 0x300, sFilePath);
	m_MusicInfo.sTitle = buf;
	GetPrivateProfileString("Main", "Band", "", buf, 0x300, sFilePath);
	m_MusicInfo.sBand = buf;
	GetPrivateProfileString("Main", "Img", "", buf, 0x300, sFilePath);
	if (buf[0] != '\0') {
		sprintf_s(buf2, MAX_PATH, "%s%s\\%s", APP_PATH_MUSIC, m_pCfl->at(m_nMusicIndex).c_str(), buf);
		if (bInit && m_MusicInfo.hImg)
			DeleteGraph(m_MusicInfo.hImg);
		m_MusicInfo.hImg = LoadGraph(buf2);
	}
	GetPrivateProfileString("Main", "Title", "", buf, 0x300, sFilePath);
	m_MusicInfo.nBPM = GetPrivateProfileInt("Music", "BPM", 120, sFilePath);
	GetPrivateProfileString("Authors", "Lyrics", "", buf, 0x300, sFilePath);
	m_MusicInfo.sLyrics = buf;
	GetPrivateProfileString("Authors", "Music", "", buf, 0x300, sFilePath);
	m_MusicInfo.sMusic = buf;
	GetPrivateProfileString("Authors", "Arranged", "", buf, 0x300, sFilePath);
	m_MusicInfo.sArranged = buf;
	if (m_nMusicLevel == LV_EASY)
		m_MusicInfo.nLv = GetPrivateProfileInt("Level", "Easy", 0, sFilePath);
	else if (m_nMusicLevel == LV_NORMAL)
		m_MusicInfo.nLv = GetPrivateProfileInt("Level", "Normal", 0, sFilePath);
	else if (m_nMusicLevel == LV_HARD)
		m_MusicInfo.nLv = GetPrivateProfileInt("Level", "Hard", 0, sFilePath);
	else if (m_nMusicLevel == LV_EXPERT)
		m_MusicInfo.nLv = GetPrivateProfileInt("Level", "Expert", 0, sFilePath);
	else
		m_MusicInfo.nLv = 0;
	bInit = true;
}