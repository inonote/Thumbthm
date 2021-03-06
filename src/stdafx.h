// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: プログラムに必要な追加ヘッダーをここで参照してください
#include "DxLib.h"
#include "fps.h"
#include "Structs.h"
#include <string>
#include <vector>
#include <iostream>
#include "CAnimation.h"

using namespace std;


//各種アプリケーション設定
#define APP_TITLE	"Thumbthm v0.3.5"
#define APP_WNDSX				1440
#define APP_WNDSY				810
#define APP_FPS					240
#define APP_DRAWFPS				60

#define APP_PATH_MUSIC			"data\\Musics\\"
#define APP_PATH_SKIN			"data\\Skins\\"
#define APP_PATH_SE				"data\\SE\\"
#define APP_PATH				"data\\"

#define APP_TICK_MEASURE		7680//1920
#define APP_TICK_ADJUST			4
#define APP_TICK_ERR_AUTO		(40 * APP_TICK_ADJUST)
#define APP_TICK_ERR_PERFECT	(80 * APP_TICK_ADJUST)
#define APP_TICK_ERR_GREAT		(120* APP_TICK_ADJUST)
#define APP_TICK_ERR_GOOD		(160* APP_TICK_ADJUST)
#define APP_TICK_ERR_BAD		(200* APP_TICK_ADJUST)

#define APP_LVCOL_EASY			0x4374F1
#define APP_LVCOL_NORMAL		0x5CF037
#define APP_LVCOL_HARD			0xFAC32F
#define APP_LVCOL_EXPERT		0xFE2A2E

#define LV_EASY					0
#define LV_NORMAL				1
#define LV_HARD					2
#define LV_EXPERT				3

#define APP_CMW_MARGINX				40		//余白X
#define APP_CMW_MARGINY				80		//余白Y
#define APP_CMW_SPACE				20		//曲一覧と詳細ウィンドウの隙間の幅
#define APP_CMW_INFOSX				400		//詳細ウィンドウの幅
#define APP_CMW_LIST_MARGINX		12		//曲一覧の余白X
#define APP_CMW_LIST_FONTHEIGHT		32		//曲一覧のフォントの高さ
#define APP_CMW_LIST_ITEMHEIGHT		64		//曲一覧の項目の高さ
#define APP_CMW_LIST_FONTHEIGHT_SEL	42		//曲一覧のフォントの高さ(選択)
#define APP_CMW_LIST_ITEMHEIGHT_SEL	72		//曲一覧の項目の高さ(選択)
#define APP_CMW_LIST_TEXTCOL		0xCCCCCC//曲一覧の文字列の色
#define APP_CMW_LIST_TEXTCOL_SEL	0xFFFFFF//曲一覧の文字列の色(選択)
#define APP_CMW_LIST_TEXTCOL_EDGE	0x202020//曲一覧の文字列の縁の色
#define APP_CMW_INFO_MARGINX_SEL	60		//詳細ウィンドウの余白X(選択)
#define APP_CMW_INFO_MARGINX		20		//詳細ウィンドウの余白X
#define APP_CMW_INFO_MARGINY		20		//詳細ウィンドウの余白Y
#define APP_CMW_INFO_IMGSIZEX		340		//詳細ウィンドウの画像の大きさX
#define APP_CMW_INFO_IMGSIZEY		340		//詳細ウィンドウの画像の大きさY
#define APP_CMW_INFO_FONTHEIGHT		14		//詳細ウィンドウのフォントの高さ
#define APP_CMW_INFO_ITEMHEIGHT		16		//詳細ウィンドウの項目の高さ
#define APP_CMW_INFO_SPHEIGHT		4		//詳細ウィンドウの項目のスペーサーの高さ
#define APP_CMW_INFO_TEXTCOL		0xFFFFFF//詳細ウィンドウの文字列の色
#define APP_CMW_INFO_TEXTCOL_EDGE	0x202020//詳細ウィンドウの文字列の縁の色
#define APP_CMW_STAT_FONTHEIGHT		24		//ステータスバーのフォントの高さ

#define SecToFrame(x) ((double)APP_FPS * x)
#define SecToFrameD(x) ((double)APP_DRAWFPS * x)