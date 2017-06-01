#ifndef GYROCALIBMENU
#define GYROCALIBMENU

#include <gtkmm/actiongroup.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/widget.h>
#include <gtkmm/box.h>
#include <gtkmm/filechooserdialog.h>
#include <string>
#include "gyroCalib.h"
#include <gtkmm/window.h>

class GyroCalibWindow;


class GyroCalibMenu {
 public:
  GyroCalibMenu(GyroCalib* gyroCalib, std::string calibFile,GyroCalibWindow* win);
  virtual ~GyroCalibMenu();
  void onFileOpen();
  void onFileSave();
  void onFileSaveAs();
  void onFileQuit();

 private:
  Gtk::Widget* menubar;
  Glib::RefPtr<Gtk::ActionGroup> menubar_action;
  Glib::RefPtr<Gtk::UIManager> menubar_uimanager;
  GyroCalib* gyroCalib;
  std::string calibFile;
  Gtk::FileChooserDialog* file_chooser;
  GyroCalibWindow * win;

 public:
  void addTo(Gtk::Window* win, Gtk::VBox& c);
};

#endif //GYROCALIBMENU
