/*****

@Filename  : CChooseMusic.cpp
@Author    : inonote
@Modified  : 2018/08/08
@First ed. : 不明
@Comment   : 楽曲選択画面のソースコード。

*****/

#include "stdafx.h"
#include "CResultWnd.h"


CResultWnd::CResultWnd(ResultData* rd)
{
	m_Result = *rd;

}


CResultWnd::~CResultWnd()
{
}


void CResultWnd::MainLoop(int hGraph) {
	Draw(hGraph);
}

void CResultWnd::Draw(int hGraph) {

}

void CResultWnd::Update() {

}

void CResultWnd::Hittest() {

}