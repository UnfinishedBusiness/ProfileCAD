#include "wx/wxprec.h"

using namespace std;

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/menu.h"
#include "wx/menuitem.h"
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
std::vector<PopupMenu_t> PopupMenuStack;
bool PostRedisplay_Register = false;
point_t MousePosition;
wxPoint MouseSreenPosition;
string StatusText;
string LastStatusText;
bool ShowPopupMenu_Register = false;
wxWindow *MainWindow;
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
    EVT_LEFT_UP(GLCanvas::OnMouseLeftUp)
    EVT_RIGHT_DOWN(GLCanvas::OnMouseRightDown)
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
    //wxPaintDC dc(this);

    #ifdef __APPLE__
      wxClientDC dc(this);
    #elif __CYGWIN__
      wxBufferedPaintDC dc(this, wxBUFFER_VIRTUAL_AREA);
    #else
      wxBufferedPaintDC dc(this,wxBUFFER_VIRTUAL_AREA);
    #endif


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
void GLCanvas::OnMouseLeftUp(wxMouseEvent& event)
{
  scriptEval("OnMouseClick();");
}
void PopupMenuHandle(wxCommandEvent &event)
{
  for (int i = 0; i < PopupMenuStack.size(); i++)
  {
    //5000 is id offset
    if (i + POPUP_MENU_ID_OFFSET == event.GetId())
    {
      scriptEval(PopupMenuStack[i].callback);
    }
  }
}
void ShowPopupMenu()
{
  ShowPopupMenu_Register = true;
}
void GLCanvas::OnMouseRightDown(wxMouseEvent& event)
{
  wxMenu *right_menu = new wxMenu;
  for (int i = 0; i < PopupMenuStack.size(); i++)
  {
    right_menu->Append(i + POPUP_MENU_ID_OFFSET, PopupMenuStack[i].label.c_str());
    Bind( wxEVT_COMMAND_MENU_SELECTED, &PopupMenuHandle, i + POPUP_MENU_ID_OFFSET );
  }
  PopupMenu(right_menu, MouseSreenPosition);
}
void GLCanvas::OnMouse(wxMouseEvent& event)
{
  int ScrollWheel = event.GetWheelRotation();

  wxPoint m = event.GetPosition();
  MouseSreenPosition = m;
  MousePosition = cadScreenCordToCadCord(m.x, m.y);
  scriptEval("OnMouseMotion(" + to_string(MousePosition.x) + ", " + to_string(MousePosition.y) + ")");

  //wxLogStatus("X: %.6f Y: %.6f Zoom: %.6f", MousePosition.x, MousePosition.y, sceneGetScale());
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
    int keycode = event.GetKeyCode();
    char eval[1024];
    string mod = "None";
    if (KbMods.Shift == true) mod = "Shift";
    if (KbMods.Ctrl== true) mod = "Ctrl";
    if (KbMods.Alt == true) mod = "Alt";
    sprintf(eval, "OnKeyDown(\"%s\",%d);",mod.c_str(),keycode);
    scriptEval(string(eval));
    switch ( keycode )
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
            //fileOpen("test/dxf/box.dxf");
            //ShowPopupMenu();
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
            scriptSource("scripts/main.js");
            scriptEval("main();");
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
  if (ShowPopupMenu_Register == true)
  {
    ShowPopupMenu_Register = false;
    wxCommandEvent evt = wxCommandEvent(wxEVT_RIGHT_DOWN, wxID_OK );
    ProcessEvent(evt);
  }
  if (LastStatusText != StatusText)
  {
    wxLogStatus(StatusText.c_str());
  }
  LastStatusText = StatusText;
  if (PostRedisplay_Register == true)
  {
    Refresh(false);
    PostRedisplay_Register = false;
  }
	/* Force the redraw immediately, gets the gfx card to its max */
  scriptEval("OnIdle();");
  usleep(400);
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

    MainWindow = this;
    SetIcon(wxICON(icon));

    Maximize();


    scriptRegisterFunctions();

    /*Begin File Menu*/
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(FILE_OPEN, "Open");
    file_menu->Append(FILE_SAVE, "Save");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_CLOSE);
    /* End Menu */

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(file_menu, wxT("&File"));


    SetMenuBar(menuBar);

    CreateStatusBar();

    SetClientSize(400, 400);
    Show();
}
void MyFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
    CleanupAndExit();
}
void MyFrame::OnOpen (wxCommandEvent& WXUNUSED(event) )
{
  scriptEval(string("FileOpenDialog()"));
}
void MyFrame::OnSave (wxCommandEvent& WXUNUSED(event) )
{
  scriptEval(string("FileSaveDialog()"));
}
void CleanupAndExit()
{
  duk_destroy_heap(ctx);
  kill(0, SIGKILL);
}
