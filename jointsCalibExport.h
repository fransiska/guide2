/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef JOINTSCALIBEXPORT
#define JOINTSCALIBEXPORT
#include <app/guide2/guideDescription.h>

///description to save gyro calib
namespace helium{

class JointCalibExport:public Description<cfg::guide::Joint>{
public:
  JointCalibExport(cfg::guide::Joint& s):Description<cfg::guide::Joint>(s){
  }
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.calib),"calib")); 
  }
};

class JointsCalibExport:public Description<cfg::guide::Joints>{
public:
  JointsCalibExport(cfg::guide::Joints& s):Description<cfg::guide::Joints>(s){
  }
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(psvectMapping(cvd<JointCalibExport>(obj),"motors","motor","robot")); //import the array elements first, so we get the size
    members.push_back(hexMapping(obj.defaultCalib.calibMin,IDPath("robot.motors.calibmin","."))); //import the array elements first, so we get the size
    members.push_back(hexMapping(obj.defaultCalib.calibMax,IDPath("robot.motors.calibmax","."))); //import the array elements first, so we get the size
  }
};
}//helium

#endif
