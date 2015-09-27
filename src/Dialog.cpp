#include <Dialog.h>

#define BUTTON 0
#define TEXTBOX 1
#define LABEL 2

#define DIALOG_BACKGROUND color_t{0.8, 0.8, 0.8, 1}
int DIALOG_WIDTH = 300;
int DIALOG_HEIGHT = 200;

using namespace std;
int dialogWindow;

int CurrentFocus = -1;

struct button_t{
  string Text;
  point_t Position;
  float Width;
  float Height;
  void (*ClickCallback)(void);
};
struct textbox_t{
  string Text;
  float Width;
  float Height;
  point_t Position;
  string ID;
};
struct label_t{
  string Text;
  point_t Position;
};
struct DialogWidget{
  int Type;
  textbox_t Textbox;
  button_t Button;
  label_t Label;
};
vector<DialogWidget> Dialog;

string dialogTextboxGetString(string id)
{
  for (int x = 0; x < Dialog.size(); x++)
  {
    if (Dialog[x].Type == TEXTBOX)
    {
      if (Dialog[x].Textbox.ID == id)
      {
        return Dialog[x].Textbox.Text;
      }
    }
  }
  return "";
}
void dialogAddButton(point_t p, float w, float h, std::string t, void (*c)(void))
{
  button_t b;
  b.Text = t;
  b.Position = p;
  b.Width = w;
  b.Height = h;
  b.ClickCallback = c;
  DialogWidget widget;
  widget.Type = BUTTON;
  widget.Button = b;
  Dialog.push_back(widget);
}
void dialogAddTextBox(point_t p, float w, float h, string id)
{
  textbox_t t;
  t.Position = p;
  t.Width = w;
  t.Height = h;
  t.ID = id;
  DialogWidget widget;
  widget.Type = TEXTBOX;
  widget.Textbox = t;
  Dialog.push_back(widget);
}
void dialogAddLabel(point_t p, string t)
{
  label_t l;
  l.Position = p;
  l.Text = t;
  DialogWidget widget;
  widget.Type = LABEL;
  widget.Label = l;
  Dialog.push_back(widget);
}
void dialogKeyPush(string c)
{
  if (CurrentFocus > -1)
  {
    Dialog[CurrentFocus].Textbox.Text.append(c);
    glutPostRedisplay();
  }

}
void dialogKeyBackspace()
{
  if (CurrentFocus > -1)
  {
    if (Dialog[CurrentFocus].Textbox.Text.size() > 0)
    {
      Dialog[CurrentFocus].Textbox.Text.pop_back();
    }
    glutPostRedisplay();
  }
}
void dialogMouse(int btn, int state, int x, int y)
{
  if(btn==GLUT_LEFT_BUTTON && state==GLUT_UP)
  {
    point_t pos = cadScreenCordToCadCord(x, y);
    pos.x = pos.x * 0.1;
    pos.y = pos.y * 0.1;
    V cout << KRED << "(dialogMouse) Clicked" << KCYAN << " => " << KGREEN << "X: " << pos.x << " Y: " << pos.y << KNORMAL << endl;
    for (int x = 0; x < Dialog.size(); x++)
    {
      if (Dialog[x].Type == BUTTON)
      {
        if ((pos.x > Dialog[x].Button.Position.x && pos.y > Dialog[x].Button.Position.y ) && (pos.x < (Dialog[x].Button.Position.x + Dialog[x].Button.Width) && pos.y < (Dialog[x].Button.Position.y + Dialog[x].Button.Height) ))
        {
          //cout << "Clicked button " << x << endl;
          Dialog[x].Button.ClickCallback();
        }
      }
      if (Dialog[x].Type == TEXTBOX)
      {
        if ((pos.x > Dialog[x].Textbox.Position.x && pos.y > Dialog[x].Textbox.Position.y ) && (pos.x < (Dialog[x].Textbox.Position.x + Dialog[x].Textbox.Width) && pos.y < (Dialog[x].Textbox.Position.y + Dialog[x].Textbox.Height) ))
        {
          cout << "Clicked textbox " << x << endl;
          CurrentFocus = x;
        }
        else
        {
          CurrentFocus = -1;
        }
      }
    }
  }
  if (glutGetWindow() == dialogWindow)
  {
    glutPostRedisplay();
  }
}
void dialogKeyboard(unsigned char key, int x, int y)
{
    int mod = glutGetModifiers();
    switch(key)
    {
        case 97 : dialogKeyPush("a"); break;
        case 98 : dialogKeyPush("b"); break;
        case 99 : dialogKeyPush("c"); break;
        case 100 : dialogKeyPush("d"); break;
        case 101 : dialogKeyPush("e"); break;
        case 102 : dialogKeyPush("f"); break;
        case 103 : dialogKeyPush("g"); break;
        case 104 : dialogKeyPush("h"); break;
        case 105 : dialogKeyPush("i"); break;
        case 106 : dialogKeyPush("j"); break;
        case 107 : dialogKeyPush("k"); break;
        case 108 : dialogKeyPush("l"); break;
        case 109 : dialogKeyPush("m"); break;
        case 110 : dialogKeyPush("n"); break;
        case 111 : dialogKeyPush("o"); break;
        case 112 : dialogKeyPush("p"); break;
        case 113 : dialogKeyPush("q"); break;
        case 114 : dialogKeyPush("r"); break;
        case 115 : dialogKeyPush("s"); break;
        case 116 : dialogKeyPush("t"); break;
        case 117 : dialogKeyPush("u"); break;
        case 118 : dialogKeyPush("v"); break;
        case 119 : dialogKeyPush("w"); break;
        case 120 : dialogKeyPush("x"); break;
        case 121 : dialogKeyPush("y"); break;
        case 122 : dialogKeyPush("z"); break;

        case 48 : dialogKeyPush("0"); break;
        case 49 : dialogKeyPush("1"); break;
        case 50 : dialogKeyPush("2"); break;
        case 51 : dialogKeyPush("3"); break;
        case 52 : dialogKeyPush("4"); break;
        case 53 : dialogKeyPush("5"); break;
        case 54 : dialogKeyPush("6"); break;
        case 55 : dialogKeyPush("7"); break;
        case 56 : dialogKeyPush("8"); break;
        case 57 : dialogKeyPush("9"); break;

        case 45 : dialogKeyPush("-"); break;
        case 42 : dialogKeyPush("*"); break;
        case 47 : dialogKeyPush("/"); break;

        case 8 : dialogKeyBackspace(); break; //Normal backspace

        #ifdef __APPLE__
          case 127 : dialogKeyBackspace(); break; //Mac delete
        #endif

        case 27 : dialogClose(); break; //Escape

        case 46 : dialogKeyPush("."); break;

        case 32 : dialogKeyPush(" "); break;

        case 13 : break; //Enter key

        case 96 : dialogKeyPush("`"); break; //backtick key
      }
}
void dialogResize(int width, int height)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();//these two lines are unchanged

  double bound = 1.0; //I reduced the edge distance to make the shape larger in the window
  double scaleX = double(width)/DIALOG_WIDTH;
  double scaleY = double(height)/DIALOG_HEIGHT;
  double Yoffset = 100;

  glOrtho( -bound*scaleX/2, bound*scaleX/2, -bound*scaleY, bound*scaleY, -1,1 );
  glScalef(1, DIALOG_WIDTH/DIALOG_HEIGHT + DIALOG_WIDTH/DIALOG_HEIGHT + 0.5, 1); //Not sure wtf, but it works!
  //gluPerspective(0, 16.0/9.0*float(width)/float(height), -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0,0,width,height);
}
void dialogRender()
{
  //V cout << "Rendering!" << endl;
  glClearColor(DIALOG_BACKGROUND.r, DIALOG_BACKGROUND.g, DIALOG_BACKGROUND.b, DIALOG_BACKGROUND.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (int x = 0; x < Dialog.size(); x++)
  {
    if (Dialog[x].Type == LABEL)
    {
      sceneColor(BLACK);
      uiDrawText(GLUT_BITMAP_HELVETICA_12, (char *)Dialog[x].Label.Text.c_str(),
                        Dialog[x].Label.Position.x,
                        Dialog[x].Label.Position.y, 0);
    }
    if (Dialog[x].Type == BUTTON)
    {
      sceneColor(LIGHTGREY);
      glRectf(Dialog[x].Button.Position.x, Dialog[x].Button.Position.y, (Dialog[x].Button.Position.x + Dialog[x].Button.Width), (Dialog[x].Button.Position.y + Dialog[x].Button.Height));

      sceneColor(WHITE);
      uiDrawText(GLUT_BITMAP_HELVETICA_12, (char *)Dialog[x].Button.Text.c_str(),
                        Dialog[x].Button.Position.x + (Dialog[x].Button.Width/2) - (0.01 * Dialog[x].Button.Text.length()),
                        Dialog[x].Button.Position.y + (Dialog[x].Button.Height/2) - 0.01, 0);
    }
    if (Dialog[x].Type == TEXTBOX)
    {
      //cout << "Found Text box!" << endl;
      float LineWidth = 0.005;
      if (CurrentFocus == x)
      {
        sceneColor(BLUE);
      }
      else
      {
        sceneColor(LIGHTGREY);
      }
      glRectf(Dialog[x].Textbox.Position.x, Dialog[x].Textbox.Position.y, (Dialog[x].Textbox.Position.x + Dialog[x].Textbox.Width), (Dialog[x].Textbox.Position.y + Dialog[x].Textbox.Height));
      sceneColor(WHITE);
      glRectf(Dialog[x].Textbox.Position.x + LineWidth, Dialog[x].Textbox.Position.y + (LineWidth), (Dialog[x].Textbox.Position.x + Dialog[x].Textbox.Width) - (LineWidth - 0.001), (Dialog[x].Textbox.Position.y + Dialog[x].Textbox.Height) - (LineWidth));
      sceneColor(LIGHTGREY);
      uiDrawText(GLUT_BITMAP_HELVETICA_12, (char *)Dialog[x].Textbox.Text.c_str(),
                        Dialog[x].Textbox.Position.x + LineWidth + 0.005,
                        Dialog[x].Textbox.Position.y + LineWidth + 0.022, 0);
    }
  }
  glutSwapBuffers();
}
void dialogClose()
{
  //V cout << "Closing Dialog!" << endl;
  glutDestroyWindow(dialogWindow);
  dialogDestroy();
}
void dialogDestroy()
{
  Dialog.clear();
}
void dialogOpen()
{
  //glutSetWindow(dialogWindow);
  //glutShowWindow();
  glutInitWindowSize(DIALOG_WIDTH, DIALOG_HEIGHT);
  dialogWindow = glutCreateWindow(WINDOW_TITLE);
  V cout << KRED << "(dialogInit) Dialogwindow" << KCYAN << " => " << KGREEN << dialogWindow << KNORMAL << endl;
  glutCloseFunc(dialogDestroy);
  glutDisplayFunc(dialogRender);
  glutReshapeFunc(dialogResize);
  glutKeyboardFunc(dialogKeyboard);
  glutMouseFunc(dialogMouse);
}
void dialogInit()
{

  //glutHideWindow();
}
