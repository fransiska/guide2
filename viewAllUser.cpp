#include <helium/robot/application.h>
#include "guideDescription.h"

#include <helium/robot/motionParser.h>
#include <app/guide2/guideMotionManagement.h>
#include "robotModel.h"
#include "postureCaptureWindow.h" 
#include <app/guide2/guideTeachWindow.h>
#include <app/guide2/activePassiveGradeWindow.h>
#include "teachViewerWindow.h"
#include <app/guide2/userInfo.h>
using namespace std;

const std::string mainfolder = "/home/fransiska/research/touchExperiment/";

class ModelWindowUnit {
  helium::convex::ConvexCollisionModel ccm;
  FloatingFrameModelWindow modelWindow;
  int userid;
  std::vector<helium::Array<double,experiment::MOT> > postureangle;
public:
  ModelWindowUnit(GuideState &guideState, RobotModel &robotModel, int pid):
    ccm(*(robotModel.getCollisionModel()),NULL),
    modelWindow(guideState,ccm),
    userid(pid)
  {
    modelWindow.getModelWindow().show();
    modelWindow.getModelWindow().set_size_request(320,240);
    modelWindow.getModelWindow().set_title(experiment::user[userid]);
  }
  void setId(int id) {
    if(id<postureangle.size()) {
      modelWindow.rotateModel(postureangle[id].data);
    }
  }
  void setPosture(std::string posture) {
    postureangle.clear();
    std::string file = mainfolder+experiment::user[userid]+"/"+posture+".conv.tea.pout.out";
    ifstream in;
    in.open(file.c_str());
    if(in) {
      cout << "loaded " << file << endl;
      std::string line;
      getline(in,line);
      while(in) {
	helium::Array<double,experiment::MOT> d;
	stringstream ss;
	ss << line;
	ss >> d;
	postureangle.push_back(d);
	//helium::convert(line,postureangle.back());
	//cout << postureangle.back() << endl;
	getline(in,line);
      }
    }
    else {
      cout << "error " << file << endl;
    }
  }
};

class ModelWindowArray {
  RobotModel robotModel;
  GuideState &gs;
  void allocModelWindow(ModelWindowUnit* m, int i) {
    new (m) ModelWindowUnit(gs,robotModel,i);
  }
  helium::Array<ModelWindowUnit,helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::DynamicClassPlacerAllocator<ModelWindowUnit,ModelWindowArray> > > mwu;
public:
  ModelWindowArray(GuideState &guideState):
    robotModel(guideState),
    gs(guideState),
    mwu(std::make_pair(this,&ModelWindowArray::allocModelWindow),experiment::USR-1),
    callOnIdChange(this),
    callOnFileChange(this)
  {
  }
  void setId(int id) {
    for(size_t i=0;i<mwu.size();i++)
      mwu[i].setId(id);
  }
  void setFile(std::string s) {
    std::string posture = s.substr(s.find_last_of("_")+1,s.size());
    for(size_t i=0;i<mwu.size();i++)
      mwu[i].setPosture(posture);
  }
  helium::StaticConnReference<int,ModelWindowArray,int,&ModelWindowArray::setId> callOnIdChange;
  helium::StaticConnReference<std::string,ModelWindowArray,std::string,&ModelWindowArray::setFile> callOnFileChange;
};

class ViewAllUser:public helium::Application {
  void run() {
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
    SensorCalibSubWindow sensorCalibWindow(guideState.sensors,guideState.pref.sensorGui,guideState.settings.colors,&guideMotionManagement.eh,guideState.conn,config["sensors"]);


    ModelWindowArray modelWindowArray(guideState);
    TeachViewerWindow teachViewerWindow(sensorCalibWindow,guideTeachWindow,modelWindow,NULL);

    teachViewerWindow.updateSensorPlot.connect(modelWindowArray.callOnFileChange);
    teachViewerWindow.changeAPid.connect(modelWindowArray.callOnIdChange);


    Gtk::Main::run(teachViewerWindow); 
  }
};

int main(int argc,char** argv) {
  ViewAllUser g;

  
  if(!Glib::thread_supported()){
    Glib::thread_init();
  }
  helium::GtkSignalling::init();
  gdk_threads_enter();
  g.launch(argc,argv,"conf/guide.neony.xml",helium::Application::AS_RUN);
  gdk_threads_leave();  
  return 0;
}
