#include <helium/robot/application.h>
#include "guideDescription.h"
#include "sensorCalibWindow/sensorCalibWindow.h"
#include <helium/robot/motionParser.h>
#include <app/guide2/guideMotionManagement.h>
#include "robotModel.h"
#include "postureCaptureWindow.h" 
#include <app/guide2/guideTeachWindow.h>
#include "floatingModelWindow.h"
#include <helium/util/ioUtil.h>
#include <helium/util/mathUtil.h>

using namespace std;

const int maxSensorId = 113;
const int sensorSize = 90;
const int motorSize = 22;
const string defaultFolder="/home/fransiska/research/dataAnalysis/touchExperiment/";

class CollisionCheckerWindow : public Gtk::Window {
  FloatingFrameModelWindow &modelWindow;
  helium::Posture p;
  Gtk::Button but;
  string f;
public:
  CollisionCheckerWindow(FloatingFrameModelWindow &pmodelWindow,std::string filename=""):
    modelWindow(pmodelWindow),
    p(22),
    but("run"),
    f(filename)
  {
      //view model 
      modelWindow.getModelWindow().show();
      this->resize(100,100);
      add(but);
      but.signal_clicked().connect(sigc::mem_fun(this,&CollisionCheckerWindow::run));
      helium::zero(p);
      string ataru = "  -0.47793 0.296317 -1.07269 1.1311 0.622371 -0.785929 1.0472 -3000 0 0 0 0 0 0 0 0 0 0 0 0 0 0";
	//0 0 0.535695 -0.535695 1.57252 -1.56442 1.05411 -1.05411 0 0 0 0 0 0 0 0 0 0 0 0 0 0";
      helium::convert(p,ataru);
      modelWindow.rotateModel(p.data);
      if(modelWindow.getCCM().isColliding(false) )
	cout << "collide " << p << endl;

      if(filename.size())
	this->signal_show().connect(sigc::mem_fun(this,&CollisionCheckerWindow::run));

      //show();
      show_all_children();

  }
  void run() {

    if(f.size()) {      
      //convert file only
      try {
	convert(f);
	//Gtk::Main::quit();
      }
      catch(...){
	cerr << "error in converting" << endl;
      }
      this->hide();
      modelWindow.getModelWindow().hide();
      exit(0); //returns to shell
    }
    int j=0;
    static int i=-100;
    //for(int i=-100;i<=100;i++) {	
    p[j] = helium::getDegtoRad(i);
    modelWindow.rotateModel(j,p[j]);
    if(modelWindow.getCCM().isColliding(false) )
      cout << j << " " << i << " collide " << 1 << " " << p << endl;
    //}
    i++;
  }
  void convert(std::string filename) {
    std::vector<TeachIO> tl;
    helium::XmlLoader xml;
    helium::Memory inmem;
    xml.loadDataAndModel(inmem,filename);
    helium::dimport(tl,inmem);

    ofstream myfile;

    stringstream st;
    st << filename << ".col";
    myfile.open(st.str().c_str());
    

    for(size_t s=0;s<tl.size();s++) {
      cout << "input " << s << endl;
      for(size_t j=0;j<22;j++) {
	//cout << "motor " << j << endl;
	p = tl[s].output;
	modelWindow.rotateModel(p.data);
	stringstream ss;
	for(int i=-100;i<=100;i++) {
	  p[j] = helium::getDegtoRad(i);
	  modelWindow.rotateModel(j,helium::getDegtoRad(i));
	  Gdk::Window::process_all_updates();
	  //cout << s << " " << j << " " << p << endl;
	  ss << modelWindow.getCCM().isColliding(true) << " ";
	}
	myfile << ss.str() << "\n";
      }
    }
    myfile.close();

  }
};

struct Parameter {
  std::string filename;
  Parameter() {
    filename="";
  }
};

namespace helium {
template<> 
  class DefDescr<Parameter>:public Description<Parameter>{
  public:
  DefDescr(Parameter& s):Description<Parameter>(s){
  }
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.filename)),"teachFile"));
  }
  };  
}

class CollisionChecker:public helium::Application{
public:


  CollisionChecker() {
    addParamID("teachFile");
  }
  void run(){
    Parameter parameter;
    cout << "importing param" << endl;

    if(argc>1)
      dimport(parameter,mem);
    cout << "imported param" << endl;

    GuideState guideState; //create guide config tree
    dimport(guideState,mem);
    cout << "imported gs" << endl;
    Gtk::Main kit(argc, argv);

    std::auto_ptr<helium::JointPosConverter> pc;
    {
      helium::KnownPosNormalizer knownPosNormalizer(mem);
      pc.reset(knownPosNormalizer.getNewJointPosConverter()); 
    }

    RobotModel robotModel(guideState);
    FloatingFrameModelWindow modelWindow(guideState,*robotModel.getCollisionModel());

    CollisionCheckerWindow collisionCheckerWindow(modelWindow,parameter.filename);

    Gtk::Main::run(collisionCheckerWindow); 
  }
};

int main(int argc,char** argv) {
  CollisionChecker g;


  if(!Glib::thread_supported()){
    Glib::thread_init();
  }
  helium::GtkSignalling::init();
  gdk_threads_enter();



  g.launch(argc,argv,"conf/guide.xml",helium::Application::AS_RUN);
  gdk_threads_leave();  
  return 0;
}
