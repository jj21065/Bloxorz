#pragma once

// CMemberSDialog ��ܤ��

class CMemberSDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMemberSDialog)

public:
	CBitmap m_bitmap_intro ;
	BITMAP bitmap_intro;
	CDC m_dc_intro;

	CMemberSDialog(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~CMemberSDialog();

// ��ܤ�����
	enum { IDD = IDD_MEMBERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};
