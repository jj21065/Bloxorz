#pragma once
// Cintro ��ܤ��

class Cintro : public CDialogEx
{
	DECLARE_DYNAMIC(Cintro)

public:
	Cintro(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~Cintro();
	CBitmap m_bitmap_intro ;
	BITMAP bitmap_intro;
	CDC m_dc_intro;

// ��ܤ�����
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

};
