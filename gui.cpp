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
#include "gui.h"
#include "407_wdr.h"

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/timer.h"
#include "shape.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include <sstream>
using namespace std;


#define ID_TIMER 12312

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_TIMER(ID_TIMER, MyFrame::OnTimer)
    EVT_LIST_ITEM_SELECTED(ID_LISTCTRL, MyFrame::OnSelectShape)
    EVT_COMBOBOX(ID_COMBO, MyFrame::OnListFace)
    EVT_BUTTON(ID_ORB, MyFrame::OnOrbit)
    EVT_BUTTON(ID_STAB, MyFrame::OnStab)
    EVT_COMBOBOX(ID_ROT_BOX, MyFrame::OnRotBox)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style)
         : wxFrame(frame, -1, title, pos, size, style), m_timer(this, ID_TIMER), 
         m_shapeIndex(-1)
{
    wxPanel * panel = new wxPanel(this);

    m_html = new wxHtmlWindow(panel, ID_HTML, wxDefaultPosition,
        wxSize(200,200), wxHW_SCROLLBAR_AUTO);
    m_html->Show(true);

    try
    {

    m_canvas = new SolidGLCanvas(panel, ID_GLCANVAS
        , wxDefaultPosition);
    }
    catch (std::exception &e)
    {
        wxMessageBox(e.what(), "Exception!");
        wxExit();

    }

    MainPanel(panel, true, true);

    CreateImageList();
    wxComboBox * list = (wxComboBox *)FindWindow(ID_ROT_BOX);
    list->Clear();

    Show(true);

    m_count = 1000;
    //m_timer.Start(50);

}

MyFrame::~MyFrame()
{
    delete m_imageList;
    delete m_canvas;
}

void MyFrame::CreateImageList()
{

    wxBitmap cube("images/cube.png", wxBITMAP_TYPE_PNG);
    wxBitmap cuboctahedron("images/cuboctahedron.png", wxBITMAP_TYPE_PNG);
    wxBitmap icosidodecahedron("images/icosidodecahedron.png", wxBITMAP_TYPE_PNG);
    wxBitmap octahedron("images/octahedron.png", wxBITMAP_TYPE_PNG);
    wxBitmap pentakisdodecahedron("images/pentakisdodecahedron.png", wxBITMAP_TYPE_PNG);
    wxBitmap snub_cuboctahedron("images/snub_cuboctahedron.png", wxBITMAP_TYPE_PNG); 
    wxBitmap snub_icosidodecahedron("images/snub_icosidodecahedron.png", wxBITMAP_TYPE_PNG);
    //wxBitmap truncated_cuboctahedron("images/truncated_cuboctahedron.png", wxBITMAP_TYPE_PNG);
    wxBitmap truncated_dodecahedron("images/truncated_dodecahedron.png", wxBITMAP_TYPE_PNG);
    wxBitmap truncated_icosahedron("images/truncated_icosahedron.png", wxBITMAP_TYPE_PNG);
    //wxBitmap truncated_icosidodecahedron("images/truncated_icosidodecahedron.png", wxBITMAP_TYPE_PNG);

    m_imageList = new wxImageList(128,128, false);
    m_imageList->Add(cube);
    m_imageList->Add(cuboctahedron);
    m_imageList->Add(icosidodecahedron);
    m_imageList->Add(octahedron);
    m_imageList->Add(pentakisdodecahedron);
    m_imageList->Add(snub_cuboctahedron);
    m_imageList->Add(snub_icosidodecahedron);
    //m_imageList->Add(truncated_cuboctahedron);
    m_imageList->Add(truncated_dodecahedron);
    m_imageList->Add(truncated_icosahedron);
//    m_imageList->Add(truncated_icosidodecahedron);

    wxListCtrl * list = (wxListCtrl *)FindWindow(ID_LISTCTRL);
    //list->SetSingleStyle(wxLC_ICON); 
    list->SetImageList(m_imageList, wxIMAGE_LIST_NORMAL);
    
    list->InsertItem(0, "triakisicosahedron", -1);
    list->InsertItem(0, "triakisoctahedron");
    list->InsertItem(0, "truncated_cube");
    list->InsertItem(0, "truncated_octahedron");
    list->InsertItem(0, "trapezoidal_icositetrahedron");
    list->InsertItem(0, "trapezoidal_hexecontahedron");
    list->InsertItem(0, "tetrakishexahedron");
    list->InsertItem(0, "rhombicuboctahedron");
    list->InsertItem(0, "rhombicosidodecahedron");
    list->InsertItem(0, "rhombic_triacontahedron");
    list->InsertItem(0, "rhombic_dodecahedron");
    list->InsertItem(0, "pentagonal_icositetrahedron");
    list->InsertItem(0, "pentagonal_hexecontahedron");
    list->InsertItem(0, "great_rhombicosidodecahedron");
    list->InsertItem(0, "disdyakis_triacontahedron");
    list->InsertItem(0, "disdyakis_dodecahedron");
    //list->InsertItem(0, "", 10);
   // list->InsertItem(0, "", 9);
    list->InsertItem(0, "", 8);
    list->InsertItem(0, "", 7);
    list->InsertItem(0, "", 6);
    list->InsertItem(0, "", 5);
    list->InsertItem(0, "", 4);
    list->InsertItem(0, "", 3);
    list->InsertItem(0, "", 2);
    list->InsertItem(0, "", 1);
    list->InsertItem(0, "", 0);


}

