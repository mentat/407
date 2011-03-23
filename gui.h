// --*-c++-*--
/*
    Symmetry Groups Software
    Copyright (C) 2003  Jesse Lovelace <jllovela@ncsu.edu>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,

    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MA407_GUI_H
#define MA407_GUI_H

#include "wx/wx.h"
#include "wx/html/htmlwin.h"
#include "wx/glcanvas.h"
#include "wx/listctrl.h"
#include "shape.h"

#include <GL/gl.h>
#include <GL/glu.h>

// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

// Define a new frame type
class SolidGLCanvas;
class wxImageList;
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    ~MyFrame();

    void OnExit(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnSelectShape(wxListEvent& event);
    void OnListFace(wxCommandEvent& event);
    void OnOrbit(wxCommandEvent& event);
    void OnStab(wxCommandEvent& event);

    void OnRotBox(wxCommandEvent& event);
    
public:
    void CreateImageList();

    /// Load the face data into the choice box
    void LoadFaceData();

    wxHtmlWindow * m_html;
    int m_count;
    int m_shapeIndex;
    wxTimer m_timer;

    wxImageList * m_imageList;

    SolidGLCanvas*    m_canvas;

    DECLARE_EVENT_TABLE()
};

class SolidGLCanvas: public wxGLCanvas
{
    friend class MyFrame;
public:
    SolidGLCanvas(wxWindow *parent, const wxWindowID id = -1, 
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = "TestGLCanvas");

    ~SolidGLCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnEnterWindow( wxMouseEvent& event );

    void OnMouseEvent( wxMouseEvent& event );
     
    void Render( void );
    void InitGL(void);
    void Rotate( GLfloat deg );

private:

   // GLvoid glPrint2d(const string& text, float x, float y);
    GLvoid glPrint(const string& text, bool coords= false, float x=0.0, float y=0.0, float z=0.0);
    // Custom GL "Print" Routine
    GLvoid BuildFont(GLvoid);
    void DrawXYZ(wxPaintDC * dc);

    Shape * m_shape;

    bool   m_init;

    /// for fonts
    GLuint m_base;
    GLYPHMETRICSFLOAT m_gmf[256];

    DECLARE_EVENT_TABLE()
};


#endif
