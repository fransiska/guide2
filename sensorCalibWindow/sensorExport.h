/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include <helium/system/cellDescription.h>

namespace helium {

class SensorExport:public Description<cfg::guide::Sensor>{
public:
  SensorExport(cfg::guide::Sensor& s):Description<cfg::guide::Sensor>(s){
  }
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.board),"board"));
    members.push_back(scalarMapping(d(obj.channel),"channel"));
    members.push_back(scalarMapping(d(obj.limit.first),"min"));
    members.push_back(scalarMapping(d(obj.limit.second),"max"));
    members.push_back(scalarMapping(d(obj.name),"name"));
  }
};

class SensorsExport:public Description<cfg::guide::Sensors>{
public:
  SensorsExport(cfg::guide::Sensors& s):Description<cfg::guide::Sensors>(s){
  }
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(psvectMapping(cvd<SensorExport>(obj),"sensors","sensor","robot"));
  }
};
}//helium
