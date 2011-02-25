// IIR_3DNR�t�B���^  by H_Kasahara(aka.HK) ���q��


//---------------------------------------------------------------------
//		�������������p
//---------------------------------------------------------------------

#include <Windows.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#define FRAME_PICTURE	1
#define FIELD_PICTURE	2
#define MAX_SEARCH_EXTENT 32	//�S�T���̍ő�T���͈́B+-���̒l�܂ŁB

//---------------------------------------------------------------------
//		�֐���`
//---------------------------------------------------------------------
void make_motion_lookup_table();
BOOL mvec(unsigned char* current_pix,unsigned char* bef_pix,int* vx,int* vy,int lx,int ly,int threshold,int pict_struct,int SC_level);
int tree_search(unsigned char* current_pix,unsigned char* bef_pix,int lx,int ly,int *vx,int *vy,int search_block_x,int search_block_y,int min,int pict_struct);
int full_search(unsigned char* current_pix,unsigned char* bef_pix,int lx,int ly,int *vx,int *vy,int search_block_x,int search_block_y,int min,int pict_struct, int search_extent);
int dist( unsigned char *p1, unsigned char *p2, int lx, int distlim, int block_hight );

//---------------------------------------------------------------------
//		�O���[�o���ϐ�
//---------------------------------------------------------------------
int	block_hight, lx2;

int x_plus[] = {1,1,1,2,1,1,1,1,
				1,1,1,2,1,1,1,1,
				1,1,1,2,1,1,1,1,
				1,1,1,2,1,1,1,1,
				1,1,1,2,1,1,1,1,};
int dxplus[] = {16,16,16,32,16,16,16, 0,
				 8, 8, 8,16, 8, 8, 8, 0,
				 4, 4, 4, 8, 4, 4, 4, 0,
				 2, 2, 2, 4, 2, 2, 2, 0,
				 1, 1, 1, 2, 1, 1, 1, 0,};
