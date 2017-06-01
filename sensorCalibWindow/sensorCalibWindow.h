/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef SENSORCALIBWINDOW
#define SENSORCALIBWINDOW

#include <gtkmm.h>
#include "sensorCalib.h"
#include "sensorCalibDrawingWidget.h"
  #ifdef PLOT
  #include "sensorCalibDrawingWidgetPlot.h"
  #endif

#include "sensorCalibMenu.h"
//#include <guiObjects/guiConnectionLine.h>
#include <helium/gtk/gtkExceptionHandling.h>
#include "sensorExport.h"

class GuiConnectionLine;

//template<class T = SensorCalibDrawingWidget>
class SensorCalibWindow : public Gtk::Window {
  enum SensorEntry {SENSORNO,SENSORBOARD,SENSORCHANNEL};
protected:
  cfg::guide::Sensors &sensors;
private:
  cfg::guide::SensorGui &sensorGui;
  helium::GtkExceptionHandling *eh;
  helium::GtkProxy<helium::Connection> &conn;
  const std::string &sensorFile;
  bool sensorsConnected;
  bool visible;
  int currentMax;
  int currentMin;
public:


SensorCalibDrawingWidget& sensorCalibDrawingWidget;
//#ifdef PLOT
//  SensorCalibDrawingWidgetPlot sensorCalibDrawingWidget;
//#endif
//#endif
//
//#ifdef PLOTEM
//  SensorCalibDrawingWidgetPlotEM sensorCalibDrawingWidget;
//#endif
//#endif


private:
  Gtk::Label label_sensor_no;
  Gtk::Label label_sensor_value;
  Gtk::Label label_sensor_status;
  Gtk::Label label_sensor_info;

  Gtk::Label label_sensor_board; 
  Gtk::Label label_sensor_channel;
  Gtk::Label label_sensor_min;
  Gtk::Label label_sensor_max;

  Gtk::Label label_sensor_cur_min;
  Gtk::Label label_sensor_cur_max;

  Gtk::Label label_sensor_ignored; /// list of error ignored sensors

  Gtk::Button button_max;
  Gtk::Button button_min;
  Gtk::Button button_quit;
  Gtk::Button button_save;
  Gtk::Button button_reset_calib;
  Gtk::CheckButton button_ignore;
  sigc::connection button_ignore_handler;

  //DEBUG replaced below Gtk::Label label_sensor;
  Gtk::SpinButton entry_sensor;
  Gtk::SpinButton entry_sensor_board; ///choose sensor board
  Gtk::SpinButton entry_sensor_channel; ///choose sensor channel
  Gtk::SpinButton entry_sensor_min;
  Gtk::SpinButton entry_sensor_max;

  Gtk::Entry entry_sensor_value;

  Gtk::HBox hbox_outer, hbox_top, hbox_bottom, hbox_minmax;
  Gtk::HButtonBox hbbox_bottom,hbbox_menu;
  Gtk::VBox vbox_button, vbox_sensor_no, vbox_sensor_value, vbox_right, vbox_sensor_board, vbox_sensor_channel, vbox_min,vbox_max;
 public:
  Gtk::VBox vbox_outer;
 private:
  Gtk::Frame frame_button;
  Gtk::HSeparator separator_1, separator_2, separator_3;
  Gtk::AspectFrame aspect_frame;

  Gtk::EventBox event_box;

  bool updating; //prevent double signal trigger

 public:
  //helium::ValueHub<int> active_sensor;
  SensorCalibMenu sensorCalibMenu;
 public:
  SensorCalibWindow(cfg::guide::Sensors &psensors,cfg::guide::SensorGui &psensorgui,cfg::guide::Colors &pcolors, helium::GtkExceptionHandling *peh, helium::GtkProxy<helium::Connection> &pconn, const std::string& pfilename, SensorCalibDrawingWidget& pscdw);

 protected:
  void on_button_min();
  void on_button_max();
  void on_button_quit();
  void on_button_save();
  void on_button_ignore();
  void on_button_reset_calib();
  void onShowWindow();
  void onHideWindow();
  void onConnection(const helium::Connection &c);
  void connect(bool b);

  void update_info_label();
  void update_status_label();  
  void update_sensor_values();
  void update_sensor_min();
  void update_sensor_max();
  void update_active_sensor(SensorEntry s);
  virtual bool on_eventbox_button_press(GdkEventButton* event);
  void onMonitorModeUpdate(const std::pair<int,helium::MonitorMode>& hm);
  bool getVisible();
  int getSensorNumber(int board, int channel);

  //helium::StaticConnReference<helium::Array<int>,SensorCalibWindow,void,&SensorCalibWindow::update_sensor_values> callOnRawValueUpdate;
  //TODEL helium::InternalConnReference<const helium::SensorValList<int>& > callOnRawValueUpdate;
 



 


protected:

  struct Callers;
  friend struct Callers;
  std::auto_ptr<Callers> callers;
  
  //to refresh upon sensor value update



};

class SensorCalibSubWindow : public SensorCalibWindow {
 public:
  SensorCalibSubWindow(cfg::guide::Sensors &sensors,cfg::guide::SensorGui &sensorgui,cfg::guide::Colors &pcolors,helium::GtkExceptionHandling *peh, helium::GtkProxy<helium::Connection> &pconn,const std::string& pfilename,SensorCalibDrawingWidget& pscdw):
    SensorCalibWindow(sensors,sensorgui,pcolors,peh,pconn,pfilename,pscdw) {
  }
};

#endif //SENSORCALIBWINDOW
