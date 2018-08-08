#include <math.h>
#include "stdafx.h"
#pragma once

class Fps {
	int mStartTime;         //����J�n����
	int mCount;             //�J�E���^
	float mFps;             //fps
	static const int N = 30;//���ς����T���v����
	int FPS = 60;	//�ݒ肵��FPS

	int hFont = 0;

public:
	Fps() {
		mStartTime = 0;
		mCount = 0;
		mFps = 0;
	}
	Fps(int n) {
		mStartTime = 0;
		mCount = 0;
		mFps = 0;
		FPS = n;
	}

	void SetFps(int fps) {
		if (FPS != fps) {
			FPS = fps;
			mCount = 0;
			mStartTime = GetNowCount();
		}
	}

	int GetFps() {
		return FPS;
	}

	bool Update() {
		if (mCount == 0) { //1�t���[���ڂȂ玞�����L��
			mStartTime = GetNowCount();
		}
		if (mCount == N) { //60�t���[���ڂȂ畽�ς��v�Z����
			int t = GetNowCount();
			mFps = 1000.f / ((t - mStartTime) / (float)N);
			mCount = 0;
			mStartTime = t;
		}
		mCount++;
		return true;
	}

	void Draw() {
		DrawFormatString(0, 0, GetColor(255, 255, 255), "%.1f fps", mFps);
	}

	void Wait() {
		int tookTime = GetNowCount() - mStartTime;	//������������
		int waitTime = mCount * 1000 / FPS - tookTime;	//�҂ׂ�����
		if (waitTime > 0) {
			Sleep(waitTime);	//�ҋ@
		}
	}
};