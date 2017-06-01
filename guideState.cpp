/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "guideState.h"
#include <helium/xml/xmlLoader.h>
#include <helium/gtk/gtkSpool.h>
//#include <helium/robot/motorOffMode.h>
#include <helium/robot/motorSpooler.h>
//#include <helium/robot/sensorSpooler.h>


using namespace helium;
 

namespace cfg{
namespace guide{

  

  helium::ValueHub<int>& Calib::operator[](int i){
    switch((CalibType::Type)i) {
    case CalibType::MIN: return absLim.minv;
    case CalibType::MAX: return absLim.maxv;
    case CalibType::ZERO: 
    default:
      return absLim.zero;
    }
    
  }


  //helium::MoutPrinter<int,helium::msg::WARNING> spoolControlPrinter;
  Mutex spoolerMutex("gs::SpoolerMutex");




  Joint::Joint(): //frameValue(std::make_pair(helium::VALUEFRAME,0)),
		  //jointEnable(true),
    motorPosition(helium::RelMotorSign::INVALIDPOS),
    motorRawPosition(helium::AbsMotorSign::INVALIDPOS), 
    motorMode(helium::MotorSpooler<helium::RelMotorSign>::NOMOTORMODE.getMotorModeId()),
    motorNormProperties(Void()),
    motorRawProperties(Void()),
    remoteLinearNormTarget(Void()),
    remoteLinearRawTarget(Void()),
    mirrorId(-1),
    frameValue(std::make_pair(helium::VALUEFRAME,0)),
    desiredTarget(NAN),
    displayedTarget(NAN),
    absTarget(helium::AbsMotorSign::INVALIDPOS),
    calibCurrentPosition(helium::AbsMotorSign::INVALIDPOS),     
    //jointPower(true),	    
    jointPower(false),
    jointTemp(helium::motor::Interface::INVALIDPROP),
    jointState(helium::UNKNOWNHWSTATE),
    jointMonitor(helium::NOHWMON)
  {
    syncState[0] = cfg::guide::SyncType::DISCONNECT;
    syncState[1] = cfg::guide::SyncType::DISCONNECT;
    prevSyncState[0] = cfg::guide::SyncType::NUM;
    prevSyncState[1] = cfg::guide::SyncType::NUM;
    //frameValue.set<GuideEvent::INITIALIZEJOINT>(std::make_pair(helium::VALUEFRAME,0));
  }

  
  double Joint::getLimit(CalibType::Type c) { 
	if(c == CalibType::MIN)
	  return calib.relLim.minv; 
	if(c == CalibType::MAX)
	  return calib.relLim.maxv; 
        return 0;
      }


  bool Joint::isOverLimit(double b) { //angle
	if(b < calib.relLim.minv || b > calib.relLim.maxv) 
	  return true;
	return false;
      }

  
  bool Joint::isOverLimit(int b) { //abs
	if(b < calib.absLim.minv || b > calib.absLim.maxv) 
	  return true;
	return false;
      }


  bool Joint::validateAbsTarget() {
	if(absTarget.get() < calib.absLim.minv || absTarget.get() > calib.absLim.maxv) 
	  return false;
	return true;
      }
  void Joint::validateValue(double &v){ //DEBUG
	if(v > calib.relLim.maxv)
	  v = calib.relLim.maxv;
	else if(v < calib.relLim.minv)
	  v = calib.relLim.minv;
      }

  void Joint:: getCalibLimit(CalibType::Type t,int &min, int &max)  {
	min = -20000;
	max = 20000;
	switch(t) {
	case CalibType::ZERO: min = calib.absLim.minv.get(); max = calib.absLim.maxv.get(); break;
	case CalibType::MIN: max = calib.absLim.zero.get(); break;
	case CalibType::MAX: min = calib.absLim.zero.get(); break;
	}
      }

