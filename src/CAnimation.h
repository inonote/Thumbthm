#pragma once
#include "stdafx.h"

class CAnimation
{
private:
	double m_Count;
	double m_Val;
	double m_Max;
	int m_nType;
	bool m_bPause = false;

public:
#define CATYPE_ADD	0
#define CATYPE_SUB	1
#define CATYPE_MUL	2
#define CATYPE_DIV	3
	CAnimation() {};
	CAnimation(double count, double val, int type) {
		m_Count = count;
		m_Val = val;
		m_nType = type;
		m_bPause = false;
	};
	inline void Set(double count, double val, int type) {
		m_Count = count;
		m_Val = val;
		m_nType = type;
		m_bPause = false;
	};
	inline void Set(double count, double val, int type, double max) {
		m_Count = count;
		m_Val = val;
		m_nType = type;
		m_Max = max;
		m_bPause = false;
	};
	inline double Update() {
		if (!m_bPause) {
			if (m_nType == CATYPE_SUB)
				m_Count -= m_Val;
			else if (m_nType == CATYPE_MUL)
				m_Count *= m_Val;
			else if (m_nType == CATYPE_DIV)
				m_Count *= m_Val;
			else
				m_Count += m_Val;
		}
		if (m_Max != 0.0 && abs(m_Count)>=abs(m_Max))
			Pause();
		return m_Count;
	};
	inline double Update(double count) {
		if (!m_bPause)
			m_Count += count;
		if (m_Max != 0.0 && abs(m_Count) >= abs(m_Max))
			Pause();
		return m_Count;
	};
	inline double GetCount() {
		return m_Count;
	};
	inline double GetP() {
		if (m_Max == 0.0)
			return 0;
		return (m_Count / m_Max);
	};
	inline void Pause() {
		m_bPause = true;
	};
	inline bool IsPause() {
		return m_bPause;
	};
	inline void Continue() {
		m_bPause = false;
	};

};

