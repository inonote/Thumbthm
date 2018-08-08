#pragma once
#include <string>

typedef unsigned int uint;
typedef unsigned char uchar;

typedef struct {
	uint nLane;
	uint nTick;
	uint nType;
	uint nLongEndIndex;
	bool bTapped;
	bool bHide;
	bool bMissed;
} NoteData, *PNoteData;

typedef struct {
	uint nLane;
	uint nTick;
	uint nLongBeginIndex;
	uint nLongEndIndex;
	int nTouchID;
} LongNoteData;

typedef struct {
	uint uPerfect;
	uint uGreat;
	uint uGood;
	uint uBad;
	uint uMiss;
	uint uMaxCombo;
	uint uScore;
	bool bAutoPlay;
	uint uLvVal;
	uint uLv;
	std::string sTitle;
} ResultData;

typedef struct {
	int BG;
	int Lane;
	struct {
		int Tap;
		int Skill;
		int Long;
		int Long_Point;
		int Flick;
		POINT Tap_size;
		int Flickg;
		int Bar;
		POINT Bar_size;
		int Longg;
		POINT Longg_size;
		int TapEffect;
		POINT TapEffect_size;
		int TouchEffect;
		POINT TouchEffect_size;
		int TouchEffect2;
		POINT TouchEffect2_size;
		int touch;
	} note;
	struct {
		int Tap;
		int Skill;
		int Long;
		int Flick;
	} EffectColor;
	int DxLogo;
} SkinData;

typedef struct {
	int Tap;
	int Perfect;
	int Good;
	int Long;
	int Flick;
	int Skill;
} SEData;

typedef struct {
	float x;
	float y;
} POINTF;

typedef struct {
	float x;
	float y;
	float z;
} BOXF;

typedef struct {
	int nCount;
	int x;
	int y;
	int z;
	int app;
} AnimationData;

typedef struct {
	struct {
		int x, y;
	} pt;
	int id;
} TouchInfo;