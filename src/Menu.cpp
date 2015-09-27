#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <iostream>
#include <Menu.h>
using namespace std;

void Test()
{
  GtkWidget *window;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
  gtk_widget_show(window);
  gtk_window_present(GTK_WINDOW(window));
}
void menuInit()
{
  glutCreateMenu(menuCallback);
  glutAddMenuEntry("Repaint", 1);
  glutAddMenuEntry("Toggle Snap Center", 2);
  glutAddMenuEntry("Toggle Snap Arc Enpoints", 3);
  glutAddMenuEntry("Toggle Snap Line Enpoints", 4);
  glutAddMenuEntry("Toggle Snap Midpoint", 5);
  glutAddMenuEntry("Toggle Snap Intersection", 6);
  glutAddMenuEntry("Toggle Snap Vector", 7);
  glutAddMenuEntry("Test", 8);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void menuCallback(int choice)
{
  switch (choice)
  {
    case 1: cadRedraw(); break;
    case 2: mouseToggleCenterSnap(); break;
    case 3: mouseToggleArcEndpointSnap(); break;
    case 4: mouseToggleLineEndpointSnap(); break;
    case 5: mouseToggleMidpointSnap(); break;
    case 6: mouseToggleIntersectionSnap(); break;
    case 7: mouseToggleVectorSnap(); break;
    case 8: Test(); break;
  }
  uiSnapIndicator();
}
