/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "sensorCalib.h"
#include <iostream>
#include <helium/vector/operations.h>

using namespace std;


SensorCalib::SensorCalib(cfg::guide::Sensors &psensors):
  sensors(psensors),
  oldRawValues(sensors.size()),
  rawTSensorValues(sensors.size())
{
  //zero(vals);
  zero(oldRawValues);
  zero(rawTSensorValues);
}

/* no need?? -> already covered with GtkProxy??
void SensorCalib::onRawTSensorUpdate(){
  //do not update if no new value is updated
  if(helium::equals(rawTSensorValues,oldRawValues))
    return;

  //update the old value
  oldRawValues = rawTSensorValues;


  for (size_t i=0;i<sensors.size();i++)
    vals[i]=sensors[i].getRatio(i,rawTSensorValues[i]);


  cout << "onRawTSensorUpdate" << endl;
  
  //DEBUG
  if (sdm.get_active_sensor_no()!=-1)
    currRawVal=rawTSensorValues[sdm.get_active_sensor_no()];
  else
    currRawVal=-1;
  


  //DEBUG: only selected sensor will highlight
  //int ID = sdm.get_active_sensor_no(); 
  //vals[ID]=scm.getRatio(ID,rawTSensorValues[ID]);
  //cout<<"update"<<endl;

  updateDispatcher();
}
*/