//---------------------------------------------------------------------
//		�����덷����֐�
//---------------------------------------------------------------------
//[ru] �����x�N�g���̍��v��Ԃ�
int tree=0, full=0;
int mvec(unsigned char* current_pix, 	//���t���[���̋P�x�B8�r�b�g�B
		  unsigned char* bef_pix,		//�O�t���[���̋P�x�B8�r�b�g�B
		  int lx,						//�摜�̉���
		  int ly,						//�摜�̏c��
		  int threshold,				//�������x�B(100-fp->track[1])*50 �c�c 50�͓K���Ȓl�B
		  int pict_struct)				//"1"�Ȃ�t���[�������A"2"�Ȃ�t�B�[���h����
{
	int x, y;
	unsigned char *p1, *p2;
	int motion_vector_total = 0;

//�֐����Ăяo�����Ɍv�Z�����ɂ��ނ悤�O���[�o���ϐ��Ƃ���
	lx2 = lx*pict_struct;
	block_hight = 16/pict_struct;

	for(int i=0;i<pict_struct;i++)
	{
		for(y=i;y<ly;y+=16)	//�S�̏c��
		{
			p1 = current_pix + y*lx;
			p2 = bef_pix + y*lx;
			for(x=0;x<lx;x+=16)	//�S�̉���
			{
				int vx=0, vy=0;
				int min = dist( p1, p2, lx2, INT_MAX, block_hight );
				if( threshold < (min = tree_search( p1, p2, lx, ly, &vx, &vy, x, y, min, pict_struct)) && (vx!=0 || vy!=0) )
					if( threshold < (min = tree_search( p1, &p2[vy * lx + vx], lx, ly, &vx, &vy, x+vx, y+vy, min, pict_struct)) )
						if( threshold < (min = tree_search( p1, &p2[vy * lx + vx], lx, ly, &vx, &vy, x+vx, y+vy, min, pict_struct)) )
//3��̃c���[�T���ł��t���[���Ԃ̐�Βl�����傫����ΑS�T���������Ȃ�
							full_search( p1, &p2[vy * lx + vx], lx, ly, &vx, &vy, x+vx, y+vy, min, pict_struct, max(abs(vx),abs(vy))*2 );

//�����x�N�g���̍��v���V�[���`�F���W���x���𒴂��Ă�����A�V�[���`�F���W�Ɣ��肵��TRUE��Ԃ��ďI��
				motion_vector_total += abs(vx)+abs(vy);

				p1+=16;
				p2+=16;
			}
		}
	}

	/*char str[500];
	sprintf_s(str, 500, "tree:%d, full:%d", tree, full);
	MessageBox(NULL, str, 0, 0);*/

	return motion_vector_total;
}
//---------------------------------------------------------------------
//		�c���[�T���@���������֐�
//---------------------------------------------------------------------
int tree_search(unsigned char* current_pix,	//���t���[���̋P�x�B8�r�b�g�B
				unsigned char* bef_pix,		//�O�t���[���̋P�x�B8�r�b�g�B
				int lx,						//�摜�̉���
				int ly,						//�摜�̏c��
				int *vx,					//x�����̓����x�N�g������������B
				int *vy,					//y�����̓����x�N�g������������B
				int search_block_x,			//�����ʒu
				int search_block_y,			//�����ʒu
				int min,					//���ʒu�ł̃t���[���Ԃ̐�Βl���B�֐����ł͓��ʒu�̔�r�����Ȃ��̂ŁA�Ăяo���O�ɍs���K�v����B
				int pict_struct)			//"1"�Ȃ�t���[�������A"2"�Ȃ�t�B�[���h����
{
	tree++;
	int dx, dy, ddx=0, ddy=0, xs=0, ys;
	int d;
	int x,y;
	int speedup = pict_struct-1;
	int *x_plus_p = x_plus,
		*dxplus_p = dxplus;
//�����͈͂̏���Ɖ�����ݒ�
	int ylow  = 0 - search_block_y;
	int yhigh = ly- search_block_y-16;
	int xlow  = 0 - search_block_x;
	int xhigh = lx- search_block_x-16;

//�����͈͂��摜����͂ݏo�Ȃ��悤�Ȃ�A�����������̂��߂ɁA��ʊO�ɏo�����̃`�F�b�N���s��Ȃ����[�`�����g�p����B
	if(-31<ylow || yhigh<31 || -31<xlow || xhigh<31)
	{
		for(int i=16;i>speedup;i=i/2){	//������
//�O�t���[���̃u���b�N�̈ʒu���ړ��������t���[���̂Ƃ̐�Βl����ǉ����Ă����B
			for(y=0,dy=ddy-i;y<3;y++,dy+=i){
				if( dy<ylow || dy>yhigh )	continue;	//�����ʒu����ʊO�ɏo�Ă����猟���������Ȃ�Ȃ��B
				ys = dy * lx;	//�����ʒu�c��
				for(x=0,dx=xs-i;x<3;x+=*x_plus_p,dx+=*dxplus_p){
					if( dx<xlow || dx>xhigh )	continue;	//�����ʒu����ʊO�ɏo�Ă����猟���������Ȃ�Ȃ��B
					d = dist( current_pix, &bef_pix[ys+dx], lx2, min, block_hight );
					if( d <= min ){	//����܂ł̌������t���[���Ԃ̐�Βl���������������炻�ꂼ�����B
						min = d;
						ddx = dx;
						ddy = dy;
					}
					x_plus_p++;
					dxplus_p++;
				}
			}
			xs = ddx;
		}
		if(pict_struct==FIELD_PICTURE){
			for(x=0,dx=ddx-1;x<3;x+=2,dx+=2){
				if( search_block_x+dx<0 || search_block_x+dx+16>lx )	continue;	//�����ʒu����ʊO�ɏo�Ă����猟���������Ȃ�Ȃ��B
				d = dist( current_pix, &bef_pix[ys+dx], lx2, min, block_hight );
				if( d <= min ){	//����܂ł̌������t���[���Ԃ̐�Βl���������������炻�ꂼ�����B
					min = d;
					ddx = dx;
				}
			}
		}
	}
	else
	{
		for(int i=16;i>speedup;i=i/2){	//������
//�O�t���[���̃u���b�N�̈ʒu���ړ��������t���[���̂Ƃ̐�Βl����ǉ����Ă����B
			for(y=0,dy=ddy-i;y<3;y++,dy+=i){
				ys = dy * lx;	//�����ʒu�c��
				for(x=0,dx=xs-i;x<3;x+=*x_plus_p,dx+=*dxplus_p){
					d = dist( current_pix, &bef_pix[ys+dx], lx2, min, block_hight );
					if( d <= min ){	//����܂ł̌������t���[���Ԃ̐�Βl���������������炻�ꂼ�����B
						min = d;
						ddx = dx;
						ddy = dy;
					}
					x_plus_p++;
					dxplus_p++;
				}
			}
			xs = ddx;
		}
		if(pict_struct==FIELD_PICTURE){
			for(x=0,dx=ddx-1;x<3;x+=2,dx+=2){
				d = dist( current_pix, &bef_pix[ys+dx], lx2, min, block_hight );
				if( d <= min ){	//����܂ł̌������t���[���Ԃ̐�Βl���������������炻�ꂼ�����B
					min = d;
					ddx = dx;
				}
			}
		}
	}

	*vx += ddx;
	*vy += ddy;

	return min;
}
//---------------------------------------------------------------------
//		�S�T���@���������֐�
//---------------------------------------------------------------------
int full_search(unsigned char* current_pix,	//���t���[���̋P�x�B8�r�b�g�B
				unsigned char* bef_pix,		//�O�t���[���̋P�x�B8�r�b�g�B
				int lx,						//�摜�̉���
				int ly,						//�摜�̏c��
				int *vx,					//x�����̓����x�N�g������������B
				int *vy,					//y�����̓����x�N�g������������B
				int search_block_x,			//�����ʒu
				int search_block_y,			//�����ʒu
				int min,					//�t���[���Ԃ̐�Βl���B�ŏ��̒T���ł�INT_MAX�������Ă���B
				int pict_struct,			//"1"�Ȃ�t���[�������A"2"�Ȃ�t�B�[���h����
				int search_extent)			//�T���͈́B
{
	full++;
	int dx, dy, ddx=0, ddy=0, ys;
	int d;
	int search_point;
	unsigned char* p2;

	if(search_extent>MAX_SEARCH_EXTENT)
		search_extent = MAX_SEARCH_EXTENT;

//�����͈͂̏���Ɖ������摜����͂ݏo���Ă��Ȃ����`�F�b�N
	int ylow  = 0 - ( (search_block_y-search_extent<0) ? search_block_y : search_extent );
	int yhigh = (search_block_y+search_extent+16>ly) ? ly-search_block_y-16 : search_extent;
	int xlow  = 0 - ( (search_block_x-search_extent<0) ? search_block_x : search_extent );
	int xhigh = (search_block_x+search_extent+16>lx) ? lx-search_block_x-16 : search_extent;

	for(dy=ylow;dy<=yhigh;dy+=pict_struct)
	{
		p2 = bef_pix + dy*lx + xlow;	//Y�������ʒu�Bxlow�͕��̒l�Ȃ̂�"p2=bef_pix+dy*lx-xlow"�Ƃ͂Ȃ�Ȃ�
		for(dx=xlow;dx<=xhigh;dx++)
		{
			d = dist( current_pix, p2, lx2, min, block_hight );
			if(d <= min)	//����܂ł̌������t���[���Ԃ̐�Βl���������������炻�ꂼ�����B
			{
				min = d;
				ddx = dx;
				ddy = dy;
			}
			p2++;
		}
	}

	*vx += ddx;
	*vy += ddy;

	return min;
}
//---------------------------------------------------------------------
//		�t���[���Ԑ�Βl�����v�֐�
//---------------------------------------------------------------------
//bbMPEG�̃\�[�X�𗬗p
#include <emmintrin.h>

