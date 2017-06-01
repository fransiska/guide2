/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#include "connectionCell.h"
#include <helium/robot/application.h>
//#include <helium/robot/limitFactory.h>
#include <helium/system/cellProtocol.h>
#undef max //WIN hack
#include "guideState.h"
#include "guideHistory.h"
#include "guideLogic.h"
#include "guideMotionManagement.h"
#include "guideColdetWindow.h"
#include "guideDescription.h"
#include <helium/robot/gyroStructures.h>
#include <helium/system/valueProxy.h>
#include "robotModels.h"
#include <app/guide2/guideErrorDialog.h>

#include <sstream>



//#define GCONFIG

//#ifndef GCONFIG

#include "guideWindow.h"
#include <helium/robot/motionParser.h>


#include <helium/core/exception.h>
#include <helium/thread/thread.h>
#include <helium/system/keyboardCellServer.h>
#include <helium/robot/knownPosNormalizer.h>


//extra windows
#include "sensorCalibWindow/sensorCalibWindow.h"
#include "gyroCalibWindow/gyroCalibWindow.h" 
#include <app/guide2/guideTeachWindow.h>
#include <app/guide2/guideDebugWindow.h>
#include <app/guide2/guideMotionManagement.h>


//#define YARP

#ifdef YARP
#include "yarpInterface.h"
#endif


using std::cout;
using std::endl;
using std::stringstream;


class StatePrinter:public helium::Callback<const std::pair<int,helium::HwState>& >,
		   public helium::ConnReference<const std::pair<int,helium::HwState>& >{

  std::map<int,helium::HwState> faulty;

public:
  StatePrinter():helium::ConnReference<const std::pair<int,helium::HwState>& >(this){
}

  void operator()(const std::pair<int,helium::HwState>& p){
    faulty[p.first]=p.second;
    //std::cout<<"Now bad sensors are";
    for (std::map<int,helium::HwState>::iterator it=faulty.begin();it!=faulty.end();++it){
      if (it->second!=helium::WORKINGHWSTATE){
	//std::cout<<" "<<it->first<<"="<<helium::en::eio<<it->second;
      }
    }
  }

};



class Guide:public helium::Application{
  GuideState guideState; //create guide config tree
  ConnectionCell connCell;  
 
 

  void run(){

   
#ifndef YARP
    dexport(mem,connCell,helium::IDPath("guide.connection","."));
#endif   

    dimport(guideState,mem);

    std::auto_ptr<helium::JointPosConverter> pc;
    {
      helium::KnownPosNormalizer knownPosNormalizer(mem);
      pc.reset(knownPosNormalizer.getNewJointPosConverter()); 
    }

    GuideLogic guideLogic(guideState,*pc); //resize motion here

    //time for log file
    struct tm * timeinfo;
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer,80,"%Y%m%d%H%M%S",timeinfo);
    guideState.settings.timestamp = std::string(buffer);

    guideState.renameLogfiles(guideState.pref.teachLogfile);
    guideState.renameLogfiles(guideState.pref.motionLogfile);
    guideState.renameLogfiles(guideState.pref.sensorLogfile);

    long long t0 = helium::getSystemMillis();
    GuideHistory guideHistory(guideState,t0);

    StatePrinter sensorStatePrinter;

    guideState.sensors.state.connect(sensorStatePrinter);

    Gtk::Main kit(argc, argv);
    helium::MotionParser motionParser(guideState.pref.robot);
    helium::GtkExceptionHandling gtkEh;
    std::vector<GuideSubWindow> subwins;

    GyroCalibWindow *gyroCalibWindow=NULL;
    SensorCalibDrawingWidget *sensorCalibDrawingWidget=NULL;
    SensorCalibSubWindow *sensorCalibWindow=NULL;
    GuideTeachWindow *guideTeachWindow=NULL;
    GuideColdetWindow *guideColdetWindow = NULL; //coldet parameter window
    std::auto_ptr<RobotModels> robotModels;

    GuideMotionManagement guideMotionManagement(guideState,gtkEh,*pc,motionParser);
    guideMotionManagement.t0 = t0;

    GuideDebugWindow guideDebugWindow(mem,&guideMotionManagement.eh);
    //GuideHardwareWindow guideHardwareWindow(guideState.sensors);
    //subwins.push_back(GuideSubWindow(&guideHardwareWindow,"Hardware Window"));
    subwins.push_back(GuideSubWindow(&guideDebugWindow,"Tree Window"));

    std::auto_ptr<coldet::ColdetManager> coldetManager;



    ///create robot Models
    if(guideState.pref.hasModel) {      
      robotModels.reset(new RobotModels(guideState)); 
    }

    if(guideState.pref.hasColdet) {
      coldetManager.reset(new coldet::ColdetManager(guideMotionManagement,guideHistory.getMotionChangedSignal(),robotModels->coldetModel));
      //coldetManager.get().robotCollide.connect(robotModels->mainCanvasArea.callSetModeltoRobot); /// \todo
    }


    ///create Gyro Window
    if(guideState.pref.hasGyro) {
      gyroCalibWindow = new GyroCalibWindow(guideState.joints,
					    guideState.gyro,
					    guideState.conn,
					    config["gyro"],//guideState.pref.gyroFilename,
					    guideState.settings.colors,
					    &guideMotionManagement.eh,
					    1,//default value
					    guideState.pref.hasModel?robotModels.get()->gyroArea.get():NULL);
      subwins.push_back(GuideSubWindow(gyroCalibWindow,"Gyro Calib Window"));
    }

