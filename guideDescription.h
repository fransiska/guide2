/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDE_DESCRITION
#define GUIDE_DESCRITION


#include <helium/system/cellDescription.h>
#include <helium/system/hexMapping.h>
#include "guideState.h"
#include "guideLogic.h"
#include <helium/robot/postureDescription.h>

/*
using helium::Description;
using helium::GenericIDPath;
using helium::NodeMapping;
using helium::IDPath;
*/

namespace helium{  
  
  template<> 
  class DefDescr<Point2D>:public Description<Point2D>{  
  public:
    DefDescr(Point2D& loc):Description<Point2D>(loc){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  /// template for reading from xml
  template<typename T,T V> 
  class DefDescr<DefPairValue<T,V> >:public Description<DefPairValue<T,V> >{
  public:
    DefDescr(DefPairValue<T,V>& loc):Description<DefPairValue<T,V> >(loc){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
    members.push_back(fscalarMapping<SILENTFAIL>(this->obj.first,"default"));
    members.push_back(fscalarMapping<SILENTFAIL>(this->obj.second,"current"));
    }
  };

  template<> 
  class DefDescr<cfg::guide::Pedal>:public Description<cfg::guide::Pedal>{  
  public:
    DefDescr(cfg::guide::Pedal& p):Description<cfg::guide::Pedal>(p){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Calib>:public Description<cfg::guide::Calib>{
  public:
    DefDescr(cfg::guide::Calib& s):Description<cfg::guide::Calib>(s){
    }    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Joint>:public Description<cfg::guide::Joint>{
  public:
    DefDescr(cfg::guide::Joint& j):Description<cfg::guide::Joint>(j){
    }
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    void onInitializationFinished(helium::Memory& mem);
  };
  

  
  template<>
  class DefDescr<MotionInfo>:public Description<MotionInfo>{
  public:
    DefDescr(MotionInfo& m):Description<MotionInfo>(m){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };
  

  template<> 
  class DefDescr<cfg::guide::Joints>:public Description<cfg::guide::Joints>{   
  public:
    DefDescr(cfg::guide::Joints& j):Description<cfg::guide::Joints>(j){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    void onInitializationFinished(helium::Memory& mem);
  };


  template<> 
  class DefDescr<cfg::guide::Size>:public Description<cfg::guide::Size>{
  public:
    DefDescr(cfg::guide::Size& s):Description<cfg::guide::Size>(s){
    }
    //void import(Memory& mem,const IDPath &path);
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Colors>:public Description<cfg::guide::Colors>{
  public:
    DefDescr(cfg::guide::Colors& c):Description<cfg::guide::Colors>(c){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };
  
  template<> 
  class DefDescr<cfg::guide::Sizes>:public Description<cfg::guide::Sizes>{
  public:
    DefDescr(cfg::guide::Sizes& t):Description<cfg::guide::Sizes>(t){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Settings>:public Description<cfg::guide::Settings>{
  public:    
    DefDescr(cfg::guide::Settings& s):Description<cfg::guide::Settings>(s){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };


  //20120113
  template<> 
  class DefDescr<cfg::guide::ImageInfo>:public Description<cfg::guide::ImageInfo>{
  public:    
    DefDescr(cfg::guide::ImageInfo& s):Description<cfg::guide::ImageInfo>(s){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };
  template<> 
  class DefDescr<cfg::guide::SensorGui>:public Description<cfg::guide::SensorGui>{
  public:    
    DefDescr(cfg::guide::SensorGui& s):Description<cfg::guide::SensorGui>(s){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Preferences>:public Description<cfg::guide::Preferences>{
  public:    
    DefDescr(cfg::guide::Preferences& s):Description<cfg::guide::Preferences>(s){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Limb>:public Description<cfg::guide::Limb>{
  public:    
    DefDescr(cfg::guide::Limb& l):Description<cfg::guide::Limb>(l){      
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };


  //added Limbs 20110616
  template<> 
  class DefDescr<cfg::guide::Limbs>:public Description<cfg::guide::Limbs>{
  public:
    DefDescr(cfg::guide::Limbs& l):Description<cfg::guide::Limbs>(l){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };  


  //added Sensors 20111007
  template<> 
  class DefDescr<cfg::guide::Sensor>:public Description<cfg::guide::Sensor>{
  public:    
    DefDescr(cfg::guide::Sensor& s):Description<cfg::guide::Sensor>(s){      
    }

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);

  };


  template<> 
  class DefDescr<cfg::guide::Sensors>:public Description<cfg::guide::Sensors>{
  public:
    DefDescr(cfg::guide::Sensors& s):Description<cfg::guide::Sensors>(s){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    void onInitializationFinished(helium::Memory& mem);
  };  

  template<> 
  class DefDescr<cfg::guide::Gyro>:public Description<cfg::guide::Gyro>{
  public:
    DefDescr(cfg::guide::Gyro& s):Description<cfg::guide::Gyro>(s){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    void onInitializationFinished(helium::Memory& mem);
  };  

  /*
  template<> 
  class DefDescr<helium::Posture>:public Description<helium::Posture>{
  public:    
    DefDescr(helium::Posture& s):Description<helium::Posture>(s){      
      // std::cout<<"new Description of posture @"<<this<<std::endl;
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };
  */

  template<> 
  class DefDescr<cfg::guide::Postures>:public Description<cfg::guide::Postures>{
  public:
    DefDescr(cfg::guide::Postures& s):Description<cfg::guide::Postures>(s){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };  

  template<>
  class DefDescr<GuideState>:public Description<GuideState>{
    std::string host;
    int port;
  public:
    DefDescr(GuideState& c):Description<GuideState>(c){      
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    void onInitializationFinished(Memory& mem);
  };

  /*
  template<>
  class DefDescr<GuideLogic::JointLogic>:public  Description<GuideLogic::JointLogic>{
  public:
    DefDescr(GuideLogic::JointLogic& c):Description<GuideLogic::JointLogic>(c){      
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };


  template<>
  class DefDescr<GuideLogic>:public  Description<GuideLogic>{
    public:
    DefDescr(GuideLogic& c):Description<GuideLogic>(c){      
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };
  */

  /// 20130708 Descriptor for collision detection class
  template<> 
  class DefDescr<cfg::guide::Coldet>:public Description<cfg::guide::Coldet>{  
  public:
    DefDescr(cfg::guide::Coldet& loc):Description<cfg::guide::Coldet>(loc){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Teach::Power>:public Description<cfg::guide::Teach::Power>{  
  public:
    DefDescr(cfg::guide::Teach::Power& loc):Description<cfg::guide::Teach::Power>(loc){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Teach::Touch>:public Description<cfg::guide::Teach::Touch>{  
  public:
    DefDescr(cfg::guide::Teach::Touch& loc):Description<cfg::guide::Teach::Touch>(loc){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<> 
  class DefDescr<cfg::guide::Teach>:public Description<cfg::guide::Teach>{  
  public:
    DefDescr(cfg::guide::Teach& loc):Description<cfg::guide::Teach>(loc){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

}


#endif
