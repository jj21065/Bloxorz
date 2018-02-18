// glPrintf.cpp
//
// Created by Gary Ho, ma_hty@hotmail.com, 2009
//
#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <GL/glut.h>

int glPrintf( const char *format, ... )
{
	DWORD fdwCharSet = CHINESEBIG5_CHARSET;
	char szFace[] = "Courier New";
	int nHeight = 16;


	char buffer[65536];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	static GLuint base = 0;
	static bool loaded[65536] = {0};
	static HFONT hFont;
	if( base==0 )
	{
		base = glGenLists(65536);
		hFont = CreateFont( nHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, fdwCharSet, 
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			FF_DONTCARE | DEFAULT_PITCH, szFace );
	}

	float p0[4], p1[4], c0[2][4] = {{0,0,0,1},{1,1,1,1}};
	int i, j, len, offset[2]={1,0};
	wchar_t *wstr;
	GLint viewport[4];
	HDC hdc = 0;

	glGetIntegerv( GL_VIEWPORT, viewport );
	glGetFloatv( GL_CURRENT_RASTER_POSITION, p0 );

	glPushAttrib( GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0,viewport[2],0,viewport[3]); 

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if( strcmp( buffer, ">>glut" )==0 )
	{
		glRasterPos2f( 4, viewport[3]-nHeight );
	}else if( strcmp( buffer, ">>free" )==0 )
	{
		glDeleteLists(base,65536); base=0;
		memset( loaded, 0, 65536*sizeof(bool) );
		DeleteObject(hFont);
	}else
	{
		glRasterPos2f( p0[0], p0[1] );
		glGetFloatv( GL_CURRENT_RASTER_COLOR, c0[1] );
		len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, 0, 0);
		wstr = (wchar_t*)malloc( len*sizeof(wchar_t) );
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, wstr, len);
		for( j=0; j<2; j++ )
		{
			glColor4fv( c0[j] );
			glRasterPos2f( p0[0]+offset[j], p0[1]-offset[j] );
			for( i=0; i<len-1; i++ )
			{
				if( wstr[i]=='\n' )
				{
					glGetFloatv( GL_CURRENT_RASTER_POSITION, (float*)&p1 );
					glRasterPos2f( 4+offset[j], p1[1]-(nHeight+2) );
				}else
				{
					if( !loaded[wstr[i]] )
					{
						if( hdc==0 )
						{
							hdc = wglGetCurrentDC();
							SelectObject( hdc, hFont );
						}
						wglUseFontBitmapsW( hdc, wstr[i], 1, base+wstr[i] );
						loaded[wstr[i]] = true;
					}
					glCallList(base+wstr[i]);
				}
			}
		}
		free(wstr);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();

	return 0;
}