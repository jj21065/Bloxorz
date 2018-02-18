
// OpenGLView.cpp : COpenGLView 類別的實作
//

#include "stdafx.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "OpenGL.h"
#endif
#include "MainFrm.h"
#include "OpenGLDoc.h"
#include "OpenGLView.h"
#include <math.h>
#include "vfw.h"                          /// for playing the music ///method 2
#pragma comment (lib,"vfw32.lib")
#ifdef _DEBUG
#define new DEBUG_NEW

#endif
#define MAX_CHAR        128
#define FALL_OUT 0
#define FALL_IN 1 
#define NOMOVE 2
#define PI 3.1419
#define L_MOUSE_DOWN 1 
#define R_MOUSE_DOWN 2 
// COpenGLView
enum SoundType{CUBEROLL,NO,WIN};   /// 轉動//禁止//獲勝


IMPLEMENT_DYNCREATE(COpenGLView, CView)

BEGIN_MESSAGE_MAP(COpenGLView, CView)
	// 標準列印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &COpenGLView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_SETTING_SOUND, &COpenGLView::OnSettingSound)
	ON_COMMAND(ID_SETTING_LEVEL, &COpenGLView::OnSettingLevel)
	ON_UPDATE_COMMAND_UI(ID_SETTING_SOUND, OnMute)
	ON_COMMAND(ID_MEMBERS, &COpenGLView::OnMembers)
END_MESSAGE_MAP()

// COpenGLView 建構/解構

COpenGLView::COpenGLView()
{
	scene = -1;
	// TODO: 在此加入建構程式碼
	c = PI/180.0f; //弧度和角度转换参数
	dx=-300;dy=-200;dz=0;
	movex = -300;movey = -200;
	du=70,oldmy=-1,oldmx=-1; //du是眼睛在Y軸上的角度,opengl默認Y軸是上
	r=10.0f,h=60.0f; //r眼睛繞y軸的半徑，h眼睛在Y軸上的高度
	zoom =1;

	levelCount = 0;
	m_levelindex = 0;
	loadLevelmap();   /// load the map first 
	
	//initialModel();
	
	//hMCI = MCIWndCreate(NULL, NULL,WS_POPUP|MCIWNDF_NOPLAYBAR|MCIWNDF_NOMENU,"BACK.mp3");
	//MCIWndPlay(hMCI);    /// play music (method 2)
	ifnextlv = false;
	ismute = false;
	m_time= 0;
}
COpenGLView::~COpenGLView()
{
}
BOOL COpenGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs 
	// 達到修改視窗類別或樣式的目的

	return CView::PreCreateWindow(cs);
}
// COpenGLView 描繪

void COpenGLView::OnDraw(CDC* /*pDC*/)
{
	COpenGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if(scene == -1) //// load the initial setting 
	{
		Texture();
		initialModel();
		scene++;
	}
	else if(scene == 0)    /// the begining frame
	{
		SetTimer(1,1,NULL);
	}
	else if(scene == 1)          /// the level selection scene
	{
		glClearColor(0.0f, 0.0f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Clear The Screen And The Depth Buffer 
		glLoadIdentity();
	}
	else if(scene == 2)   // the main game scene
	DrawScene();

	// Tell OpenGL to flush its pipeline
	::glFinish();

	// Now Swap the buffers
	::SwapBuffers( m_pDC->GetSafeHdc() );
}

// COpenGLView 列印
void COpenGLView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}
BOOL COpenGLView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 預設的準備列印程式碼
	return DoPreparePrinting(pInfo);
}
void COpenGLView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印前額外的初始設定
}
void COpenGLView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印後的清除程式碼
}

