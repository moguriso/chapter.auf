//---------------------------------------------------------------------
//		�v���O�C���ݒ�w�b�_�t�@�C��
//---------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "filter.h"

#define NUMHIS 30	// �ۑ����闚���̐�
#define STRLEN 256	// ������̍ő咷

class CfgDlg;

typedef struct 
{
	// �`���v�^���͏�����ő�100�܂�
	int m_numChapter;
	int m_Frame[100];
	char m_strTitle[100][STRLEN];
} PrfDat;

typedef struct {
	int frame;
	std::string title;
} chapter;

class CfgDlg
{
	HWND m_hDlg;
	FILTER *m_fp;
	EXFUNC *m_exfunc;
	void *m_editp;
	int m_scale;
	int m_rate;
	int m_frame;
	int m_numFrame;
	char m_strTime[STRLEN];
	char m_strHis[NUMHIS][STRLEN];
	int m_numHis;
	bool m_loadfile;
	int m_autosave;

	void AddHis();

public:
	int m_numChapter;
	int m_Frame[100];
	char m_strTitle[100][STRLEN];

	void ShowList();
	void Init(HWND hwnd,void *editp,FILTER *fp);
	void SetFrame(int frame);
	void SetFps(int rate,int scale);
	void SetFrameN(void *editp,int frame_n);
	void AddList();
	void DelList();
	void Seek();
	void Save();
	void AutoSave();
	void Load();
	void AuotSaveCheck();

	//[ru]�֐��ǉ�
	void DetectMute();
	//�����܂�
};
