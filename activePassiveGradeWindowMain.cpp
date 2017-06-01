#include <iostream>
#include <helium/robot/application.h>
#include "guideDescription.h"
#include <helium/robot/motionParser.h>
#include <app/guide2/guideMotionManagement.h>
#include <app/guide2/activePassiveGradeWindow.h>
#include "robotModels.h"

using namespace std;

class ActivePassiveGradeApp : public helium::Application {
  void run(){
    GuideState guideState; //create guide config tree
    dimport(guideState,mem);
    Gtk::Main kit(argc, argv);
    std::auto_ptr<helium::JointPosConverter> pc;
    {
      helium::KnownPosNormalizer knownPosNormalizer(mem);
      pc.reset(knownPosNormalizer.getNewJointPosConverter()); 
    }
    helium::GtkExceptionHandling gtkEh;
    helium::MotionParser motionParser(guideState.pref.robot); 
    GuideMotionManagement guideMotionManagement(guideState,gtkEh,*pc,motionParser);
    ActivePassiveGradeWindow activePassiveGradeWindow;

    RobotModels robotModels(guideState);
    ActivePassiveModelWindow modelWindow(guideState);
    GuideTeachWindow guideTeachWindow(guideMotionManagement,&robotModels);

    SensorCalibDrawingWidget sensorCalibDrawingWidget(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors);
    SensorCalibSubWindow sensorCalibSubWindow(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors,&guideMotionManagement.eh,guideState.conn,config["sensors"],sensorCalibDrawingWidget); //pass

    ActivePassiveGradeMainWindow activePassiveGradeMainWindow(sensorCalibSubWindow, guideTeachWindow,activePassiveGradeWindow,modelWindow);
    Gtk::Main::run(activePassiveGradeMainWindow); 
  }
};

int main(int argc, char**argv) {
  ActivePassiveGradeApp app;
  if(!Glib::thread_supported()){
    Glib::thread_init();
  }
  helium::GtkSignalling::init();
  gdk_threads_enter();
  app.launch(argc,argv,"conf/guide.neony.xml",helium::Application::AS_RUN);
  gdk_threads_leave(); 
  return 0;
} 
