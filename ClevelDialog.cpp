// ClevelDialog.cpp : 實作檔
//

#include "stdafx.h"
#include "OpenGL.h"
#include "ClevelDialog.h"
#include "afxdialogex.h"


// ClevelDialog 對話方塊

IMPLEMENT_DYNAMIC(ClevelDialog, CDialogEx)

ClevelDialog::ClevelDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(ClevelDialog::IDD, pParent)
{
	level_index = -1 ;
}

ClevelDialog::~ClevelDialog()
{
}

void ClevelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ClevelDialog, CDialogEx)
	ON_BN_CLICKED(IDC_LEVEL1, &ClevelDialog::OnBnClickedLevel1)
	ON_BN_CLICKED(IDC_LEVEL2, &ClevelDialog::OnBnClickedLevel2)
	ON_BN_CLICKED(IDC_LEVEL3, &ClevelDialog::OnBnClickedLevel3)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// ClevelDialog 訊息處理常式
void ClevelDialog::OnBnClickedLevel1()
{
	level_index = 0;
	this->EndDialog(0);
	this->DestroyWindow();
	// TODO: 在此加入控制項告知處理常式程式碼
}
void ClevelDialog::OnBnClickedLevel2()
{
	level_index = 1;
	this->EndDialog(0);
	this->DestroyWindow();
	// TODO: 在此加入控制項告知處理常式程式碼
}
BOOL ClevelDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	if(pMsg->message == WM_KEYDOWN){
		if(pMsg->wParam == VK_RETURN)
			return TRUE;
		if(pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void ClevelDialog::OnBnClickedLevel3()
{
	level_index = 2;
	this->EndDialog(0);
	this->DestroyWindow();
	// TODO: 在此加入控制項告知處理常式程式碼
}


void ClevelDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);
	//opne a bitmap file
	CString FileNamePath = "Picture\\levelSelect.bmp";
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
