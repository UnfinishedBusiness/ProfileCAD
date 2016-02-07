#include "wx/wxprec.h"

using namespace std;

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "main.h"
#include "application.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../res/icon.xpm"
#endif

/****************** Globals ***********************/
bool PostRedisplay_Register = false;
point_t MousePosition;


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

static void CheckGLError()
{
    GLenum errLast = GL_NO_ERROR;

    for ( ;; )
    {
        GLenum err = glGetError();
        if ( err == GL_NO_ERROR )
            return;

        // normally the error is reset by the call to glGetError() but if
        // glGetError() itself returns an error, we risk looping forever here
        // so check that we get a different error than the last time
        if ( err == errLast )
        {
            wxLogError(wxT("OpenGL error state couldn't be reset."));
            return;
        }

        errLast = err;

        wxLogError(wxT("OpenGL error %d"), err);
    }
}

// function to draw the texture for cube faces
static wxImage DrawDice(int size, unsigned num)
{
    wxASSERT_MSG( num >= 1 && num <= 6, wxT("invalid dice index") );

    const int dot = size/16;        // radius of a single dot
    const int gap = 5*size/32;      // gap between dots

    wxBitmap bmp(size, size);
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetBrush(*wxBLACK_BRUSH);

    // the upper left and lower right points
    if ( num != 1 )
    {
        dc.DrawCircle(gap + dot, gap + dot, dot);
        dc.DrawCircle(size - gap - dot, size - gap - dot, dot);
    }

    // draw the central point for odd dices
    if ( num % 2 )
    {
        dc.DrawCircle(size/2, size/2, dot);
    }

    // the upper right and lower left points
    if ( num > 3 )
    {
        dc.DrawCircle(size - gap - dot, gap + dot, dot);
        dc.DrawCircle(gap + dot, size - gap - dot, dot);
    }

    // finally those 2 are only for the last dice
    if ( num == 6 )
    {
        dc.DrawCircle(gap + dot, size/2, dot);
        dc.DrawCircle(size - gap - dot, size/2, dot);
    }

    dc.SelectObject(wxNullBitmap);

    return bmp.ConvertToImage();
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// TestGLContext
// ----------------------------------------------------------------------------

TestGLContext::TestGLContext(wxGLCanvas *canvas)
             : wxGLContext(canvas)
{
    SetCurrent(*canvas);

    // set up the parameters we want to use
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    // add slightly more light, the default lighting is rather dark
    GLfloat ambient[] = { 0.5, 0.5, 0.5, 0.5 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    // set viewing projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

    // create the textures to use for cube sides: they will be reused by all
    // canvases (which is probably not critical in the case of simple textures
    // we use here but could be really important for a real application where
    // each texture could take many megabytes)
    glGenTextures(WXSIZEOF(m_textures), m_textures);

    for ( unsigned i = 0; i < WXSIZEOF(m_textures); i++ )
    {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        const wxImage img(DrawDice(256, i + 1));

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(),
                     0, GL_RGB, GL_UNSIGNED_BYTE, img.GetData());
    }

    CheckGLError();
}
void TestGLContext::DrawScene()
{
  sceneDraw();
}
void TestGLContext::DrawRotatedCube(float xangle, float yangle)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(xangle, 1.0f, 0.0f, 0.0f);
    glRotatef(yangle, 0.0f, 1.0f, 0.0f);

    // draw six faces of a cube of size 1 centered at (0, 0, 0)
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glBegin(GL_QUADS);
        glNormal3f( 0.0f, 0.0f, 1.0f);
        glTexCoord2f(0, 0); glVertex3f( 0.5f, 0.5f, 0.5f);
        glTexCoord2f(1, 0); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1, 1); glVertex3f(-0.5f,-0.5f, 0.5f);
        glTexCoord2f(0, 1); glVertex3f( 0.5f,-0.5f, 0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[1]);
    glBegin(GL_QUADS);
        glNormal3f( 0.0f, 0.0f,-1.0f);
        glTexCoord2f(0, 0); glVertex3f(-0.5f,-0.5f,-0.5f);
        glTexCoord2f(1, 0); glVertex3f(-0.5f, 0.5f,-0.5f);
        glTexCoord2f(1, 1); glVertex3f( 0.5f, 0.5f,-0.5f);
        glTexCoord2f(0, 1); glVertex3f( 0.5f,-0.5f,-0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[2]);
    glBegin(GL_QUADS);
        glNormal3f( 0.0f, 1.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f( 0.5f, 0.5f, 0.5f);
        glTexCoord2f(1, 0); glVertex3f( 0.5f, 0.5f,-0.5f);
        glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f,-0.5f);
        glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[3]);
    glBegin(GL_QUADS);
        glNormal3f( 0.0f,-1.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(-0.5f,-0.5f,-0.5f);
        glTexCoord2f(1, 0); glVertex3f( 0.5f,-0.5f,-0.5f);
        glTexCoord2f(1, 1); glVertex3f( 0.5f,-0.5f, 0.5f);
        glTexCoord2f(0, 1); glVertex3f(-0.5f,-0.5f, 0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[4]);
    glBegin(GL_QUADS);
        glNormal3f( 1.0f, 0.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f( 0.5f, 0.5f, 0.5f);
        glTexCoord2f(1, 0); glVertex3f( 0.5f,-0.5f, 0.5f);
        glTexCoord2f(1, 1); glVertex3f( 0.5f,-0.5f,-0.5f);
        glTexCoord2f(0, 1); glVertex3f( 0.5f, 0.5f,-0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[5]);
    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(-0.5f,-0.5f,-0.5f);
        glTexCoord2f(1, 0); glVertex3f(-0.5f,-0.5f, 0.5f);
        glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f,-0.5f);
    glEnd();

    glFlush();

    CheckGLError();
}


