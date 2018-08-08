#include <math.h>
#include "stdafx.h"
#pragma once

class Fps {
	int mStartTime;         //測定開始時刻
	int mCount;             //カウンタ
	float mFps;             //fps
	static const int N = 30;//平均を取るサンプル数
	int FPS = 60;	//設定したFPS

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
		if (mCount == 0) { //1フレーム目なら時刻を記憶
			mStartTime = GetNowCount();
		}
		if (mCount == N) { //60フレーム目なら平均を計算する
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
		int tookTime = GetNowCount() - mStartTime;	//かかった時間
		int waitTime = mCount * 1000 / FPS - tookTime;	//待つべき時間
		if (waitTime > 0) {
			Sleep(waitTime);	//待機
		}
	}
};