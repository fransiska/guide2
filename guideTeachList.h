/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDETEACHLIST
#define GUIDETEACHLIST
#include <vector>
#include <helium/robot/frame.h>
#include <helium/vector/operations.h>
#include <tinyxml/tinyxml.h>
#include <helium/container/historyStack.h>
#include <helium/system/cellDescription.h>
#include "guideMotionManagement.h"
#include "teachIO.h"

class GuideTeachList {
  cfg::guide::Sensors &sensors;
  std::vector<TeachIO> teachList;
public:
  std::string &filename;
  GuideTeachList(GuideMotionManagement &gmm);//std::string &pfilename,cfg::guide::Sensors &psensors);
  const std::vector<TeachIO>& gettl() const;

  std::string getSensorString(int id) const;
  std::string getFilename() const {return filename;}
  void loadTeachList(const char* name, std::vector<TeachIO> &tl) const;
  void saveTeachList(const char* name) const;
  void saveTeachListTxt(std::string name) const;

  void addTeach(const helium::Array<int>& sensor, const helium::Posture& anglei, const helium::Array<int>& gyro, const helium::Posture& angleo);
  void acquireTeach(const helium::Array<int>& sensor, const helium::Posture& anglei,const helium::Array<int>& gyro,const helium::Time &tin);
  void acquiredTeach(const helium::Posture& angleo,const helium::Time &tout);
  void deleteTeach(const int id);
  void saveTeachList();
  void clear();
  void replaceTeachList(const std::vector<TeachIO> &tl);
  void appendTeachList(const std::vector<TeachIO> &tl);
};


#endif
