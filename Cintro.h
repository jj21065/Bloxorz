#pragma once
// Cintro 對話方塊

class Cintro : public CDialogEx
{
	DECLARE_DYNAMIC(Cintro)

public:
	Cintro(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~Cintro();
	CBitmap m_bitmap_intro ;
	BITMAP bitmap_intro;
	CDC m_dc_intro;

// 對話方塊資料
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

};
