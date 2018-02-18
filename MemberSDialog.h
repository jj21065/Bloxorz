#pragma once

// CMemberSDialog 對話方塊

class CMemberSDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMemberSDialog)

public:
	CBitmap m_bitmap_intro ;
	BITMAP bitmap_intro;
	CDC m_dc_intro;

	CMemberSDialog(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CMemberSDialog();

// 對話方塊資料
	enum { IDD = IDD_MEMBERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};
