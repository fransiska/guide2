#ifndef GYROCONNECTIONLINE
#define GYROCONNECTIONLINE
#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <glibmm/thread.h>

class GyroCalib;
class GyroConnectionLine:public Gtk::HBox {
  GyroCalib* g;
  bool state;
  bool changedExternally;
  void setStateCB();
  Glib::Dispatcher stateDispatcher;
 public:   
  Gtk::CheckButton ipcb;
  Gtk::Entry ipe;
  GyroConnectionLine(GyroCalib* pgyroCalib,const std::string &host);//const std::string &host);

  void setState(bool state);
  void requestConnection();
};

#endif
