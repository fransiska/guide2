/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GYROCALIBBOX
#define GYROCALIBBOX

#include <gtkmm.h>

static const int SETS=5;

class GyroCalibBox: public Gtk::VBox {
  Gtk::Window *win;
 public:
  GyroCalibBox(Gtk::Window *pwin);
  
  Gtk::VBox boxv_main;

  Gtk::CheckButton c_button[SETS];
  Gtk::RadioButton r_button[SETS+1];
  Gtk::Entry entry[SETS+1];
  Gtk::Label label[SETS+1]; ///star indicates opened file, ones with statistics writte on the label have acquired data which can be re-calculated
  Gtk::HBox boxh[SETS+1];
  Gtk::HSeparator separatorh1;
  Gtk::HSeparator separatorh2;
  Gtk::HSeparator separatorh3;

  Gtk::HBox boxh_status;
  Gtk::Label l_data;
  Gtk::CheckButton c_power;
  Gtk::CheckButton c_mmode;

  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
};

#endif
