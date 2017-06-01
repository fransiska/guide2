/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef SENSORCALIBDRAWINGWIDGET
#define SENSORCALIBDRAWINGWIDGET

#include <gtkmm.h>
#include <helium/vector/array.h>
#include <app/guide2/guideState.h>

class SensorCalibDrawingWidget : public Gtk::DrawingArea {
protected:
  cfg::guide::Sensors &sensors;
  cfg::guide::SensorGui &sensorGui;
  cfg::guide::Colors &colors;


 public:
  SensorCalibDrawingWidget(cfg::guide::Sensors &sensors,cfg::guide::SensorGui &sensorGui,cfg::guide::Colors &pcolors);//,helium::ValueHub<int>& pas);
  void on_refresh();
 protected:
  //OVERRIDES events
  virtual bool on_expose_event(GdkEventExpose* event);

  //to load bg image
  Glib::RefPtr<Gdk::Pixbuf> bgimage;
  int width,height;
  float pr;

  

 public:
  //const helium::ValueHub<int>& active_sensor;
  helium::ValueHub<int> active_sensor;

  float get_pr() {
    return pr;
  }

  int get_active_sensor_no();
  //void set_active_sensor_no(int i);
  int update_active_sensor_no(int x, int y,double scale);

  helium::StaticConnReference<int,helium::ValueHub<int>,const int&,&helium::ValueHub<int>::assure> callAssureActiveSensor;

  //void set_selected_sensor(int i);
};

#endif //DRAWINGWIDGET
