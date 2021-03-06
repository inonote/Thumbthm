// Thumbthm.cpp: アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Thumbthm.h"
#include "CMainLoop.h"

BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ret;
	ret = GetPrivateProfileInt("Main", "Flag", 0x11, "data\\Thumbthm.ini");
	ret = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_SETTING), HWND_DESKTOP, DlgProc, ret);
	if (ret == 0)
		return 0;
	WritePrivateProfileString("Main", "Flag", std::to_string(ret).c_str(), "data\\Thumbthm.ini");
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode((ret&0xF)!=1);
	SetWindowStyleMode(7);
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	SetMainWindowText(APP_TITLE);
	SetAlwaysRunFlag(TRUE);
	SetWindowSizeChangeEnableFlag(TRUE);
	SetMainWindowClassName(APP_TITLE);
	SetWaitVSyncFlag(FALSE);
	SetWindowIconID(IDI_THUMBTHM);
	SetWindowUserCloseEnableFlag(FALSE);
	SetGraphMode(APP_WNDSX, APP_WNDSY, 32);

	if (DxLib_Init() == -1)
		return -1;	

	Sleep(16);

	CMainLoop *cML = new CMainLoop; //メインループのクラスを作成

	//メインループ
	cML->Run(hInstance, GetMainWindowHandle(), (ret&0xF)==3, ((ret&0xF0) >> 4)!=1);
	delete cML;

	DxLib_End();

	return 0;
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp) {
	int nID;
	RECT rc;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_RADIO1+(lp&0xF)-1), BM_SETCHECK, TRUE, 0);
		SendMessage(GetDlgItem(hWnd, IDC_RADIO4+((lp&0xF0)>>4)-1), BM_SETCHECK, TRUE, 0);
		SetDlgItemText(hWnd, IDC_STATIC_VER, APP_TITLE);
		GetWindowRect(hWnd, &rc);
		SetWindowPos(hWnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - rc.right + rc.left) / 2, (GetSystemMetrics(SM_CYSCREEN) - rc.bottom + rc.top) / 2,
			0, 0, SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER);
		return TRUE;
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	case WM_COMMAND:
		switch (wp)
		{
		case IDOK:
			nID = (SendMessage(GetDlgItem(hWnd, IDC_RADIO1), BM_GETCHECK, 0, 0) |
				SendMessage(GetDlgItem(hWnd, IDC_RADIO2), BM_GETCHECK, 0, 0) * 2 |
				SendMessage(GetDlgItem(hWnd, IDC_RADIO3), BM_GETCHECK, 0, 0) * 3)&0xF;
			nID |= ((SendMessage(GetDlgItem(hWnd, IDC_RADIO4), BM_GETCHECK, 0, 0) |
				SendMessage(GetDlgItem(hWnd, IDC_RADIO5), BM_GETCHECK, 0, 0) * 2)&0xF) << 4;
			EndDialog(hWnd, nID);
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
	}
	return FALSE;
}