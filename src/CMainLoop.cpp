/*****

@Filename  : CMainLoop.cpp
@Author    : inonote
@Modified  : 2018/08/08
@First ed. : 2018/06/29
@Comment   : ���C�����[�v���̃\�[�X�t�@�C���ł��B

*****/

#include "stdafx.h"
#include "CMainLoop.h"
#include "MessageBox.h"

void FullScreen(int mode = -1) {
	static bool flag = false;
	static bool bZoomed = false;
	if (mode == 0 && !flag)
		return;
	if (mode == 1 && flag)
		return;

	if (flag) { //�E�B���h�E���[�h
		SendMessage(GetMainWindowHandle(), WM_SYSCOMMAND, SC_RESTORE, 0);
		SetWindowStyleMode(7);
		if (bZoomed)
			SendMessage(GetMainWindowHandle(), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		flag = false;
	}
	else {
		bZoomed = IsZoomed(GetMainWindowHandle());
		SetWindowLong(GetMainWindowHandle(), GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE);
		SendMessage(GetMainWindowHandle(), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		flag = true;
	}
}

//�^�u���b�g���[�h���`�F�b�N
bool IsTabletMode() {
	HKEY hKey;
	INT nVal = 0;
	DWORD dwSize = sizeof(nVal);
	if (RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS)
		return false;
	RegQueryValueEx(hKey, "TabletMode", NULL, NULL, (LPBYTE)&nVal, &dwSize);
	RegCloseKey(hKey);
	return (bool)nVal;
}

CMainLoop::~CMainLoop() {
	if (!cmm)
		delete cmm;
	if (!cchm)
		delete cchm;
	DeleteGraph(hBackBuffer);
	DeleteSoundMem(m_SE.Tap);
	DeleteSoundMem(m_SE.Perfect);
	DeleteSoundMem(m_SE.Good);
	DeleteSoundMem(m_SE.Long);
	DeleteSoundMem(m_SE.Flick);
	DeleteSoundMem(m_SE.Skill);

}

//�v���O���������s
void CMainLoop::Run(HINSTANCE hInst, HWND hMainWnd, bool bFullScreen, bool bUseTouchPanel) {
	FullScreen(bFullScreen);
	hBackBuffer = MakeScreen(APP_WNDSX, APP_WNDSY, TRUE);
	bool bF11Held = true;

	ChangeFontType(DX_FONTTYPE_EDGE);
	SetFontSpace(-2);

	m_hInstance = hInst;
	m_hWnd = hMainWnd;
	m_bUseTouchPanel = bUseTouchPanel;

	m_cFps.SetFps(APP_FPS);

	//�e�N�X�`���ǂݍ���
	LoadSE();
	LoadSkin("Default");

	ChooseMusic();
	//
	//���C�����[�v�J�n
	while (!m_bBreak) {
		if (ProcessMessage() == -1 || GetWindowUserCloseFlag() == TRUE) {
			if (ct == CT_PLAY && cmm) {
				//���t���̏ꍇ�͋����I�ɍĐ����~���Ȃ��ƁA�v���Z�X���c�����܂܂ɂȂ�B
				delete cmm; //�f�X�g���N�^���ĂсA�C���X�^���X��j���B
				cmm = NULL;
			}

			m_bBreak = true;
			break;
		}
		
		m_cFps.Update();
		if (CheckHitKey(KEY_INPUT_F11)) {
			if (!bF11Held)
				FullScreen();
			bF11Held = true;
		}
		else {
			bF11Held = false;
		}
		Draw();
		m_cFps.Wait();
	}


}

//��ʂɕ`��
void CMainLoop::Draw() {
	SetDrawBright(255, 255, 255);
	SetDrawScreen(hBackBuffer);
	
	switch (ct)
	{
	case CMainLoop::CT_CHOOSEMUSIC:
		if (cchm) {
			cchm->MainLoop(hBackBuffer);

			if (CheckHitKey(KEY_INPUT_F1)) {
				DxMesageBox("Thumbthm v0.2.0.0\ntwitter:@inonote\n\n���̃\�t�g�E�F�A�͈ȉ��̃��C�u�������g�p���Ă��܂��B�ڍׂ͖{�\�t�g�̐��������������������B\nDX���C�u���� / libjpeg / libtiff / libpng / zlib / Bullet / ogg_static / vorbis_static / vorbisfile_static / libtheora_static / Opus audio codec / Opusfile / Mersenne Twister", "About", 0, hBackBuffer, m_cFps, true);
			}
			if (CheckHitKey(KEY_INPUT_F10)) {
				m_bBreak = true;
			}
			if (cchm->GetIndex() >= 0) {
				char buf[MAX_PATH + 1];
				sprintf_s(buf, MAX_PATH, "%s%s\\", APP_PATH_MUSIC, cchm->GetName()->c_str());
				int nLv = cchm->GetLevel();
				if (LoadMusic(buf, nLv)) {
					DxMesageBox("�w�肵�����x���̕��ʂ�������܂���ł����B", "�G���[", 0, hBackBuffer, m_cFps);
					ChooseMusic();
				}
				else {
					delete cchm;
					cchm = NULL;
				}
			}
		}
		break;
	case CMainLoop::CT_PLAY:
		if (cmm) {
			cmm->MainLoop(hBackBuffer);
			if (CheckHitKey(KEY_INPUT_ESCAPE)) {
				cmm->Pause();
				if (DxMesageBox("���t�𒆎~���ċȑI����ʂɖ߂�܂���?", "�ꎞ��~", MB_YESNO, hBackBuffer, m_cFps) == IDYES){
					delete cmm;
					cmm = NULL;
					ChooseMusic();
					break;
				}
				else
					cmm->Continue();
			}
			if (cmm->GetState() == 0) { //�����I��
				ResultData rd;
				cmm->GetResultData(&rd);
				delete cmm;
				cmm = NULL;
				ChooseMusic();
				//ShowResult(&rd);
				break;
			}
		}
		break;
	case CMainLoop::CT_RESULT:
		if (crw) {
			crw->MainLoop(hBackBuffer);
			if (CheckHitKey(KEY_INPUT_ESCAPE)) {
				delete crw;
				crw = NULL;
				ChooseMusic();
			}
		}
		break;
	case CMainLoop::CT_SETTING:
		break;
	}

	m_cFps.Draw();
	SetDrawScreen(DX_SCREEN_FRONT);
	DrawGraph(0, 0, hBackBuffer, 0);
	//ScreenFlip();
}

//�X�L���܂Ƃ߂ēǂݍ���
void CMainLoop::LoadSkin(const char* SkinName) {
	char fpath[MAX_PATH + 1];
	sprintf_s(fpath, MAX_PATH, "%s%s\\bg.png", APP_PATH_SKIN, SkinName);
	m_Skin.BG = LoadGraph(fpath);
	sprintf_s(fpath, MAX_PATH, "%s%s\\lane.png", APP_PATH_SKIN, SkinName);
	m_Skin.Lane = LoadGraph(fpath);
	sprintf_s(fpath, MAX_PATH, "%s%s\\note1.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.Tap = LoadGraph(fpath);
	GetGraphSize(m_Skin.note.Tap, (int*)&m_Skin.note.Tap_size.x, (int*)&m_Skin.note.Tap_size.y);
	sprintf_s(fpath, MAX_PATH, "%s%s\\note2.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.Skill = LoadGraph(fpath);
	GetGraphSize(m_Skin.note.Tap, (int*)&m_Skin.note.Tap_size.x, (int*)&m_Skin.note.Tap_size.y);
	sprintf_s(fpath, MAX_PATH, "%s%s\\long1.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.Long = LoadGraph(fpath);
	sprintf_s(fpath, MAX_PATH, "%s%s\\long2.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.Long_Point = LoadGraph(fpath);
	sprintf_s(fpath, MAX_PATH, "%s%s\\flick.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.Flick = LoadGraph(fpath);
	sprintf_s(fpath, MAX_PATH, "%s%s\\flickg.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.Flickg = LoadGraph(fpath);
	sprintf_s(fpath, MAX_PATH, "%s%s\\bar.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.Bar = LoadGraph(fpath);
	GetGraphSize(m_Skin.note.Bar, (int*)&m_Skin.note.Bar_size.x, (int*)&m_Skin.note.Bar_size.y);
	sprintf_s(fpath, MAX_PATH, "%s%s\\longg.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.Longg = LoadGraph(fpath);
	GetGraphSize(m_Skin.note.Longg, (int*)&m_Skin.note.Longg_size.x, (int*)&m_Skin.note.Longg_size.y);
	sprintf_s(fpath, MAX_PATH, "%s%s\\tapeffect.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.TapEffect = LoadGraph(fpath);
	GetGraphSize(m_Skin.note.TapEffect, (int*)&m_Skin.note.TapEffect_size.x, (int*)&m_Skin.note.TapEffect_size.y);
	sprintf_s(fpath, MAX_PATH, "%s%s\\toucheffect.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.TouchEffect = LoadGraph(fpath);
	GetGraphSize(m_Skin.note.TouchEffect, (int*)&m_Skin.note.TouchEffect_size.x, (int*)&m_Skin.note.TouchEffect_size.y);
	sprintf_s(fpath, MAX_PATH, "%s%s\\toucheffect2.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.TouchEffect2 = LoadGraph(fpath);
	GetGraphSize(m_Skin.note.TouchEffect2, (int*)&m_Skin.note.TouchEffect2_size.x, (int*)&m_Skin.note.TouchEffect2_size.y);
	sprintf_s(fpath, MAX_PATH, "%s%s\\touch.png", APP_PATH_SKIN, SkinName);
	m_Skin.note.touch = LoadGraph(fpath);

	sprintf_s(fpath, MAX_PATH, "%s%s\\info.ini", APP_PATH_SKIN, SkinName);
	m_Skin.EffectColor.Tap = GetPrivateProfileInt("EffectColor", "Tap", 0xFFFFFFFF, fpath);
	m_Skin.EffectColor.Skill = GetPrivateProfileInt("EffectColor", "Skill", 0xFFFFFFFF, fpath);
	m_Skin.EffectColor.Long = GetPrivateProfileInt("EffectColor", "Long", 0xFFFFFFFF, fpath);
	m_Skin.EffectColor.Flick = GetPrivateProfileInt("EffectColor", "Flick", 0xFFFFFFFF, fpath);

	sprintf_s(fpath, MAX_PATH, "%sDxLogo.png", APP_PATH);
	m_Skin.DxLogo = LoadGraph(fpath);
	
}

//SE�ǂݍ���
void CMainLoop::LoadSE() {
	m_SE.Tap = LoadSoundMem("data\\SE\\tap.wav");
	m_SE.Perfect = LoadSoundMem("data\\SE\\perfect.wav");
	m_SE.Good = LoadSoundMem("data\\SE\\good.wav");
	m_SE.Long = LoadSoundMem("data\\SE\\long.wav");
	m_SE.Flick = LoadSoundMem("data\\SE\\flick.wav");
	m_SE.Skill = LoadSoundMem("data\\SE\\skill.wav");
	ChangeVolumeSoundMem(200, m_SE.Tap);
	ChangeVolumeSoundMem(200, m_SE.Perfect);
	ChangeVolumeSoundMem(200, m_SE.Good);
	ChangeVolumeSoundMem(80, m_SE.Long);
	ChangeVolumeSoundMem(125, m_SE.Flick);
	ChangeVolumeSoundMem(125, m_SE.Skill);
}

//���y�ǂݍ���
bool CMainLoop::LoadMusic(const char* FolderPath, int nLv) {
	ct = CT_PLAY;
	cmm = new CMusicMan(FolderPath, nLv, &m_Skin, &m_SE, m_bUseTouchPanel);

	//�G���[����
	if (cmm->GetError()) {
		ct = CT_CHOOSEMUSIC;
		delete cmm;
		cmm = NULL;
		return true;
	}
	return false;
}

//�ȑI��
void CMainLoop::ChooseMusic() {
	ct = CT_CHOOSEMUSIC;
	if (!cchm)
		cchm = new CChooseMusic(&m_Skin, &m_SE);;
}

//���ʕ\��
void CMainLoop::ShowResult(ResultData* pRd) {
	ct = CT_RESULT;
	if (!crw)
		crw = new CResultWnd(pRd);;
}
