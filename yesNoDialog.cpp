/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "yesNoDialog.h"
using namespace std;
YesNoDialog::YesNoDialog(string title, string message):
  Gtk::Dialog(title,true),
  dLabel(message),
  image(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DND)
{
  set_size_request(200,90);
  set_resizable(false);
  image.show();
  image.set_size_request(40,40);
  dBox.pack_start(image,Gtk::PACK_SHRINK);
  dBox.pack_start(dLabel,Gtk::PACK_SHRINK);
  dLabel.set_size_request(160,40);
  dLabel.set_line_wrap();
  dLabel.set_justify(Gtk::JUSTIFY_CENTER);
  //Glib::RefPtr<Gdk::Pixbuf> pixbuf = render_icon( Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG );
  //set_icon(pixbuf);
  get_vbox()->pack_start(dBox,Gtk::PACK_SHRINK);
  get_action_area()->set_layout(Gtk::BUTTONBOX_CENTER);
  add_button(Gtk::Stock::OK,1);
  add_button(Gtk::Stock::CANCEL,0);
  show_all_children();
}
