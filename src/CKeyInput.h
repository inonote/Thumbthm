#pragma once
#include "stdafx.h"

class CKeyInput
{
public:
	inline void Update() {
		//以前の状態を m_old_KeyFlag変数に避難させる。
		memcpy(m_cold_KeyFlag, m_cKeyFlag, 256 * sizeof(char));
		GetHitKeyStateAll(m_cKeyFlag);
	};
	inline bool KeyDown(int nKeyCode) {
		if (nKeyCode < 0 || nKeyCode > 255)
			return 0;
		return (m_cKeyFlag[nKeyCode] && !m_cold_KeyFlag[nKeyCode]);
	};
	inline bool KeyUp(int nKeyCode) {
		if (nKeyCode < 0 || nKeyCode > 255)
			return 0;
		return (!m_cKeyFlag[nKeyCode] && m_cold_KeyFlag[nKeyCode]);
	};
	inline bool KeyHeld(int nKeyCode) {
		if (nKeyCode < 0 || nKeyCode > 255)
			return 0;
		return m_cKeyFlag[nKeyCode];
	};

private:
	char m_cKeyFlag[256];
	char m_cold_KeyFlag[256];
};

