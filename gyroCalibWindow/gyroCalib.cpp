/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "gyroCalib.h"
//#include <math/random.h>
//#include <math/getGyroCalibration.h>
#include <fstream>
#include <helium/util/ioUtil.h>
#include <helium/math/lapackUtil.h> //leastsquares
#include <helium/math/algebra.h>
#include <helium/xml/xmlLoader.h>
#include <helium/os/time.h>

using std::cout;
using std::endl;
using std::fstream;
using helium::MMFilter;

GyroCalib::GyroCalib(//ImFormat* im, GyroCalibManager* pgcm,
		     cfg::guide::Gyro &pgyro,
		     //helium::GtkProxy<helium::Connection> &pconn,
		     //int* argc,char** argv,
		     int pdesample,
		     RobotModels::ModelArea *pma):
		     //helium::convex::DrawConvex* pdrawConvex):
  //(new ODEWorld()),win(new SimulWin(w)),motors(new double[im->posSize]),
  //gcm(pgcm),
  //motors(new double[im->posSize]),
  gyro(pgyro),//conn(pconn),
  //dataLock("dataLock"),
  desample(pdesample),samplec(1),collectData(false),
  viewAngle(0),
  ma(pma),//drawConvex(pdrawConvex),
  //callOnConnection(this),
 
  callOnGyroUpdate(this)
  //,conn(NULL)
 
{
  for (int i=0;i<5;i++){
    filter[i]=new MMFilter(100);
  }


  double e[16];
  helium::eye(e); 
  if (ma){
   ma->setWorldMatrix(e);
  }
  //v4=new V4Gstructure(motors,NULL);
  //r=w->addRobot(v4);
  //conn.connect(callOnConnection);
}

GyroCalib::~GyroCalib(){
  for (int i=0;i<5;i++){
    delete filter[i];
  }
}

void GyroCalib::setDataCollection(bool state){
  //std::cout<<"enable "<<state<<" of "<<this<<std::endl;
  collectData=state;
}

void GyroCalib::setDesampling(int n){
  desample=n;
}


/*class ViewAdapter{
  
  double lastRot[16],headRot[16];

public:
  ViewAdapter(){
    helium::setAsRotation(lastRot,helium::Axis::Z ,0*M_PI);
    helium::setAsRotation(headRot,helium::Axis::X ,-M_PI/2);    
  }

  void getCameraRot(double* c,const double* m,double angle){

    //c=    rot(axisY,angle)*headrot*m*lastrot

    double tmp[16],camRot[16];
    helium::mul(camRot,headRot,m);//camRot used as swap
    helium::mul(tmp,camRot,lastRot);        
    helium::setAsRotation(camRot,helium::Axis::Y,angle);
    helium::mul(c,camRot,tmp);
    c[11]=-0.64935;
  }
  };*/


class ViewAdapter{
  
  double lastRot[16],headRot[16];

public:
  ViewAdapter(){
    helium::setAsRotation(headRot,helium::Axis::X ,-M_PI/2);    
  }

  void getCameraRot(double* c,const double* m,double angle){

    //c=    rot(axisY,angle)*headrot*m

    double tmp[16],camRot[16];
    helium::mul(tmp,headRot,m);//camRot used as swap
    helium::setAsRotation(camRot,helium::Axis::Y,angle);
    helium::mul(c,camRot,tmp);
    c[11]=-0.64935;
  }
};



  static ViewAdapter viewAdapter;

void GyroCalib::onGyroUpdate(const helium::SensorValList<int> &gv) {
  //std::cerr << "error: implement gyrocalib ongyro;update" << endl;

  static helium::Time prevTime = helium::getSystemMillis();
  static double plusAngle = 0;

  if (samplec++%desample) return;
  //helium::Array<double> gyro //(currentStatus.getFormat()->gyroSize); //get gyro
  //currentStatus.getGyro(gyro.data);

  if (collectData){
    helium::Array<double,3> d;
    for(size_t i=0;i<d.size();i++)//saves the first 3 values of gyro read on sensorvallist
      d[i] = gv.vals[i];
    data.push_back(d);
    //cout << data.size() << ": " << d  << " - " << gv.vals << endl;
    //dataLock.countUnlock();
  }else{
    //std::cout<<"waste data of"<<this<<std::endl;
  }    
  
  double filtered[5];
  for (int i=0;i<5;i++){
    filter[i]->push(gv.vals[i]); //gyro[i]);
    filtered[i]=filter[i]->getValue();
  }

  if (ma!=NULL) { //drawing robot rotation
    double md[16],camera[16];    
    gyro.calib.getRototranslation(md,filtered);
    viewAngle=2*M_PI;//* helium::getSystemMillis()/10000;
    if(helium::getElapsedMillis(prevTime)>3000) {
      plusAngle += 0.5*M_PI;
      prevTime = helium::getSystemMillis();
    }
    viewAngle += plusAngle;
    viewAdapter.getCameraRot(camera,md,viewAngle);	       
    ma->setViewMatrix(camera);
    ma->update();
  }
}

