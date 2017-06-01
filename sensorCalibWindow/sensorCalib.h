/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef SENSORCALIB
#define SENSORCALIB

#include <app/guide2/guideState.h>

class SensorCalib {
 public:
  cfg::guide::Sensors &sensors;
  double currRawVal;
  Glib::Dispatcher updateDispatcher; 
 private:
  //GuiConnectionLine* guiConnectionLine;
  helium::Array<int> oldRawValues,rawTSensorValues;

 public:
  SensorCalib(cfg::guide::Sensors &sensors);
  void connect(const char* host, int port);
  void disconnect();
  void onSocketDisconnection();
  void setPowerState(bool state);
  //void registerConnectionLine(GuiConnectionLine* g);
  //void onRawTSensorUpdate();

};

#endif
