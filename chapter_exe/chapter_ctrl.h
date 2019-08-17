#pragma once
#include <tchar.h>
#include <mutex>
#include "input.h"
#include <map>



class chapter_ctrl
{
	typedef struct foundChapter {
		int frame;
		TCHAR title[256];
		INPUT_INFO* vii;
	} F_CHAPTER;

private:
	short mMute = 50;
	int mSeri = 10;
	int mThreadCount = 1;
	TCHAR* mVideo = NULL;
	TCHAR* mAudio = NULL;
	TCHAR* mOutFile = NULL;
	std::mutex mMutex;
	std::map<int, F_CHAPTER*> mPoolData;

public:
	chapter_ctrl();
	~chapter_ctrl();
	int write(unsigned char* f, int nchap, int frame, TCHAR* title, INPUT_INFO* iip);
	void chapter_ctrl::setMute(short val);
	short chapter_ctrl::getMute();
	void chapter_ctrl::setSeri(int val);
	int chapter_ctrl::getSeri();
	void setVideo(TCHAR* val);
	TCHAR* getVideo();
	void setAudio(TCHAR* val);
	TCHAR* getAudio();
	bool isSameSource();
	int getAudioNameLen();
	int getVideoNameLen();
	void setOutFile(TCHAR* val);
	TCHAR* getOutFile();
	void setThreadCount(int val);
	int getThreadCount();
	bool stock(int pos, int frame, TCHAR* title, INPUT_INFO* iip);
	void writeOut(FILE* fout);
};

