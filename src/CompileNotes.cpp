/*****

@Filename  : CompileNotes.cpp
@Author    : inonote
@Modified  : 2018/06/29
@First ed. : 2018/08/15
@Comment   : ���ʂ��R���p�C������\�[�X�R�[�h�B

*****/

#include "stdafx.h"
#include "CompileNotes.h"
#include <algorithm>

#define MAX_NOTEID	32

bool GetBMSData(const string &sLine, BMSData &Data);
int b36toi(const char *_String);
bool isNum(const char *_String);

bool CompileNotes(vector<NoteData>& NotesData, const string& sData, size_t nLen) {
	string sLine = "";
	vector<string> sParams;
	NoteData data;
	size_t nIndex;
	size_t nOldIndex;
	size_t nCount;

	size_t nLongNoteBeginIndex[MAX_NOTEID];
	ZeroMemory(nLongNoteBeginIndex, sizeof(size_t)*MAX_NOTEID);
	NotesData.clear();

	ZeroMemory(&data, sizeof(data));
	
	//��s���ǂݎ���Ă���
	//NOTE : 1920 Tick��1����

	for (size_t i = 0; i < nLen; i++) {
		if (sData[i] == 0x0D) {
		}
		else if (sData[i] != 0x0A) {
			sLine += sData[i];
		}
		else {
			if (sLine[0] == '#') {
				//Tab��؂�̃f�[�^�[�𕪉�
				sParams.clear();
				sLine += '\t';
				nIndex = 0;
				nOldIndex = 0;
				while ((nIndex = sLine.find('\t', nIndex)) != string::npos) {
					sParams.emplace_back(sLine.substr(nOldIndex + 1, nIndex - nOldIndex - 1));
					nOldIndex = nIndex;
					nIndex++;
				}
				ZeroMemory(&data, sizeof(data));
				data.nTick = atoi(sParams[0].c_str()) * APP_TICK_MEASURE + atoi(sParams[1].c_str()) * APP_TICK_ADJUST;
				for (size_t j = 2; j < sParams.size(); j++) {
					//@�ŕ���
					nIndex = 0;
					nOldIndex = -1;
					nCount = 0;
					sParams[j] += '@';
					while ((nIndex = sParams[j].find('@', nIndex)) != string::npos) {
						if (nCount == 0) {
							data.nType = atoi(sParams[j].substr(nOldIndex + 1, nIndex - nOldIndex - 1).c_str());
						}else if(nCount == 1) {
							data.nLane = atoi(sParams[j].substr(nOldIndex + 1, nIndex - nOldIndex - 1).c_str());
						}else if (nCount == 2) {
							size_t n = atoi(sParams[j].substr(nOldIndex + 1, nIndex - nOldIndex - 1).c_str());

							//�����O�m�[�c�̊J�n�ƏI���������N������
							if ((data.nType >= 4) && (data.nType <= 6)) { //�J�n
								if (n < MAX_NOTEID) {
									if (nLongNoteBeginIndex[n] > 0) {

										NotesData[nLongNoteBeginIndex[n] - 1].nLongEndIndex = NotesData.size();

									}
									nLongNoteBeginIndex[n] = NotesData.size() + 1;
								}
							}
							else if ((data.nType >= 7) && (data.nType <= 9)) { //�I��
								if (n < MAX_NOTEID) {
									if (nLongNoteBeginIndex[n] > 0) {
										NotesData[nLongNoteBeginIndex[n] - 1].nLongEndIndex = NotesData.size();
									}
									nLongNoteBeginIndex[n] = 0;
								}
							}
						}else{
							break;
						}
						nOldIndex = nIndex;
						nIndex++;
						nCount++;
					}
					//NotesData�Ƀv�b�V��
					NotesData.push_back(data);
				}
			}
			sLine = "";
		}

	}
	return true;
}

