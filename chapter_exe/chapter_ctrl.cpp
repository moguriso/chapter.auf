#include "stdafx.h"
#include "chapter_ctrl.h"

chapter_ctrl::chapter_ctrl() {
	this->mVideo = NULL;
	this->mAudio = NULL;
}

chapter_ctrl::~chapter_ctrl() {

}

bool chapter_ctrl::stock(int pos, int frame, TCHAR* title, INPUT_INFO* iip) {
	F_CHAPTER* fc = (F_CHAPTER*)calloc(1, sizeof(F_CHAPTER));
	this->mMutex.lock();
	fc->frame = frame;
	strncpy(fc->title, title, strlen(title));
	fc->vii = iip;
	this->mPoolData.insert(std::make_pair(pos, fc));
	this->mMutex.unlock();
	return true;
}

void chapter_ctrl::writeOut(FILE* fout) {
	LONGLONG t, h, m;
	double s;
	int ii = 0;

	for (auto const& ent : this->mPoolData) {
		char tmp[1024] = {};
		t = (LONGLONG)ent.second->frame * 10000000 * ent.second->vii->scale / ent.second->vii->rate;
		h = t / 36000000000;
		m = (t - h * 36000000000) / 600000000;
		s = (t - h * 36000000000 - m * 600000000) / 10000000.0;
		
		sprintf(tmp, "CHAPTER%02d=%02d:%02d:%06.3f\n", ii, (int)h, (int)m, s);
		fputs((const char*)tmp, fout);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "CHAPTER%02dNAME=%s\n", ii, ent.second->title);
		fputs((const char*)tmp, fout);
		fflush(fout);
		free(ent.second);
		ii++;
	}
}

int chapter_ctrl::write(unsigned char* f, int nchap, int frame, TCHAR* title, INPUT_INFO* iip) {
	LONGLONG t, h, m;
	double s;
	char tmp[1024] = {};
	int write_len = 0;

	t = (LONGLONG)frame * 10000000 * iip->scale / iip->rate;
	h = t / 36000000000;
	m = (t - h * 36000000000) / 600000000;
	s = (t - h * 36000000000 - m * 600000000) / 10000000.0;

	write_len += sprintf(tmp, "CHAPTER%02d=%02d:%02d:%06.3f\n", nchap, (int)h, (int)m, s);
	strncat((char*)f, tmp, (size_t)write_len);
	memset(tmp, 0, sizeof(tmp));
	write_len += sprintf(tmp, "CHAPTER%02dNAME=%s\n", nchap, title);
	strncat((char*)f, tmp, (size_t)write_len);
	return write_len;
}

void chapter_ctrl::setMute(short val) {
	this->mMute = val;
}

short chapter_ctrl::getMute() {
	return this->mMute;
}

void chapter_ctrl::setSeri(int val) {
	this->mSeri = val;
}

int chapter_ctrl::getSeri() {
	return this->mSeri;
}

void chapter_ctrl::setVideo(TCHAR* val) {
	this->mVideo = val;
}

TCHAR* chapter_ctrl::getVideo() {
	return this->mVideo;
}

void chapter_ctrl::setAudio(TCHAR* val) {
	this->mAudio = val;
}

TCHAR* chapter_ctrl::getAudio() {
	return this->mAudio;
}

void chapter_ctrl::setOutFile(TCHAR* val) {
	this->mOutFile = val;
}

TCHAR* chapter_ctrl::getOutFile() {
	return this->mOutFile;
}

bool chapter_ctrl::isSameSource() {
	if (mVideo != NULL && mAudio != NULL) {
		if (strcmp(this->mVideo, this->mAudio) == 0) {
			return true;
		}
	}
	return false;
}

void chapter_ctrl::setThreadCount(int val) {
	this->mThreadCount = val;
}

int chapter_ctrl::getThreadCount() {
	return this->mThreadCount;
}

int chapter_ctrl::getAudioNameLen() {
	return strlen(this->mAudio);
}

int chapter_ctrl::getVideoNameLen() {
	return strlen(this->mVideo);
}