/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef YESNODIALOG
#define YESNODIALOG

#include <gtkmm.h>

class YesNoDialog : public Gtk::Dialog
{
public:
  YesNoDialog(std::string title, std::string message);
protected:
  Gtk::Label dLabel;
  Gtk::HBox dBox;
  Gtk::Image image;
};


#endif
