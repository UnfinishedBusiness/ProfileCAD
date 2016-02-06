#ifndef _WX_CUBE_H_
#define _WX_CUBE_H_

#include "wx/glcanvas.h"

bool PostRedisplay_Register = false;
// the rendering context used by all GL canvases
class TestGLContext : public wxGLContext
{
public:
    TestGLContext(wxGLCanvas *canvas);

    // render the cube showing it at given angles
    void DrawRotatedCube(float xangle, float yangle);

private:
    // textures for the cube faces
    GLuint m_textures[6];
};

// Define a new application type
class MyApp : public wxApp
{
public:
    MyApp() { m_glContext = NULL; m_glStereoContext = NULL; }

    // Returns the shared context used by all frames and sets it as current for
    // the given canvas.
    TestGLContext& GetContext(wxGLCanvas *canvas, bool useStereo);

    // virtual wxApp methods
    virtual bool OnInit();
    virtual int OnExit();

private:
    // the GL context we use for all our mono rendering windows
    TestGLContext *m_glContext;
    // the GL context we use for all our stereo rendering windows
    TestGLContext *m_glStereoContext;
};

// Define a new frame type
class MyFrame : public wxFrame
{
public:
    MyFrame(bool stereoWindow = false);

private:
    void OnClose(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
};

class GLCanvas : public wxGLCanvas
{
public:
    GLCanvas(wxWindow *parent, int *attribList = NULL);
private:
    void OnPaint(wxPaintEvent& event);
    void Spin(float xSpin, float ySpin);
    void OnKeyDown(wxKeyEvent& event);
    void OnSpinTimer(wxTimerEvent& WXUNUSED(event));
    void OnIdle(wxIdleEvent &event);
    // angles of rotation around x- and y- axis
    float m_xangle,
          m_yangle;

    wxTimer m_spinTimer;
    bool m_useStereo,
         m_stereoWarningAlreadyDisplayed;

    wxDECLARE_EVENT_TABLE();
};
enum { FILE_SAVE = wxID_HIGHEST + 1 };
enum { FILE_OPEN = wxID_HIGHEST + 2 };

enum { CREATE_LINE_VERTICAL = wxID_HIGHEST + 3 };
enum { CREATE_LINE_HORIZONTAL = wxID_HIGHEST + 4 };
enum { CREATE_LINE_ENDPOINTS = wxID_HIGHEST + 5 };
enum { CREATE_LINE_POLAR = wxID_HIGHEST + 6 };


#endif // _WX_CUBE_H_
