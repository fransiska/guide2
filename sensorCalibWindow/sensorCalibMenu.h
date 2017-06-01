/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef SENSORCALIBMENU
#define SENSORCALIBMENU

#include <gtkmm.h>
#include <string>

class SensorCalibMenu {
 public:
  SensorCalibMenu();
  void onFileSave();
  void onFileQuit();

 private:
  std::string filename;
  Gtk::Widget* menubar;
  Glib::RefPtr<Gtk::ActionGroup> menubar_action;
  Glib::RefPtr<Gtk::UIManager> menubar_uimanager;

 public:
  void addTo(Gtk::Window* win, Gtk::VBox& c);
};

#endif //SENSORCALIBMENU
