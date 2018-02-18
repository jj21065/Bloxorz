// MemberSDialog.cpp : 實作檔
//

#include "stdafx.h"
#include "OpenGL.h"
#include "MemberSDialog.h"
#include "afxdialogex.h"
#include "resource.h"

// CMemberSDialog 對話方塊

IMPLEMENT_DYNAMIC(CMemberSDialog, CDialogEx)

CMemberSDialog::CMemberSDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMemberSDialog::IDD, pParent)
{
	//this->MoveWindow(0,0,320,240);
	//SetWindowPos(NULL, 0,0,320,240,0);
	//SetWindowPos(NULL, 0,0,320,240,0);
}

CMemberSDialog::~CMemberSDialog()
{
}

void CMemberSDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMemberSDialog, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CMemberSDialog 訊息處理常式


int CMemberSDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	this->MoveWindow(0,0,800,600);
	// TODO:  在此加入特別建立的程式碼

	return 0;
}


void CMemberSDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);
	//opne a bitmap file
	CString FileNamePath = "Picture\\member.bmp";
	m_bitmap_intro.Detach();
	m_bitmap_intro.m_hObject  = (HBITMAP)LoadImage(NULL, FileNamePath, IMAGE_BITMAP,
		rect.Width(), rect.Height(), LR_LOADFROMFILE);
	m_bitmap_intro.GetBitmap(&bitmap_intro);
	CClientDC DC(this);
	m_dc_intro.CreateCompatibleDC(&DC);
	m_dc_intro.SelectObject(&m_bitmap_intro);
	DC.BitBlt(0,0,bitmap_intro.bmWidth,bitmap_intro.bmHeight,&m_dc_intro,0,0,SRCCOPY);
	m_dc_intro.DeleteDC();
	// TODO: 在此加入您的訊息處理常式程式碼
	// 不要呼叫圖片訊息的 CDialogEx::OnPaint()
}
