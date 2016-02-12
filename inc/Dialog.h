#ifndef DIALOG_
#define DIALOG_

#include <wx/wx.h>
#include <application.h>

#define DIALOG_STATIC_BOX 0
#define DIALOG_TEXT_BOX 1
#define DIALOG_BUTTON 2
#define DIALOG_RADIO_BUTTON 3

struct dialog_static_box_t{
  std::string text;
  wxPoint position;
  wxSize size;
};

struct dialog_radio_button_t{
  std::string text;
  wxPoint position;
};

struct dialog_text_box_t{
  std::string default_text;
  wxPoint position;
};

struct dialog_button_t{
  std::string text;
  wxPoint position;
  wxSize size;
};

struct dialog_t{
  int type;
  dialog_button_t button;
  dialog_static_box_t static_box;
  dialog_text_box_t textbox;
  dialog_radio_button_t radio_button;
};

extern std::vector<dialog_t> DialogStack;

#define DIALOG_ID_OFFSET 7000

class Dialog : public wxDialog
{
public:
  Dialog(const wxString& title, wxSize size);
  void OnButtonClick(wxCommandEvent& event);
  void CloseDialog();
};
#endif
