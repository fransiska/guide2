
/// DEPRECATED
#ifndef GUIDEHARDWAREWINDOW
#define GUIDEHARDWAREWINDOW
#include <gtkmm.h>
#include "guideState.h"
class GuideHardwareWindow: public Gtk::Window {
public:
  GuideHardwareWindow(cfg::guide::Sensors &psensors);
protected:
  cfg::guide::Sensors &sensors;
  virtual void on_button_quit() {hide();}
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TextView m_TextView;
  virtual void on_show();
};

#endif