void  GyroCalib::clearAcquiredData(){
  //dataLock.lock();
  data.clear();
  //dataLock.unlock();
}

int GyroCalib::getDataSize(){
  return data.size();
}

void GyroCalib::getData(std::vector<helium::Array<double,3> >&d){//returns a pointer to the data,delete after using
  //dataLock.lock();
  /*m=new double[data.size()*3]; //helium::Array<double> m(data.size()*3); //  

  for (size_t i=0;i<data.size();i++){
    memcpy(&m[i*3],data[i].data,3*sizeof(double));
  }
  //dataLock.unlock();
  return data.size();*/

  d=data;
}

/*
int GyroCalib::getData(helium::Array<double> m) {
  m.resize(data.size()*3);
  for(size_t i=0;i<data.size();i++) {
    for(size_t j=0;j<3;j++)
      m[i+j] = data[i].data[j];
  }
  return data.size();
}
*/

void GyroCalib::saveGyroCalib(const std::string &filename){
  helium::Memory outmem;
  helium::XmlLoader out;
  helium::cexport<helium::GyroExport>(outmem,gyro);
  out.save(filename.c_str(),outmem);  
}

void GyroCalib::openGyroCalib(const std::string &filename,helium::Array<double> &d){
  helium::Memory inmem;
  helium::XmlLoader in;
  in.loadDataAndModel(inmem,filename);
  cfg::guide::Gyro tgyro;
  helium::cimport<helium::GyroExport>(tgyro,inmem);
  d.resize(5);
  d[0] = tgyro.calib.xoffset;
  d[1] = tgyro.calib.yoffset;
  d[2] = tgyro.calib.zoffset;
  d[3] = tgyro.calib.xratio;
  d[4] = tgyro.calib.yratio;
}

void GyroCalib::getCalib(helium::Array<double> &c) { /// gets calibration as an array, size is FIXED as 5
  c.resize(5);
  c[0] = gyro.calib.xoffset;
  c[1] = gyro.calib.yoffset;
  c[2] = gyro.calib.zoffset;
  c[3] = gyro.calib.xratio;
  c[4] = gyro.calib.yratio;
}

void GyroCalib::getGyroCalibration(double* c,const std::vector<helium::Array<double,3> > &data,std::vector<std::pair<int,int> > &idx) {


  double* a=new double[5*idx.size()];
  double* b=new double[idx.size()];
  for (unsigned int i=0;i<idx.size();i++){
    int v1=idx[i].first;
    int v2=idx[i].second;   
    for (int j=0;j<3;j++){
      a[i*5+j]=-2*(data[v1][j]-data[v2][j]);
      //a[i*5+j]=-2*(data[v1*3+j]-data[v2*3+j]);
    }
    for (int j=0;j<2;j++){
      a[i*5+3+j]=data[v1][j]*data[v1][j]-data[v2][j]*data[v2][j];
      //a[i*5+3+j]=data[v1*3+j]*data[v1*3+j]-data[v2*3+j]*data[v2*3+j];
    }
    b[i]=-(data[v1][2]*data[v1][2]-data[v2][2]*data[v2][2]);
  }
  //std::cout<<"A"<<std::endl;
  //print(a,idx.size(),5)<<std::endl;
  //cout<<"B";
  //print(b,idx.size(),1)<<endl;
  helium::leastSquares(c,a,b,idx.size(),5,1,true);
  c[0]/=c[3];
  c[1]/=c[4];
  c[3]=sqrt(c[3]);
  c[4]=sqrt(c[4]);
  delete[] a;
  delete[] b;


  /*
  helium::Array<double> a(5*idx.size()); //double* a=new double[5*idx.size()];
  helium::Array<double> b(idx.size()); //double* b=new double[idx.size()];
  for (size_t i=0;i<idx.size();i++){
    int v1=idx[i].first;
    int v2=idx[i].second;   
    for (int j=0;j<3;j++){
      a[i*5+j]=-2*(data[v1*3+j]-data[v2*3+j]);
    }
    for (int j=0;j<2;j++){
      a[i*5+3+j]=data[v1*3+j]*data[v1*3+j]-data[v2*3+j]*data[v2*3+j];
    }
    b[i]=-(data[v1*3+2]*data[v1*3+2]-data[v2*3+2]*data[v2*3+2]);
  }
  helium::leastSquares(c,a.data,b.data,idx.size(),5,1,true);
  c[0]/=c[3];
  c[1]/=c[4];
  c[3]=sqrt(c[3]);
  c[4]=sqrt(c[4]);
  */
}