//BMS�t�H�[�}�b�g����ǂݍ���
bool CompileNotesFromBMS(vector<NoteData>& NotesData, const string& sData, size_t nLen) {
	string sLine = "";
	vector<string> sParams;
	NoteData data;
	size_t nIndex;
	BMSData bmsData;
	string bmsData_Notes;
	size_t n;
	char str[3];
	bmsData.pData = &bmsData_Notes;
	size_t nLongNoteBeginIndex[MAX_NOTEID];
	ZeroMemory(nLongNoteBeginIndex, sizeof(size_t)*MAX_NOTEID);
	NotesData.clear();

	ZeroMemory(&data, sizeof(data));
	
	//��s���ǂݎ���Ă���
	//NOTE : 1920 Tick��1����

	for (size_t i = 0; i < nLen; i++) {
		if (sData[i] == 0x0D) {
		}
		else if (sData[i] != 0x0A){
			sLine += sData[i];
		}
		else {
			if (GetBMSData(sLine, bmsData)) { //BMS�̃f�[�^�[���擾
				//�`�����l���ԍ���11����19�܂�
				if ((bmsData.nChannelNo >= 11) && (bmsData.nChannelNo <= 19)) {
					size_t uStep = APP_TICK_MEASURE / (bmsData_Notes.length() / 2); //�l�ƒl�̕��ʏ�̊Ԋu
					for (size_t j = 0; j < bmsData_Notes.length() / 2; j++) {
						//�m�[�c�̃C���f�b�N�X���擾
						str[0] = bmsData_Notes[j * 2];
						str[1] = bmsData_Notes[j * 2 + 1];
						str[2] = 0;
						nIndex = b36toi(str);
						if (nIndex > 0) {
							ZeroMemory(&data, sizeof(data));

							data.nTick = j * uStep + bmsData.nMeasNo * APP_TICK_MEASURE;
							data.nType = (nIndex - 1) % 10;
							data.nLane = bmsData.nChannelNo - 11;
							if (data.nLane >= 7)
								data.nLane -= 2;
							data.nID = (nIndex - 1) / 10;
							//NotesData�Ƀv�b�V��
							NotesData.push_back(data);
						}
					}
				}
			}
			sLine = "";
		}

	}

	//�\�[�g
	sort(NotesData.begin(), NotesData.end(),
		[](const NoteData& l, const NoteData& r) {
		return l.nTick == r.nTick ? l.nLane < r.nLane : l.nTick < r.nTick;});

	//�����O�m�[�c�̊J�n�ƏI���������N������
	for (auto itr = NotesData.begin(); itr != NotesData.end(); itr++) {
		if ((itr->nType >= 4) && (itr->nType <= 6)) { //�J�n
			if (itr->nID < MAX_NOTEID) {
				if (nLongNoteBeginIndex[itr->nID] > 0) {

					NotesData[nLongNoteBeginIndex[itr->nID] - 1].nLongEndIndex = (itr - NotesData.begin());

				}
				nLongNoteBeginIndex[itr->nID] = (itr - NotesData.begin()) + 1;
			}
		}
		else if ((itr->nType >= 7) && (itr->nType <= 9)) { //�I��
			if (itr->nID < MAX_NOTEID) {
				if (nLongNoteBeginIndex[itr->nID] > 0) {
					NotesData[nLongNoteBeginIndex[itr->nID] - 1].nLongEndIndex = (itr - NotesData.begin());
				}
				nLongNoteBeginIndex[itr->nID] = 0;
			}
		}
	}

	return true;
}

//36�i����int��
int b36toi(const char *_String) {
	int num = 0;
	while (*_String) {
		if ((*_String >= '0') && (*_String <= '9'))
			num = num * 36 + (*_String - '0');
		else if ((*_String >= 'a') && (*_String <= 'z'))
			num = num * 36 + (*_String - 'a' + 10);
		else if ((*_String >= 'A') && (*_String <= 'Z'))
			num = num * 36 + (*_String - 'A' + 10);
		_String++;
	}
	return num;
}

//���l������
bool isNum(const char *_String) {
	bool b = true;
	while (*_String) {
		if (!isdigit(*_String)) {
			b = false;
			break;
		}
		_String++;
	}
	return b;
}

//BMS�t�H�[�}�b�g�̃f�[�^�[�擾
bool GetBMSData(const string &sLine, BMSData &Data) {
	if (sLine[0] != '#')
		return false;
	if (!isNum(sLine.substr(1, 3).c_str())) //�������ǂ����̔���
		return false;
	Data.nMeasNo = atoi(sLine.substr(1, 3).c_str());
	Data.nChannelNo = atoi(sLine.substr(4, 2).c_str());
	*Data.pData = sLine.substr(7, string::npos);
	return true;
}