 void Joint::setCalib(CalibType::Type t,int c) {
	//std::cout << "setcalibtype " << t << " " << c<< std::endl;



	switch(t) {
	case CalibType::ZERO:
	  calib.absLim.zero.assure(c);//(helium::limit(calib.absLim.minv.get(),calib.absLim.maxv.get(),c));
	  break;
	case CalibType::MIN: 
	  calib.absLim.minv.assure(c);//(std::min)(calib.absLim.zero.get(),c));
	  break;
	case CalibType::MAX:
	  calib.absLim.maxv.assure(c);//(std::max)(calib.absLim.zero.get(),c));	  
	  break;
	}

 }
  

  Joints::Joints():
    remoteLinearNormWholeTarget(Void()),
    multipleMotorMode(Void())
    //scPrinterConn(&spoolControlPrinter)
  {
  }

  void Joints::mirrorPosture() {
    //obtain mirror posture
    helium::Posture mirrorPost(this->size());
    for(size_t i=0;i<this->size();i++) {
      mirrorPost[i] = data[i].frameValue.get().second;
    }
    getMirrorPosture(mirrorPost.data);

    for(size_t i=0;i<this->size();i++) {
      data[i].frameValue.assure<GuideEvent::MIRRORJOINTS>(std::make_pair(helium::VALUEFRAME,mirrorPost[i]));
    }
    postureChange(GuideEvent::MIRRORPOSTURE);
  }

  void Joints::getMirrorPosture(double *pose) {
    helium::Posture originalPose(this->size());
    helium::copyMem(originalPose.data,pose,this->size());
    
    for(size_t i=0;i<this->size();i++) {
      if(data[i].mirrorId < 0) {
	continue;
      }
      pose[i] = originalPose[data[i].mirrorId]*-1;
      pose[i] = ((std::fabs(pose[i]) < std::pow((double)10,(int)-12))? 0: pose[i]);
    }
  }


  Pedal::Pedal(){
    checksum1=207;
    checksum2=2;
    rdownThreshold=145;
    rupThreshold=180;
    ldownThreshold=145;
    lupThreshold=180;
  }



 
  Sensors::Sensors()
  {
  }





}//guide
};//cfg

void GuideState::getCurrentPot(helium::Posture &p) {
  for(int i=0;i<(int)joints.size();i++) {
    p[i] = joints[i].displayedTarget.get();
  }

}

static const helium::motProp::ReadJointPropertyRequest potentioRequest(helium::motor::POTENTIOPOSITION,-1,false);

void GuideState::getActualCurrentPot(helium::Posture &p) {

  

  for(int i=0;i<(int)joints.size();i++) {
    joints[i].motorNormProperties.get(p[i],potentioRequest);
  }
}


bool GuideState::isRobotMoving() {
  for(int i=0;i<(int)joints.size();i++) {
    if(joints[i].displayedTarget.get() == helium::motor::Interface::MOVINGPOS)
      return true;
  }
  return false;
}

bool GuideState::isRobotMoving(double* posture) {
  for(size_t i=0;i<joints.size();i++) {
    if(posture[i] == helium::motor::Interface::MOVINGPOS)
      return true;
  }
  return false;
}

void GuideState::getCurrentRobotPosture(helium::Posture &p) {
  for(int i=0;i<(int)joints.size();i++) {
    p[i] = joints[i].desiredTarget.get();
  }

  /*
    static helium::Posture prev(joints.size());
    for(int i=0;i<(int)joints.size();i++) {
      p[i] = joints[i].displayedTarget.get();

      if(p[i] == helium::motor::Interface::INVALIDPOS && p[i] == helium::motor::Interface::FREEPOS)
	p[i] = prev[i];
      else {
	if(p[i] == helium::motor::Interface::MOVINGPOS)
	  p[i] = joints[i].desiredTarget.get();
	prev[i] = p[i];
      }
    }
    */
}

GuideState::GuideState():conn(helium::Connection()),view(cfg::guide::GuideView::NUM),errorn(0),verbose(false){
 
}


/*
cfg::guide::Teach::Limb::checkPower() {
  bool sensoron[2];
  sensoron[0] = sensoron[1] = false;
  for(size_t s=0;s<sensor.size();s++) {
    for(size_t ss=0;ss<sensor[s].size();ss++) {
      if(ratio[limb[l].sensor[0]]>0) {
	  sensoron[s] = true;
	  break;
	}	
      }
    }
}
*/
