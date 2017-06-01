#include <gtkmm.h>
#include "sensorCalibWindow.h"
#include "sensorCalibMainWindow.h"
#include "sensorCalib.h"
#include <sstream>
#include <vstone/V4Gstructure.h>

using namespace std;

int main(int argc, char** argv){ 
  string host="localhost";
  int port=5001;

  if (argc>1){
    host=argv[1];
  }
  if (argc>2){
    stringstream ss;
    ss<<argv[2];
    ss>>port;    
  }
  
  Gtk::Main kit(argc,argv);

#ifndef SENSORXML  
  SensorCalibManager scm(SENSORMINFILENAME,SENSORMAXFILENAME,SENSORCONFIGFILENAME);
#else
  SensorCalibManager scm(SENSORCALIBXMLFILENAME);
#endif

  SensorCalib sensorCalib(&scm, SENSORDRAWPOSFILENAME);
  SensorCalibMainWindow sensorCalibMainWindow(&sensorCalib, &scm, SENSORMINFILENAME,SENSORMAXFILENAME,host,port,new V4Gstructure(NULL,NULL));

  //sensorCalibWindow
  //add menu bar
  //#ifdef SENSORAPP
  GuiConnectionLine guiConnectionLine(&sensorCalib,host,port);
  sensorCalibMainWindow.registerConnectionLine(&guiConnectionLine);

  //not needed: sensorCalibWindow.sensorCalibMenu.addTo(&sensorCalibWindow,sensorCalibWindow.vbox_outer);
  sensorCalibMainWindow.vbox_outer.pack_start(guiConnectionLine,Gtk::PACK_SHRINK,5);
  sensorCalibMainWindow.show_all_children();

  sensorCalib.connect(host.c_str(),port);
  //#endif

  Gtk::Main::run(sensorCalibWindow);
  return 0;
}
