#include <app/guide2/readablePostureDrawer.h>
#include <helium/robot/application.h>
#include <helium/robot/limitFactory.h>
#include "connectionCell.h"
#include "guideState.h"
#include "guideDescription.h"
#include <gtkmm.h>
#include <app/guide2/floatingModelWindow.h>

using namespace std;

class RobotModel{
  helium::convex::ConvexModel convexModel;
  helium::convex::ConvexCollisionModel ccm;
public:
  RobotModel(cfg::guide::Config& config):
    convexModel(config.joints.modelFilename.c_str()),
    ccm(&convexModel)
  {
  }
  helium::convex::ConvexCollisionModel* getCollisionModel(){
    return &ccm;
  }
  
};

class GuidePixbufWindow:public Gtk::Window {
  helium::convex::ConvexCollisionModel* ccm;
  cfg::guide::Config &config;
  Gtk::Button buton;
  FloatingModelWindow* fmodelWin;
public:
  GuidePixbufWindow(helium::convex::ConvexCollisionModel* pcm,cfg::guide::Config &pconfig):
    ccm(pcm),config(pconfig),
    fmodelWin(ccm?new FloatingModelWindow(config,*ccm):NULL)
  {
    add(buton);
  }
  virtual ~GuidePixbufWindow() {
    if(fmodelWin) delete fmodelWin;
  }
};

class Guide:public helium::Application{
  helium::ConnectionCell connCell;  
  void run(){
    helium::LimitFactory motor(mem); //rellimit (converting motor hex value into int)
    cfg::guide::Config config;
    dimport(config,mem);
    dexport(connCell,mem,IDPath("guide.connection","."));
    config.joints.modelFilename = "v4gModel.xml";
    RobotModel* robotModel=config.pref.hasModel=="true"?new RobotModel(config):NULL;
    Gtk::Main kit(argc, argv);    
    GuidePixbufWindow guidePixbufWindow(robotModel?robotModel->getCollisionModel():NULL,config);
    Gtk::Main::run(guidePixbufWindow); 

    if (robotModel) delete robotModel;
  }
public:
  Guide():connCell(mem){
  }
};

int main(int argc,char** argv) {
  Guide g;
  Glib::RefPtr<Gtk::Settings> s = Gtk::Settings::get_default();
  if(!Glib::thread_supported()) Glib::thread_init();
  gdk_threads_enter();
  g.launch(argc,argv,"conf/guide.xml",helium::Application::AS_RUN);
  gdk_threads_leave();
}
