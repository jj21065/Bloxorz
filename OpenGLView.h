// OpenGLView.h : COpenGLView 類別的介面
#pragma once
//#include "gl\gl.h"
//#include "gl\glu.h"
//#include "gl\glut.h"
//#include "Cintro.h"
#include "ClevelDialog.h"
#include "CMap.h"
#include "resource.h"
#include "GLee.h"
#include "GLUT.h"
#include "glPrintf.h"
#include "MemberSDialog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <vector>
#include <mmsystem.h>

#pragma comment( lib, "GLee.lib" )
using namespace std;

class CubeState
{
	
public:
	
	float x_degree ,z_degree ,y_animate,v_animate;
	UINT keyFlag;                          // to record the status 
	float *Material_color;
	float cubeSize;
	int x,z;
	float x_displacement,z_displacement,y_displacement; // record the absolute position of the cube 
	vector<int> x_count,z_count;                 // only -1 or +1 // +1 means a 90 degree,-1 means a -90 degree
	bool animate_isdone ;                // see if the animation is done //can't control the keyboard until it's true
	
	float x_[3],z_[3];  /// x axis and z axis unit vector// maybe need the structure and coding 
	vector<float> a[3],b[3];  // to record every x_ z_ array for the transform steps 
	
	
};
class COpenGLView : public CView
{
public: // 僅從序列化建立
	COpenGLView();
	DECLARE_DYNCREATE(COpenGLView)

// 屬性
public:
	//// for the dialog setting

	int m_levelindex;
	ClevelDialog levelDlg;          /// choose the level 
	CMemberSDialog membersDlg;		/// show the developers 
	int levelCount ; 
	bool ifnextlv;
	/////// for the map setting 
	Cmap level[10]; 
	int goalxy[2];
	int startxy[2];
	int m_time ;
	//Cintro introDlg;
	HWND hMCI; /// play mp3
	/////////////*****the scene setting 
	int scene;
	bool ismute;
	////////////////  for render the scene and rotation and mouse control
	float c ; //弧度和角度转换参数
	int du,oldmy,oldmx; //du是眼睛在Y軸上的角度,opengl默認Y軸是上
	int dx,dy,dz,movex,movey;
	float r,h; //r眼睛繞y軸的半徑，h眼睛在Y軸上的高度
	float dh;
	float zoom;         // 
	int mouse_count;
	/////////******************************************************///////////////
	/////////*******  for the cube state control 
	CubeState cube ; 
	float pic_z[3];			// to record the cube z axis vector after the cube rotates respective to the original x axis 
	int zislong;			// the rotation state of along z axis
	int xislong;			// the rotation state of along z axis
	int tempx; // predict the next move position 
	int tempz;
	int tempx2;
	int tempz2;
	/////////******************************************************///////////////
	GLfloat rainbowColor[4];
	bool rainbowflag;
	unsigned int textures[10];  
	/////////////*** for bitmap 
	CBitmap m_bitmap ;
	BITMAP bitmap;
	CDC m_dc;
	///////////////*** for show data and time 
	SYSTEMTIME st; 
	int old_millisec;				/// record the old miliisecond of the clock
	int move_count ;               ////  Count the steps that users 

	//
	void initialModel();		           /// initial the models every time entering a new map 
	void SetCube(float csize);             //// set the STL format of a cube 
	void DrawCube();						// draw the cube ,including the position and animation 
	void DrawGrid();                      // draw out a xz grid
	void ZMatrixTranf(float x1,float y1,float z1,float x2,float y2,float z2);  // transform matrix of rotating by z axis 90 degree
	void XMatrixTranf(float x1,float y1,float z1,float x2,float y2,float z2);  // transform matrix of rotating by x axis  90 degree
	void ZMatrixTranf2(float x1,float y1,float z1,float x2,float y2,float z2); // transform matrix of rotating by z axis -90 degree
	void XMatrixTranf2(float x1,float y1,float z1,float x2,float y2,float z2); // transform matrix of rotating by x axis  -90 degree
	void Texture();               //  set the texture of several picture ,but still got a bug which will lead to a break of memory
	void SetTexObj(char *name,int i);
	void drawString(const char* str) ;
	unsigned char *LoadBitmapFile(char *fileName, BITMAPINFO *bitmapInfo);

	void DrawMap(int levelindex);	  /// draw the map which the user selects
	void DrawFloorCube();			  //  setup the cube model of the map 
	void DrawDestination();			  // setup the destination cube modle 
	void loadLevelmap();              /// read from .txt
	void Playmusic(int type);
	void fallanimation();
	bool tick;
	afx_msg void OnMute(CCmdUI *aCmdUI);
	COpenGLDoc* GetDocument() const;

// 作業
public:
	CClientDC *m_pDC;
	HGLRC hrc;

	void Init(void);
	void DrawScene(void);
	bool bSetupPixelFormat(void);
// 覆寫
public:
	virtual void OnDraw(CDC* pDC);  // 覆寫以描繪此檢視
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 程式碼實作
public:
	virtual ~COpenGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSettingSound();
	afx_msg void OnSettingLevel();
	afx_msg void OnMembers();
};

#ifndef _DEBUG  // OpenGLView.cpp 中的偵錯版本
inline COpenGLDoc* COpenGLView::GetDocument() const
   { return reinterpret_cast<COpenGLDoc*>(m_pDocument); }
#endif

