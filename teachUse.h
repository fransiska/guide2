/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */



#ifndef TEACHUSE
#define TEACHUSE

/*
#include "guideMotionManagement.h"
#include "im.h" //temporar file for tbt demo


// struct PowerOffCondition {
//   bool active;
//   helium::Array<bool> prevPower;
//   void setPrevPower(const cfg::guide::Joints &joints);
//   std::vector<std::vector<int> > sensorCondition; ///ands of ors of a sensor set
//   std::vector<int> powerEffect; ///joint power to be turned off/on based on condition
//   PowerOffCondition(std::vector<int> sensorA, std::vector<int> sensorB, std::vector<int> power);
//   bool verifyCondition(const helium::Array<double> &sensorRatios); ///called every sensor updates
//   void setPower(cfg::guide::Joints &joints,bool b); ///set power
//   PowerOffCondition();
// };


class TeachUse {
  
  GuideMotionManagement &gmm;
  bool teachActive;
  ImFormat format;
  ImFilter filter;
  Im im;
  //helium::Time prevTime;
  bool isElapsedTimeValid;

  //std::vector<PowerOffCondition> powerOffConditions;
  helium::Array<double> sensorRatios;
  std::vector<int> sensorUse;
  //std::vector<std::set<int> > sensorChecks;

public:
  TeachUse(GuideMotionManagement &pgmm,const std::vector<TeachIO> &pteachList);
  void setTeachActive(bool b);
  void updateSensorRatio(int value,int id);
  void setSensors(std::vector<int>&);

private:
  typedef helium::ParamStaticConnReference<int,TeachUse,int,&TeachUse::updateSensorRatio> CallUpdateRatio;
  helium::Array<CallUpdateRatio,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ParamIdxAllocator<CallUpdateRatio,TeachUse*> > 
		>callUpdateRatios;

};
*/
#endif
