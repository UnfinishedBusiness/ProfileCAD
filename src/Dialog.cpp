#include "Dialog.h"

using namespace std;
vector<dialog_t> DialogStack;
vector<wxTextCtrl*> TextBoxStack;
vector<wxRadioButton*> RadioButtonStack;

void Dialog::CloseDialog()
{
  TextBoxStack.clear();
  Close();
  Destroy();
}
std::string Dialog::GetTextBoxValue(int id) //Id before offset
{
  for (int i = 0; i < TextBoxStack.size(); i++)
  {
    if (TextBoxStack[i]->GetId() == id + DIALOG_ID_OFFSET)
    {
      return string(TextBoxStack[i]->GetValue());
    }
  }
}
std::string Dialog::GetRadioButtonValue(int id) //Id before offset
{
  for (int i = 0; i < RadioButtonStack.size(); i++)
  {
    if (RadioButtonStack[i]->GetId() == id + DIALOG_ID_OFFSET)
    {
      if (RadioButtonStack[i]->GetValue() == true)
      {
        return "true";
      }
      else
      {
        return "false";
      }
    }
  }
}
void Dialog::OnButtonClick(wxCommandEvent& event)
{
  for (int i = 0; i < DialogStack.size(); i++)
  {
    if (i + DIALOG_ID_OFFSET == GetId())
    {
      scriptEval("ExecuteButtonCallback(\"" + to_string(GetId() - DIALOG_ID_OFFSET) + "\");");
    }
  }
}
Dialog::Dialog(const wxString & title, wxSize size)
       : wxDialog(NULL, -1, title, wxDefaultPosition, size)
{
  wxPanel *panel = new wxPanel(this, -1);

  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

  for (int i = 0; i < DialogStack.size(); i++)
  {
    if (DialogStack[i].type == DIALOG_STATIC_BOX )
    {
      wxStaticBox *st = new wxStaticBox(panel, i + DIALOG_ID_OFFSET ,DialogStack[i].static_box.text.c_str(),
                                                  DialogStack[i].static_box.position,
                                                  DialogStack[i].static_box.size);
    }
    if (DialogStack[i].type == DIALOG_BUTTON )
    {
      wxButton *Button = new wxButton(this, i + DIALOG_ID_OFFSET, DialogStack[i].button.text.c_str(),
                                                DialogStack[i].button.position,
                                                DialogStack[i].button.size);
      Button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Dialog::OnButtonClick));
      hbox->Add(Button, 1);
    }
    if (DialogStack[i].type == DIALOG_TEXT_BOX )
    {
      wxTextCtrl *tc = new wxTextCtrl(panel, i + DIALOG_ID_OFFSET, DialogStack[i].textbox.default_text.c_str(),
                                                 DialogStack[i].textbox.position);
      TextBoxStack.push_back(tc);
    }
    if (DialogStack[i].type == DIALOG_RADIO_BUTTON )
    {
      wxRadioButton *rb = new wxRadioButton(panel, i + DIALOG_ID_OFFSET, DialogStack[i].radio_button.text.c_str(),
                                                  DialogStack[i].radio_button.position);
      RadioButtonStack.push_back(rb);
    }
    if (DialogStack[i].type == DIALOG_STATIC_TEXT )
    {
      wxStaticText *st = new wxStaticText(panel, i + DIALOG_ID_OFFSET, DialogStack[i].static_text.text.c_str(),
                                                       DialogStack[i].static_text.position);
    }

  }
  vbox->Add(panel, 1);
  vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

  SetSizer(vbox);

  Centre();
}
