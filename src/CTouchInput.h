#pragma once
#include "stdafx.h"

class CTouchInput
{
public:
	CTouchInput() {
		m_noldTouchNum = 0;
		m_nTouchNum = 0;
	}
	void Update() {
		//ˆÈ‘O‚Ìó‘Ô‚ğ m_old_KeyFlag•Ï”‚É”ğ“ï‚³‚¹‚éB
		memcpy(m_cold_KeyFlag, m_cKeyFlag, 64 * sizeof(TouchInfo));
		m_noldTouchNum = m_nTouchNum;
		m_nTouchNum = min(GetTouchInputNum(), 63);
		for (size_t i = 0; i < m_nTouchNum; i++) {
			GetTouchInput(i, &m_cKeyFlag[i].pt.x, &m_cKeyFlag[i].pt.y, &m_cKeyFlag[i].id, NULL);
		}
	}

	inline int GetCount() {
		return m_nTouchNum;
	};

	inline int GetCount_old() {
		return m_noldTouchNum;
	};

	inline TouchInfo* GetInfo() {
		return m_cKeyFlag;
	};

	inline TouchInfo* GetInfo_old() {
		return m_cold_KeyFlag;
	};

	inline bool IsKeyDown(int nID) {
		int nIndex = IdToIndex(nID, m_cKeyFlag, m_nTouchNum);
		int nIndex2 = IdToIndex(nID, m_cold_KeyFlag, m_noldTouchNum);
		return ((nIndex != -1) && (nIndex2 == -1));
	};
	inline bool IsKeyUp(int nID) {
		int nIndex = IdToIndex(nID, m_cKeyFlag, m_nTouchNum);
		int nIndex2 = IdToIndex(nID, m_cold_KeyFlag, m_noldTouchNum);
		return ((nIndex == -1) && (nIndex2 != -1));
	};
	inline bool IsKeyHeld(int nID) {
		int nIndex = IdToIndex(nID, m_cKeyFlag, m_nTouchNum);
		return (nIndex != -1);
	}; 

private:
	TouchInfo m_cold_KeyFlag[64];
	TouchInfo m_cKeyFlag[64];
	int m_nTouchNum;
	int m_noldTouchNum;

	int IdToIndex(int ID, const TouchInfo* ti, int nMax) {
		int nIndex = -1;
		for (size_t i = 0; i < nMax; i++) {
			if (ti[i].id == ID) {
				nIndex = i;
				break;
			}
		}
		return nIndex;
	}
};