void MyFrame::OnOrbit(wxCommandEvent& event)
{
    if (m_canvas->m_shape)
    {
        if (m_canvas->m_shape->getSelectedFace() == -1)
            return;

        if (m_canvas->m_shape->getOrbit().size() == 0)
        {
           // wxLogMessage("Need to calculate orbit.");
            m_canvas->m_shape->findOrbit();
           
        }

        RGB rgb;
        rgb.R = 50;
        rgb.G = 50;
        rgb.B = 50;
        m_canvas->m_shape->highlightFace(m_canvas->m_shape->getOrbit(), rgb); 
        m_canvas->Refresh(false);

        stringstream st;
        for (int i = 0; i < m_canvas->m_shape->getOrbit().size(); i++)
        {
            if (i != (m_canvas->m_shape->getOrbit().size() - 1))
                st << m_canvas->m_shape->getOrbit()[i] << ", ";
            else
                st << m_canvas->m_shape->getOrbit()[i];
        }

        wxString info;
        info.Printf("<html><body><h1>%s</h1>Orb(%d) = { %s }</body></html>", 
            m_canvas->m_shape->name().c_str(), 
            m_canvas->m_shape->getSelectedFace(),
            st.str().c_str());

        m_html->SetPage(info);
       // wxLogMessage("Order of orbit is: %d", m_canvas->m_shape->getOrbit().size());
    }



}

void MyFrame::OnStab(wxCommandEvent& event)
{
    if (m_canvas->m_shape)
    {
        if (m_canvas->m_shape->getSelectedFace() == -1)
            return;

        m_canvas->m_shape->calcStab();
        vector<double> stabs = m_canvas->m_shape->getStabs();
        
        stringstream st;
        //st << "Stabs are: ";
        wxComboBox * list = (wxComboBox *)FindWindow(ID_ROT_BOX);
        list->Clear();
        for (unsigned int i = 0; i < stabs.size(); i++)
        {
            if (i != (stabs.size() - 1))
                st << "R" << stabs[i] << ", ";
            else
                st << "R" << stabs[i];

            stringstream tmp;
            tmp << stabs[i];
            list->Append(wxString(tmp.str().c_str()) + wxString(" degrees"));
        }

        wxString info;
        info.Printf("<html><body><h1>%s</h1>Stab(%d) = { %s }</body></html>", 
            m_canvas->m_shape->name().c_str(), 
            m_canvas->m_shape->getSelectedFace(),
            st.str().c_str());

        m_html->SetPage(info);



        //wxMessageBox(st.str().c_str(), "Stabilizers");
    }
}

void MyFrame::OnSize(wxSizeEvent& event)
{
}

void MyFrame::OnRotBox(wxCommandEvent& event)
{

}

void MyFrame::OnListFace(wxCommandEvent& event)
{
    wxComboBox * tmp = (wxComboBox *)FindWindow(ID_COMBO);

    if (tmp->GetSelection() == -1)
        return; // no selection

    RGB rgb;
    rgb.R = 0;
    rgb.B = 200;
    rgb.G = 0;

    if (m_canvas->m_shape)
    {
        m_canvas->m_shape->selectFace(tmp->GetSelection(), rgb);
        m_canvas->Refresh(false);
    }

}

void MyFrame::LoadFaceData()
{
    wxComboBox * tmp = (wxComboBox *)FindWindow(ID_COMBO);
    tmp->Clear();

    if (m_canvas->m_shape)
    {
        for (unsigned int i = 0; i < m_canvas->m_shape->getFaces().size(); i++)
        {
            stringstream doh;

            if (m_canvas->m_shape->getFaces()[i].verts.size() == 3)
                doh << "Triangle ";
            if (m_canvas->m_shape->getFaces()[i].verts.size() == 4)
                doh << "Quadrilateral ";
            if (m_canvas->m_shape->getFaces()[i].verts.size() == 5)
                doh << "Pentagon ";
            if (m_canvas->m_shape->getFaces()[i].verts.size() == 6)
                doh << "Hexagon ";
            if (m_canvas->m_shape->getFaces()[i].verts.size() == 7)
                doh << "Heptagon ";
            if (m_canvas->m_shape->getFaces()[i].verts.size() == 8)
                doh << "Octagon ";
            if (m_canvas->m_shape->getFaces()[i].verts.size() == 9)
                doh << "Nonagon ";
            if (m_canvas->m_shape->getFaces()[i].verts.size() == 10)
                doh << "Decagon ";

            /// add one since indexed from 0 :)
            doh << (i + 1);
            tmp->Append(doh.str().c_str());
        }
    }

}

