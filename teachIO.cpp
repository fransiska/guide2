/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "teachIO.h"
#include <helium/math/filter.h>
#include <helium/robot/postureDescription.h>

TeachPosture::TeachPosture(){}

TeachPosture::TeachPosture(const TeachPosture &t):
  time(t.time),
  posture(t.posture),
  filteredGyro(t.filteredGyro),
  rototra(t.rototra)
{
  for(size_t i=0;i<t.gyro.size();i++) {
    gyro.push_back(t.gyro[i]);
  }
  for(size_t i=0;i<t.touch.size();i++) {
    touch.push_back(t.touch[i]);
  }
}

TeachPosture::TeachPosture(helium::Time ptime, size_t postureSize):
  time(ptime),
  posture(postureSize)
{  
}
TeachPosture::TeachPosture(helium::Time ptime, size_t postureSize, const TeachSensorLogList &ptouch, const TeachSensorLogList &pgyro):
  time(ptime),posture(postureSize),touch(ptouch),gyro(pgyro),filteredGyro(gyro[0].second.size()),rototra(16)
{
  helium::setAll(posture,0);
}

TeachPostureInput::TeachPostureInput(){}

TeachSensorLogList::TeachSensorLogList(const TeachSensorLogList &l) {
  for (TeachSensorLogList::const_iterator it=l.begin(); it != l.end(); it++) {
    push_back(*it);
  }
}

TeachSensorLogVector::TeachSensorLogVector(const TeachSensorLogList&t) {
  for (TeachSensorLogList::const_iterator it=t.begin(); it != t.end(); it++) {
    push_back(*it);
  }
}

const helium::Array<double>& TeachPostureInput::getGyro() const {
  return rototra;
}

TeachPostureInput::TeachPostureInput(const TeachPostureInput &t):
  TeachPosture(t),
  touchedSensor(t.touchedSensor),
  ratio(t.ratio),
  touchMax(t.touchMax),
  touchMin(t.touchMin),
  gyroCalib(t.gyroCalib)
{
  if(ratio.size() != touchMax.size()) {
    ratio.resize(touchMax.size());
  }
  calcTouchRatio();
  calcRototra();
}

TeachPostureInput::TeachPostureInput(helium::Time time,size_t postureSize,size_t touchSize, const helium::GyroCalib &pgyroCalib, const TeachSensorLogList& ptouch, const TeachSensorLogList& pgyro) :
  TeachPosture(time,postureSize,ptouch,pgyro),  
  touchedSensor(""),
  ratio(touchSize),
  touchMax(touchSize),
  touchMin(touchSize),
  gyroCalib(pgyroCalib)
{
  calcRototra();
  helium::setAll(touchMax,0);
  helium::setAll(touchMin,0);
}

TeachIO::TeachIO(){
}

TeachIO::TeachIO(const TeachIO &t):
  input(t.input),
  output(t.output)
{
}

TeachIO::TeachIO(const TeachPostureInput &p):
  input(p),
  output(0,p.posture.size())
{
}

void TeachIO::setOutput(const TeachPosture &o) {
  output = o;
}

void TeachPostureInput::calcTouchRatio() {
  for(size_t i=0;i<ratio.size();i++) {
    if (touchMax[i]==touchMin[i]) ratio[i] = 0.5;
    else ratio[i] = (double)(touch[0].second[i]-touchMin[i])/(touchMax[i]-touchMin[i]);
  }
}

void TeachPostureInput::calcRototra() {
  //filter gyro data
  if(gyro.size()) {
    for(size_t i=0;i<gyro[0].second.size();i++) {
      helium::MMFilter filter(gyro.size());
      for(size_t j=0;j<gyro.size();j++) {
	filter.push(gyro[j].second[i]);
      }
      filteredGyro[i] = filter.getValue();
    }
  
    //get rototra
    gyroCalib.getRototranslation(rototra,filteredGyro);
  }
  else {
    helium::setAll(rototra,0);
    helium::setAll(filteredGyro,0);
  }
}

namespace helium {
  void DefDescr<TeachPosture>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
    members.push_back(scalarMapping(d(obj.time),IDPath("time",".")));
    members.push_back(psvectMapping(vd(obj.touch),"touch","log"));
    members.push_back(psvectMapping(vd(obj.gyro),"gyro","log"));
    members.push_back(scalarMapping(d(obj.filteredGyro),IDPath("filteredGyro",".")));
    members.push_back(scalarMapping(d(obj.rototra),IDPath("rototra",".")));
    members.push_back(scalarMapping(d(obj.posture),IDPath("posture",".")));
  }
  void DefDescr<TeachPostureInput>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
    members.push_back(scalarMapping(d(obj.time),IDPath("time",".")));
    members.push_back(psvectMapping(vd(obj.touch),"touch","log"));
    members.push_back(psvectMapping(vd(obj.gyro),"gyro","log"));
    members.push_back(scalarMapping(d(obj.filteredGyro),IDPath("filteredGyro",".")));
    members.push_back(scalarMapping(d(obj.rototra),IDPath("rototra",".")));
    members.push_back(scalarMapping(d(obj.posture),IDPath("posture",".")));
    members.push_back(scalarMapping(obj.touchedSensor,IDPath("touchedSensor",".")));
    members.push_back(scalarMapping(d(obj.touchMin),IDPath("calib.touch.min",".")));
    members.push_back(scalarMapping(d(obj.touchMax),IDPath("calib.touch.max",".")));
    members.push_back(scalarMapping(d(obj.gyroCalib),IDPath("calib.gyro",".")));
  }
  void DefDescr<TeachIO>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
    members.push_back(scalarMapping(d(obj.input),IDPath("input",".")));
    members.push_back(scalarMapping(d(obj.output),IDPath("output",".")));
  }
  void DefDescr<TeachIOs >::getMapMembers(GenericIDPath &hopName,std::vector<NodeMapping*> &members){
    members.push_back(psvectMapping(vd(obj),"instructions","instruction"));
  }
};

std::ostream& operator <<(std::ostream& o,const TeachPosture &t) {
  std::stringstream ss;
  ss << t.time;
  ss << t.touch;
  ss << t.gyro;
  ss << t.posture;
  return o << ss.str();
}

std::ostream& operator <<(std::ostream& o,const TeachPostureInput &t) {
  std::stringstream ss;
  ss << t.time;
  ss << t.touch;
  ss << t.gyro;
  ss << t.posture;
  ss << " " << t.touchedSensor;
  return o << ss.str();
}

std::ostream& operator <<(std::ostream& o,const TeachIO &teachIO) {
  std::stringstream ss;
  ss << " " << teachIO.input;
  ss << " " << teachIO.output;
  return o << ss.str();
}

std::ostream& operator <<(std::ostream& o,const TeachSensorLogList&t) {
  for (TeachSensorLogList::const_iterator it=t.begin(); it != t.end(); it++) {
    o << " " << (*it).first << " " << (*it).second;
  }
  return o;
}
std::ostream& operator <<(std::ostream& o,const TeachSensorLogVector &t) {
  for (TeachSensorLogVector::const_iterator it=t.begin(); it != t.end(); it++) {
    o << " " << (*it).first << " " << (*it).second;
  }
  return o;
}
