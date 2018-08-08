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

	static int m_nMusicLevel;		//�Ȃ̃��x��
	static int m_nMusicIndex;		//�I�𒆂̋Ȃ̃C���f�b�N�X
	int m_nMusicMax;				//�Ȃ̐�
	int m_nMusicMax_Visible;		//�����Ă���Ȃ̐�
	
	CKeyInput m_Cki;

	//�y�ȃf�[�^�[
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

	//�`��֌W
	int m_hFont_StatusBar;
	int m_hFont_MusicList;
	int m_hFont_MusicList_S;
	int m_hFont_MusicInfo;

	int m_hBackBuffer;
	CAnimation m_CAnim_Wnd;		//�E�B���h�E�̃A�j���[�V����
	CAnimation m_CAnim_List;	//�Ȉꗗ�̃A�j���[�V����
	bool m_anim_List_Way;

	int m_Ret;


};