void MyFrame::OnSelectShape(wxListEvent& event)
{
    if (event.GetIndex() == m_shapeIndex)
        return; // same item was selected

    if (m_canvas->m_shape)
    {
        delete m_canvas->m_shape;
        m_canvas->m_shape = 0;
    }
    
    switch(event.GetIndex())
    {
    case (0): m_canvas->m_shape = new Cube(); break;
    case (1): m_canvas->m_shape = new Cuboctahedron(); break;
    case (2): m_canvas->m_shape = new Icosidodecahedron(); break;
    case (3): m_canvas->m_shape = new Octahedron(); break;
    case (4): m_canvas->m_shape = new Pentakisdodecahedron(); break;
    case (5): m_canvas->m_shape = new SnubCuboctahedron(); break;
    case (6): m_canvas->m_shape = new SnubIcosidodecahedron(); break;
    case (7): m_canvas->m_shape = new TruncatedDodecahedron(); break;
    case (8): m_canvas->m_shape = new TruncatedIcosahedron(); break;
    default: m_canvas->m_shape = new Cube();
    }

    m_shapeIndex = event.GetIndex();
    LoadFaceData();

    m_canvas->Refresh(false);

}


void MyFrame::OnTimer(wxTimerEvent& event)
{

    if (m_count-- == 0)
        m_timer.Stop();

    m_canvas->Refresh(false);

}


// Intercept menu commands
void MyFrame::OnExit(wxCommandEvent& event)
{
    Destroy();
}

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
    wxImage::AddHandler(new wxPNGHandler);


    MyFrame *frame = new MyFrame(NULL, "MA407 Symmetry Groups", wxPoint(50, 50),
                                wxSize(400, 300));
    frame->SetIcon(wxIcon("mondrian"));

    return TRUE;
}

GLfloat		yrot;
GLfloat     xrot;

GLfloat LightAmbient[]=  { 0.5f, 0.5f, 0.5f, 1.0f };	// Ambient Light Values
GLfloat LightDiffuse[]=	 { 1.0f, 1.0f, 1.0f, 1.0f };	// Diffuse Light Values
GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };	// Light Position

BEGIN_EVENT_TABLE(SolidGLCanvas, wxGLCanvas)
  EVT_SIZE(SolidGLCanvas::OnSize)
  EVT_PAINT(SolidGLCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(SolidGLCanvas::OnEraseBackground)
  EVT_ENTER_WINDOW( SolidGLCanvas::OnEnterWindow )
  EVT_MOUSE_EVENTS(SolidGLCanvas::OnMouseEvent)
END_EVENT_TABLE()


SolidGLCanvas::SolidGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name):
  wxGLCanvas(parent, id, pos, size, style, name ), m_shape(0)
{
    m_init = false;
    InitGL();
}


SolidGLCanvas::~SolidGLCanvas()
{
    delete m_shape;
    glDeleteLists(m_base, 256);
}

void SolidGLCanvas::Render()
{
}

void SolidGLCanvas::OnEnterWindow( wxMouseEvent& event )
{
    SetFocus();
}

void SolidGLCanvas::DrawXYZ(wxPaintDC * dc)
{

}

