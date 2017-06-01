/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#include "teachUse.h"
#include <iostream>

using std::endl;
using std::cout;

/*

TeachUse::TeachUse(GuideMotionManagement &pgmm,const std::vector<TeachIO> &pteachList):
  gmm(pgmm),
  teachActive(false),
  format(gmm.gs.joints.size(),gmm.gs.sensors.size()),
  filter(format),
  im(pteachList),
  isElapsedTimeValid(false),
  sensorRatios(gmm.gs.sensors.size()),
  callUpdateRatios(this,gmm.gs.sensors.size())
{
}

void TeachUse::setTeachActive(bool b) {
  if(b && !teachActive) {
    teachActive = true;

    //imPoint insert from teachlist
    im.points.clear();
    for(size_t i=0;i<im.teachList.size();i++) {
      helium::Array<double> sen(gmm.gs.sensors.size());
      cout << "[" << i << "] ";
      for(size_t s=0;s<gmm.gs.sensors.size();s++) {
	sen[s] = gmm.gs.sensors[s].getRatio(im.teachList[i].sensor[s]);
	cout << sen[s] << " ";
      }
      cout << endl;
      im.points.push_back(ImPoint(im.teachList[i].input,sen,im.teachList[i].output));
      
    }

    for(size_t i=0;i<gmm.gs.sensors.size();i++) {
      gmm.gs.sensors[i].rawValue.connect(callUpdateRatios[i]);
    }
  }
  else if(teachActive){
    teachActive = false;
    isElapsedTimeValid = false;
    for(size_t i=0;i<gmm.gs.sensors.size();i++) {
      gmm.gs.sensors[i].rawValue.disconnect(callUpdateRatios[i]);
    }    
  }
}

void TeachUse::updateSensorRatio(int value,int id) {
  static helium::Time prevTime = helium::getSystemMillis();

  //newTime-prevTime
  helium::Time newTime = helium::getSystemMillis();
  //cout << "newTime " << newTime << endl;

  filter.setElapsedTime(isElapsedTimeValid?newTime-prevTime:0);

  //get current posture, pressed sensor
  ImInput status(format);
  gmm.gs.getCurrentRobotPosture(status.pos);   //status.getStatus(gmm);
  gmm.gs.getCurrentSensor(status.tsens);

  //set for final output
  helium::Posture v(gmm.gs.joints.size());
  gmm.gs.getCurrentRobotPosture(v);   //status.getStatus(gmm);
  //if(prevTime != newTime)
  //std::cout << "t " << newTime-prevTime << " " << id << " " << value << std::endl;

  //get the diff output
  helium::Posture dv(gmm.gs.joints.size());
  helium::zero(dv);
  std::vector<std::pair<int,double> > weights;
  im.getOutput(dv,status,&filter,true,&weights);

  //set final output
  for (size_t i=0;i<gmm.gs.joints.size();i++){
    v[i]+=dv[i];
    //make sure not exceeding limit limitExceeded|=!calMan->respectsLimitsRad(i,v[i]);
    gmm.gs.joints[i].validateValue(v[i]);
    //v[i]=calMan->getLimitedRadFromRad(i,v[i]);	
    //std::cout << v[i] << " ";
  }
  //std::cout  << std::endl;
  
    //rotate
  for (size_t i=0;i<gmm.gs.joints.size();i++) {
    gmm.gs.joints[i].desiredTarget.assure(v[i]);
    //cout << gmm.gs.joints[i].desiredTarget.get() << " ";
  }
  //std::cout  << std::endl;

  prevTime = newTime;
  isElapsedTimeValid = true; //avoid incorporating long pause between uses to affect touch response
}
*/









  /*
  helium::Posture diff = gtl.get().gettl()[id].output;
  helium::subn(diff,gtl.get().gettl()[id].input,gmm.gs.joints.size());
  cout << diff << endl;
  helium::mulScalarn(diff,0.2,gmm.gs.joints.size());
  
  helium::Posture newpos(gmm.gs.joints.size());
  
  for(size_t i=0;i<gmm.gs.joints.size();i++) {
    newpos[i] = gmm.gs.joints[i].displayedTarget.get();
  }
  
  helium::sumn(newpos,diff,gmm.gs.joints.size());
  for(size_t i=0;i<gmm.gs.joints.size();i++) {
    gmm.gs.joints[i].desiredTarget.assure(newpos[i]);      
  }
  */
