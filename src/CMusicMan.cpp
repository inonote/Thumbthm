/*****

@Filename  : CMusicMan.cpp
@Author    : inonote
@Modified  : 2018/08/08
@First ed. : 2018/06/29
@Comment   : 楽曲を管理するソースコード。

*****/

#include "stdafx.h"
#include "CMusicMan.h"
#include "CompileNotes.h"
#include "MessageBox.h"
#include "Image3D.h"
#include "CKeyInput.h"
#include "Easing.h"

#define GetColor_R(color) ((color >> 24) & 0x000000FF)
#define GetColor_G(color) ((color >> 16) & 0x000000FF)
#define GetColor_B(color) ((color >> 8) & 0x000000FF)
#define GetColor_A(color) (color & 0x000000FF)

CMusicMan::CMusicMan(const char* FolderPath, int Level, const SkinData* pSkin, const SEData* pSE, bool bUseTouchPanel)
{
	char sFilePath[MAX_PATH + 1];
	char sBuf[0x301];
	char sBuf2[0x301];

	m_pSkin = pSkin;
	m_pSE = pSE;

	//ファイルを読み込み
	switch (Level)
	{
	case LV_EASY:
		sprintf_s(sFilePath, MAX_PATH, "%sEasy.tbm", FolderPath);
		break;
	case LV_NORMAL:
		sprintf_s(sFilePath, MAX_PATH, "%sNormal.tbm", FolderPath);
		break;
	case LV_HARD:
		sprintf_s(sFilePath, MAX_PATH, "%sHard.tbm", FolderPath);
		break;
	default:
		sprintf_s(sFilePath, MAX_PATH, "%sExpert.tbm", FolderPath);
		break;
	}

	int nSize = FileRead_size(sFilePath);
	if (nSize == -1) { //ファイルが存在しない
		m_nErrorID = 1;
		return;
	}

	int nhFile = FileRead_open(sFilePath, 0);
	char *str = new (nothrow) char[nSize + 1];
	str[nSize] = 0;
	if (str == NULL) { //メモリの確保に失敗
		m_nErrorID = 2;
		return;
	}
	FileRead_read(str, nSize, nhFile);
	m_sData = str; //代入
	m_uDataSize = m_sData.length();

	delete[] str;
	FileRead_close(nhFile);

	//譜面を解析
	CompileNotes(m_Notes, m_sData, m_uDataSize);

	//楽曲情報を取得
	sprintf_s(sFilePath, MAX_PATH, "%sInfo.ini", FolderPath);
	GetPrivateProfileString("Main", "Title", "", sBuf, 0x300, sFilePath);
	m_MusicInfo.Main.sTitle = sBuf;
	GetPrivateProfileString("Main", "Band", "", sBuf, 0x300, sFilePath);
	m_MusicInfo.Main.sBand = sBuf;

	GetPrivateProfileString("Music", "Wave", "", sBuf, 0x300, sFilePath);
	if (sBuf[0] == 0) {
		GetPrivateProfileString("Music", "MV", "", sBuf, 0x300, sFilePath);
		m_MusicInfo.Music.bMV = true;
	}else
		m_MusicInfo.Music.bMV = false;

	m_MusicInfo.Music.sWaveFilename = FolderPath;
	m_MusicInfo.Music.sWaveFilename += sBuf;
	GetPrivateProfileString("Music", "Adjust", "1", sBuf, 0x300, sFilePath);
	m_MusicInfo.Music.dAdjust = atof(sBuf);
	m_MusicInfo.Music.nBPM = GetPrivateProfileInt("Music", "BPM", 120, sFilePath);
	m_MusicInfo.Music.nOffset = GetPrivateProfileInt("Music", "Offset", 0, sFilePath) * APP_TICK_ADJUST;

	GetPrivateProfileString("Authors", "Lyrics", "", sBuf, 0x300, sFilePath);
	m_MusicInfo.Authors.sLyrics = sBuf;
	GetPrivateProfileString("Authors", "Music", "", sBuf, 0x300, sFilePath);
	m_MusicInfo.Authors.sMusic = sBuf;
	GetPrivateProfileString("Authors", "Arranged", "", sBuf, 0x300, sFilePath);
	m_MusicInfo.Authors.sArranged = sBuf;

	m_MusicInfo.nLv = Level;
	if (Level == LV_EASY)
		m_MusicInfo.nLvVal = GetPrivateProfileInt("Level", "Easy", 0, sFilePath);
	else if (Level == LV_NORMAL)
		m_MusicInfo.nLvVal = GetPrivateProfileInt("Level", "Normal", 0, sFilePath);
	else if (Level == LV_HARD)
		m_MusicInfo.nLvVal = GetPrivateProfileInt("Level", "Hard", 0, sFilePath);
	else if (Level == LV_EXPERT)
		m_MusicInfo.nLvVal = GetPrivateProfileInt("Level", "Expert", 0, sFilePath);
	else
		m_MusicInfo.nLvVal = 0;

	//サムネ読み込み
	GetPrivateProfileString("Main", "Img", "", sBuf, 0x300, sFilePath);
	if (sBuf[0] != '\0') {
		sprintf_s(sBuf2, MAX_PATH, "%s%s", FolderPath, sBuf);
		m_MusicInfo.hImg = LoadGraph(sBuf2);
	}

	//音楽を読み込む
	if (m_MusicInfo.Music.bMV) {
		m_hBGM = LoadGraph(m_MusicInfo.Music.sWaveFilename.c_str());
	}
	else {
		SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMPRESS);
		m_hBGM = LoadSoundMem(m_MusicInfo.Music.sWaveFilename.c_str());
		ChangeVolumeSoundMem(200, m_hBGM);
		SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);
	}

	m_uWidth = APP_WNDSX;
	m_uHeight = 10000;
	m_ptOrigin.x = m_uWidth / 2;
	m_ptOrigin.y = 9670;
	m_ptOriginW.y = 670;
	
	//ノーツの速度を表示する小節数に変換(β)
	m_uVisibleTick = (double)(400.0 * ((double)(12-GetPrivateProfileInt("Music", "Speed", 5, "data\\Thumbthm.ini"))/8)) / (60.0 / m_MusicInfo.Music.nBPM * 1000) * 4 * APP_TICK_MEASURE;
	m_nTick = 0;
	m_NoteAt = 0;
	m_nCount_Miss = 0;
	m_nCount_Bad = 0;
	m_nCount_Good = 0;
	m_nCount_Great = 0;
	m_nCount_Perfect = 0;
	m_nCount_Combo = 0;
	m_nCount_MaxCombo = 0;
	m_nCount_VisibleNotes = 0;

	m_nLv = Level;

	m_bEverAutoPlay = m_bAutoPlay = false;

	m_bPause = true;
	m_uMusicIntroCount = 0;
	m_bShowDebugMsg = false;

	m_bEnableTouch = bUseTouchPanel;

	m_hFont_Judgement = CreateFontToHandle("メイリオ", 36, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	m_hFont_ComboCount = CreateFontToHandle("メイリオ", 100, 8, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	m_hFont_Combo = CreateFontToHandle("メイリオ", 36, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	m_hFont_Title = CreateFontToHandle("メイリオ", 36, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	m_hFont_Authors = CreateFontToHandle("メイリオ", 24, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}


CMusicMan::~CMusicMan()
{
	//開放
	DeleteFontToHandle(m_hFont_Judgement);
	DeleteFontToHandle(m_hFont_ComboCount);
	DeleteFontToHandle(m_hFont_Combo);
	DeleteFontToHandle(m_hFont_Title);
	DeleteFontToHandle(m_hFont_Authors);

	if (m_MusicInfo.hImg)
		DeleteGraph(m_MusicInfo.hImg);
	if (m_MusicInfo.Music.bMV) {
		PauseMovieToGraph(m_hBGM);
		DeleteGraph(m_hBGM);
	}
	else {
		StopSoundMem(m_hBGM);
		DeleteSoundMem(m_hBGM);
	}
	StopSoundMem(m_pSE->Long);

}

//結果の情報を取得
void CMusicMan::GetResultData(ResultData *pRd) {
	if (!pRd)
		return;
	pRd->uPerfect = m_nCount_Perfect;
	pRd->uGreat = m_nCount_Great;
	pRd->uGood = m_nCount_Good;
	pRd->uBad = m_nCount_Bad;
	pRd->uMiss = m_nCount_Miss;
	pRd->uMaxCombo = m_nCount_MaxCombo;
	pRd->uScore = 0;
	pRd->uLvVal = m_MusicInfo.nLvVal;
	pRd->uLv = m_MusicInfo.nLv;
	pRd->sTitle = m_MusicInfo.Main.sTitle;
	pRd->bAutoPlay = m_bEverAutoPlay;
}

void CMusicMan::MainLoop(int hGraph) {
	m_Cki.Update();
	m_Cti.Update();

	//再生位置(ms)からTickを算出
	if (m_MusicInfo.Music.bMV) {
		m_nTick = (int64_t)(((double)TellMovieToGraph(m_hBGM) / (double)(60000 / m_MusicInfo.Music.nBPM) / 4 * APP_TICK_MEASURE) + m_MusicInfo.Music.nOffset) * m_MusicInfo.Music.dAdjust;
	}
	else {
		m_nTick = (int64_t)(((double)GetSoundCurrentTime(m_hBGM) / (double)(60000 / m_MusicInfo.Music.nBPM) / 4 * APP_TICK_MEASURE) + m_MusicInfo.Music.nOffset) * m_MusicInfo.Music.dAdjust;
	}

	if (m_uMusicIntroCount <= SecToFrame(4)) { //曲のイントロ
		ClearDrawScreen();
		POINT pt;
		RECT rc;
		int col;
		char buf[0x101];

		//サムネイルを描画
		pt.x = (APP_WNDSX - APP_CMW_INFO_IMGSIZEX) / 2;
		pt.y = (APP_WNDSY - APP_CMW_INFO_IMGSIZEY) / 3;
		if (m_nLv == LV_EASY)
			col = APP_LVCOL_EASY;
		else if (m_nLv == LV_NORMAL)
			col = APP_LVCOL_NORMAL;
		else if (m_nLv == LV_HARD)
			col = APP_LVCOL_HARD;
		else if (m_nLv == LV_EXPERT)
			col = APP_LVCOL_EXPERT;
		else
			col = 0x000000;
		DrawBox(pt.x + 5, pt.y - 5,
			pt.x + APP_CMW_INFO_IMGSIZEX + 5,
			pt.y + APP_CMW_INFO_IMGSIZEY - 5,
			col, TRUE);
		DrawExtendGraph(pt.x, pt.y,
			pt.x + APP_CMW_INFO_IMGSIZEX,
			pt.y + APP_CMW_INFO_IMGSIZEY,
			m_MusicInfo.hImg, FALSE);

		//曲名を描画
		m_uMusicIntroCount++;
		rc.left = 0;
		rc.top = pt.y + APP_CMW_INFO_IMGSIZEY + 40;
		rc.right = m_uWidth;
		rc.bottom = rc.top + 70;
		DrawBox(rc.left, rc.top, rc.right, rc.bottom, 0xDDDDDD, TRUE);
		CalcPos_TextCenter(&pt, &rc, m_MusicInfo.Main.sTitle.c_str(), m_hFont_Title);
		DrawStringToHandle(pt.x, pt.y, m_MusicInfo.Main.sTitle.c_str(), 0x404040, m_hFont_Title, 0xC0C0C0);

		rc.top = rc.bottom + 20;
		rc.bottom = rc.top + 30;
		sprintf_s(buf, 0x100, "作詞 : %s", m_MusicInfo.Authors.sLyrics.c_str());
		CalcPos_TextCenter(&pt, &rc, buf, m_hFont_Authors);
		DrawStringToHandle(pt.x, pt.y, buf, 0xFFFFFF, m_hFont_Authors, 0x202020);
		rc.top = rc.bottom + 5;
		rc.bottom = rc.top + 30;
		sprintf_s(buf, 0x100, "作曲 : %s", m_MusicInfo.Authors.sMusic.c_str());
		CalcPos_TextCenter(&pt, &rc, buf, m_hFont_Authors);
		DrawStringToHandle(pt.x, pt.y, buf, 0xFFFFFF, m_hFont_Authors, 0x202020);
		rc.top = rc.bottom + 5;
		rc.bottom = rc.top + 30;
		sprintf_s(buf, 0x100, "編曲 : %s", m_MusicInfo.Authors.sArranged.c_str());
		CalcPos_TextCenter(&pt, &rc, buf, m_hFont_Authors);
		DrawStringToHandle(pt.x, pt.y, buf, 0xFFFFFF, m_hFont_Authors, 0x202020);

		if (m_uMusicIntroCount == SecToFrame(4))
			Continue();
	}
	else {
		static int nDrawCount = 0;
		Update();
		if (nDrawCount >= APP_FPS / APP_DRAWFPS) {
			nDrawCount = 0;
			Draw(hGraph);
		}else
			nDrawCount++;
		Hittest(m_nTick);

		//SE
		if (CheckSoundMem(m_pSE->Long) != 1) {
			if (m_LongNoteData.size() > 0)
				PlaySoundMem(m_pSE->Long, DX_PLAYTYPE_LOOP);
		}
		else {
			if (m_LongNoteData.size() <= 0)
				StopSoundMem(m_pSE->Long);
		}


		if (m_Cki.KeyDown(KEY_INPUT_F3)) {
			char buf[100];
			sprintf_s(buf, 100, "Tick: %d", m_nTick / APP_TICK_ADJUST);
			Fps f(APP_FPS);
			Pause();
			DxMesageBox(buf, "デバッグ", 0, hGraph, f);
			Continue();
		}
	}

	if (m_Cki.KeyDown(KEY_INPUT_F4)) {
		m_bAutoPlay = !m_bAutoPlay;
		if (m_bAutoPlay)
			m_bEverAutoPlay = true;
	}

	if (m_Cki.KeyDown(KEY_INPUT_F2))
		m_bShowDebugMsg = !m_bShowDebugMsg;
	
	if (m_bShowDebugMsg) {
		//デバッグ出力
		DrawFormatString(0, 18 * 1, 0xFFFFFF, "Music   %s", m_MusicInfo.Main.sTitle.c_str());
		DrawFormatString(0, 18 * 2, 0xFFFFFF, "Wave    %s", m_MusicInfo.Music.sWaveFilename.c_str());
		DrawFormatString(0, 18 * 3, 0xFFFFFF, "BPM     %d", m_MusicInfo.Music.nBPM);
		DrawFormatString(0, 18 * 4, 0xFFFFFF, "Meas    %d:%d (%04d) ", (int)m_nTick / APP_TICK_MEASURE + 1, (int)m_nTick / (APP_TICK_MEASURE / 4) % 4 + 1, ((int)m_nTick % APP_TICK_MEASURE) / APP_TICK_ADJUST);
		DrawFormatString(0, 18 * 5, 0xFFFFFF, "Notes   %d/%d (%d)", m_NoteCount, m_Notes.size(), m_nCount_VisibleNotes);
		DrawFormatString(0, 18 * 7, 0xFFFFFF, "Perfect %d", m_nCount_Perfect);
		DrawFormatString(0, 18 * 8, 0xFFFFFF, "Great   %d", m_nCount_Great);
		DrawFormatString(0, 18 * 9, 0xFFFFFF, "Good    %d", m_nCount_Good);
		DrawFormatString(0, 18 * 10, 0xFFFFFF, "Bad     %d", m_nCount_Bad);
		DrawFormatString(0, 18 * 11, 0xFFFFFF, "Miss    %d", m_nCount_Miss);
		DrawFormatString(0, 18 * 13, 0xFFFFFF, "Combo   %d (Max %d)", m_nCount_Combo, m_nCount_MaxCombo);
		if (m_bAutoPlay)
			DrawString(180, 0, "[Auto Play]", 0xFFFFFF);
	}

}

void CMusicMan::Draw(int hGraph){
	ClearDrawScreen();
	SetCameraPositionAndTarget_UpVecY(VGet(APP_WNDSX / 2, -55, -500), VGet(APP_WNDSX / 2, 800, 0));
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);

	//背景描画
	if (m_MusicInfo.Music.bMV) {
		DrawExtendGraph(0, 0, APP_WNDSX, APP_WNDSY, m_hBGM, FALSE);
	}
	else {
		DrawExtendGraph(0, 0, APP_WNDSX, APP_WNDSY, m_pSkin->BG, FALSE);
	}
	DrawExtendGraph(0, 0, APP_WNDSX, APP_WNDSY, m_pSkin->Lane, TRUE);

	size_t at = m_NoteAt;
	size_t i;
	size_t nDoubleNotes_Lane = 0;
	size_t nDoubleNotes_Tick = 0;
	int hnd;
	float x = 0, x2 = 0, y = 0, y2 = 0, sx = 0, sy = 0;

	//キー入力判定
	bool bKeyDown[7];
	ZeroMemory(bKeyDown, sizeof(bool) * 7);
	if (m_bEnableTouch) {
		int nTouch = m_Cti.GetCount();
		for (size_t i = 0; i < nTouch; i++) {
			for (size_t j = 0; j < 7; j++) {
				if (m_Cti.GetInfo()[i].pt.x >= m_notepos[j] - 164 / 2 && m_Cti.GetInfo()[i].pt.x < m_notepos[j] + 164 / 2 && m_Cti.GetInfo()[i].pt.y >= m_ptOriginW.y - 150 && m_Cti.GetInfo()[i].pt.y <= m_ptOriginW.y + 150) {
					bKeyDown[j] = true;
				}
			}
		}
	}
	else {
		bKeyDown[0] = CheckHitKey(KEY_INPUT_X);
		bKeyDown[1] = CheckHitKey(KEY_INPUT_C);
		bKeyDown[2] = CheckHitKey(KEY_INPUT_V);
		bKeyDown[3] = CheckHitKey(KEY_INPUT_B);
		bKeyDown[4] = CheckHitKey(KEY_INPUT_N);
		bKeyDown[5] = CheckHitKey(KEY_INPUT_M);
		bKeyDown[6] = CheckHitKey(KEY_INPUT_COMMA);
	}
	for (i = 0; i < 7; i++) {
		if (bKeyDown[i]) {
			DrawGraph3DEx(m_notepos[i] - 164 / 2,
				m_uHeight - m_ptOrigin.y + 164 / 2,
				70.0f,
				70.0f,
				164,
				m_uHeight / 4,
				m_pSkin->note.touch, TRUE);
		}
	}

	m_nCount_VisibleNotes = 0;
	while ((at < m_Notes.size()) && (m_Notes[at].nTick < (m_nTick + m_uVisibleTick))) {
		if (!m_Notes[at].bHide) {
			//Y座標計算
			y = m_uHeight - (m_ptOrigin.y - (float)(((m_Notes[at].nTick - m_nTick) * ((float)(m_uHeight - m_pSkin->note.Tap_size.y / 2) / (float)m_uVisibleTick))));
			//同じTickでタップの場合は、ダブルノーツとして扱う
			if ((nDoubleNotes_Lane > 0) && (nDoubleNotes_Tick == m_Notes[at].nTick) && (m_Notes[at].nType != NOTETYPE_LONG_POINT)) {
				DrawGraph3DEx(m_notepos[nDoubleNotes_Lane - 1], y + (m_pSkin->note.Tap_size.y - m_pSkin->note.Bar_size.y) / 2, 69.2f,
					m_notepos[m_Notes[at].nLane] - m_notepos[nDoubleNotes_Lane - 1], m_pSkin->note.Bar_size.y,
					m_pSkin->note.Bar, TRUE);
			}

			//種類によって画像を変える
			if ((m_Notes[at].nType == NOTETYPE_SKILL) || (m_Notes[at].nType == NOTETYPE_LONG_BEGIN_SKILL) || (m_Notes[at].nType == NOTETYPE_LONG_END_SKILL)) {
				hnd = m_pSkin->note.Skill;
			}
			else if (m_Notes[at].nType == NOTETYPE_LONG_POINT) {
				hnd = m_pSkin->note.Long_Point;
			}
			else if ((m_Notes[at].nType == NOTETYPE_LONG_BEGIN) || (m_Notes[at].nType == NOTETYPE_LONG_END)) {
				hnd = m_pSkin->note.Long;
			}
			else if ((m_Notes[at].nType == NOTETYPE_FLICK) || (m_Notes[at].nType == NOTETYPE_LONG_END_FLICK)) {
				hnd = m_pSkin->note.Flick;

				//フリックのガイド的なヤツを描画
				static int oz = 0;
				oz = (GetNowCount() % 400) / 10; //時間軸の変化に合わせてZ軸を動かす
				GetGraphSizeF(m_pSkin->note.Flickg, &sx, &sy);
				DrawGraph3DEx(m_notepos[m_Notes[at].nLane] - (m_pSkin->note.Tap_size.x - sx) / 2,
					y + sy,
					69.0f - sy - 30 - oz, 69.0f - 30 - oz,
					m_pSkin->note.Flickg, TRUE);
			}
			else
				hnd = m_pSkin->note.Tap;

			if ((m_Notes[at].nType == NOTETYPE_LONG_BEGIN) || (m_Notes[at].nType == NOTETYPE_LONG_BEGIN_SKILL) || (m_Notes[at].nType == NOTETYPE_LONG_POINT)) {

				//ロングの補間ラインを描画
				x = m_notepos[m_Notes[at].nLane] - (m_pSkin->note.Tap_size.x - m_pSkin->note.Longg_size.x) / 2;
				x2 = m_notepos[m_Notes[m_Notes[at].nLongEndIndex].nLane] - (m_pSkin->note.Tap_size.x - m_pSkin->note.Longg_size.x) / 2;
				y2 = m_uHeight - (m_ptOrigin.y - (float)(((m_Notes[m_Notes[at].nLongEndIndex].nTick - m_nTick) * ((float)(m_uHeight - m_pSkin->note.Tap_size.y / 2) / (float)m_uVisibleTick))));
				BOXF box[4] = { BoxGet(x2, y2 + (m_pSkin->note.Tap_size.y - m_pSkin->note.Bar_size.y) / 2, 69.1f),
					BoxGet(x2 + m_pSkin->note.Longg_size.x, y2 + (m_pSkin->note.Tap_size.y - m_pSkin->note.Bar_size.y) / 2, 69.1f),
					BoxGet(x + m_pSkin->note.Longg_size.x, y + m_pSkin->note.Tap_size.y / 2, 69.1f),
					BoxGet(x, y + m_pSkin->note.Tap_size.y / 2, 69.1f) };
				DrawGraph3DEx(box, m_pSkin->note.Longg, TRUE);
			}

			//ノーツを描画
			DrawGraph3DEx(m_notepos[m_Notes[at].nLane] - m_pSkin->note.Tap_size.x / 2,
				y,
				69.0f,
				hnd, TRUE);

			if (m_Notes[at].nType != NOTETYPE_LONG_POINT) {
				nDoubleNotes_Lane = m_Notes[at].nLane + 1;
				nDoubleNotes_Tick = m_Notes[at].nTick;
			}
		}
		if (m_Notes[at].nTick < m_nTick - APP_TICK_MEASURE) {
			m_NoteAt = at + 1; //開始位置を設定
		}
		if (m_Notes[at].nTick < m_nTick) {
			m_NoteCount = at + 1;
		}
		at++;
		m_nCount_VisibleNotes++;

	}

	float a = 0;
	static int tm_lgn = GetNowCount();
	int tm_lgn2 = GetNowCount();
	//開始ロングノーツが消えた時のための補間ライン描画処理
	for (i = 0; i < m_LongNoteData.size(); i++) {
		//開始点の座標を求める
		x = m_notepos[m_Notes[m_LongNoteData[i].nLongBeginIndex].nLane];
		y = m_uHeight - (m_ptOrigin.y - (float)(((m_Notes[m_LongNoteData[i].nLongBeginIndex].nTick - m_nTick) * ((float)m_uHeight / (float)m_uVisibleTick))));
		x2 = m_notepos[m_Notes[m_LongNoteData[i].nLongEndIndex].nLane];
		y2 = m_uHeight - (m_ptOrigin.y - (float)(((m_Notes[m_LongNoteData[i].nLongEndIndex].nTick - m_nTick) * ((float)m_uHeight / (float)m_uVisibleTick))));
		if ((x2 - x) != 0.0f) {
			a = (y2 - y) / (x2 - x);
			x = (m_uHeight - m_ptOrigin.y - y) / a + m_notepos[m_Notes[m_LongNoteData[i].nLongBeginIndex].nLane];
			y = m_uHeight - m_ptOrigin.y;
		}
		else {
			y = m_uHeight - m_ptOrigin.y;
		}

		//画像を変形して描画
		BOXF box[4] = { BoxGet(x2 - (m_pSkin->note.Tap_size.x - m_pSkin->note.Longg_size.x) / 2, y2 + m_pSkin->note.Tap_size.y / 2, 69.1f),
			BoxGet(x2 + m_pSkin->note.Longg_size.x - (m_pSkin->note.Tap_size.x - m_pSkin->note.Longg_size.x) / 2, y2 + m_pSkin->note.Tap_size.y / 2, 69.1f),
			BoxGet(x + m_pSkin->note.Longg_size.x - (m_pSkin->note.Tap_size.x - m_pSkin->note.Longg_size.x) / 2, y + m_pSkin->note.Tap_size.y / 2, 69.1f),
			BoxGet(x - (m_pSkin->note.Tap_size.x - m_pSkin->note.Longg_size.x) / 2, y + m_pSkin->note.Tap_size.y / 2, 69.1f) };
		DrawGraph3DEx(box, m_pSkin->note.Longg, TRUE);

		DrawGraph3DEx(x - m_pSkin->note.Tap_size.x / 2,
			y,
			69.1f,
			m_pSkin->note.Long, TRUE);
		if (tm_lgn2 - tm_lgn > 100){
			AnimationData adata;
			adata.nCount = 0;
			adata.x = x;
			adata.y = m_ptOrigin.y;
			adata.z = m_pSkin->EffectColor.Long;
			adata.app = 1;
			m_TapAnimation.push_back(adata);
		}
	}
	if (tm_lgn2 - tm_lgn > 100) {
		tm_lgn = tm_lgn2;
	}

	//タップエフェクトをまとめて描画する
	for (i = 0; i < m_TapAnimation.size(); i++) {
		x = 180 * (Easing::easeOutQuad(m_TapAnimation[i].nCount / SecToFrameD(0.25)) + 1);
		y = 180 * (Easing::easeOutQuad(m_TapAnimation[i].nCount / SecToFrameD(0.25)) + 1);
		DrawGraph3DEx(m_TapAnimation[i].x - x / 2,
			m_uHeight - (m_TapAnimation[i].y - (m_pSkin->note.Tap_size.y - y) / 2),
			69.0f,
			69.0f,
			x,
			y,
			m_pSkin->note.TapEffect, TRUE, 255, 255, 255, 200 - Easing::easeInCubic(m_TapAnimation[i].nCount / SecToFrameD(0.25))*200);

		if (m_TapAnimation[i].app == 0) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - Easing::easeOutQuad(m_TapAnimation[i].nCount / SecToFrameD(0.25)) * 255);
			x = m_TapAnimation[i].x - m_pSkin->note.TouchEffect_size.x / 2;
			y = m_ptOriginW.y - m_pSkin->note.TouchEffect_size.y;
			DrawExtendGraph(x, y - Easing::easeOutQuad(m_TapAnimation[i].nCount / SecToFrameD(0.25)) * 40,
				x + m_pSkin->note.TouchEffect_size.x,
				y + m_pSkin->note.TouchEffect_size.y,
				m_pSkin->note.TouchEffect, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - Easing::easeInCubic(m_TapAnimation[i].nCount / SecToFrameD(0.25)) * 255);
			x = m_TapAnimation[i].x - m_pSkin->note.TouchEffect2_size.x / 2;
			y = m_ptOriginW.y - m_pSkin->note.TouchEffect2_size.y / 2;
			DrawGraph(x, y,	m_pSkin->note.TouchEffect2, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		m_TapAnimation[i].nCount++;
	}

	{
		static char str[0x101];
		switch (m_nJudgement)
		{
		case 1:
			strcpy_s(str, 0x100, "PERFECT");
			break;
		case 2:
			strcpy_s(str, 0x100, "GREAT");
			break;
		case 3:
			strcpy_s(str, 0x100, "GOOD");
			break;
		case 4:
			strcpy_s(str, 0x100, "BAD");
			break;
		case 5:
			strcpy_s(str, 0x100, "MISS");
			break;
		}
		if (m_nJudgement) {
			//判定を表示
			float zx = max(Easing::easeOutBack(min(m_caJudgement.GetCount() / SecToFrameD(0.1), 1.0)), 0.9);
			x = (m_uWidth - GetDrawStringWidthToHandle(str, strlen(str), m_hFont_Judgement)*zx) / 2;
			y = (m_ptOriginW.y - 45 - 36 * zx / 2);
			DrawExtendStringToHandle(x, y, zx, zx, str, 0xFFFFFF, m_hFont_Judgement, 0x808080);
		}

		if (m_nCount_Combo) {
			//コンボ数
			sprintf_s(str, 0x100, "%d", m_nCount_Combo);
			float zx = max(Easing::easeOutBack(min(m_caJudgement.GetCount() / SecToFrameD(0.1), 1.0)), 0.9);
			x = m_uWidth - 300 - GetDrawStringWidthToHandle(str, strlen(str), m_hFont_ComboCount)*zx / 2;
			y = 300 - 72 * zx / 2;
			DrawExtendStringToHandle(x, y, zx, zx, str, 0xFFFFFF, m_hFont_ComboCount, 0x808080);

			strcpy_s(str, 0x100, "COMBO");
			x = m_uWidth - 300 - GetDrawStringWidthToHandle(str, strlen(str), m_hFont_Combo)*zx / 2;
			y = 300 + 100 * zx / 2;

			DrawExtendStringToHandle(x, y, zx, zx, str, 0xFFFFFF, m_hFont_Combo, 0x808080);
		}
	}
	if (m_caJudgement.Update() > SecToFrameD(2)) {
		m_caJudgement.Pause();
		m_nJudgement = 0;
	}
	
}

void CMusicMan::Update() {
	size_t i = 0;
	//開始ロングノーツが消えた時のための補間ラインの処理
	for (i = 0; i < m_LongNoteData.size(); i++) {
		if ((m_Notes[m_LongNoteData[i].nLongEndIndex].nTick < m_nTick) || (m_Notes[m_LongNoteData[i].nLongEndIndex].bHide)) {
			m_LongNoteData.erase(m_LongNoteData.begin() + i); //見えなくなって要らなくなったやつは消す
			i--;
		}
	}

	//タップエフェクト
	for (i = 0; i < m_TapAnimation.size(); i++) {
		if (m_TapAnimation[i].nCount > SecToFrameD(0.25)) {
			m_TapAnimation.erase(m_TapAnimation.begin() + i);
			i--;
		}
	}
}

//当たり判定
void CMusicMan::Hittest(int64_t &nTick){
	size_t i = 0;
	size_t at = m_NoteAt;
	LongNoteData data;
	AnimationData adata;
	bool bPlay_Flick = false, bPlay_Tap = false, bPlay_Skill = false;

	bool bKeyHold[9], bKeyUp[9], bKeyDown[9];
	ZeroMemory(bKeyHold, sizeof(bool) * 9);
	ZeroMemory(bKeyUp, sizeof(bool) * 9);
	ZeroMemory(bKeyDown, sizeof(bool) * 9);
	if (m_bEnableTouch) {
		int nTouch = m_Cti.GetCount();
		for (size_t i = 0; i < nTouch; i++) {
			for (size_t j = 0; j < 7; j++) {
				if (m_Cti.GetInfo()[i].pt.x >= m_notepos[j] - 164 / 2 && m_Cti.GetInfo()[i].pt.x < m_notepos[j] + 164 / 2 && m_Cti.GetInfo()[i].pt.y >= m_ptOriginW.y - 150 && m_Cti.GetInfo()[i].pt.y <= m_ptOriginW.y + 150) {
					bKeyHold[j + 1] = true;
					bKeyDown[j + 1] = m_Cti.IsKeyDown(m_Cti.GetInfo()[i].id);
				}
			}
		}
		nTouch = m_Cti.GetCount_old();
		for (size_t i = 0; i < nTouch; i++) {
			for (size_t j = 0; j < 7; j++) {
				if (m_Cti.GetInfo()[i].pt.x >= m_notepos[j] - 164 / 2 && m_Cti.GetInfo()[i].pt.x < m_notepos[j] + 164 / 2 && m_Cti.GetInfo()[i].pt.y >= m_ptOriginW.y - 150 && m_Cti.GetInfo()[i].pt.y <= m_ptOriginW.y + 150) {
					bKeyUp[j + 1] = m_Cti.IsKeyUp(m_Cti.GetInfo_old()[i].id);
				}
			}
		}
	}
	else {
		bKeyHold[0] = m_Cki.KeyHeld(KEY_INPUT_Z);
		bKeyHold[1] = m_Cki.KeyHeld(KEY_INPUT_X);
		bKeyHold[2] = m_Cki.KeyHeld(KEY_INPUT_C);
		bKeyHold[3] = m_Cki.KeyHeld(KEY_INPUT_V);
		bKeyHold[4] = m_Cki.KeyHeld(KEY_INPUT_B);
		bKeyHold[5] = m_Cki.KeyHeld(KEY_INPUT_N);
		bKeyHold[6] = m_Cki.KeyHeld(KEY_INPUT_M);
		bKeyHold[7] = m_Cki.KeyHeld(KEY_INPUT_COMMA);
		bKeyHold[8] = m_Cki.KeyHeld(KEY_INPUT_COLON);
		bKeyUp[0] = m_Cki.KeyUp(KEY_INPUT_Z);
		bKeyUp[1] = m_Cki.KeyUp(KEY_INPUT_X);
		bKeyUp[2] = m_Cki.KeyUp(KEY_INPUT_C);
		bKeyUp[3] = m_Cki.KeyUp(KEY_INPUT_V);
		bKeyUp[4] = m_Cki.KeyUp(KEY_INPUT_B);
		bKeyUp[5] = m_Cki.KeyUp(KEY_INPUT_N);
		bKeyUp[6] = m_Cki.KeyUp(KEY_INPUT_M);
		bKeyUp[7] = m_Cki.KeyUp(KEY_INPUT_COMMA);
		bKeyUp[8] = m_Cki.KeyUp(KEY_INPUT_COLON);
		bKeyDown[0] = m_Cki.KeyDown(KEY_INPUT_Z);
		bKeyDown[1] = m_Cki.KeyDown(KEY_INPUT_X);
		bKeyDown[2] = m_Cki.KeyDown(KEY_INPUT_C);
		bKeyDown[3] = m_Cki.KeyDown(KEY_INPUT_V);
		bKeyDown[4] = m_Cki.KeyDown(KEY_INPUT_B);
		bKeyDown[5] = m_Cki.KeyDown(KEY_INPUT_N);
		bKeyDown[6] = m_Cki.KeyDown(KEY_INPUT_M);
		bKeyDown[7] = m_Cki.KeyDown(KEY_INPUT_COMMA);
		bKeyDown[8] = m_Cki.KeyDown(KEY_INPUT_COLON);
	}
	

	//ノーツの当たり判定
	while ((at < m_Notes.size()) && (m_Notes[at].nTick < (m_nTick + m_uVisibleTick))) {
		bool bOK = false;
		bool bKeyFlag = false;
		if (m_Notes[at].nType == NOTETYPE_LONG_POINT) {
			bKeyFlag = (bKeyHold[m_Notes[at].nLane] || bKeyHold[m_Notes[at].nLane + 1] || bKeyHold[m_Notes[at].nLane + 2]);
		}else if((m_Notes[at].nType == NOTETYPE_LONG_END) || (m_Notes[at].nType == NOTETYPE_LONG_END_FLICK) || (m_Notes[at].nType == NOTETYPE_LONG_END_SKILL)) {
			bKeyFlag = (bKeyUp[m_Notes[at].nLane] || bKeyUp[m_Notes[at].nLane + 1] || bKeyUp[m_Notes[at].nLane + 2]);
		}
		else {
			bKeyFlag = (bKeyDown[m_Notes[at].nLane] || bKeyDown[m_Notes[at].nLane + 1] || bKeyDown[m_Notes[at].nLane + 2]);
		}
		if ((m_bAutoPlay || ((!m_Notes[at].bTapped) && bKeyFlag)) && !m_Notes[at].bHide) {
			if (m_bAutoPlay) {
				if ((m_Notes[at].nTick <= nTick) && (m_Notes[at].nTick >= (nTick - APP_TICK_ERR_BAD))) {
					m_nCount_Perfect++;
					m_nCount_Combo++;
					if (m_nCount_Combo > m_nCount_MaxCombo) {
						m_nCount_MaxCombo = m_nCount_Combo;
					}

					m_caJudgement.Set(0, 1, CATYPE_ADD);
					m_nJudgement = 1;
					bOK = true;
				}
			}
			else {
				if ((m_Notes[at].nTick <= (nTick + APP_TICK_ERR_PERFECT)) && (m_Notes[at].nTick >= nTick - APP_TICK_ERR_PERFECT)) {
					m_nCount_Perfect++;
					m_nCount_Combo++;
					if (m_nCount_Combo > m_nCount_MaxCombo) {
						m_nCount_MaxCombo = m_nCount_Combo;
					}

					m_caJudgement.Set(0, 1, CATYPE_ADD);
					m_nJudgement = 1;
					bOK = true;
				}
				else if ((m_Notes[at].nType != NOTETYPE_LONG_POINT) && (m_Notes[at].nTick <= (nTick + APP_TICK_ERR_GREAT)) && (m_Notes[at].nTick >= nTick - APP_TICK_ERR_GREAT)) {
					m_nCount_Great++;
					m_nCount_Combo++;
					if (m_nCount_Combo > m_nCount_MaxCombo) {
						m_nCount_MaxCombo = m_nCount_Combo;
					}

					m_caJudgement.Set(0, 1, CATYPE_ADD);
					m_nJudgement = 2;
					bOK = true;
				}
				else if ((m_Notes[at].nType != NOTETYPE_LONG_POINT) && (m_Notes[at].nTick <= (nTick + APP_TICK_ERR_GOOD)) && (m_Notes[at].nTick >= nTick - APP_TICK_ERR_GOOD)) {
					m_nCount_Good++;
					m_nCount_Combo = 0;

					m_caJudgement.Set(0, 1, CATYPE_ADD);
					m_nJudgement = 3;
					bOK = true;
				}
				else if ((m_Notes[at].nType != NOTETYPE_LONG_POINT) && (m_Notes[at].nTick <= (nTick + APP_TICK_ERR_BAD)) && (m_Notes[at].nTick >= nTick - APP_TICK_ERR_BAD)) {
					m_nCount_Bad++;
					m_nCount_Combo = 0;

					m_caJudgement.Set(0, 1, CATYPE_ADD);
					m_nJudgement = 4;
					bOK = true;
				}
			}
			if (bOK) {
				bKeyDown[m_Notes[at].nLane] = false;
				bKeyDown[m_Notes[at].nLane + 1] = false;
				bKeyDown[m_Notes[at].nLane + 2] = false;
				bKeyUp[m_Notes[at].nLane] = false;
				bKeyUp[m_Notes[at].nLane + 1] = false;
				bKeyUp[m_Notes[at].nLane + 2] = false;
				bKeyHold[m_Notes[at].nLane] = false;
				bKeyHold[m_Notes[at].nLane + 1] = false;
				bKeyHold[m_Notes[at].nLane + 2] = false;
				m_Notes[at].bTapped = true;
				m_Notes[at].bHide = true;
				if ((m_Notes[at].nType == NOTETYPE_FLICK) || (m_Notes[at].nType == NOTETYPE_LONG_END_FLICK)) {
					if (!bPlay_Flick) {
						PlaySoundMem(m_pSE->Flick, DX_PLAYTYPE_BACK);
						bPlay_Flick = true;
					}
				}
				else if ((m_Notes[at].nType == NOTETYPE_SKILL) || (m_Notes[at].nType == NOTETYPE_LONG_BEGIN_SKILL) || (m_Notes[at].nType == NOTETYPE_LONG_END_SKILL)) {
					if (!bPlay_Skill) {
						PlaySoundMem(m_pSE->Skill, DX_PLAYTYPE_BACK);
						bPlay_Skill = true;
					}
				}
				else{
					if (!bPlay_Tap) {
						PlaySoundMem(m_pSE->Perfect, DX_PLAYTYPE_BACK);
						bPlay_Tap = true;
					}
				}

				//ロングノーツの補間ラインの処理
				if ((m_Notes[at].nType >= NOTETYPE_LONG_BEGIN) && (m_Notes[at].nType <= NOTETYPE_LONG_POINT)) { //開始
					data.nLongBeginIndex = at;
					data.nLongEndIndex = m_Notes[at].nLongEndIndex;
					data.nLane = m_Notes[at].nLane;
					data.nTick = m_Notes[at].nTick;
					m_LongNoteData.push_back(data);
				}
				adata.nCount = 0;
				adata.x = m_notepos[m_Notes[at].nLane];
				adata.y = m_ptOrigin.y;
				adata.app = 0;
				if ((m_Notes[at].nType == NOTETYPE_FLICK) || ((m_Notes[at].nType == NOTETYPE_LONG_END_FLICK))) {
					adata.z = m_pSkin->EffectColor.Flick;
				}
				else if ((m_Notes[at].nType >= NOTETYPE_LONG_BEGIN) && ((m_Notes[at].nType <= NOTETYPE_LONG_END))) {
					adata.z = m_pSkin->EffectColor.Long;
				}
				else if ((m_Notes[at].nType >= NOTETYPE_SKILL) && ((m_Notes[at].nType <= NOTETYPE_LONG_END_SKILL))) {
					adata.z = m_pSkin->EffectColor.Skill;
				}
				else {
					adata.z = m_pSkin->EffectColor.Tap;
				}
				m_TapAnimation.push_back(adata);
			}
		}
		else if ((!m_Notes[at].bMissed) && (!m_Notes[at].bHide) && (m_Notes[at].nTick < (m_nTick - APP_TICK_ERR_BAD))){
			m_nCount_Miss++;
			m_Notes[at].bMissed = 1;
			m_nCount_Combo = 0;

			m_caJudgement.Set(0, 1, CATYPE_ADD);
			m_nJudgement = 5;
		}
		at++;
	}
}

//