GLvoid SolidGLCanvas::glPrint(const string& text, bool coords, float x, float y, float z)	// Custom GL "Print" Routine
{
    /// from Nehe's text tutorial 14

    float length=0;		// Used To Find The Length Of The Text
    if (text.empty())	// If There's No Text
		return;			// Do Nothing 

    for (unsigned int loop=0; loop < text.length(); loop++)	// Loop To Find Text Length
	{
        length+=m_gmf[text[loop]].gmfCellIncX;	// Increase Length By Each Characters Width
	}

    if (coords == true)
        glTranslatef(x, y, z);
    else
        glTranslatef(-length/2,1.7f,0.0f);	// Center Our Text On The Screen

    glPushAttrib(GL_LIST_BIT);			// Pushes The Display List Bits
	glListBase(m_base);					// Sets The Base Character to 0
    glCallLists((GLsizei)text.length(), GL_UNSIGNED_BYTE, text.c_str());	// Draws The Display List Text
	glPopAttrib();						// Pops The Display List Bits


    if (coords == true)
        glTranslatef(-x, -y, -z);
    else
        glTranslatef(length/2,-1.7f,0.0f);
}
/*
GLvoid SolidGLCanvas::glPrint2d(const string& text, float x, float y)
{
    /// from Nehe's text tutorial 14

    float length=0;		// Used To Find The Length Of The Text
    if (text.empty())	// If There's No Text
		return;			// Do Nothing 

    for (unsigned int loop=0; loop < text.length(); loop++)	// Loop To Find Text Length
	{
        length+=m_gmf[text[loop]].gmfCellIncX;	// Increase Length By Each Characters Width
	}

    gluOrtho2d(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);

    glTranslatef(x, y, 0);
    
    glPushAttrib(GL_LIST_BIT);			// Pushes The Display List Bits
	glListBase(m_base);					// Sets The Base Character to 0
    glCallLists(text.length(), GL_UNSIGNED_BYTE, text.c_str());	// Draws The Display List Text
	glPopAttrib();	// Pops The Display List Bits


    glTranslatef(-x, -y, 0);

}
*/
GLvoid SolidGLCanvas::BuildFont(GLvoid)					// Build Our Bitmap Font
{
    m_base = glGenLists(256);					// Storage For 256 Characters
#ifdef __WXMSW__

#include <windows.h>      

    HDC hDC=GetDC((HWND)GetHandle()) ;	// APPEL wxWindows
    HFONT font = CreateFont(12,12,0,0,
        FW_BOLD,
        FALSE,
        FALSE,
        FALSE,
        ANSI_CHARSET,
        OUT_TT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        FF_DONTCARE|DEFAULT_PITCH,
        "Arial");		// Font Name

    SelectObject(hDC, font);		// Selects The Font We Created

    wglUseFontOutlines(	hDC,		// Select The Current DC
				0,					// Starting Character
				255,				// Number Of Display Lists To Build
				m_base,				// Starting Display Lists
				0.0f,				// Deviation From The True Outlines
				0.2f,				// Font Thickness In The Z Direction
				WGL_FONT_POLYGONS,		// Use Polygons, Not Lines
				m_gmf);				// Address Of Buffer To Recieve Data
#endif
}


void SolidGLCanvas::OnPaint( wxPaintEvent& event )
{
    
    wxPaintDC dc(this);

    if (!GetContext()) 
    {
        wxLogMessage("No context");
        return;
    }

    SetCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();					// Reset The View
  
    glTranslatef(0.0f,0.0f,-6.0f);
    glRotatef( yrot, 0.0, 1.0, 0.0 );
    glRotatef( xrot, 1.0, 0.0, 0.0 );	// Rotate 

    if (m_shape)
    {
        m_shape->render();

        glScalef(0.5f, 0.5f, 0.5f);
        glColor4f(0.0f,0.0f,0.8f,1.0f);
        glPrint("X", true, 3.0f, -0.2f, 0.1f);
        glPrint("Y", true, -0.9f, 3.0f, 0.1f);
        glPrint("Z", true, -1.5f, -0.2f, -3.0f);
    }

    //DrawXYZ(&dc);

    glFlush();
    SwapBuffers();

}

void SolidGLCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);

    if (GetContext())
    {
        SetCurrent();

        glViewport(0, 0, (GLint) w, (GLint) h);

        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	    glLoadIdentity();									// Reset The Projection Matrix

	    // Calculate The Aspect Ratio Of The Window
	    gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);

	    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	    glLoadIdentity();	

    }
}

void SolidGLCanvas::OnEraseBackground(wxEraseEvent& event)
{
  // Do nothing, to avoid flashing.
}

void SolidGLCanvas::InitGL()
{
    //SetCurrent();
    m_init = true;
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	//glClearColor( .3, .4, .6, 1 );
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_BLEND);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	//glColor4f(1.0f,1.0f,1.0f,0.5f);			// Full Brightness, 50% Alpha ( NEW )
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		// Really Nice Perspective Calculations
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);				// Setup The Diffuse Light
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);

    glEnable(GL_LIGHT1);

    BuildFont();

}

void SolidGLCanvas::OnMouseEvent(wxMouseEvent& event)
{
    static int dragging = 0;
    static float last_x, last_y;

    if (event.LeftIsDown()) 
    {
        if (!dragging) 
        {
            dragging = 1;
        } 
        else 
        {
            yrot += (event.GetX() - last_x)*1.0;
            xrot += (event.GetY() - last_y)*1.0;
            Refresh(false);
        }

        last_x = event.GetX();
        last_y = event.GetY();
    } 
    else
        dragging = 0;
}