// ----------------------------------------------------------------------------
// MyApp: the application object
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    new MyFrame();

    return true;
}

int MyApp::OnExit()
{
    delete m_glContext;
    delete m_glStereoContext;

    return wxApp::OnExit();
}

TestGLContext& MyApp::GetContext(wxGLCanvas *canvas, bool useStereo)
{
    TestGLContext *glContext;
    if ( useStereo )
    {
        if ( !m_glStereoContext )
        {
            // Create the OpenGL context for the first stereo window which needs it:
            // subsequently created windows will all share the same context.
            m_glStereoContext = new TestGLContext(canvas);
        }
        glContext = m_glStereoContext;
    }
    else
    {
        if ( !m_glContext )
        {
            // Create the OpenGL context for the first mono window which needs it:
            // subsequently created windows will all share the same context.
            m_glContext = new TestGLContext(canvas);
        }
        glContext = m_glContext;
    }

    glContext->SetCurrent(*canvas);
    return *glContext;
}

// ----------------------------------------------------------------------------
// GLCanvas
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
    EVT_PAINT(GLCanvas::OnPaint)
    EVT_KEY_DOWN(GLCanvas::OnKeyDown)
    EVT_KEY_UP(GLCanvas::OnKeyUp)
    EVT_LEFT_DOWN(GLCanvas::OnMouseLeftDown)
    EVT_MOUSE_EVENTS(GLCanvas::OnMouse)
    EVT_IDLE(GLCanvas::OnIdle)
wxEND_EVENT_TABLE()

GLCanvas::GLCanvas(wxWindow *parent, int *attribList)
    // With perspective OpenGL graphics, the wxFULL_REPAINT_ON_RESIZE style
    // flag should always be set, because even making the canvas smaller should
    // be followed by a paint event that updates the entire canvas with new
    // viewport settings.
    : wxGLCanvas(parent, wxID_ANY, attribList,
                 wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE),
      m_xangle(30.0),
      m_yangle(30.0),
      m_useStereo(false),
      m_stereoWarningAlreadyDisplayed(false)
{
    if ( attribList )
    {
        int i = 0;
        while ( attribList[i] != 0 )
        {
            if ( attribList[i] == WX_GL_STEREO )
                m_useStereo = true;
            ++i;
        }
    }
}

void GLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // This is required even though dc is not used otherwise.
    wxPaintDC dc(this);
    const wxSize ClientSize = GetClientSize();

    TestGLContext& canvas = wxGetApp().GetContext(this, m_useStereo);

    //glViewport(0, 0, ClientSize.x, ClientSize.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();//these two lines are unchanged

    glOrtho( -ClientSize.x/2, ClientSize.x/2, -ClientSize.y/2, ClientSize.y/2, -1,1 );
    //glScalef(1, ClientSize.x/ClientSize.y + ClientSize.x/ClientSize.y + 0.5, 1); //Not sure wtf, but it works!
    //gluPerspective(0, 16.0/9.0*float(width)/float(height), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0,0,ClientSize.x,ClientSize.y);



    canvas.DrawScene();
    SwapBuffers();
}

