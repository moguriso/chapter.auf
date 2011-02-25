//----------------------------------------------------------------------------------
//		�`���v�^�[�ҏW�v���O�C�� by �ۂ� 
//----------------------------------------------------------------------------------
#include <windows.h>
#include <imm.h>
#include "config.h"
#include "resource.h"
#include "mylib.h"

//---------------------------------------------------------------------
//		�t�B���^�\���̒�`
//---------------------------------------------------------------------
FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE|FILTER_FLAG_MAIN_MESSAGE|FILTER_FLAG_WINDOW_SIZE|FILTER_FLAG_DISP_FILTER|FILTER_FLAG_EX_INFORMATION,	// int flag
	567,435,	// int x,y
	"�`���v�^�[�ҏW",	// TCHAR *name
	NULL,NULL,NULL,	// int track_n, TCHAR **track_name, int *track_default
	NULL,NULL,	// int *track_s, *track_e
	NULL,NULL,NULL,	// int check_n, TCHAR **check_name, int *check_default
	NULL,	// (*func_proc)
	NULL,	// (*func_init)
	NULL,	// (*func_exit)
	NULL,	// (*func_update)
	func_WndProc,	// (*func_WndProc)
	NULL,NULL,	// reserved
	NULL,	// void *ex_data_ptr
	NULL,	// int ex_data_size
	"�`���v�^�[�ҏW ver0.6 by �ۂ� + �������V�[���`�F���W�����@�\ by ru",	// TCHAR *information
	func_save_start,	// (*func_save_start)
	NULL,	// (*func_save_end)
	NULL,	// EXFUNC *exfunc;
	NULL,	// HWND	hwnd;
	NULL,	// HINSTANCE dll_hinst;
	NULL,	// void	*ex_data_def;
	NULL,	// (*func_is_saveframe)
	func_project_load,	// (*func_project_load)
	func_project_save,	// (*func_project_save)
	NULL,	// (*func_modify_title)
	NULL,	// TCHAR *dll_path;
};

//---------------------------------------------------------------------
//		�ϐ�
//---------------------------------------------------------------------
CfgDlg	g_config;
PrfDat	g_prf;
HHOOK	g_hHook;
HWND	g_hwnd;
int		g_keyhook;

//---------------------------------------------------------------------
//		�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable( void )
{
	return &filter;
}

//---------------------------------------------------------------------
//		YUY2�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTableYUY2( void )
{
	return &filter;
}

//---------------------------------------------------------------------
//		func_save_start
//---------------------------------------------------------------------
BOOL func_save_start(FILTER *fp,int s,int e,void *editp) {
	g_config.AutoSave();

	return TRUE;
}

//---------------------------------------------------------------------
//		func_WndProc
//---------------------------------------------------------------------
LRESULT CALLBACK KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	static int ime = 0;

	// Enter�L�[�œ���
	if (!g_keyhook && nCode == HC_ACTION && wParam == 0x0D && GetForegroundWindow() == g_hwnd) {
		// IME�ł�Enter�𖳎�����
		HIMC hIMC = ImmGetContext(g_hwnd);
		if(ImmGetOpenStatus(hIMC) && ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0)) ime = 2;
		ImmReleaseContext(g_hwnd,hIMC);
		if(!ime) {
			g_config.AddList();
			return TRUE;
		}
	}
	if(ime) ime--;
	if(g_keyhook) g_keyhook--;	// �_�C�A���O�ł�Enter�𖳎�����̂Ɏg��

	return CallNextHookEx(g_hHook,nCode,wParam,lParam);
}

BOOL func_WndProc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam,void *editp,FILTER *fp)
{
	FILE_INFO fip;

	switch(message) {
		case WM_FILTER_INIT:
			g_config.Init(hwnd,editp,fp);
			g_hwnd = hwnd;
			g_hHook = SetWindowsHookEx(WH_KEYBOARD,KeyboardProc,0,GetCurrentThreadId());
			g_keyhook = 0;
			break;
		case WM_FILTER_EXIT:
			UnhookWindowsHookEx(g_hHook);
			break;
		case WM_FILTER_UPDATE:	//�ҏW����
			g_config.SetFrame(fp->exfunc->get_frame(editp));
			g_config.SetFrameN(editp,fp->exfunc->get_frame_n(editp));
			break;
		case WM_FILTER_FILE_OPEN:	//�t�@�C���ǂݍ���
			if(fp->exfunc->get_file_info(editp,&fip)) g_config.SetFps(fip.video_rate,fip.video_scale);
			g_config.SetFrameN(editp,fp->exfunc->get_frame_n(editp));
			break;
		//[ru]�����Ƃ�
		case WM_FILTER_FILE_CLOSE:
			g_config.SetFrameN(NULL, 0);
			g_config.SetFps(10, 10);
			break;
		//�����܂�
		case WM_COMMAND:
			switch(LOWORD(wparam)) {
				case IDC_BUADD:
					g_config.AddList();
					break;
				case IDC_BUDEL:
					g_config.DelList();
					break;
				case IDC_LIST1:
					if(HIWORD(wparam) != LBN_SELCHANGE) break;
					g_config.Seek();
					return TRUE;
				case IDC_BUSAVE:
					g_config.Save();
					g_keyhook = 1;
					break;
				case IDC_BULOAD:
					g_config.Load();
					g_keyhook = 1;
					break;
				case IDC_CHECK1:
				//[ru]���ł�
				case IDC_CHECKSC:
				//�����܂�
					g_config.AuotSaveCheck();
					break;
				//[ru]�ǉ�
				case IDC_BUDETECT:
					g_config.DetectMute();
					break;
				//�����܂�
			}
			break;
	}
	return FALSE;
}

//---------------------------------------------------------------------
//		func_project_save
//---------------------------------------------------------------------
BOOL func_project_save( FILTER *fp,void *editp,void *data,int *size ) {
	*size = sizeof(PrfDat);
	if(data == NULL) return TRUE;	// ���̊֐���2��Ă΂��

	g_prf.m_numChapter = g_config.m_numChapter;
	CopyMemory(g_prf.m_Frame,g_config.m_Frame,sizeof(int)*100);
	CopyMemory(g_prf.m_strTitle,g_config.m_strTitle,sizeof(char)*100*STRLEN);
	CopyMemory(data,&g_prf,*size);
	return TRUE;
}

//---------------------------------------------------------------------
//		func_project_load
//---------------------------------------------------------------------
BOOL func_project_load( FILTER *fp,void *editp,void *data,int size ) {
	if(size == sizeof(PrfDat)) {
		CopyMemory(&g_prf,data,size);
		g_config.m_numChapter = g_prf.m_numChapter;
		CopyMemory(g_config.m_Frame,g_prf.m_Frame,sizeof(int)*100);
		CopyMemory(g_config.m_strTitle,g_prf.m_strTitle,sizeof(char)*100*STRLEN);
		g_config.ShowList();
	}
	return TRUE;
}