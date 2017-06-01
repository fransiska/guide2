#include <helium/robot/application.h>
#include "guideDescription.h"

#include <helium/robot/motionParser.h>
#include <app/guide2/guideMotionManagement.h>
//#include "robotModel.h"
#include "postureCaptureWindow.h" 
#include <app/guide2/guideTeachWindow.h>
#include <app/guide2/activePassiveGradeWindow.h>
#include "teachViewerWindow.h"
using namespace std;


class TeachViewer:public helium::Application{
  void run(){
    GuideState guideState; //create guide config tree
    dimport(guideState,mem);

    Gtk::Main kit(argc, argv);
    std::auto_ptr<helium::JointPosConverter> pc;
    {
      helium::KnownPosNormalizer knownPosNormalizer(mem);
      pc.reset(knownPosNormalizer.getNewJointPosConverter()); 
    }

    RobotModel robotModel(guideState);
    RobotModels::CaptureModelWindow postureCaptureWindow(guideState.settings.sizes.model.w,guideState.settings.sizes.model.h,robotModel.getCollisionModel());

    helium::GtkExceptionHandling gtkEh;
    helium::MotionParser motionParser(guideState.pref.robot); 
    GuideMotionManagement guideMotionManagement(guideState,gtkEh,*pc,motionParser);


    GuideTeachWindow guideTeachWindow(guideMotionManagement,&postureCaptureWindow); //pass
    FloatingFrameModelWindow modelWindow(guideState,*robotModel.getCollisionModel());


#ifdef PLOT
    SensorPlotWindow sensorCalibWindow(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors,&guideMotionManagement.eh,guideState.conn,config["sensors"]);
#else
    SensorCalibSubWindow sensorCalibWindow(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors,&guideMotionManagement.eh,guideState.conn,config["sensors"]);
#endif

#ifdef APGRADE
    ActivePassiveGradeWindow apWindow;
    TeachViewerWindow teachViewerWindow(sensorCalibWindow,guideTeachWindow,modelWindow,&apWindow);
    teachViewerWindow.updateSensorPlot.connect(apWindow.callOnFileChange);
    teachViewerWindow.changeAPid.connect(apWindow.callOnIdChange);
    apWindow.moveToNextId.connect(teachViewerWindow.callMoveToNextId);

#ifdef PLOT
#ifndef PIE
    sensorCalibWindow.signalPlotUpdate.connect(apWindow.callOnValueChange);
    apWindow.reloadFile.connect(sensorCalibWindow.callSetValueChange);
#else
    //apWindow.reloadFile.connect(sensorCalibWindow.callOnPostureChangeAP);
#endif
#endif

#else
    TeachViewerWindow teachViewerWindow(sensorCalibWindow,guideTeachWindow,modelWindow,NULL);
#endif 


#ifdef PLOT
#ifndef PIE
    teachViewerWindow.updateSensorPlot.connect(sensorCalibWindow.callOnValueChange);
#else
    teachViewerWindow.updateSensorPlot.connect(sensorCalibWindow.callOnPostureChange);
#endif
#endif


    Gtk::Main::run(teachViewerWindow); 
  }
};

int main(int argc,char** argv) {
  TeachViewer g;


  if(!Glib::thread_supported()){
    Glib::thread_init();
  }
  helium::GtkSignalling::init();
  gdk_threads_enter();
#ifndef PIE
  cout << "PIE" << endl;
  g.launch(argc,argv,"conf/guide.neony.plot.xml",helium::Application::AS_RUN);
#else
  cout << "PLOT" << endl;
  g.launch(argc,argv,"conf/guide.neony.pie.xml",helium::Application::AS_RUN);
#endif
  cout << "END" << endl;
  gdk_threads_leave();  
  return 0;
}
