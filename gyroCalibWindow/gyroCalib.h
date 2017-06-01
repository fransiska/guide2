/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GYROCALIB
#define GYROCALIB

#include <iostream>

#include <helium/vector/array.h>
//#include <heliumBased/robot/_sensorCalibManager.h>
#include <helium/graphics/glDraw.h>
#include <helium/util/ioUtil.h>
//#include <math/filter.h>
#include <helium/math/filter.h>
//#include <heliumBased/robot/gyroCalibManager.h>
//#include <robot/im.h>
//#include <guiObjects/guiConnectionLine.h>

#include <helium/thread/mutex.h>
#include <app/guide2/guideState.h>
#include <app/guide2/guideDescription.h>
#include <app/guide2/robotModels.h>


class GyroCalib {
 private:
  
  //GyroCalibManager* gcm;
  //double* motors;
  cfg::guide::Gyro &gyro;
  //helium::CountMutex dataLock;
  int desample;
  int samplec;
  bool collectData;
  helium::MMFilter* filter[5];
  std::vector<helium::Array<double,3> > data; ///< where the gyro data collection is put
  // private:
  // double c[5];
  double viewAngle;
public:
  RobotModels::ModelArea *ma;
  //GuiConnectionLine* conn;
  GyroCalib(// ImFormat* im,GyroCalibManager* pgcm,
	    cfg::guide::Gyro &gyro,
	    //helium::GtkProxy<helium::Connection> &pconn,
	    //int* argc,char** argv,
	    int pdesample,
	    RobotModels::ModelArea *pma);
  virtual ~GyroCalib();
  void setDataCollection(bool state);
  void setDesampling(int n);
  void onGyroUpdate(const helium::SensorValList<int> &); ///< update robot model orientation
  void onPositionUpdate(bool potentioUpdate);
  void clearAcquiredData();
  int getDataSize();
  /**
   * pass the data obtained from gyro readings 
   * @param d where the data will be copied
   */
  void getData(std::vector<helium::Array<double,3> >&d);

  //int getData(helium::Array<double> m);
  //void setCalib(const double* c);
  /**
   * saves current calib
   * @param filename where the file is to be saved
   * @param c the used calib
   */
  void saveGyroCalib(const std::string &filename);
  /**
   * open a calib file
   * @param filename to be opened
   * @param d where the calib will be read to
   */
  void openGyroCalib(const std::string &filename,helium::Array<double> &d);
  void getCalib(helium::Array<double> &c);
  //void connect(const char* host, int port);
  //void disconnect();
  //void onSocketDisconnection();
  //void registerConnectionLine(GuiConnectionLine* g);
  //void setPowerState(bool state);


  //functions from gyroCalibManager
  void setOffset(double ax,double ay,double az);
  void setRatio(double rx,double ry);
  /**
   * gyro function, from robot/gyroCalibManager.h
   * @param m
   * @param data
   */
  void getRototranslation(double *m,const double * data);
  void getCalib(double* c);
  int getRawDataNum();
  /**
   * calibration function, from robot/getGyroCalibration.h
   * @param c
   * @param data
   * @param idx
   */
  void getGyroCalibration(double* c,const std::vector<helium::Array<double,3> > &data,std::vector<std::pair<int,int> > &idx);
  //void onConnection(const helium::Connection &c);
public:
  //helium::StaticConnReference<helium::Connection,GyroCalib,const helium::Connection&,&GyroCalib::onConnection> callOnConnection;
  helium::StaticConnReference<helium::SensorValList<int>,GyroCalib,const helium::SensorValList<int> &,&GyroCalib::onGyroUpdate> callOnGyroUpdate;
};

///description to save gyro calib
namespace helium{
class GyroExport:public Description<cfg::guide::Gyro>{
public:
  GyroExport(cfg::guide::Gyro& s):Description<cfg::guide::Gyro>(s){
  }
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.calib),IDPath("robot.gyro",".")));
  }
};
}//helium


#endif //GYROCALIB

