#ifndef SENSORPLOTWINDOW
#define SENSORPLOTWINDOW

#include <app/guide2/sensorCalibWindow/sensorCalibWindow.h>


const int SEN = 90;

class SensorPlotWindow : public SensorCalibSubWindow {
  helium::Array<double> sensorPlotValue;
  helium::Array<helium::Array<double,experiment::USR>,experiment::RSEN> sensorPlotPie;
public:
  SensorPlotWindow(cfg::guide::Sensors &sensors,cfg::guide::SensorGui &sensorgui,cfg::guide::Colors &pcolors,helium::GtkExceptionHandling *peh, helium::GtkProxy<helium::Connection> &pconn,const std::string& pfilename):
    SensorCalibSubWindow(sensors,sensorgui,pcolors,peh,pconn,pfilename),
    callOnValueChange(this),
    callOnPostureChange(this),
    callSetValueChange(this),
    callOnValueChangeAP(this),
    callOnPostureChangeAP(this)
  {
    set_title("Sensor Plot");
    sensorPlotValue.resize(sensors.size());
  }
  void setPlotValue(std::string s) {
#ifdef PLOT
    sensorCalibDrawingWidget.plotValue = sensorPlotValue.data;
#endif
    //sensorPlotValue = p;
    std::cout << "setPlotValue " <<     s.substr(0,s.find_last_of("_")) << std::endl;
    std::cout << "   pos " <<     s.substr(s.find_last_of("_")+1,s.size()) << std::endl;

    std::ifstream in;
    std::stringstream sf;
    sf << s.substr(0,s.find_last_of("_")) << "/" << s.substr(s.find_last_of("_")+1,s.size()) << ".sensor3.dot";
    in.open(sf.str().c_str());
    std::string line;
    std::getline(in,line);
    size_t i=0;
    while(in && i<sensors.size()) {
      helium::convert(sensorPlotValue[i],line);
      //std::cout << sensorPlotValue[i] << std::endl;
      std::getline(in,line);
      i++;
    }
    sensorCalibDrawingWidget.on_refresh();
    signalPlotUpdate(sensorPlotValue);
  }
  void setPieValue(std::string s) {
    std::string posture = s.substr(s.find_last_of("_")+1,s.size());
    std::string folder = s.substr(0,s.find_last_of("/\\"));
    std::cout << "setPieValue " << folder << " " << posture << std::endl;
    //readfile
    std::ifstream in;


    std::string nama = s.substr(s.find_last_of("/\\")+1,s.find_last_of("_"));
    nama = nama.substr(0,nama.find_last_of("_"));

#ifdef PLOT
    std::cout << "    user " << sensorCalibDrawingWidget.user  << " " << nama << std::endl;
    for(size_t ou=0;ou<experiment::USR;ou++) 
      if(strcmp(nama.c_str(),experiment::user[ou].c_str())==0) {
	std::cout << "    !!! i am user #"<<ou << std::endl;
	sensorCalibDrawingWidget.user = ou;
	break;
      }
#endif

    for(int u=0;u<experiment::USR;u++) {
      std::stringstream pieValueFile;
      pieValueFile << folder << "/" << experiment::user[u] << "/" << posture << ".sensor3.dot";
      std::cout << u << " " << pieValueFile.str() << std::endl;
      in.open(pieValueFile.str().c_str());
      std::string line;
      std::getline(in,line);
      //std::cout << line << std::endl;
      int i = 0;
      helium::Array<double,experiment::RSEN> pieUser;
      while(in && i<experiment::RSEN) {
#ifdef PLOT
	//if(u == sensorCalibDrawingWidget.user)
	//for(size_t o=0;o<10;o++) 
	//std::cout << line << std::endl;
#endif
	helium::convert(sensorPlotPie[i][u],line);
	std::getline(in,line);
	i++;
      }
      in.close();



    }
#ifdef PLOT
    sensorCalibDrawingWidget.plotPie = sensorPlotPie.data;
#endif
    sensorCalibDrawingWidget.on_refresh();
  }
  void setPlotValueChange(std::pair<int,double> m) {
    sensorPlotValue[(size_t)m.first] = m.second;
  }

  helium::StaticConnReference<std::string,SensorPlotWindow,std::string,&SensorPlotWindow::setPlotValue> callOnValueChange;
  helium::StaticConnReference<std::string,SensorPlotWindow,std::string,&SensorPlotWindow::setPieValue> callOnPostureChange;


  helium::StaticConnReference<std::pair<int,double>,SensorPlotWindow,std::pair<int,double>,&SensorPlotWindow::setPlotValueChange> callSetValueChange;
  helium::StaticConnReference<std::string,SensorPlotWindow,std::string,&SensorPlotWindow::setPlotValue> callOnValueChangeAP;
  helium::StaticConnReference<std::string,SensorPlotWindow,std::string,&SensorPlotWindow::setPieValue> callOnPostureChangeAP;

  helium::Signal<helium::Array<double> > signalPlotUpdate;

};

#endif