    ///create Touch Sensor Window
    if(guideState.pref.hasTouchSensor) {
      sensorCalibDrawingWidget = new SensorCalibDrawingWidget(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors);
      sensorCalibWindow = new SensorCalibSubWindow(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors,&guideMotionManagement.eh,guideState.conn,config["sensors"],*sensorCalibDrawingWidget); //pass
      guideTeachWindow = new GuideTeachWindow(guideMotionManagement,guideState.pref.hasModel?robotModels.get():NULL); //pass

      subwins.push_back(GuideSubWindow(guideTeachWindow,"Teach List Window"));
      subwins.push_back(GuideSubWindow(sensorCalibWindow,"Sensor Calib Window"));

      //selecting teach id will select correlating sensor, and the other way around
      //2015.06.17  guideTeachWindow->sensorSelectionSignal.connect(sensorCalibDrawingWidget->callAssureActiveSensor);
      //2015.06.17  sensorCalibDrawingWidget->active_sensor.connect(guideTeachWindow->callSelectSensor);
    }

    ///create Coldet Window
    if(guideState.pref.hasColdet){
      guideColdetWindow = new GuideColdetWindow(guideMotionManagement.gs.joints,guideState.coldet,*coldetManager.get(),&guideMotionManagement.eh,config["coldet"]);
      subwins.push_back(GuideSubWindow(guideColdetWindow,"Collision Detection Window"));
    }

    if(guideState.pref.hasModel) {
      subwins.push_back(GuideSubWindow(&(robotModels.get()->robotWindow),"Robot View"));
      subwins.push_back(GuideSubWindow(&(robotModels.get()->frameWindow),"Frame View"));
      subwins.push_back(GuideSubWindow(&(robotModels.get()->fixWindow),"Fix View"));
    }

    //pedal
    helium::LogitecGTForce *pedal=NULL;
    helium::StaticConnReference<void,GuideState,void,&GuideState::powerOffRobot> callPowerOffRobot(&guideState);

    if(guideState.pref.pedal.dev.size()) {
      pedal = new helium::LogitecGTForce(guideState.pref.pedal.dev.c_str(),false,guideState.pref.pedal.checksum1,guideState.pref.pedal.checksum2,guideState.pref.pedal.rdownThreshold,
					 guideState.pref.pedal.rupThreshold, guideState.pref.pedal.ldownThreshold,guideState.pref.pedal.lupThreshold);
      pedal->start(); 
      pedal->leftPedalDown.connect(callPowerOffRobot);
      if(guideTeachWindow) {
	pedal->rightPedalDown.connect(guideTeachWindow->callSetAcquireOn);
	pedal->rightPedalUp.connect(guideTeachWindow->callSetAcquireOff);
      }
      std::cout << "Loading pedal OK" << std::endl;
    }
  
    guideState.pref.plFilename = config["postures"];
    
    GuideWindow guideWindow(guideMotionManagement,
			    guideHistory,
			    guideState.pref.hasModel?robotModels.get():NULL,
			    subwins,
			    guideState.pref.hasColdet?coldetManager.get():NULL
			    );
    ErrorDialogEH errorDialog(guideWindow,&gtkEh,guideState.errorn);    
    gtkEh.setExceptionHandler(&errorDialog);
    helium::GtkExceptionHandling::setDefaultExceptionHandler(&errorDialog);
    helium::Thread::setGlobalExceptionHandler(&errorDialog);
    guideWindow.addSubWin(errorDialog.getErrorDialog(),"Error List Window");

    
    /*
    std::vector<TeachIO> tl;
    std::string a = "a";
    GuideTeachList gtl(a,guideState.sensors);
    gtl.loadTeachList("teachLists.tea",tl);
    */


    helium::KeyboardCellServer kb(mem);
    //DEBUG VC2010 kb.start();
    helium::GtkSignalling::init();


#ifdef YARP
    YarpInterface yarp("/icubSim/",guideState);
    helium::dexport(mem,yarp,"guide");
#endif



    Gtk::Main::run(guideWindow); 

    connCell.setData(helium::Connection::disconnect());

    
    
    if(pedal) {
      pedal->leftPedalDown.disconnect(callPowerOffRobot);      
      if(guideTeachWindow) {
	pedal->rightPedalDown.disconnect(guideTeachWindow->callSetAcquireOn);
	pedal->rightPedalUp.disconnect(guideTeachWindow->callSetAcquireOff);
      }
      delete pedal;
    }

    if(gyroCalibWindow) delete gyroCalibWindow;
    if(sensorCalibWindow) delete sensorCalibWindow;
    if(sensorCalibDrawingWidget) delete sensorCalibDrawingWidget;
    if(guideTeachWindow) delete guideTeachWindow;
    if(guideColdetWindow) delete guideColdetWindow;
  }  
public:
  Guide():connCell(mem,guideState){
    addParamValue("ip","localhost");
    addParamValue("port",helium::toString(helium::CELLPORT));
    addParamID("pedal");
  }

};

int main(int argc,char** argv){
  

  Guide g;
  if(!Glib::thread_supported()){
    Glib::thread_init();
  }
  helium::GtkSignalling::init();
  gdk_threads_enter();
  g.launch(argc,argv,"conf/guide.xml",helium::Application::AS_RUN);
  gdk_threads_leave();  
}

