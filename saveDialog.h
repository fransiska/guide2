#ifndef YESNODIALOG
#define YESNODIALOG

#include <gtkmm.h>

class YesNoDialog : public Gtk::Dialog
{
public:
  YesNoDialog();

protected:
  //Child widgets:
  Gtk::VButtonBox m_ButtonBox;
  Gtk::Button m_Button_Info, m_Button_Question;
};


#endif