void GLCanvas::Spin(float xSpin, float ySpin)
{
    m_xangle += xSpin;
    m_yangle += ySpin;

    //Refresh(false);
    PostRedisplay();
}
void PostRedisplay()
{
  PostRedisplay_Register = true;
  //V printf("Redrawing!\n");
}

/* See http://docs.wxwidgets.org/trunk/classwx_mouse_event.html for mouse events */

point_t LastMouseScrollPosition;
void GLCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
  wxPoint m = event.GetPosition();
  MousePosition = cadScreenCordToCadCord(m.x, m.y);
  mouseClick(MOUSE_LEFT_BUTTON, MOUSE_DOWN, MousePosition.x, MousePosition.y);
}
void GLCanvas::OnMouse(wxMouseEvent& event)
{
  int ScrollWheel = event.GetWheelRotation();

  wxPoint m = event.GetPosition();
  MousePosition = cadScreenCordToCadCord(m.x, m.y);

  wxLogStatus("X: %.6f Y: %.6f Zoom: %.6f", MousePosition.x, MousePosition.y, sceneGetScale());
  //MousePosition.z = m.z;
  if (ScrollWheel == 120)
  {
    sceneIncZoom(0.1 * sceneGetScale());
    wxSize size = GetSize();
    WarpPointer(size.x/2, size.y/2);
    sceneIncPan(LastMouseScrollPosition.x - MousePosition.x, LastMouseScrollPosition.y - MousePosition.y, 0);
    LastMouseScrollPosition = MousePosition;
  }
  else if (ScrollWheel == -120)
  {
    sceneIncZoom(-0.1 * sceneGetScale());
    wxSize size = GetSize();
    WarpPointer(size.x/2, size.y/2);
    sceneIncPan(LastMouseScrollPosition.x - MousePosition.x, LastMouseScrollPosition.y - MousePosition.y, 0);
    LastMouseScrollPosition = MousePosition;
  }
  //printf("Event: %d\n", event);
  mousePassiveMotionCallback(MousePosition.x, MousePosition.y);

}
KbMods_t KbMods;
void GLCanvas::OnKeyUp(wxKeyEvent& event)
{
  switch ( event.GetKeyCode() )
  {
      case WXK_CONTROL:
        KbMods.Ctrl = false;
        break;
      case WXK_SHIFT:
        KbMods.Shift = false;
        break;
      case WXK_ALT:
        KbMods.Alt = false;
        break;
  }
}
//See http://docs.wxwidgets.org/2.6.3/wx_keycodes.html for keycodes
void GLCanvas::OnKeyDown(wxKeyEvent& event)
{
    //printf("Keycode is %d\n", event.GetKeyCode());
    switch ( event.GetKeyCode() )
    {
        case WXK_RIGHT:
            //Spin( 0.0, -angle );
            sceneIncPan(+5, 0, 0);
            break;

        case WXK_LEFT:
            sceneIncPan(-5, 0, 0);
            break;

        case WXK_DOWN:
            sceneIncPan(0, -5, 0);
            break;

        case WXK_UP:
            sceneIncPan(0, +5, 0);
            break;

        case WXK_SPACE:
            fileOpen("test/dxf/box.dxf");
            break;

        case WXK_CONTROL:
            KbMods.Ctrl = true;
          break;
        case WXK_SHIFT:
          KbMods.Shift = true;
          break;
        case WXK_ALT:
          KbMods.Alt = true;
          break;

        case 65: //a
          if (KbMods.Ctrl)
          {
            //cliScreenSelectAll();
          }
          break;
        case 83: //s
          if (KbMods.Ctrl)
          {
            //Save current file
          }
          break;
        case 90: //z
          if (KbMods.Ctrl)
          {
            cadUndo();
          }
          break;

        case 27: //esc
          if (KbMods.Ctrl)
          {
            CleanupAndExit();
          }
          break;

        case 82: //r
          if (KbMods.Ctrl)
          {
            cliRunScript("scripts/main.js");
          }
          break;
        default:
            event.Skip();
            return;
    }
}

void GLCanvas::OnSpinTimer(wxTimerEvent& WXUNUSED(event))
{
    Spin(0.0, 4.0);
}
void GLCanvas::OnIdle(wxIdleEvent &event)
{
  if (PostRedisplay_Register == true)
  {
    Refresh(false);
    PostRedisplay_Register = false;
  }
	/* Force the redraw immediately, gets the gfx card to its max */

	event.RequestMore();
}