int dist( unsigned char *p1, unsigned char *p2, int lx, int distlim, int block_height )
{
	if (block_height == 8) {
		__m128i a, b, r;

		a = _mm_load_si128 ((__m128i*)p1 +  0);
		b = _mm_loadu_si128((__m128i*)p2 +  0);
		r = _mm_sad_epu8(a, b);
		
		a = _mm_load_si128 ((__m128i*)(p1 + lx));
		b = _mm_loadu_si128((__m128i*)(p2 + lx));
		r = _mm_add_epi32(r, _mm_sad_epu8(a, b));
		
		a = _mm_load_si128 ((__m128i*)(p1 + 2*lx));
		b = _mm_loadu_si128((__m128i*)(p2 + 2*lx));
		r = _mm_add_epi32(r, _mm_sad_epu8(a, b));
		
		a = _mm_load_si128 ((__m128i*)(p1 + 3*lx));
		b = _mm_loadu_si128((__m128i*)(p2 + 3*lx));
		r = _mm_add_epi32(r, _mm_sad_epu8(a, b));
		
		a = _mm_load_si128 ((__m128i*)(p1 + 4*lx));
		b = _mm_loadu_si128((__m128i*)(p2 + 4*lx));
		r = _mm_add_epi32(r, _mm_sad_epu8(a, b));
		
		a = _mm_load_si128 ((__m128i*)(p1 + 5*lx));
		b = _mm_loadu_si128((__m128i*)(p2 + 5*lx));
		r = _mm_add_epi32(r, _mm_sad_epu8(a, b));
		
		a = _mm_load_si128 ((__m128i*)(p1 + 6*lx));
		b = _mm_loadu_si128((__m128i*)(p2 + 6*lx));
		r = _mm_add_epi32(r, _mm_sad_epu8(a, b));
		
		a = _mm_load_si128 ((__m128i*)(p1 + 7*lx));
		b = _mm_loadu_si128((__m128i*)(p2 + 7*lx));
		r = _mm_add_epi32(r, _mm_sad_epu8(a, b));
		return _mm_extract_epi16(r, 0) + _mm_extract_epi16(r, 4);;
	}
	
	int s = 0;
	for(int i=0;i<block_height;i++)
	{
		/*
		s += motion_lookup[p1[0]][p2[0]];
		s += motion_lookup[p1[1]][p2[1]];
		s += motion_lookup[p1[2]][p2[2]];
		s += motion_lookup[p1[3]][p2[3]];
		s += motion_lookup[p1[4]][p2[4]];
		s += motion_lookup[p1[5]][p2[5]];
		s += motion_lookup[p1[6]][p2[6]];
		s += motion_lookup[p1[7]][p2[7]];
		s += motion_lookup[p1[8]][p2[8]];
		s += motion_lookup[p1[9]][p2[9]];
		s += motion_lookup[p1[10]][p2[10]];
		s += motion_lookup[p1[11]][p2[11]];
		s += motion_lookup[p1[12]][p2[12]];
		s += motion_lookup[p1[13]][p2[13]];
		s += motion_lookup[p1[14]][p2[14]];
		s += motion_lookup[p1[15]][p2[15]];*/
		
		__m128i a = _mm_load_si128((__m128i*)p1);
		__m128i b = _mm_loadu_si128((__m128i*)p2);
		__m128i r = _mm_sad_epu8(a, b);
		s += _mm_extract_epi16(r, 0) + _mm_extract_epi16(r, 4);

		if (s > distlim)	break;

		p1 += lx;
		p2 += lx;
	}
	return s;
}
//---------------------------------------------------------------------
//		�t���[���Ԑ�Βl�����v�֐�(SSE�o�[�W����)
//---------------------------------------------------------------------
int dist_SSE( unsigned char *p1, unsigned char *p2, int lx, int distlim, int block_hight )
{
	int s = 0;
/*
dist_normal������ƕ�����悤�ɁAp1��p2�̐�Βl���𑫂��Ă��Adistlim�𒴂����炻�̍��v��Ԃ������B
block_hight�ɂ�8��16���������Ă���A�O�҂̓t�B�[���h�����A��҂��t���[�������p�B
block_hight��8���������Ă�����΁Alx�ɂ͉摜�̉������������Ă���B
block_hight��16���������Ă�����΁Alx�ɂ͉摜�̉����̓�{�̒l���������Ă���B
�ǂȂ����A�������쐬���Ă�����������΁A���Ɋ��ӂ������܂��B
*/
	return s;
}