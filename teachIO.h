/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef TEACHIO
#define TEACHIO
#include <helium/robot/frame.h>
#include <helium/robot/gyroStructures.h>
#include <helium/system/cellDescription.h>


typedef std::pair<helium::Time, helium::Array<int> > TeachSensorLog;

struct TeachSensorLogList : public std::list<TeachSensorLog> {
  TeachSensorLogList() {}
  TeachSensorLogList(const TeachSensorLogList &l);
  void push(helium::Time t, helium::Array<int>);
  friend std::ostream& operator <<(std::ostream& o,const TeachSensorLogList&t);
};

struct TeachSensorLogVector : public std::vector<TeachSensorLog> {
  TeachSensorLogVector() {}
  TeachSensorLogVector(const TeachSensorLogList&t);
  friend std::ostream& operator <<(std::ostream& o,const TeachSensorLogVector &t);
};

struct TeachPosture {
  long long time;
  helium::Posture posture;
  TeachSensorLogVector touch;
  TeachSensorLogVector gyro;  
  helium::Array<double> filteredGyro;
  helium::Array<double> rototra;

  TeachPosture(); ///<for vector resize of TeachIO
  TeachPosture(const TeachPosture&);
  TeachPosture(helium::Time ptime,size_t postureSize); ///<for setting up output during acquire process
  TeachPosture(helium::Time ptime, size_t postureSize, const TeachSensorLogList& ptouch, const TeachSensorLogList&pgyro);
  friend std::ostream& operator <<(std::ostream& o,const TeachPosture &t);
};

struct TeachPostureInput : public TeachPosture {
  std::string touchedSensor; 
  helium::Array<double> ratio;
  helium::Array<int> touchMax;
  helium::Array<int> touchMin;
  helium::GyroCalib gyroCalib;

  TeachPostureInput(); ///<for vector resize of TeachIO
  TeachPostureInput(const TeachPostureInput &t); 
  //TeachPostureInput(helium::Time time,size_t postureSize,size_t touchSize, const helium::GyroCalib &gyroCalib);
  TeachPostureInput(helium::Time time,size_t postureSize,size_t touchSize, const helium::GyroCalib &gyroCalib,const TeachSensorLogList& ptouch, const TeachSensorLogList&pgyro);
  void calcTouchRatio();
  void calcRototra();
  const helium::Array<double>& getGyro() const;
  friend std::ostream& operator <<(std::ostream& o,const TeachPostureInput &t);
};

class TeachIO { 
public:
  TeachPostureInput input;
  TeachPosture output;

  TeachIO(); ///<for vector resize
  TeachIO(const TeachIO &t);
  TeachIO(const TeachPostureInput& p);

  void setOutput(const TeachPosture &o);

  friend std::ostream& operator <<(std::ostream& o,const TeachIO &teachIO);
};

typedef std::vector<TeachIO> TeachIOs;

namespace helium {
  template<>
  class DefDescr<TeachSensorLog >:public Description<TeachSensorLog >{
  public:
    DefDescr(TeachSensorLog & t):Description<TeachSensorLog >(t){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
      members.push_back(scalarMapping(d(obj.first),IDPath("time",".")));
      members.push_back(scalarMapping(d(obj.second),IDPath("value",".")));
    }
  };

  template<>
  class DefDescr<TeachPosture>:public Description<TeachPosture>{
  public:
    DefDescr(TeachPosture& t):Description<TeachPosture>(t){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<>
  class DefDescr<TeachPostureInput>:public Description<TeachPostureInput>{
  public:
    DefDescr(TeachPostureInput& t):Description<TeachPostureInput>(t){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<>
  class DefDescr<TeachIO>:public Description<TeachIO>{
  public:
    DefDescr(TeachIO& t):Description<TeachIO>(t){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<>
    class DefDescr<TeachIOs>:public Description<TeachIOs > {
  public:
    DefDescr<TeachIOs >(TeachIOs & r):Description<TeachIOs >(r){
    }
    void getMapMembers(GenericIDPath &hopName,std::vector<NodeMapping*> &members);  
  };

};

#endif
