#include "stdafx.h"
#include "MessageBox.h"
#include "CAnimation.h"
#include "CKeyInput.h"
#include "Easing.h"

void DrawStringToHandleLF(int x, int y, int width, const char *String, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0U);

int DxMesageBox(const char* sMessage, const char* sTitle, int nType, int hGraph, Fps& fps, bool bSmallFont) {
	int hBackBuffer = MakeScreen(APP_WNDSX, APP_WNDSY, FALSE);
	int hMainBuffer;
	int nRet = -1;
	int px, py, sx, sy;
	bool bBreak = false;
	double aval;
	int hFont, hFontT, hFontS;

	CAnimation CAnim_Wnd; //ウィンドウのアニメーション
	CKeyInput Cki;
	Cki.Update();

	sx = 800;
	sy = 400;
	px = (APP_WNDSX - sx) / 2;
	py = (APP_WNDSY - sy) / 2;
	hMainBuffer = MakeScreen(sx, sy, TRUE);
	CAnim_Wnd.Set(0, 1, CATYPE_ADD, SecToFrame(0.5));
	if (!bSmallFont)
		hFont = CreateFontToHandle("メイリオ", 24, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	else
		hFont = CreateFontToHandle("メイリオ", 18, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	hFontT = CreateFontToHandle("メイリオ", 28, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	hFontS = CreateFontToHandle("メイリオ", 18, 6, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	SetDrawScreen(hBackBuffer);
	DrawGraph(0, 0, hGraph, FALSE);

	while (ProcessMessage() != -1 || GetWindowUserCloseFlag() == TRUE) {
		fps.Update();
		Cki.Update();
		CAnim_Wnd.Update();

		//キー入力判定
		if (!bBreak){
			if (nType == MB_YESNO) {
				if (Cki.KeyDown(KEY_INPUT_V)) {
					nRet = IDYES;
				}
				else if (Cki.KeyDown(KEY_INPUT_M)) {
					nRet = IDNO;
				}
				else if (Cki.KeyDown(KEY_INPUT_ESCAPE)) {
					nRet = IDNO;
				}
			}
			else {
				if (Cki.KeyDown(KEY_INPUT_SPACE)) {
					nRet = IDOK;
				}
				else if (Cki.KeyDown(KEY_INPUT_ESCAPE)) {
					nRet = IDOK;
				}
			}
			if (nRet != -1) {
				CAnim_Wnd.Set(0, 1, CATYPE_ADD, SecToFrame(0.5));
				bBreak = true;
			}
		}

		//描画

		SetDrawScreen(hMainBuffer);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
		DrawBox(0, 0, sx, sy, 0x303030, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawStringToHandleLF(40, 40, sx - 80, sTitle, 0xFFFFFF, hFontT, 0x202020);
		DrawStringToHandleLF(50, 90, sx - 100, sMessage, 0xFFFFFF, hFont, 0x202020);
		if (nType == MB_YESNO) {
			DrawStringToHandleLF(40, sy - 32, sx - 80, "V : はい   M : いいえ", 0xFFFFFF, hFontS, 0x202020);
		}else{
			DrawStringToHandleLF(40, sy - 32, sx - 80, "Space : 閉じる", 0xFFFFFF, hFontS, 0x202020);
		}

		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();

		aval = Easing::easeOutQuart(CAnim_Wnd.GetP());
		if (bBreak)
			aval = 1 - aval;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - 180 * aval);
		DrawGraph(0, 0, hBackBuffer, FALSE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * aval);
		if (CAnim_Wnd.IsPause()) {
			DrawGraph(px, py, hMainBuffer, TRUE);
		}
		else {
			if (bBreak && nRet != IDNO) {
				aval = Easing::easeOutQuart(CAnim_Wnd.GetP());
				DrawExtendGraphF( - 20 * aval + px, - 20 * aval + py,
					sx + 20 * aval + px, sy + 20 * aval + py,
					hMainBuffer, TRUE);
			}
			else {
				DrawExtendGraphF(20 - 20 * aval + px, 20 - 20 * aval + py,
					sx - 20 + 20 * aval + px, sy - 20 + 20 * aval + py,
					hMainBuffer, TRUE);
			}
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (bBreak && CAnim_Wnd.IsPause())
			break;

		fps.Draw();
		fps.Wait();
		ScreenFlip();
	}

	DeleteGraph(hBackBuffer);
	DeleteGraph(hMainBuffer);
	DeleteFontToHandle(hFont);
	DeleteFontToHandle(hFontT);
	DeleteFontToHandle(hFontS);
	return nRet;
}

//折り返して文字列を描画
void DrawStringToHandleLF(int x, int y, int width, const char *String, unsigned int Color, int FontHandle, unsigned int EdgeColor) {
	char wbuf[3] = { 0 };
	bool bWide = false;
	int px = x, py = y;
	int sx, sy, lc;
	const char *p = String;
	size_t n = 0;
	while (*p != '\0') {
		if (IsDBCSLeadByte(*p)) {
			wbuf[0] = *p;
			wbuf[1] = *(p + 1);
			DrawStringToHandle(px, py, wbuf, Color, FontHandle, EdgeColor);
			GetDrawStringSizeToHandle(&sx, &sy, &lc, wbuf, 2, FontHandle);
			px += sx;
			if (lc>1) {
				px = x;
				py += sy / 2;
			}
			else if ((width != -1) && (px - x) > width) {
				px = x;
				py += sy;
			}
			wbuf[0] = 0;
			wbuf[1] = 0;
			p++;
		}
		else
		{
			wbuf[0] = *p;
			wbuf[1] = 0;
			DrawStringToHandle(px, py, wbuf, Color, FontHandle, EdgeColor);
			GetDrawStringSizeToHandle(&sx, &sy, &lc, wbuf, 2, FontHandle);
			px += sx;
			if (lc>1) {
				px = x;
				py += sy / 2;
			}
			else if ((width != -1) && (px - x) > width) {
				px = x;
				py += sy;
			}
			wbuf[0] = 0;
			wbuf[1] = 0;
		}
		n++;
		p++;
	}
}

void CalcPos_TextCenter(POINT *out, const RECT *in, const char *text, int FontHandle) {
	int sx, sy, lc;
	GetDrawStringSizeToHandle(&sx, &sy, &lc, text, strlen(text), FontHandle);
	out->x = ((in->right - in->left - sx) / 2 + in->left);
	out->y = ((in->bottom - in->top - sy + GetFontLineSpaceToHandle(FontHandle) / 2) / 2 + in->top);
}
