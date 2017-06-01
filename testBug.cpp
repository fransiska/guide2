//#include "guideTeachWindow.h"
#include "guideDescription.h"
//#include "guideTeachList.h"
//#include "postureCaptureWindow.h" 
#include "guideState.h"
#include <helium/robot/application.h>
#include <helium/system/cellDescription.h>




namespace helium{

  /*class Posture:public Array<double>{
  };

  namespace cfg{
    namespace guide{
      class Postures:public Array<Posture>{
      };
    }
    }*/

  /*    template<> 
  class DefDescr<Posture>:public Description<Posture>{

    
  public:    
    
    DefDescr(const DefDescr& c):Description(c.obj){
      std::cout<<"FUCK!"<<std::endl;
    }

    ~DefDescr(){
      std::cout<<"Description of posture @"<<this<< " gone "<<std::endl;
    }

    DefDescr(helium::Posture& s):Description<helium::Posture>(s){      
    }



    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
            std::cout<<"new Description of posture @"<<this<<std::endl;
          std::cout<<"@"<<this<<"pushing";
	  members.push_back(scalarMapping(d(obj.name),"name"));
    members.push_back(scalarMapping(static_cast<Array<double>&>(obj),"angle"));
    
    }
  };


  template<>
    class DefDescr<cfg::guide::Postures>:public Description<cfg::guide::Postures>{
  public:
    DefDescr(cfg::guide::Postures& s):Description<cfg::guide::Postures>(s){
    }
      void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
	    members.push_back(psvectMapping(vd(obj),"postures","posture","guide"));
      }
  };  
  */



}

using namespace helium;
//class GB:public Application{

//  void run(){
    //cfg::guide::Postures p;
    //dimport(p,mem);

    /*cfg::guide::Postures p;
      dimport(p,mem);*/

    //GuideState guideState;  
    //dimport(guideState,mem);
    /*std::auto_ptr<helium::JointPosConverter> pc;
    {
      helium::KnownPosNormalizer knownPosNormalizer(mem);
      pc.reset(knownPosNormalizer.getNewJointPosConverter()); 
    }
    helium::MotionParser motionParser(guideState.pref.robot);  
    helium::GtkExceptionHandling gtkEh;
    GuideMotionManagement guideMotionManagement(guideState,gtkEh,*pc,motionParser);
    PostureCaptureWindow* postureCaptureWindow=NULL;
    GuideTeachWindow gw(guideMotionManagement,postureCaptureWindow);
    */
//  }

//};


int main(int argc,char**argv){
  /*std::string file="/tmp/teachList";
  cfg::guide::Sensors psensors;
  GuideTeachList gtl(file,psensors);
  std::vector<TeachList> tl;
  gtl.loadTeachList(file.c_str(),tl);*/

  

  /*GB gb;
    gb.launch(argc,argv,"conf/guide.xml",helium::Application::AS_RUN);*/


  cfg::guide::Postures ps;

  VectAllocator<DefDescr<Posture>,cfg::guide::Postures>  alloc(ps);
  Array<DefDescr<Posture>,array::RESIZABLE,
	VectAllocator<DefDescr<Posture>,cfg::guide::Postures> > a(alloc,0);
  ps.resize(3);
  a.resize(3);

}
