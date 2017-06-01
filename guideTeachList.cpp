/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "guideTeachList.h"
#include <helium/xml/xmlLoader.h>
#include "sensorCalibWindow/sensorExport.h"
#include <fstream>

using std::cout;
using std::endl;
using namespace helium;
using std::ofstream;
using std::stringstream;
GuideTeachList::GuideTeachList(GuideMotionManagement &gmm):
  //n(pn),lastid(0),
  sensors(gmm.gs.sensors),
  filename(gmm.gs.pref.tlFilename)
{
  size_t found = gmm.gs.pref.tlFilename.rfind(".tea");
  if(found != std::string::npos || found == gmm.gs.pref.tlFilename.length()-4) 
    gmm.gs.pref.tlFilename = gmm.gs.pref.tlFilename.substr(0,found);

  gmm.gs.pref.tlFilename.append(gmm.gs.settings.timestamp+".tea");
  if(gmm.gs.pref.tlFilename[0] == '~')
    gmm.gs.pref.tlFilename = Glib::get_home_dir() + gmm.gs.pref.tlFilename.substr(1,gmm.gs.pref.tlFilename.length());

  //cout <<"gtl " << filename<< endl;
}

void GuideTeachList::addTeach(const helium::Array<int>& sensor, const helium::Posture& anglei, const helium::Array<int>& gyro, const helium::Posture& angleo) {//with posture
  teachList.push_back(TeachIO (sensor,anglei,gyro,angleo));
  saveTeachList();
}

void GuideTeachList::deleteTeach(const int id) {
  teachList.erase(teachList.begin()+id);
  saveTeachList();
}

void GuideTeachList::loadTeachList(const char* name, std::vector<TeachIO> &tl) const {
  helium::XmlLoader xml;
  helium::Memory inmem;
  xml.loadDataAndModel(inmem,name);
  helium::dimport(tl,inmem);
}

void GuideTeachList::saveTeachList() {
  saveTeachList(filename.c_str());
}

void GuideTeachList::saveTeachList(const char* name) const {    
  helium::Memory outmem;    
  helium::XmlLoader out;    
  //std::vector<TeachIO> teachListCopy(teachList);
  helium::dexport(outmem,const_cast<std::vector<TeachIO>&>(teachList));    
  //helium::dexport(outmem,teachListCopy);    
  out.save(name,outmem);  
}

void GuideTeachList::saveTeachListTxt(std::string name) const {
  size_t found = name.rfind(".txt");
  if(found != std::string::npos || found == name.length()-4) 
    name = name.substr(0,found);

  ofstream myfile;
  stringstream ss;

  //sensor
  ss << name << ".sensor.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<teachList.size();i++) {
    myfile <<  teachList[i].sensor << "\n";
  }
  myfile.close();

  //gyro
  ss.str("");
  ss << name << ".gyro.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<teachList.size();i++) {
    myfile <<  teachList[i].gyro << "\n";
  }
  myfile.close();

  //pin
  ss.str("");
  ss << name << ".pin.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<teachList.size();i++) {
    myfile <<  teachList[i].input << "\n";
  }
  myfile.close();

  //pout
  ss.str("");
  ss << name << ".pout.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<teachList.size();i++) {
    myfile <<  teachList[i].output << "\n";
  }
  myfile.close();

  //tin
  ss.str("");
  ss << name << ".tin.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<teachList.size();i++) {
    myfile <<  teachList[i].tin << "\n";
  }
  myfile.close();

  //tout
  ss.str("");
  ss << name << ".tout.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<teachList.size();i++) {
    myfile <<  teachList[i].tout << "\n";
  }
  myfile.close();

  //sensor max
  ss.str("");
  ss << name << ".smax.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<teachList.size();i++) {
    myfile <<  teachList[i].sensorMax << "\n";
  }
  myfile.close();

  //sensor min
  ss.str("");
  ss << name << ".smin.out";
  myfile.open(ss.str().c_str());
  for(size_t i=0;i<teachList.size();i++) {
    myfile <<  teachList[i].sensorMin << "\n";
  }
  myfile.close();
}

//for sorting
bool sensorSort(std::pair<int,double>a,std::pair<int,double>b) {
  return (a.second<b.second);
}

std::string GuideTeachList::getSensorString(int id) const {
  std::vector<std::pair<int,double> > sensorV;
  for(size_t i=0;i<sensors.size();i++) {
    sensorV.push_back(std::make_pair(i,sensors[i].getRatio(teachList[id].sensor[i])));
  }
  std::sort(sensorV.begin(), sensorV.end(),sensorSort);
  std::reverse(sensorV.begin(), sensorV.end());
  //sensorV is sorted from biggest value

  //print top 5
  std::stringstream ss;
  for(size_t i=0;i<5;i++)
    if(sensorV[i].second>0)
      ss << sensors[sensorV[i].first].name << " ";

  return ss.str();  
}

void GuideTeachList::clear() {
  teachList.clear();
  saveTeachList();
}

const std::vector<TeachIO>& GuideTeachList::gettl() const {
  return teachList;
}

void GuideTeachList::replaceTeachList(const std::vector<TeachIO> &tl) {
  teachList = tl;
  saveTeachList();
}

void GuideTeachList::appendTeachList(const std::vector<TeachIO> &tl) {
  for(size_t i=0;i<tl.size();i++) {
    teachList.push_back(tl[i]);
  }
  saveTeachList();
}

void GuideTeachList::acquireTeach(const helium::Array<int>& sensor, const helium::Posture& anglei,const helium::Array<int>& gyro,const helium::Time& tin) {
  helium::Array<int> sensorMax(sensors.size());
  helium::Array<int> sensorMin(sensors.size());
  for(size_t i=0;i<sensors.size();i++) {
    sensorMin[i] = sensors[i].limit.first;
    sensorMax[i] = sensors[i].limit.second;
  }
    
  teachList.push_back(TeachIO (sensor,anglei,gyro,sensorMin,sensorMax,tin));
}

void GuideTeachList::acquiredTeach(const helium::Posture& angleo,const helium::Time& tout) {
  teachList[teachList.size()-1].output = angleo;
  teachList[teachList.size()-1].tout = tout;
  saveTeachList();
}
