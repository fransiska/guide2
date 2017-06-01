#include <helium/robot/application.h>
#include "guideDescription.h"

#include <helium/robot/motionParser.h>
#include <app/guide2/guideMotionManagement.h>
#include "robotModel.h"
#include "postureCaptureWindow.h" 
#include <app/guide2/guideTeachWindow.h>
#include <app/guide2/activePassiveGradeWindow.h>
#include "sensorCalibWindow/sensorCalibWindow.h"
#include "sensorCalibWindow/sensorCalibDrawingWidgetPlotEM.h"
using namespace std;


namespace helium{  
  template<> 
  class DefDescr<SensorValues>:public Description<SensorValues>{  
  public:
    DefDescr(SensorValues& loc):Description<SensorValues>(loc){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(static_cast<Array<double>&>(obj),IDPath("value")));
    }
    
  };

  template<> 
  class DefDescr<SensorYTouch>:public Description<SensorYTouch>{  
  public:
    DefDescr(SensorYTouch& loc):Description<SensorYTouch>(loc){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
      members.push_back(psvectMapping(vd(obj.v),"yvalues","yvalue","em"));
      members.push_back(scalarMapping(d(obj.exid),IDPath("em.exid",".")));
      members.push_back(scalarMapping(d(obj.misid),IDPath("em.misid",".")));
      members.push_back(scalarMapping(d(obj.user),IDPath("em.user.value",".")));
    }
  };
}

class SensorPlotWindow : public SensorCalibSubWindow {
public:
  SensorPlotWindow(cfg::guide::Sensors &sensors,cfg::guide::SensorGui &sensorgui,cfg::guide::Colors &pcolors,helium::GtkExceptionHandling *peh, helium::GtkProxy<helium::Connection> &pconn,const std::string& pfilename,SensorCalibDrawingWidgetPlotEM &scdw):
    SensorCalibSubWindow(sensors,sensorgui,pcolors,peh,pconn,pfilename,scdw)
    //callOnValueChange(this),
    //callOnPostureChange(this),
    //callSetValueChange(this),
    //callOnValueChangeAP(this),
    //callOnPostureChangeAP(this)
  {
    set_title("Sensor Plot");
    //sensorPlotValue.resize(sensors.size());
  }
};

class SensorViewer:public helium::Application{
  void run(){
    //create guide config tree
    GuideState guideState; 
    SensorYTouch sensorYTouch;
    dimport(guideState,mem);
    
    Gtk::Main kit(argc, argv);
    std::auto_ptr<helium::JointPosConverter> pc;
    {
      helium::KnownPosNormalizer knownPosNormalizer(mem);
      pc.reset(knownPosNormalizer.getNewJointPosConverter()); 
    }

    dimport(sensorYTouch,mem);

    helium::MotionParser motionParser(guideState.pref.robot); 
    helium::GtkExceptionHandling gtkEh;
    GuideMotionManagement guideMotionManagement(guideState,gtkEh,*pc,motionParser);
    
    SensorCalibDrawingWidgetPlotEM scdw(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors,sensorYTouch);
    SensorPlotWindow sensorCalibWindow(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors,&guideMotionManagement.eh,guideState.conn,config["sensors"],scdw);
    Gtk::Main::run(sensorCalibWindow); 
  }
};

int main(int argc,char** argv) {
  SensorViewer g;


  if(!Glib::thread_supported()){
    Glib::thread_init();
  }
  helium::GtkSignalling::init();
  gdk_threads_enter();

  cout << "PIE" << endl;
  g.launch(argc,argv,"conf/guide.neony.pie.xml",helium::Application::AS_RUN);

  gdk_threads_leave();  
  return 0;
}
