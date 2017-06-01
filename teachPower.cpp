/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#undef max
#endif

#include "teachPower.h"
using namespace std;

// PowerOffCondition::PowerOffCondition(std::vector<int> sensorA, std::vector<int> sensorB, std::vector<int> power):
//   active(true),powerEffect(power)
// {
//   sensorCondition.resize(2);
//   sensorCondition[0] = sensorA;
//   sensorCondition[1] = sensorB;
// }
// 
// PowerOffCondition::PowerOffCondition():active(true) {
//   sensorCondition.resize(2);
// }
// 
// bool PowerOffCondition::verifyCondition(const helium::Array<double> &sensorRatios) {
//   for(size_t i=0;i<2;i++) {
//     bool orValue = false;
//     for(size_t j=0;j<sensorCondition[i].size();j++) {
//       int idx = sensorCondition[i][j];
//       if(sensorRatios[idx] > 0.2) {
// 	orValue = true;
// 	break;
//       }
//     }
//     if(!orValue) return true;
//   }
//   return false;
// }
// 
// void PowerOffCondition::setPower(cfg::guide::Joints &joints, bool b) {
//   //static bool prevb = b;
//   //if(b == prevb) return; // avoid double printing/assuring same things
//   if(active == b) return; // avoid 
// 
//   if(!b) //only when before turning off
//     setPrevPower(joints);
// 
//   for(size_t i=0;i<powerEffect.size();i++) {
//     if(!b || (b && prevPower[powerEffect[i]])) { //turning off: turn off all, turning on: check prevPower
//       joints[powerEffect[i]].jointPower.assure(b);
//     }
//   }
//   //prevb = b;
//   active = b;
// }
// 
// TeachPower::TeachPower(GuideMotionManagement &pgmm):
//   gmm(pgmm),teachActive(false),
//   sensorRatios(gmm.gs.sensors.size()),
//   callUpdateRatios(this,gmm.gs.sensors.size())
// {
//   powerOffConditions.resize(5);
// 
//   helium::zero(sensorRatios);
// 
//   //head
//   powerOffConditions[0].sensorCondition[0].push_back(79); 
//   powerOffConditions[0].sensorCondition[0].push_back(80); 
//   powerOffConditions[0].sensorCondition[0].push_back(89); 
// 
//   powerOffConditions[0].sensorCondition[1].push_back(81);
//   powerOffConditions[0].sensorCondition[1].push_back(82);
//   powerOffConditions[0].sensorCondition[1].push_back(87);
// 
//   powerOffConditions[0].powerEffect.push_back(0);
//   powerOffConditions[0].powerEffect.push_back(1);
// 
// 
// 
//   //hand right
//   powerOffConditions[1].sensorCondition[0].push_back(52); 
//   powerOffConditions[1].sensorCondition[0].push_back(53); 
// 
//   powerOffConditions[1].sensorCondition[1].push_back(56);
//   powerOffConditions[1].sensorCondition[1].push_back(57);
// 
//   powerOffConditions[1].powerEffect.push_back(2);
//   powerOffConditions[1].powerEffect.push_back(4);
//   powerOffConditions[1].powerEffect.push_back(6);
// 
// 
// 
// 
// 
//   //hand left
//   powerOffConditions[2].sensorCondition[0].push_back(41); 
//   powerOffConditions[2].sensorCondition[0].push_back(42); 
// 
//   powerOffConditions[2].sensorCondition[1].push_back(45);
//   powerOffConditions[2].sensorCondition[1].push_back(46);
// 
//   powerOffConditions[2].powerEffect.push_back(3);
//   powerOffConditions[2].powerEffect.push_back(5);
//   powerOffConditions[2].powerEffect.push_back(7);
// 
// 
// 
// 
// 
// 
//   //leg right
//   powerOffConditions[3].sensorCondition[0].push_back(33); 
//   powerOffConditions[3].sensorCondition[0].push_back(29); 
// 
//   powerOffConditions[3].sensorCondition[1].push_back(32);
//   powerOffConditions[3].sensorCondition[1].push_back(28);
// 
//   powerOffConditions[3].powerEffect.push_back(8);
//   powerOffConditions[3].powerEffect.push_back(10);
//   powerOffConditions[3].powerEffect.push_back(12);
//   powerOffConditions[3].powerEffect.push_back(14);
//   powerOffConditions[3].powerEffect.push_back(16);
//   powerOffConditions[3].powerEffect.push_back(18);
//   powerOffConditions[3].powerEffect.push_back(20);
// 
// 
// 
// 
//   //leg left
//   powerOffConditions[4].sensorCondition[0].push_back(14); 
//   powerOffConditions[4].sensorCondition[0].push_back(10); 
// 
//   powerOffConditions[4].sensorCondition[1].push_back(15);
//   powerOffConditions[4].sensorCondition[1].push_back(11);
// 
//   powerOffConditions[4].powerEffect.push_back(9);
//   powerOffConditions[4].powerEffect.push_back(11);
//   powerOffConditions[4].powerEffect.push_back(13);
//   powerOffConditions[4].powerEffect.push_back(15);
//   powerOffConditions[4].powerEffect.push_back(17);
//   powerOffConditions[4].powerEffect.push_back(19);
//   powerOffConditions[4].powerEffect.push_back(21);
// 
// 
//   sensorChecks.resize(powerOffConditions.size());
//   for(size_t i=0;i<powerOffConditions.size();i++) {
//     for(size_t j=0;j<powerOffConditions[i].sensorCondition[0].size();j++)
//       sensorChecks[i].insert(powerOffConditions[i].sensorCondition[0][j]);
//     for(size_t j=0;j<powerOffConditions[i].sensorCondition[1].size();j++)
//       sensorChecks[i].insert(powerOffConditions[i].sensorCondition[1][j]);
//     powerOffConditions[i].prevPower.resize(powerOffConditions[i].powerEffect.size());
//   }
// }
// 
// void TeachPower::setTeachActive(bool b) {
//   if(b && !teachActive) {
//     teachActive = true;
//     for(size_t i=0;i<gmm.gs.sensors.size();i++) {
//       gmm.gs.sensors[i].rawValue.connect(callUpdateRatios[i]);
//     }
//   }
//   else if(teachActive){
//     teachActive = false;
//     for(size_t i=0;i<gmm.gs.sensors.size();i++) {
//       gmm.gs.sensors[i].rawValue.disconnect(callUpdateRatios[i]);
//     }
//     //reset all active status
//     for(size_t i=0;i<5;i++)
//       powerOffConditions[i].active = true;
//   }
// }
// 
// void PowerOffCondition::setPrevPower(const cfg::guide::Joints &joints) {
//   for(size_t i=0;i<joints.size();i++) {
//     prevPower[i] = joints[i].jointPower.get();
//   }
// }
// 
// void TeachPower::updateSensorRatio(int value,int id) {
//   //no need to do anything for sensors not in list
//   int find = -1;
//   for(size_t i=0;i<powerOffConditions.size();i++) {
//     //std::set<int>::iterator found = sensorChecks[i].find(id);
//     if(sensorChecks[i].find(id) != sensorChecks[i].end())
//       find = i;
//   }
//   if(find == -1)
//     return;
// 
//   sensorRatios[id] = gmm.gs.sensors[id].getRatio(value); //get ratio of sensors
//   powerOffConditions[find].setPower(gmm.gs.joints,powerOffConditions[find].verifyCondition(sensorRatios)); //set power according to found or not
// }

