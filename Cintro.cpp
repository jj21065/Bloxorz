// Cintro.cpp : ��@��
//

#include "stdafx.h"
#include "OpenGL.h"
#include "Cintro.h"
#include "afxdialogex.h"


// Cintro ��ܤ��

IMPLEMENT_DYNAMIC(Cintro, CDialogEx)

Cintro::Cintro(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cintro::IDD, pParent)
{
	
}

Cintro::~Cintro()
{
}

void Cintro::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Cintro, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// Cintro �T���B�z�`��


void Cintro::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);
	//opne a bitmap file
	CString FileNamePath = "Picture\\intro.bmp";
	m_bitmap_intro.Detach();
	m_bitmap_intro.m_hObject  = (HBITMAP)LoadImage(NULL, FileNamePath, IMAGE_BITMAP,
		rect.Width(), rect.Height(), LR_LOADFROMFILE);
	m_bitmap_intro.GetBitmap(&bitmap_intro);
	CClientDC DC(this);
	m_dc_intro.CreateCompatibleDC(&DC);
	m_dc_intro.SelectObject(&m_bitmap_intro);
	DC.BitBlt(0,0,bitmap_intro.bmWidth,bitmap_intro.bmHeight,&m_dc_intro,0,0,SRCCOPY);
	m_dc_intro.DeleteDC();
	// TODO: �b���[�J�z���T���B�z�`���{���X
	// ���n�I�s�Ϥ��T���� CDialogEx::OnPaint()
}