void COpenGLView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}
// COpenGLView 診斷
#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	CView::AssertValid();
}
void COpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
COpenGLDoc* COpenGLView::GetDocument() const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenGLDoc)));
	return (COpenGLDoc*)m_pDocument;
}
#endif //_DEBUG
// COpenGLView 訊息處理常式
int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init(); // initialize OpenGL

	return 0;
}
void COpenGLView::Init(void)
{
	PIXELFORMATDESCRIPTOR pfd; 	//宣告一個PixelFormat物件
	int         n;       
	m_pDC = new CClientDC(this);                  //建立一個視窗繪圖物件
	ASSERT(m_pDC != NULL);
	if (!bSetupPixelFormat())                           //設定應用所需的像素格式
		return;
	n = ::GetPixelFormat(m_pDC->GetSafeHdc());      //從Windows裡找尋符合我們設定的
	::DescribePixelFormat(m_pDC->GetSafeHdc() //PixelFormat的索引值，並將之指定
		, n, sizeof(pfd), &pfd);                                     //給pfd
	hrc = wglCreateContext(m_pDC->GetSafeHdc());         //將RC物件給DC物件
	wglMakeCurrent(m_pDC->GetSafeHdc(), hrc);              //讓這個RC設定為作用中的物件
	
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING); //Enable lighting
	//glEnable(GL_LIGHT0); //Enable light #0
	//glEnable(GL_LIGHT1);

	//glShadeModel(GL_FLAT);
	//glEnable(GL_NORMALIZE);	
	//glEnable(GL_CULL_FACE);

	GLfloat ambientLight[]={0.13f,0.13f,0.13f,1.0f};
	GLfloat diffuseLight[]={0.14f,0.14f,0.14f,1.0f};
	GLfloat specular[]={0.25f,0.25f,0.25f,1.0f};
	GLfloat lightPos[]={0.0f,500.0f,500.0f,1.0f};
	GLfloat lightPos2[]={0.0f,500.0f,10000.0f,1.0f};
	GLfloat lightPos3[]={500.0f,500.0f,500.0f,1.0f};

	GLfloat specref[]={0.32f,0.32f,0.32f,1.0f};

	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT1,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT1,GL_SPECULAR,specular);
	glLightfv(GL_LIGHT1,GL_POSITION,lightPos2);


	glLightfv(GL_LIGHT2,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT2,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT2,GL_SPECULAR,specular);
	glLightfv(GL_LIGHT2,GL_POSITION,lightPos3);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specref);
	glMateriali(GL_FRONT,GL_SHININESS,64);

}
bool COpenGLView::bSetupPixelFormat(void)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),         // size of this pfd
		1,                             		 // version number
		PFD_DRAW_TO_WINDOW |      	 // support window
		PFD_SUPPORT_OPENGL |                     // support OpenGL
		PFD_DOUBLEBUFFER,                          // double buffered
		PFD_TYPE_RGBA,                                  // RGBA type
		24,                                                       // 24-bit color depth
		0, 0, 0, 0, 0, 0,                                     // color bits ignored
		0,                                                         // no alpha buffer
		0,                                                         // shift bit ignored
		0,                                                         // no accumulation buffer
		0, 0, 0, 0,                                             // accum bits ignored
		32,                                                       // 32-bit z-buffer
		0,                                                         // no stencil buffer
		0,                                                         // no auxiliary buffer
		PFD_MAIN_PLANE,                                 // main layer
		0,                                                         // reserved
		0, 0, 0                                                   // layer masks ignored
	};
	int pixelformat;

	if ( (pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)) == 0 )
	{
		//MessageBox("ChoosePixelFormat failed");
		return FALSE;
	}

	if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
	{
		//MessageBox("SetPixelFormat failed");
		return FALSE;
	}

	return TRUE;
}
void COpenGLView::OnDestroy()
{
	CView::OnDestroy();

	HGLRC   hRC;
	//KillTimer(1);
	hRC= ::wglGetCurrentContext();
	::wglMakeCurrent(NULL,  NULL);
	if (hrc)
		::wglDeleteContext(hRC);
	if (m_pDC)
		delete m_pDC;
}
BOOL COpenGLView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	return CView::OnEraseBkgnd(pDC);
}
void COpenGLView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	GLfloat nRange=20.f;
	if(cy == 0)
	{  cy = 1;  }

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (cx <= cy)//glOrtho
		glOrtho(-nRange, nRange, -nRange*cy/cx, nRange*cy/cx, -nRange*2000, nRange*2000);
	else 
		glOrtho(-nRange*cx/cy, nRange*cx/cy, -nRange, nRange, -nRange*2000, nRange*2000);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, cx, cy);
	// TODO: 在此加入您的訊息處理常式程式碼
}
void COpenGLView::DrawScene(void)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Clear The Screen And The Depth Buffer
	glLoadIdentity();
	

	glPushMatrix();
	gluLookAt((r*cos(c*h))*cos(c*du),r*sin(c*h),(r*cos(c*h))*sin(c*du), 0, 0, 0, 0, 1, 0); 
	glTranslatef(cos(c*h)*cos(c*(du-90))*dx*0.1,-sin(c*(h+90))*dy*0.1,-cos(c*h)*cos(c*du)*dx*0.1);
	glScalef(zoom,zoom,zoom);
	
	//glPrintf( "目前時間: %s\n" );
	//CMainFrame *myFrame 
	//CFrameWnd * myframe = GetParentFrame();
	//(CMainFrame *)GetParentFrame()->SetTimer(1,100,NULL);
	//float a = ((CMainFrame *)GetParentFrame())->st.wSecond;
	//int a = ((CMainFrame *)GetParentFrame())->m_time;
	
	//int a = m_time;
	SetTimer(2,50,NULL);
	glPrintf( ">>glut" );
	glColor3f(255,255,255);

	glPrintf( "\n使用時間: %i  second \n" ,m_time);
	glPrintf( "已走步數: %i\n" ,move_count);
		// draw a big  square , just for temporarily  use 
	/*glPushMatrix();                                  
	glColor4ub(80,80,80,0.5);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0,1,0);
	glVertex3f(-cube.cubeSize*5,0,cube.cubeSize*5);
	glVertex3f(cube.cubeSize*5,0,cube.cubeSize*5);
	glVertex3f(-cube.cubeSize*5,0,-cube.cubeSize*5);	
	glNormal3f(0,1,0);
	glVertex3f(cube.cubeSize*5,0,cube.cubeSize*5);
	glVertex3f(cube.cubeSize*5,0,-cube.cubeSize*5);
	glVertex3f(-cube.cubeSize*5,0,-cube.cubeSize*5);
	glEnd();
	glPopMatrix();*/

	/*glBindTexture(GL_TEXTURE_2D,textures[1]);
	glBegin(GL_QUADS);
	glNormal3f(0,1,0);
	glTexCoord2f(0,1);glVertex3f(8,0.1,8);
	glTexCoord2f(0,0);glVertex3f(8,0.1,8+cube.cubeSize);
	glTexCoord2f(1,0);glVertex3f( 8+cube.cubeSize,0.1,8+cube.cubeSize);
	glTexCoord2f(1,1);glVertex3f( 8+cube.cubeSize,0.1,8);
	glEnd();*/
	glPushMatrix();
		if(level)
		{
			DrawMap(m_levelindex);
		}
		DrawDestination();	
		DrawCube();
		//DrawGrid();
	glPopMatrix();
	glPopMatrix();
}
void COpenGLView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if(scene == 0 )
	{
		//SetTimer(1,100,NULL);
		CRect rect;
		GetWindowRect(&rect);
		m_bitmap.Detach();
		m_bitmap.m_hObject  = (HBITMAP)LoadImage(NULL, "Picture\\intro.bmp", IMAGE_BITMAP,
			rect.Width(),rect.Height(), LR_LOADFROMFILE);
		m_bitmap.GetBitmap(&bitmap);
		CClientDC aDC(this);
		m_dc.CreateCompatibleDC(&aDC);
		m_dc.SelectObject(&m_bitmap);
		aDC.BitBlt(0,0,bitmap.bmWidth,bitmap.bmHeight,&m_dc,0,0,SRCCOPY);
		m_dc.DeleteDC();
		Sleep(400);

		m_bitmap.Detach();
		m_bitmap.m_hObject  = (HBITMAP)LoadImage(NULL, "Picture\\intro2.bmp", IMAGE_BITMAP,
			rect.Width(),rect.Height(), LR_LOADFROMFILE);
		m_bitmap.GetBitmap(&bitmap);
		m_dc.CreateCompatibleDC(&aDC);
		m_dc.SelectObject(&m_bitmap);
		aDC.BitBlt(0,0,bitmap.bmWidth,bitmap.bmHeight,&m_dc,0,0,SRCCOPY);
		m_dc.DeleteDC();
		Sleep(400);
		
	}
	else if(scene == 2)
	{
		
		GetLocalTime ( &st );
		//old_millisec =int(st.wMilliseconds)/100;
		//float tempmilli = float(st.wMilliseconds)/100+0.11;
		int tempmilli = int(st.wMilliseconds)/100;
		if(tempmilli == 0&&tick == true)
		{
			m_time+=1;
			tick = false;
		}
		if(tempmilli == 1)
		{
			tick = true;
		}
		//if(>old_millisec)
		/*if(abs(tempmilli - old_millisec)>=1)
		{
		m_time+=0.1;
		}
		*/
		if(cube.keyFlag != NOMOVE)
		{
			cube.animate_isdone = false ;
			switch  (cube.keyFlag)
			{
			case FALL_OUT:
				fallanimation();
				break;
			case FALL_IN:
				fallanimation();
				break;
			case VK_UP:
				cube.x_degree-=0.1;
					if(abs(cube.x_degree) >=0.5* PI)
					{
						cube.x_degree = 0 ;
						KillTimer(1);
						if(xislong == 2 || xislong == 4)
						{
							if(zislong == 1)
							{
								cube.z_displacement -= 2*cube.cubeSize;
								cube.y_displacement = 0;
							}
							else if(zislong == 2)
							{
								cube.z_displacement -= 2*cube.cubeSize;
								cube.y_displacement = cube.cubeSize;
							}
							else if(zislong == 3)
							{
								cube.z_displacement -=  cube.cubeSize;
								cube.y_displacement = 0;
							}
							else if(zislong ==4)
							{
								cube.z_displacement -= cube.cubeSize;
								cube.y_displacement = 0;
							}
						}
						else 
							cube.z_displacement -=  cube.cubeSize;

						cube.x_count.push_back(-1);
						cube.z_count.push_back(0);
						XMatrixTranf2(cube.x_[0],cube.x_[1],cube.x_[2],pic_z[0],pic_z[1],pic_z[2]);
						cube.a[0].push_back(cube.x_[0]);
						cube.a[1].push_back(cube.x_[1]);
						cube.a[2].push_back(cube.x_[2]);
						cube.b[0].push_back(cube.z_[0]);
						cube.b[1].push_back(cube.z_[1]);
						cube.b[2].push_back(cube.z_[2]);
						cube.animate_isdone = true ;Playmusic(CUBEROLL);
						cube.keyFlag = NOMOVE;move_count++;
					}
				break;
			case VK_DOWN:
				cube.x_degree+=0.1;
		
				if(abs(cube.x_degree) >=0.5* PI)
				{
					cube.x_degree = 0 ;
					KillTimer(1);
					if(xislong == 2 || xislong == 4)
					{
						if(zislong == 1)
						{
							cube.z_displacement += cube.cubeSize;
							cube.y_displacement = 0;

						}
						else if(zislong == 2)
						{
							cube.z_displacement += 2*cube.cubeSize;
							cube.y_displacement = cube.cubeSize;
						}
						else if(zislong == 3)
						{
							cube.z_displacement += 2* cube.cubeSize;
							cube.y_displacement = 0;
						}
						else if(zislong ==4)
						{
							cube.z_displacement += cube.cubeSize;
							cube.y_displacement = 0;
						}
					}
					else
						cube.z_displacement += cube.cubeSize;
			
					XMatrixTranf(cube.x_[0],cube.x_[1],cube.x_[2],pic_z[0],pic_z[1],pic_z[2]);
					cube.x_count.push_back(1);
					cube.z_count.push_back(0);
					cube.a[0].push_back(cube.x_[0]);
					cube.a[1].push_back(cube.x_[1]);
					cube.a[2].push_back(cube.x_[2]);
					cube.b[0].push_back(cube.z_[0]);
					cube.b[1].push_back(cube.z_[1]);
					cube.b[2].push_back(cube.z_[2]);
					cube.animate_isdone = true ;Playmusic(CUBEROLL);
					cube.keyFlag = NOMOVE;move_count++;
				}
				break;
			case VK_RIGHT:
				cube.z_degree-=0.1;
		
				if(abs(cube.z_degree) >=0.5* PI)
				{
					cube.z_degree = 0 ;
					KillTimer(1);
				
					if(zislong == 2||zislong==4)
					{
						if(xislong == 1)
						{
							cube.x_displacement += cube.cubeSize;
							cube.y_displacement = 0;
						}
						else if(xislong == 2)
						{
							cube.x_displacement += 2*cube.cubeSize;
							cube.y_displacement = cube.cubeSize;
						}
						else if(xislong == 3)
						{
							cube.x_displacement +=  2*cube.cubeSize;
							cube.y_displacement = 0;
						}
						else if(xislong ==4)
						{
							cube.x_displacement += cube.cubeSize;
							cube.y_displacement = 0;
						}
					}
					else
						cube.x_displacement += cube.cubeSize;
			

					ZMatrixTranf2(cube.x_[0],cube.x_[1],cube.x_[2],pic_z[0],pic_z[1],pic_z[2]);
					cube.x_count.push_back(0);
					cube.z_count.push_back(-1);
					cube.a[0].push_back(cube.x_[0]);
					cube.a[1].push_back(cube.x_[1]);
					cube.a[2].push_back(cube.x_[2]);
					cube.b[0].push_back(cube.z_[0]);
					cube.b[1].push_back(cube.z_[1]);
					cube.b[2].push_back(cube.z_[2]);
					cube.animate_isdone = true ;
					Playmusic(CUBEROLL);
					cube.keyFlag = NOMOVE;move_count++;
				}
				break;
			case VK_LEFT:
				cube.z_degree+=0.1;
		
				if(abs(cube.z_degree) >=0.5* PI)
				{
					cube.z_degree = 0 ;
					KillTimer(1);
					if(zislong == 2||zislong==4)
					{
						if(xislong == 1)
						{
							cube.x_displacement -= 2*cube.cubeSize;
							cube.y_displacement = 0;
						}
						else if(xislong == 2)
						{
							cube.x_displacement -= 2*cube.cubeSize;
							cube.y_displacement = cube.cubeSize;
						}
						else if(xislong == 3)
						{
							cube.x_displacement -=  cube.cubeSize;
							cube.y_displacement = 0;
						}
						else if(xislong ==4)
						{
							cube.x_displacement -= cube.cubeSize;
							cube.y_displacement = 0;
						}
					}
					else 
						cube.x_displacement-= cube.cubeSize;
					ZMatrixTranf(cube.x_[0],cube.x_[1],cube.x_[2],pic_z[0],pic_z[1],pic_z[2]);
					cube.x_count.push_back(0);
					cube.z_count.push_back(1);
					cube.a[0].push_back(cube.x_[0]);
					cube.a[1].push_back(cube.x_[1]);
					cube.a[2].push_back(cube.x_[2]);
					cube.b[0].push_back(cube.z_[0]);
					cube.b[1].push_back(cube.z_[1]);
					cube.b[2].push_back(cube.z_[2]);
					cube.animate_isdone = true ;
					Playmusic(CUBEROLL);
					cube.keyFlag = NOMOVE;move_count++;
				}
				break;
			case VK_SHIFT:
				cube.v_animate = cube.v_animate -9.8*0.1;
				cube.y_animate += cube.v_animate*0.1;
				if(cube.y_animate<=0)
				{
					cube.animate_isdone = true ;
					cube.y_animate = 0;
					KillTimer(1);	
					cube.keyFlag = NOMOVE;move_count++;
				}
				break;
			}
		}
	}
	Invalidate(NULL);
	CView::OnTimer(nIDEvent);
}
void COpenGLView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//CMainFrame *pDoc = new CMainFrame ;

	if(scene == 0) ///// select the level 
	{
		if(nChar == VK_RETURN)
		{
			scene = 1;
			KillTimer(1);
			levelDlg.DoModal();
			m_levelindex = levelDlg.level_index;
			if(levelDlg.level_index>=0)
			{
				DrawMap(m_levelindex);
				scene = 2;
				Invalidate();
				initialModel();
				cube.keyFlag = FALL_IN;
				cube.y_animate = 50 ;

				SetTimer(1,1,NULL);
			}
		}
		
	}
	tempx = cube.x_displacement/cube.cubeSize; // predict the next move position 
	tempz = cube.z_displacement/cube.cubeSize;
	/*tempz2 = tempz;
	tempx2 = tempx;*/
	if(cube.animate_isdone == true )
	{
		if(nChar == VK_UP)
		{
			cube.keyFlag = VK_UP;
			if(xislong %2 == 0)
			{
				if(zislong == 2)     /// 2 -> 1 
				{
					tempz -= 2;tempz2 = tempz +1 ;
				} 
				else if(zislong == 3)   /// 3-> 2 
				{
					tempz -= 2;tempz2 = tempz;
				}
				else if(zislong == 4)
				{
					tempz -=  1;tempz2 = tempz-1;
				}
				else if(zislong ==1)  ///1->4
				{
					tempz -= 1;tempz2 = tempz;
				}
			}
			else 
			{
				tempz -= 1;tempz2 = tempz;
			}
			if(tempx>=0&&tempx2>=0&&tempz>=0&&tempz2>=0&&tempz<=level[m_levelindex].zsize-1&&tempz2<=level[m_levelindex].zsize-1&&tempx<=level[m_levelindex].xsize-1&&tempx2<=level[m_levelindex].xsize -1)
			{
				if(level[m_levelindex].mapStatus[2+tempx+tempz*level[m_levelindex].xsize] != 0&&level[m_levelindex].mapStatus[2+tempx2+tempz2*level[m_levelindex].xsize] != 0)
				{
					if(xislong == 2 || xislong == 4)
					{
						zislong--;
						if(zislong <1)
							zislong = 4;
						if(zislong == 2)
							xislong = zislong;
						if(zislong == 4)
							xislong = zislong;
			
					}
					
					SetTimer(1,1,NULL);
				}	
				else
				{
					tempz = cube.z_displacement/cube.cubeSize;tempz2 = tempz;
					Playmusic(NO);cube.keyFlag = NOMOVE;
				}
			}
			else
			{
				tempz = cube.z_displacement/cube.cubeSize;tempz2 = tempz;
				Playmusic(NO);cube.keyFlag = NOMOVE;
			}
		}
		else if(nChar == VK_DOWN)
		{
			cube.keyFlag = VK_DOWN;
			
			if(xislong %2 == 0)
			{
				if(zislong == 4)   ///4->1
				{
					tempz += 1;tempz2 = tempz+1 ;
				}
				else if(zislong == 1)   /// 1->2
				{
					tempz += 2;tempz2 = tempz ;
				}
				else if(zislong == 2)
				{
					tempz += 2;tempz2 = tempz-1 ;
				}
				else if(zislong ==3)
				{
					tempz +=1 ;tempz2 = tempz ;
				}
			}
			else
			{
				tempz += 1;tempz2 = tempz;
			}
			if(tempx>=0&&tempx2>=0&&tempz>=0&&tempz2>=0&&tempz<=level[m_levelindex].zsize-1&&tempz2<=level[m_levelindex].zsize-1&&tempx<=level[m_levelindex].xsize-1&&tempx2<=level[m_levelindex].xsize -1)
			{
				if(level[m_levelindex].mapStatus[2+tempx+tempz*level[m_levelindex].xsize] != 0&&level[m_levelindex].mapStatus[2+tempx2+tempz2*level[m_levelindex].xsize] != 0)
				{
					if(xislong == 2 || xislong == 4)
					{
						zislong ++ ;
						if(zislong >4)
							zislong = 1;

						if(zislong == 2)
							xislong = zislong;
						if(zislong == 4)
							xislong = zislong;
					
					}
					SetTimer(1,1,NULL);
				}
				else
				{
					tempz = cube.z_displacement/cube.cubeSize;tempz2 = tempz;
					Playmusic(NO);cube.keyFlag = NOMOVE;
				}
			}
			else
			{
				tempz = cube.z_displacement/cube.cubeSize;tempz2 = tempz;
				Playmusic(NO);cube.keyFlag = NOMOVE;
			}
		}
		else if(nChar == VK_RIGHT)
		{
			cube.keyFlag = VK_RIGHT;
			if(zislong %2 == 0)
			{
				if(xislong == 4)  //4->1
				{
					tempx += 1;tempx2 = tempx+1;
				}
				else if(xislong == 1)
				{
					tempx += 2;tempx2 = tempx;
				}
				else if(xislong == 2)
				{
					tempx +=  2;tempx2 = tempx-1;
				}
				else if(xislong ==3)
				{
					tempx += 1;tempx2 = tempx;
				}
			}
			else
			{
				tempx += 1;tempx2 = tempx;
			}
			if(tempx>=0&&tempx2>=0&&tempz>=0&&tempz2>=0&&tempz<=level[m_levelindex].zsize-1&&tempz2<=level[m_levelindex].zsize-1&&tempx<=level[m_levelindex].xsize-1&&tempx2<=level[m_levelindex].xsize -1)
			{
				if(level[m_levelindex].mapStatus[2+tempx+tempz*level[m_levelindex].xsize] != 0&&level[m_levelindex].mapStatus[2+tempx2+tempz2*level[m_levelindex].xsize] != 0)
				{
					if(zislong == 2 || zislong == 4)
					{
						xislong ++ ;
						if(xislong >4)
						{
							xislong = 1;
						}
						if(xislong == 2)
							zislong = xislong;
						if(xislong == 4)
							zislong = xislong;
						
					}
					SetTimer(1,1,NULL);
				}
				else
				{
					tempx = cube.x_displacement/cube.cubeSize;tempx2 = tempx;
					Playmusic(NO);cube.keyFlag = NOMOVE;
				}
			}
			else
			{
				tempx = cube.x_displacement/cube.cubeSize;tempx2 = tempx;
				Playmusic(NO);cube.keyFlag = NOMOVE;
			}
		}
		else if(nChar == VK_LEFT)
		{
			cube.keyFlag = VK_LEFT;
			if(zislong %2 == 0)
			{
				if(xislong == 2)  //2->1
				{
					tempx -= 2;tempx2 = tempx+1;
				}
				else if(xislong == 3)
				{
					tempx -= 2;tempx2 = tempx;
				}
				else if(xislong == 4)
				{
					tempx -=  1;tempx2 = tempx-1;
				}
				else if(xislong ==1)
				{
					tempx -= 1;tempx2 = tempx;
				}
			}
			else
			{
				tempx -= 1;tempx2 = tempx;
			}
			if(tempx>=0&&tempx2>=0&&tempz>=0&&tempz2>=0&&tempz<=level[m_levelindex].zsize-1&&tempz2<=level[m_levelindex].zsize-1&&tempx<=level[m_levelindex].xsize-1&&tempx2<=level[m_levelindex].xsize -1)
			{
				if(level[m_levelindex].mapStatus[2+tempx+tempz*level[m_levelindex].xsize] != 0&&level[m_levelindex].mapStatus[2+tempx2+tempz2*level[m_levelindex].xsize] != 0)
				{
					if(zislong == 2 || zislong == 4)
					{
						xislong -- ;
						if(xislong <1)
						{
							xislong = 4;
						}
						if(xislong == 2)
							zislong = xislong;
						if(xislong == 4)
							zislong = xislong;
		
					}
					SetTimer(1,1,NULL);
				}
				else
				{
					tempx = cube.x_displacement/cube.cubeSize;tempx2 = tempx;
					Playmusic(NO);cube.keyFlag = NOMOVE;
				}
			}
			else
			{
				tempx = cube.x_displacement/cube.cubeSize;tempx2 = tempx;
				Playmusic(NO);cube.keyFlag = NOMOVE;
			}
		}
		else if(nChar == VK_SHIFT)
		{
			cube.keyFlag = VK_SHIFT;
			cube.v_animate = 10;
			SetTimer(1,1,NULL);
		}
	}
	TRACE(_T("Happened\n"));
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
	

}
void COpenGLView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	oldmx=point.x;
	oldmy=point.y;
	mouse_count =L_MOUSE_DOWN ;
	
	CView::OnLButtonDown(nFlags, point);
}
void COpenGLView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	mouse_count = 0;
	if(scene == 0) ///// select the level 
	{
		scene = 1;
		KillTimer(1);
		levelDlg.DoModal();
		m_levelindex = levelDlg.level_index;
		if(levelDlg.level_index>=0)
		{
			DrawMap(m_levelindex);
			scene = 2;
			Invalidate();
			initialModel();
			cube.keyFlag = FALL_IN;
			cube.y_animate = 50 ;
			
			SetTimer(1,1,NULL);
		}
	}
	else if(scene == 2)
	{
		du=70,oldmy=-1,oldmx=-1; //du是眼睛在Y軸上的角度,opengl默認Y軸是上
		r=10.0f,h=60.0f; //r眼睛繞y軸的半徑，h眼睛在Y軸上的高度
		Invalidate();
	}
	CView::OnLButtonUp(nFlags, point);
}
void COpenGLView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if(scene == 2)
	{
		if(mouse_count ==L_MOUSE_DOWN)  //滑鼠左鍵按下
		{
			du+=point.x-oldmx; 
			h +=(point.y-oldmy); 
			if(h>55.0f) h=55.0f; 
			else if(h<-55.0f) h=-55.0f;
			oldmx=point.x;oldmy=point.y; 
			Invalidate(NULL);
		}
		else if (mouse_count == R_MOUSE_DOWN)
		{
			dx = movex + point.x - oldmx;
			dy = movey + point.y - oldmy;
			Invalidate(NULL);
		}
	}
	
	CView::OnMouseMove(nFlags, point);
}
void COpenGLView::SetCube(float csize)
{
	//glPushMatrix();
	//glColor4ub(200,200,200,1);
	//glTranslatef(-0.5*csize,-0.5*csize,-0.5*csize);
	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(-1,0,0);  //1 
	//glVertex3f(0,0,0);
	//glVertex3f(0,0,csize);
	//glVertex3f(0,csize,0);	
	//glEnd();
	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(-1,0,0);
	//glVertex3f(0,0,csize);
	//glVertex3f(0,csize,csize);	
	//glVertex3f(0,csize,0);
	//glEnd();

	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(1,0,0);  
	//glVertex3f(csize,0,csize);
	//glVertex3f(csize,0,0);
	//glVertex3f(csize,csize,0);
	//glEnd();

	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(1,0,0);
	//glVertex3f(csize,csize,csize);
	//glVertex3f(csize,0,csize);
	//glVertex3f(csize,csize,0);
	//glEnd();

	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(0,1,0);
	//glVertex3f(0,csize,0);
	//glVertex3f(0,csize,csize);	
	//glVertex3f(csize,csize,0);
	//glEnd();


	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(0,1,0);
	//glVertex3f(csize,csize,0);
	//glVertex3f(0,csize,csize);	
	//glVertex3f(csize,csize,csize);
	//glEnd();


	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(0,-1,0);
	//glVertex3f(0,0,0);
	//glVertex3f(csize,0,0);
	//glVertex3f(0,0,csize);
	//glEnd();

	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(0,-1,0);
	//glVertex3f(csize,0,0);	
	//glVertex3f(csize,0,csize);
	//glVertex3f(0,0,csize);
	//glEnd();

	//glBegin(GL_TRIANGLE_FAN);
	////glColor4ub(rainbowColor[0],rainbowColor[1],rainbowColor[2],rainbowColor[3]);
	//glNormal3f(0,0,1);
	//glVertex3f(0,csize,csize);	
	//glVertex3f(0,0,csize);
	//glVertex3f(csize,0,csize);
	//glEnd();

	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(0,0,1);
	//glVertex3f(csize,0,csize);
	//glVertex3f(csize,csize,csize);
	//glVertex3f(0,csize,csize);
	//glEnd();
	//
	//glBegin(GL_TRIANGLE_FAN);
	//glColor4ub(200,80,200,0.8);
	//glNormal3f(0,0,-1);
	//glVertex3f(0,0,0);
	//glVertex3f(0,csize,0);
	//glVertex3f(csize,0,0);
	//	
	//glEnd();

	//glBegin(GL_TRIANGLE_FAN);
	//glNormal3f(0,0,-1);
	//glVertex3f(0,csize,0);
	//glVertex3f(csize,csize,0);	
	//glVertex3f(csize,0,0);
	//glEnd();
	//glPopMatrix();
	glPushMatrix();
	//glTranslatef(0.5*csize,0.5*csize,0.5*csize);
	glColor4ub(255,255,255,1);
	glBindTexture(GL_TEXTURE_2D,textures[3]);
	glBegin(GL_QUADS);
	glNormal3f(0,0,1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	// Back Face
	/*glNormal3f(0,0,-1);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);*/
	// Top Face
	glNormal3f(0,1,0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	// Bottom Face
	glNormal3f(0,-1,0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	// Right face
	glNormal3f(1,0,0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	// Left Face
	glNormal3f(-1,0,0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D,textures[1]);
	glBegin(GL_QUADS);
	glNormal3f(0,0,-1);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glEnd();
	glPopMatrix();


}
void COpenGLView::DrawCube()
{
	glPushMatrix();
		glTranslatef(cube.x_displacement,cube.y_displacement+cube.y_animate,cube.z_displacement); // map coordinate (grid), move the cube after rotation 
		
		switch (cube.keyFlag)                          // for adjust position and rotation axis
		{
		case VK_UP:
			if(xislong ==2||xislong == 4)
			{
				if(zislong == 1)
					glTranslatef(0,-cube.cubeSize,0);
				else if(zislong == 2)
					glTranslatef(0,0,-cube.cubeSize);
				else if(zislong == 3)
					glTranslatef(0,0,0);
				else if(zislong ==4)
					glTranslatef(0,0,0);
			}
			else
				glTranslatef(0,0,0);
			break;
		case VK_DOWN:
			if(xislong == 2 || xislong ==4)
			{
				if(zislong == 1)
					glTranslatef(0,0,cube.cubeSize);
				else if(zislong == 2)
					glTranslatef(0,0,2*cube.cubeSize);
				else if(zislong == 3)
					glTranslatef(0,-cube.cubeSize,cube.cubeSize);
				else if(zislong ==4)
					glTranslatef(0,0,cube.cubeSize);
			}
			else 
				glTranslatef(0,0,cube.cubeSize);
			break;
		case VK_RIGHT:
			if(zislong==2||zislong==4)
			{
				if(xislong == 1)
					glTranslatef(cube.cubeSize,0,0);
				else if(xislong == 2)
					glTranslatef(2*cube.cubeSize,0,0);
				else if(xislong == 3)
					glTranslatef(cube.cubeSize,-cube.cubeSize,0);
				else if(xislong ==4)
					glTranslatef(cube.cubeSize,0,0);
			}
			else
				glTranslatef(cube.cubeSize,0,0);

			break;
		case VK_LEFT:
			if(zislong ==2||zislong == 4)
			{
				if(xislong == 1)
					glTranslatef(0,-1*cube.cubeSize,0);
				else if(xislong == 2)
					glTranslatef(-1*cube.cubeSize,0,0);
				else if(xislong == 3)
					glTranslatef(0,0,0);
				else if(xislong ==4)
					glTranslatef(0,0,0);
			}
			else
				glTranslatef(0,0,0);
			//glTranslatef(0,0,0);

			break;
		case VK_SHIFT:

			break;
		}
		glPushMatrix();
			glRotatef(cube.x_degree*180/PI,1.0f,0,0);        // rotation by the x axis 
			glRotatef(cube.z_degree*180/PI,0,0,1.0f);			// rotation by the x axis 
			glPushMatrix();
				switch (cube.keyFlag)                        // for adjust position and rotation axis by the edge
				{
				case VK_UP:
					if(xislong == 2||xislong ==4)
					{
						if(zislong == 1)
							glTranslatef(cube.cubeSize/2,cube.cubeSize*1.5,cube.cubeSize*0.5);
						else if(zislong == 2)
							glTranslatef(cube.cubeSize/2,cube.cubeSize/2,cube.cubeSize*1.5);
						else if(zislong == 3)
							glTranslatef(cube.cubeSize/2,cube.cubeSize/2,cube.cubeSize/2);
						else if(zislong ==4)
							glTranslatef(cube.cubeSize/2,cube.cubeSize/2,cube.cubeSize/2);
					}
					else
						glTranslatef(cube.cubeSize/2,cube.cubeSize*0.5,cube.cubeSize*0.5);
					
					break;
				case VK_DOWN:
					if(xislong == 2||xislong ==4)
					{
						if(zislong == 1)
							glTranslatef(cube.cubeSize/2,cube.cubeSize*0.5,-cube.cubeSize*0.5);
						else if(zislong == 2)
							glTranslatef(cube.cubeSize/2,cube.cubeSize/2,-cube.cubeSize*1.5);
						else if(zislong == 3)
							glTranslatef(cube.cubeSize/2,cube.cubeSize*1.5,-cube.cubeSize*0.5);
						else if(zislong ==4)
							glTranslatef(cube.cubeSize/2,cube.cubeSize*0.5,-cube.cubeSize*0.5);
					}
					else 
						glTranslatef(cube.cubeSize/2,cube.cubeSize*0.5,-cube.cubeSize*0.5);
					
					
					break;
				case VK_RIGHT:
					if(zislong ==2||zislong == 4)
					{
						if(xislong == 1)
							glTranslatef(-cube.cubeSize*0.5,cube.cubeSize/2,cube.cubeSize/2);
						else if(xislong == 2)
							glTranslatef(-cube.cubeSize*1.5,cube.cubeSize/2,cube.cubeSize/2);
						else if(xislong == 3)
							glTranslatef(-cube.cubeSize*0.5,cube.cubeSize*1.5,cube.cubeSize*0.5);
						else if(xislong ==4)
							glTranslatef(-cube.cubeSize*0.5,cube.cubeSize*0.5,cube.cubeSize*0.5);
					}
					else
						glTranslatef(-cube.cubeSize*0.5,cube.cubeSize/2,cube.cubeSize/2);

					break;
				case VK_LEFT:
					if(zislong ==2||zislong == 4)
					{
						if(xislong == 1)
							glTranslatef(cube.cubeSize*0.5,cube.cubeSize*1.5,cube.cubeSize*0.5);
						else if(xislong == 2)
							glTranslatef(cube.cubeSize*1.5,cube.cubeSize/2,cube.cubeSize/2);
						else if(xislong == 3)
							glTranslatef(cube.cubeSize*0.5,cube.cubeSize*0.5,cube.cubeSize*0.5);
						else if(xislong ==4)
							glTranslatef(cube.cubeSize*0.5,cube.cubeSize*0.5,cube.cubeSize*0.5);
					}
					else
						glTranslatef(cube.cubeSize*0.5,cube.cubeSize/2,cube.cubeSize/2);
					break;
				case VK_SHIFT:
					glTranslatef(cube.cubeSize*0.5,cube.cubeSize/2,cube.cubeSize/2);
					break;
				case FALL_OUT:
					glTranslatef(cube.cubeSize*0.5,cube.cubeSize/2,cube.cubeSize/2);
					break;
				case FALL_IN:
					glTranslatef(cube.cubeSize*0.5,cube.cubeSize/2,cube.cubeSize/2);
					break;
				case NOMOVE:
					glTranslatef(cube.cubeSize*0.5,cube.cubeSize/2,cube.cubeSize/2);
					break;
				}

				glPushMatrix();
				if(cube.a[0].size() !=0)             /// do the tranform
				{
					for(int i = cube.a[0].size()-1;i>=0;i--)
					{
						glRotatef(90*cube.z_count.at(i),cube.b[0].at(i),cube.b[1].at(i),cube.b[2].at(i));
						glRotatef(90*cube.x_count.at(i),cube.a[0].at(i),cube.a[1].at(i),cube.a[2].at(i));
					}
				}
				SetCube(cube.cubeSize);	
				//glutSolidCube(cube.cubeSize); // the opengl cube can't have bmp texture, not very useful 
				glTranslatef(0,0,cube.cubeSize);
				SetCube(cube.cubeSize);	
				glPushMatrix();
				glBindTexture(GL_TEXTURE_2D,textures[4]);
				glBegin(GL_QUADS);
				glNormal3f(0,1,0);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2+0.01, -cube.cubeSize-cube.cubeSize/2);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2+0.01,  -cube.cubeSize+cube.cubeSize/2);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2+0.01,  -cube.cubeSize+cube.cubeSize/2);
				glTexCoord2f(1.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2+0.01, -cube.cubeSize-cube.cubeSize/2);
				glEnd();
				glPopMatrix();
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}          
void COpenGLView::DrawGrid()
{
	glPushMatrix();
	//glTranslatef(cube.cubeSize*0.5,0,cube.cubeSize*0.5);
	float grid_size = cube.cubeSize *10 ;
	glColor4ub(100,100,100,1);
	glBegin(GL_LINES);
	for(int i=-grid_size;i<grid_size;i+=cube.cubeSize)
	{
		glVertex3f(i,0,-grid_size);
		glVertex3f(i,0,grid_size);
	}
	for(int i=-grid_size;i<grid_size;i+=cube.cubeSize)
	{
		glVertex3f(-grid_size,0,i);
		glVertex3f(grid_size,0,i);
	}
	glEnd();
	glPopMatrix();
}
void COpenGLView::XMatrixTranf(float x1,float y1,float z1,float x2,float y2,float z2)
{
	//float Z[3][3] = {{1,0,0},{0,0,1},{0,-1,0}};
	float nx1,nx2,ny1,ny2,nz1,nz2;
	nx1 = 1*x1;
	ny1 = -1*z1;
	nz1 = 1*y1;
	
	nx2 = 1*x2;
	ny2 = -1*z2;
	nz2 = 1*y2;

	x1 =nx1; y1 = ny1; x1 = nz1 ;
	x2 =nx2; y2 = ny2; z2 = nz2;
	pic_z[0]=x2;pic_z[1]=y2;pic_z[2]=z2;
}
void COpenGLView::ZMatrixTranf(float x1,float y1,float z1,float x2,float y2,float z2)
{
	//float X[3][3] = {{0,1,0},{0,-1,0},{0,0,1}};
	float nx1,nx2,ny1,ny2,nz1,nz2;
	nx1 = -1*y1;
	ny1 = 1*x1;
	nz1 = 1*z1;

	nx2 = -1*y2;
	ny2 = 1*x2;
	nz2 = 1*z2;

	x1 =nx1; y1 = ny1; x1 = nz1 ;
	x2 =nx2; y2 = ny2; z2 = nz2;
	pic_z[0]=x2;pic_z[1]=y2;pic_z[2]=z2;
}
void COpenGLView::XMatrixTranf2(float x1,float y1,float z1,float x2,float y2,float z2)
{
	//float Z[3][3] = {{1,0,0},{0,0,1},{0,-1,0}};
	float nx1,nx2,ny1,ny2,nz1,nz2;
	nx1 = 1*x1;
	ny1 = 1*z1;
	nz1 = -1*y1;

	nx2 = 1*x2;
	ny2 = 1*z2;
	nz2 = -1*y2;

	x1 =nx1; y1 = ny1; x1 = nz1 ;
	x2 =nx2; y2 = ny2; z2 = nz2;
	pic_z[0]=x2;pic_z[1]=y2;pic_z[2]=z2;
}
void COpenGLView::ZMatrixTranf2(float x1,float y1,float z1,float x2,float y2,float z2)
{
	//float X[3][3] = {{0,1,0},{0,-1,0},{0,0,1}};
	float nx1,nx2,ny1,ny2,nz1,nz2;
	nx1 = 1*y1;
	ny1 = -1*x1;
	nz1 = 1*z1;

	nx2 = 1*y2;
	ny2 = -1*x2;
	nz2 = 1*z2;

	x1 =nx1; y1 = ny1; x1 = nz1 ;
	x2 =nx2; y2 = ny2; z2 = nz2;
	pic_z[0]=x2;pic_z[1]=y2;pic_z[2]=z2;
}
void COpenGLView::SetTexObj(char *name,int i)
{
	glBindTexture(GL_TEXTURE_2D,textures[i]);
	int width;
	int height;
	unsigned char *image;                     //放置圖檔，已經不是BMP圖了，是能直接讓OpenGL使用的資料了
	BITMAPINFO bmpinfo;                        //用來存放HEADER資訊

	image = LoadBitmapFile( name, &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;

	//材質控制
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);

	//使用多材質
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB ,width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, image);
	
		
			// Free The Texture Image Memory
		
	      // Free The Image Structure
	
}
void COpenGLView::Texture(void)
{
	float priorities[6]={1.0,1.0,1.0,1.0,1.0,1.0};
	glGenTextures( 6, textures);
	glPrioritizeTextures( 6, textures, priorities);
	SetTexObj( "Picture\\brick2.bmp"  ,0);
	SetTexObj( "Picture\\brick3.bmp" ,1);
	SetTexObj( "Picture\\brick2.bmp"  ,2);
	SetTexObj( "Picture\\body.bmp"  ,3);
	SetTexObj( "Picture\\head.bmp"  ,4);
	
	glEnable(GL_TEXTURE_2D);
} 
unsigned char* COpenGLView::LoadBitmapFile(char *fileName, BITMAPINFO *bitmapInfo)
{
	FILE            *fp;
	BITMAPFILEHEADER   bitmapFileHeader;   // Bitmap file header
	unsigned char       *bitmapImage;      // Bitmap image data
	unsigned int      lInfoSize;         // Size of information
	unsigned int      lBitSize;         // Size of bitmap

	unsigned char change;
	int pixel;
	int p=0;

	fp = fopen(fileName, "rb");
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);         //讀取 bitmap header

	lInfoSize = bitmapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);   //Info的size
	fread(bitmapInfo, lInfoSize, 1, fp);


	lBitSize = bitmapInfo->bmiHeader.biSizeImage;                  //配置記憶體
	bitmapImage = new BYTE[lBitSize];
	fread(bitmapImage, 1, lBitSize, fp);                        //讀取影像檔

	fclose(fp);

	//此時傳回bitmapImage的話，顏色會是BGR順序，下面迴圈會改順序為RGB
	pixel = (bitmapInfo->bmiHeader.biWidth)*(bitmapInfo->bmiHeader.biHeight);

	for( int i=0 ; i<pixel ; i++, p+=3 )
	{
		//交換bitmapImage[p]和bitmapImage[p+2]的值
		change = bitmapImage[p];
		bitmapImage[p] = bitmapImage[p+2];
		bitmapImage[p+2]  = change;
	}

	return bitmapImage;
} 
void COpenGLView::drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;

	if( isFirstCall ) { // 如果是第一次調用，執行初始化
		// 为每一個ASCII字符產生一個顯示列表
		isFirstCall = 0;

		// 申請MAX_CHAR個連續的顯示列表編號
		lists = glGenLists(MAX_CHAR);

		// 把每個字符的繪制命令都裝到對應的顯示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// 調用每個字符對應的顯示列表，繪制每個字符
	for(; *str!='\0'; ++str)
		glCallList(lists + *str);
}
void COpenGLView::DrawFloorCube()
{
	glPushMatrix();
	glColor4ub(255,255,255,1);
	glTranslatef(-cube.cubeSize*0.5,-cube.cubeSize*0.5,cube.cubeSize*0.5);
	//glutSolidCube(cube.cubeSize);
	glBindTexture(GL_TEXTURE_2D,textures[0]);
	glBegin(GL_QUADS);
	glNormal3f(0,0,1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	// Back Face
	glNormal3f(0,0,-1);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	// Top Face
	glNormal3f(0,1,0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	// Bottom Face
	glNormal3f(0,-1,0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	// Right face
	glNormal3f(1,0,0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	// Left Face
	glNormal3f(-1,0,0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2, -cube.cubeSize/2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-cube.cubeSize/2, -cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2,  cube.cubeSize/2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-cube.cubeSize/2,  cube.cubeSize/2, -cube.cubeSize/2);
	glEnd();
	glPopMatrix();
	
}
void COpenGLView::DrawMap(int levelindex)
{
	glPushMatrix();
	for(int i =0 ;i<level[levelindex].zsize;i++)
	{
		for(int j = 0;j<level[levelindex].xsize;j++)
		{
			glTranslatef(cube.cubeSize,0,0);
			if(level[levelindex].mapStatus[2+j+i*level[levelindex].xsize] == 1)
			{
				DrawFloorCube();     /// draw out the normal floor 
			}
			else if(level[levelindex].mapStatus[2+j+i*level[levelindex].xsize] == 3)
			{
				DrawFloorCube();     
				startxy[0] = j;       /// record the start position 
				startxy[1] = i;
			}
			else if(level[levelindex].mapStatus[2+j+i*level[levelindex].xsize] == 2)
			{
				goalxy[0] = j;         ///record the destination position
				goalxy[1] = i;
			}
		}
		glTranslatef(-(level[levelindex].xsize)*cube.cubeSize,0,cube.cubeSize);
	}
	glPopMatrix();
}
void COpenGLView::DrawDestination()
{
	glPushMatrix();
	glColor4ub(80,150,255,1);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0,1,0);
	glVertex3f( goalxy[0]*cube.cubeSize,0,goalxy[1]*cube.cubeSize+cube.cubeSize);
	glVertex3f(goalxy[0]*cube.cubeSize+cube.cubeSize,0,goalxy[1]*cube.cubeSize);
	glVertex3f(goalxy[0]*cube.cubeSize,0,goalxy[1]*cube.cubeSize);

	glNormal3f(0,1,0);
	glVertex3f( goalxy[0]*cube.cubeSize,0,goalxy[1]*cube.cubeSize+cube.cubeSize);
	glVertex3f(goalxy[0]*cube.cubeSize+cube.cubeSize,0,goalxy[1]*cube.cubeSize+cube.cubeSize);
	glVertex3f(goalxy[0]*cube.cubeSize+cube.cubeSize,0,goalxy[1]*cube.cubeSize);
	glEnd();
	glPopMatrix();
	if(ifnextlv == false)  /// reach the goal and the fall animation 
	{
		if(cube.z_displacement == goalxy[1]*cube.cubeSize&&cube.x_displacement == goalxy[0]*cube.cubeSize && pic_z[1] ==-1)
		{
			//MessageBox("You Win!");
			ifnextlv = true;
			cube.keyFlag = FALL_OUT;
			cube.v_animate = 0;
			SetTimer(1,1,NULL);
			Invalidate();
		}
	}
	
}
void COpenGLView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	mouse_count = R_MOUSE_DOWN;
	oldmx=point.x;oldmy=point.y; 
	CView::OnRButtonDown(nFlags, point);
}
void COpenGLView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{

	dx = movex +  point.x - oldmx;
	dy = movey + point.y - oldmy;
	movex = dx;
	movey = dy;
	mouse_count = 0;
	Invalidate(NULL);
	//ClientToScreen(&point);
	//OnContextMenu(this, point);
}
void COpenGLView::loadLevelmap()
{
		level[0].getlevelData("levelMapData\\level1.txt");
		levelCount++;
		level[1].getlevelData("levelMapData\\level2.txt");
		levelCount++;
		level[2].getlevelData("levelMapData\\level3.txt");
		levelCount++;
}
void COpenGLView::initialModel()
{
	cube.cubeSize = 4;
	cube.x_degree = 0;cube.z_degree = 0;
	GetLocalTime ( &st );
	tick = true;
	m_time = 0;
	move_count = 0 ;
	if(startxy[0]>0)
	{
		cube.x = startxy[0];
		cube.z = startxy[1];
	}
	else
	{
		cube.x = 0;
		cube.z = 0;
	}
	
	cube.x_displacement = cube.x*cube.cubeSize;
	cube.z_displacement = cube.z*cube.cubeSize; 
	cube.y_displacement = cube.cubeSize;
	cube.y_animate = 0;
	tempx = cube.x_displacement/cube.cubeSize; // predict the next move position 
	tempz = cube.z_displacement/cube.cubeSize;
	tempx2=tempx;
	tempz2=tempz;

	cube.animate_isdone = true;
	cube.y_animate = 0;
	cube.v_animate = 0;

	cube.x_[0] = 1; cube.x_[1] =0; cube.x_[2] = 0;
	cube.z_[0] = 0; cube.z_[1] =0; cube.z_[2] = 1;

	pic_z[0] = 0;pic_z[1] = 0; pic_z[2] =-1;
	zislong = 2;
	xislong = 2;

	cube.keyFlag = NOMOVE;
	XMatrixTranf(cube.x_[0],cube.x_[1],cube.x_[2],pic_z[0],pic_z[1],pic_z[2]);
	cube.x_count.clear();
	cube.z_count.clear();
	cube.a[0].clear();
	cube.a[1].clear();
	cube.a[2].clear();
	cube.b[0].clear();
	cube.b[1].clear();
	cube.b[2].clear();
	cube.x_count.push_back(1);
	cube.z_count.push_back(0);
	cube.a[0].push_back(cube.x_[0]);
	cube.a[1].push_back(cube.x_[1]);
	cube.a[2].push_back(cube.x_[2]);
	cube.b[0].push_back(cube.z_[0]);
	cube.b[1].push_back(cube.z_[1]);
	cube.b[2].push_back(cube.z_[2]);
	//////////////////////////////////// initial the cube 
	
	
}
void COpenGLView::Playmusic(int type)
{
	if(ismute == false)
	{
		switch (type)   /// .wav
		{
		case CUBEROLL:
			PlaySound("Music\\beebee.wav",NULL,SND_FILENAME|SND_ASYNC);
			break;
		case NO:
			PlaySound("Music\\Error.wav",NULL,SND_FILENAME|SND_ASYNC);
			break;
		}
	}
	
}
void COpenGLView::OnSettingSound()
{
	if(ismute == false)
	{
		ismute = true;
		//GetMenu()->CheckMenuItem(0,MF_BYPOSITION|MF_CHECKED   );
	}
	else if(ismute == true)
	{
		ismute = false;
	}
	// TODO: 在此加入您的命令處理常式程式碼
}
void COpenGLView::fallanimation()
{
	if(ifnextlv == true)
		{
			if(cube.keyFlag == FALL_OUT)
			{
				cube.v_animate = cube.v_animate -9.8*0.1;
				cube.y_animate += cube.v_animate*0.1;
				if(cube.y_animate<=-80)
				{
					cube.animate_isdone = true ;
					cube.y_animate = 0;
					ifnextlv = false ;
					KillTimer(1);

					//MessageBox("Next Level");
					initialModel();
					if(m_levelindex<levelCount-1)
					{
						m_levelindex++;
						cube.keyFlag = FALL_IN;
						cube.y_animate = 50 ;
						SetTimer(1,1,NULL);
					}
					else if (m_levelindex >= levelCount-1)          /// if all the level are finished 
					{
						/// should have a celebration scene before the restart!
						TCHAR path[MAX_PATH];                         // restart the program
						GetModuleFileName(NULL,path,MAX_PATH);
						ShellExecute(NULL,TEXT("OPEN"),path,NULL,NULL,SW_SHOWNORMAL);
						exit(0);
					}

				}
			}			
		}
	if(cube.keyFlag == FALL_IN)
	{
		cube.v_animate = cube.v_animate -9.8*0.1;
		cube.y_animate += cube.v_animate*0.1;
		DrawMap(m_levelindex);
		if(startxy[0]>0)
		{
			cube.x = startxy[0];
			cube.z = startxy[1];
			cube.x_displacement = cube.x*cube.cubeSize;
			cube.z_displacement = cube.z*cube.cubeSize; 
		}
		else
		{
			cube.x = 0;
			cube.z = 0;
			cube.x_displacement = cube.x*cube.cubeSize;
			cube.z_displacement = cube.z*cube.cubeSize; 
		}
		if(cube.y_animate<=0)
		{
			cube.animate_isdone = true ;
			cube.y_animate = 0;
			initialModel();
			KillTimer(1);
			Playmusic(CUBEROLL);
			Invalidate();
		}

	}
}
void COpenGLView::OnSettingLevel()
{
	scene = 1;
	KillTimer(1);
	levelDlg.DoModal();
	m_levelindex = levelDlg.level_index;
	if(levelDlg.level_index>=0)
	{
		DrawMap(m_levelindex);
		initialModel();
		scene = 2;
		Invalidate();
		initialModel();
		cube.keyFlag = FALL_IN;
		cube.y_animate = 50 ;
		SetTimer(1,1,NULL);
	}
	// TODO: 在此加入您的命令處理常式程式碼
}
void COpenGLView::OnMembers()
{
	// TODO: 在此加入您的命令處理常式程式碼

	if(membersDlg.DoModal()==IDOK)
	{
		Invalidate();
	}
}
afx_msg void COpenGLView::OnMute(CCmdUI *aCmdUI)
{
	aCmdUI->SetCheck(ismute == false);

}
