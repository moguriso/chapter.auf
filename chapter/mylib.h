//---------------------------------------------------------------------
//		�����p�̓K���ȃ��C�u���� by �ۂ�
//---------------------------------------------------------------------

// AviUtl��Build�ԍ�
#define VER_99e2	9912
#define VER_99f		9913
#define VER_99g3	9916
#define VER_99g4	9917

// �t���X�P�[���ł̐F��ԕϊ���
// �P�x(Y)�͈́F0�`4096
// �F����(Cb)�͈́F-2048�`2048
// �F����(Cr)�͈́F-2048�`2048
// �ԐF(R)�͈́F0�`4096
// �ΐF(G)�͈́F0�`4096
// �F(B)�͈́F0�`4096
#define YC2R(Y,Cb,Cr) (Y + 1.402*Cr)
#define YC2G(Y,Cb,Cr) (Y - 0.344*Cb - 0.714*Cr)
#define YC2B(Y,Cb,Cr) (Y + 1.772*Cb)
#define RGB2Y(R,G,B) (0.299*R + 0.587*G + 0.114*B)
#define RGB2Cb(R,G,B) (-0.169*R - 0.331*G + 0.500*B)
#define RGB2Cr(R,G,B) (0.500*R - 0.419*G - 0.081*B)

// YCbCr����RGB�ɕϊ�
inline void my_yc2rgb(int Y,int Cb,int Cr,int *R,int *G,int *B) {
	*R = (int)YC2R(Y,Cb,Cr);
	*G = (int)YC2G(Y,Cb,Cr);
	*B = (int)YC2B(Y,Cb,Cr);
}
inline void my_yc2rgb(int Y,int Cb,int Cr,short *R,short *G,short *B) {
	*R = (short)YC2R(Y,Cb,Cr);
	*G = (short)YC2G(Y,Cb,Cr);
	*B = (short)YC2B(Y,Cb,Cr);
}

// RGB����YCbCr�ɕϊ�
inline void my_rgb2yc(int R,int G,int B,int *Y,int *Cb,int *Cr) {
	*Y = (int)RGB2Y(R,G,B);
	*Cb = (int)RGB2Cb(R,G,B);
	*Cr = (int)RGB2Cr(R,G,B);
}
inline void my_rgb2yc(int R,int G,int B,short *Y,short *Cb,short *Cr) {
	*Y = (short)RGB2Y(R,G,B);
	*Cb = (short)RGB2Cb(R,G,B);
	*Cr = (short)RGB2Cr(R,G,B);
}

int my_getbuild(FILTER *fp,void *editp);		// AviUtl��Build�ԍ��̎擾(0.99e2�ȍ~)
HFONT my_getfont(FILTER *fp,void *editp);		// AviUtl�̃t�H���g�̎擾
int my_numthreads(FILTER *fp);					// AviUtl�̃X���b�h�����擾
bool my_sjis(char *chr,int pos);				// SJIS��1�����ڔ���
void my_getpath(char *path,int length);			// �p�X���̒��o�i�t�@�C�����̑O�܂Łj
void my_getexepath(char *path,int length);		// AviUtl�̂���p�X���̎擾
void my_getaufpath(FILTER *fp,char *path,int length);	// �v���O�C���̂���p�X���̎擾
