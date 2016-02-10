#include "Dialog.h"

Dialog::Dialog(const wxString & title)
       : wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(250, 230))
{
  wxPanel *panel = new wxPanel(this, -1);

  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

  wxStaticBox *st = new wxStaticBox(panel, -1, wxT("Colors"),
      wxPoint(5, 5), wxSize(240, 150));
  wxRadioButton *rb = new wxRadioButton(panel, -1,
      wxT("256 Colors"), wxPoint(15, 30), wxDefaultSize, wxRB_GROUP);

  wxRadioButton *rb1 = new wxRadioButton(panel, -1,
      wxT("16 Colors"), wxPoint(15, 55));
  wxRadioButton *rb2 = new wxRadioButton(panel, -1,
      wxT("2 Colors"), wxPoint(15, 80));
  wxRadioButton *rb3 = new wxRadioButton(panel, -1,
      wxT("Custom"), wxPoint(15, 105));
  wxTextCtrl *tc = new wxTextCtrl(panel, -1, wxT(""),
      wxPoint(95, 105));

  wxButton *okButton = new wxButton(this, -1, wxT("Ok"),
      wxDefaultPosition, wxSize(70, 30));
  wxButton *closeButton = new wxButton(this, -1, wxT("Close"),
      wxDefaultPosition, wxSize(70, 30));

  hbox->Add(okButton, 1);
  hbox->Add(closeButton, 1, wxLEFT, 5);

  vbox->Add(panel, 1);
  vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

  SetSizer(vbox);

  Centre();
  ShowModal();

  Destroy();
}
