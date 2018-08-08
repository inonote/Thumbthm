/*****

@Filename  : CompileNotes.cpp
@Author    : inonote
@Modified  : 2018/06/29
@First ed. : 2018/06/29
@Comment   : ���ʂ��R���p�C������\�[�X�R�[�h�B

*****/

#include "stdafx.h"
#include "CompileNotes.h"

bool CompileNotes(vector<NoteData>& NotesData, const string& sData, size_t nLen) {
	string sLine = "";
	vector<string> sParams;
	NoteData data;
	size_t nIndex;
	size_t nOldIndex;
	size_t nCount;


#define MAX_NOTEID	32
	size_t nLongNoteBeginIndex[MAX_NOTEID];
	ZeroMemory(nLongNoteBeginIndex, MAX_NOTEID);
	NotesData.clear();

	ZeroMemory(&data, sizeof(data));
	
	//��s���ǂݎ���Ă���
	//NOTE : 1920 Tick��1����

	for (size_t i = 0; i < nLen; i++) {
		if (sData[i] != 0x0A){
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

										NotesData[nLongNoteBeginIndex[n]].nLongEndIndex = NotesData.size();

									}
									nLongNoteBeginIndex[n] = NotesData.size();
								}
							}
							else if ((data.nType >= 7) && (data.nType <= 9)) { //�I��
								if (n < MAX_NOTEID) {
									if (nLongNoteBeginIndex[n] > 0) {
										NotesData[nLongNoteBeginIndex[n]].nLongEndIndex = NotesData.size();
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