#ifndef SENSORCALIBMAINWINDOW
#define SENSORCALIBMAINWINDOW

#include "sensorCalibWindow.h"

class SensorCalibMainWindow : public SensorCalibWindow {
  double currRawVal;

 public:
  SensorCalibMainWindow( SensorCalib *psc, SensorCalibManager* pscm, 
			 const std::string minFilename,const std::string maxFilename,
			 const std::string &host,int port) :
  SensorCalibWindow(psc,pscm,minFilename,maxFilename)
  {
  }
  
  void connect(const char* host, int port) {
    /*
    ClientSocket::connect(host,port);
    guiConnectionLine->setState(AsciiClient::isConnected());
    AsciiClient::stop();
    AsciiClient::start();    
    AsciiClient::enableRawSensorFlow();
    */
  }

  void disconnect() {
    //AsciiClient::disconnect();
  }
  
  void onSocketDisconnection(){
    guiConnectionLine->setState(false);
  }

  void setPowerState(bool state) {
    //AsciiClient::setPowerState(-1,state);
  }

 public:
  void registerConnectionLine(GuiConnectionLine* g) {
    //guiConnectionLine = g;
    std::cout << "SensorCalibMainWindow " << g << std::endl;
  }
};

#endif
