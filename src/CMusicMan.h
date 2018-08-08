/*****

@Filename  : CMusicMan.cpp
@Author    : inonote
@Modified  : 2018/08/28
@First ed. : 2018/06/29
@Comment   : 楽曲を管理するヘッダコード。

*****/

#pragma once
#include "stdafx.h"
#include "CKeyInput.h"
#include "CTouchInput.h"
#include "CAnimation.h"


class CMusicMan
{
public:
	CMusicMan(const char* FolderPath, int Level, const SkinData* pSkin, const SEData* pSE, bool bUseTouchPanel);
	~CMusicMan();

	//エラーを取得
	inline int GetError() {
		return m_nErrorID;
	};

	void MainLoop(int hGraph);
	void GetResultData(ResultData *pRd);

private:

	void Draw(int hGraph);
	void Update(void);
	void Hittest(int64_t &nTick);

	const SkinData* m_pSkin;
	const SEData* m_pSE;

	size_t m_uMusicIntroCount;
	CAnimation m_CAnim_MusicIntro;

	int m_nErrorID;
	string m_sData;
	size_t m_uDataSize;

	int m_nLv;

	int m_hBGM;
	int m_hMovie;

	bool m_bPause;

	CKeyInput m_Cki;
	CTouchInput m_Cti;

	size_t m_nCount_Miss;
	size_t m_nCount_Bad;
	size_t m_nCount_Good;
	size_t m_nCount_Great;
	size_t m_nCount_Perfect;

	size_t m_nCount_Combo;
	size_t m_nCount_MaxCombo;
	size_t m_nCount_VisibleNotes;

	int m_nJudgement;
	CAnimation m_caJudgement;
	bool m_bAutoPlay, m_bEverAutoPlay;

	//楽曲データー
	struct {
		struct {
			string sTitle;
			string sBand;
		} Main;
		struct {
			string sWaveFilename;
			string sMovieFilename;
			double dAdjust;
			int nBPM, nOffset;
			bool bMV;
		} Music;
		struct {
			string sLyrics;
			string sMusic;
			string sArranged;
		} Authors;
		int nLv;
		int nLvVal;

		int hImg;
	} m_MusicInfo;

	vector<NoteData> m_Notes;
	vector<LongNoteData> m_LongNoteData;
	vector<AnimationData> m_TapAnimation;

	size_t m_NoteCount;
	size_t m_NoteAt;

	//描画関係
	int m_hGBuffer;

	int m_hFont_Judgement;
	int m_hFont_ComboCount;
	int m_hFont_Combo;
	int m_hFont_Title;
	int m_hFont_Authors;

	int64_t m_nTick;
	ULONGLONG m_uStartTime;
	uint m_uWidth, m_uHeight;
	POINT m_ptOrigin;
	POINT m_ptOriginW;

	bool m_bShowDebugMsg;

	//ノーツの設定
	uint m_uVisibleTick;
	uint m_notepos[7] = {
		222,
		388,
		554,
		720,
		886,
		1052,
		1218
	};

	bool m_bEnableTouch;

public:
	//状態を調べる
	inline int GetState() {
		if (m_uMusicIntroCount <= SecToFrame(4))
			return 1;
		if (m_MusicInfo.Music.bMV)
			return GetMovieStateToGraph(m_hBGM) && !m_bPause;
		else
			return CheckSoundMem(m_hBGM) && !m_bPause;
	}

	inline bool Pause() {
		if (m_uMusicIntroCount < SecToFrame(4))
			return true;
		if (!m_bPause) {
			m_bPause = true;
			if (m_MusicInfo.Music.bMV)
				PauseMovieToGraph(m_hBGM);
			else
				StopSoundMem(m_hBGM);

			StopSoundMem(m_pSE->Long);
		}
		return true;
	}

	inline bool Continue() {
		if (m_uMusicIntroCount < APP_FPS * 4)
			return true;
		if (m_bPause) {
			if (m_MusicInfo.Music.bMV) {
				PlayMovieToGraph(m_hBGM);
			}
			else {
				PlaySoundMem(m_hBGM, DX_PLAYTYPE_BACK, FALSE);
				ChangeMovieVolumeToGraph(168, m_hBGM);
			}
			m_bPause = false;
		}
		return false;
	}

#define NOTETYPE_SINGLE				0
#define NOTETYPE_FLICK				1
#define NOTETYPE_SKILL				2
#define NOTETYPE_LONG_BEGIN			4
#define NOTETYPE_LONG_BEGIN_SKILL	5
#define NOTETYPE_LONG_POINT			6
#define NOTETYPE_LONG_END			7
#define NOTETYPE_LONG_END_FLICK		8
#define NOTETYPE_LONG_END_SKILL		9

};

