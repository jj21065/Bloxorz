#pragma once


// ClevelDialog ��ܤ��

class ClevelDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ClevelDialog)

public:

	int level_index;
	CBitmap m_bitmap_intro ;
	BITMAP bitmap_intro;
	CDC m_dc_intro;

	ClevelDialog(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~ClevelDialog();

// ��ܤ�����
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLevel1();
	afx_msg void OnBnClickedLevel2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedLevel3();
	afx_msg void OnPaint();
};
