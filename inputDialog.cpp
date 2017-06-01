/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "inputDialog.h"
using namespace std;

InputDialog::InputDialog(const char* message, string &pstring,helium::GtkExceptionHandling *eh,string description):
  dString(pstring),
  label(description)
{
  dEntry.set_text(pstring);
  set_title(message);
  Gtk::VBox *box = get_vbox();
  if(strcmp(description.c_str(),"") != 0)
    box->pack_start(label);
  box->pack_start(dEntry);
  button1 = add_button("OK",1);
  button0 = add_button("Cancel",0);

  if(eh) {
    signal_response().connect(eh->wrap(*this,&InputDialog::on_close));
    dEntry.signal_activate().connect(eh->wrap(*this,&InputDialog::on_activate));
  }
  else {
    signal_response().connect(sigc::mem_fun(*this,&InputDialog::on_close));
    dEntry.signal_activate().connect(sigc::mem_fun(*this,&InputDialog::on_activate));    
  }
  show_all_children();
}

void InputDialog::setEntry(std::string s) {
  dEntry.set_text(s);
}

void InputDialog::on_close(int id) {
  if(id == 1) 
    dString = dEntry.get_text();
}

void InputDialog::on_activate() {
  response(1);
}
