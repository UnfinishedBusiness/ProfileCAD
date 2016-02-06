#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/notebook.h>

class GL_Window : public wxGLCanvas
{
public:

	GL_Window(float c, wxWindow * parent, wxWindowID id,
	          const wxPoint & pos, const wxSize& size, long style=0,
	          const wxString & name = _("GLCanvas"), int * attribList = 0,
	          const wxPalette & palette = wxNullPalette)
	: wxGLCanvas(parent, id, pos, size, style, name, attribList, palette),
	  c_(c), rotate_(c) { }

	virtual ~GL_Window() { }

	void draw() {
		rotate_ += 0.01;

		SetCurrent();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, (GLint)200, (GLint)200);
		glColor3f(1.0, c_, c_);

		glBegin(GL_POLYGON);
		glVertex3f(-0.5, -0.5, 5 * cos(rotate_));
		glVertex3f(-0.5, 0.5, 5 * cos(rotate_));
		glVertex3f(0.5, 0.5, -5 * cos(rotate_));
		glVertex3f(0.5, -0.5, -5 * cos(rotate_));
		glEnd();

		SwapBuffers();
	}

	void OnIdle(wxIdleEvent & event)	{
		draw();
		event.RequestMore();
	}

private:

	float c_;
	float rotate_;

	DECLARE_EVENT_TABLE();
};

class MyApp: public wxApp
{
	virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	wxFrame* frame = new wxFrame((wxFrame *) NULL, -1,
		_("Hello GL World"), wxPoint(50, 50), wxSize(450, 340) );

	wxNotebook* book = new wxNotebook(frame, -1,
		wxPoint(-1, -1), wxSize(200, 200));

	GL_Window* MyGLCanvas = new GL_Window(1, book, -1, wxPoint(-1, -1),
		wxSize(200, 200), wxSUNKEN_BORDER, _("some text"));
	book->AddPage(MyGLCanvas, _("One"));

	MyGLCanvas = new GL_Window(0, book, -1, wxPoint(-1,-1),
		wxSize(200,200), wxSUNKEN_BORDER, _("some text"));
	book->AddPage(MyGLCanvas, _("Two"));

	frame->Show(true);
	return true;
}

BEGIN_EVENT_TABLE(GL_Window, wxGLCanvas)
    EVT_IDLE(GL_Window::OnIdle)
END_EVENT_TABLE()