wxString glGetwxString(GLenum name)
{
    const GLubyte *v = glGetString(name);
    if ( v == 0 )
    {
        // The error is not important. It is GL_INVALID_ENUM.
        // We just want to clear the error stack.
        glGetError();

        return wxString();
    }

    return wxString((const char*)v);
}


// ----------------------------------------------------------------------------
// MyFrame: main application window
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(FILE_OPEN, MyFrame::OnOpen)
    EVT_MENU(FILE_SAVE, MyFrame::OnSave)
    EVT_MENU(wxID_CLOSE, MyFrame::OnClose)
wxEND_EVENT_TABLE()

MyFrame::MyFrame( bool stereoWindow )
       : wxFrame(NULL, wxID_ANY, wxT(WINDOW_TITLE))
{
    int stereoAttribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_STEREO, 0 };

    new GLCanvas(this, stereoWindow ? stereoAttribList : NULL);

    SetIcon(wxICON(icon));

    Maximize();

    cliRegisterFunctions();


    /*Begin File Menu*/
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(FILE_OPEN, "Open");
    file_menu->Append(FILE_SAVE, "Save");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_CLOSE);
    /* End Menu */

    /*Begin Create Menu*/
    wxMenu *create_menu = new wxMenu;
    create_menu->Append(CREATE_LINE_VERTICAL, "Vertical Line");
    create_menu->Append(CREATE_LINE_HORIZONTAL, "Horizontal Line");
    create_menu->Append(CREATE_LINE_ENDPOINTS, "Line Endpoints");
    create_menu->Append(CREATE_LINE_POLAR, "Line Polar");
    /* End Menu */

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(file_menu, wxT("&File"));
    menuBar->Append(create_menu, wxT("Create"));


    SetMenuBar(menuBar);

    CreateStatusBar();

    SetClientSize(400, 400);
    Show();

    // test IsDisplaySupported() function:
    /*static const int attribs[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };
    wxLogStatus("Double-buffered display %s supported",
                wxGLCanvas::IsDisplaySupported(attribs) ? "is" : "not");*/

    /*if ( stereoWindow )
    {
        const wxString vendor = glGetwxString(GL_VENDOR).Lower();
        const wxString renderer = glGetwxString(GL_RENDERER).Lower();
        if ( vendor.find("nvidia") != wxString::npos &&
                renderer.find("quadro") == wxString::npos )
            ShowFullScreen(true);
    }*/
}

void MyFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
    CleanupAndExit();
}
void MyFrame::OnOpen (wxCommandEvent& WXUNUSED(event) )
{
  wxFileDialog* OpenDialog = new wxFileDialog( this, _("Choose a file to open"), wxEmptyString, wxEmptyString, _("Profile CAD (*.pfcad)|*.pfcad;*.PFCAD|Autocad (*.dxf)|*.dxf;*.DXF"),wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		wxString CurrentDocPath = OpenDialog->GetPath();
    fileOpen(string(CurrentDocPath.mb_str()));
		// Sets our current document to the file the user selected
		//MainEditBox->LoadFile(CurrentDocPath); //Opens that file
		SetTitle(wxString("ProfileCAD - ") <<
			OpenDialog->GetFilename()); // Set the Title to reflect the file open
	}

	// Clean up after ourselves
	OpenDialog->Destroy();
}
void MyFrame::OnSave (wxCommandEvent& WXUNUSED(event) )
{
  wxFileDialog* SaveDialog = new wxFileDialog( this, _("Choose save location"), wxEmptyString, wxEmptyString, _("Profile CAD (*.pfcad)|*.PFcad;*.PFCAD|Autocad (*.dxf)|*.dxf;*.DXF"),wxFD_SAVE, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (SaveDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		wxString CurrentDocPath = SaveDialog->GetPath();
		// Sets our current document to the file the user selected
		//MainEditBox->LoadFile(CurrentDocPath); //Opens that file
		SetTitle(wxString("ProfileCAD - ") <<
			SaveDialog->GetFilename()); // Set the Title to reflect the file open
	}

	// Clean up after ourselves
	SaveDialog->Destroy();
}
void CleanupAndExit()
{
  duk_destroy_heap(ctx);
  kill(0, SIGKILL);
}
