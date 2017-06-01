/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef INPUTDIALOG
#define INPUTDIALOG
#include <iostream>
#include <gtkmm.h>
#include <helium/gtk/gtkExceptionHandling.h>

class InputDialog : public Gtk::Dialog{
protected:
  Gtk::Entry dEntry;
  std::string &dString;
  Gtk::Label label;
  Gtk::Button* button0;
  Gtk::Button* button1;
 public:
  InputDialog(const char* message,std::string &pstring,helium::GtkExceptionHandling *eh,std::string description="");
  void on_close(int id);
  void on_activate();
  void setEntry(std::string s);
};

#endif
