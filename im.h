/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef IMFRAN
#define IMFRAN

#include<helium/vector/array.h>
#include <helium/vector/operations.h>
//#include "guideMotionManagement.h"
#include "teachIO.h"
const double pushThreshold=0.1;

struct ImFormat {
  int posSize;
  int tsensSize;
  ImFormat(int p,int t);
};

struct ImInput{
  helium::Posture pos;
  helium::Array<double> tsens;
  helium::Array<double,16> rototra;
  ImInput(const ImFormat& pformat);
  ImInput(const helium::Posture& pos, const helium::Array<double>& tsens);
  //void getStatus(const GuideMotionManagement& gmm);
  double weight(const ImInput& from);
};

struct ImOutput {
  helium::Posture pos;
  ImOutput(const ImFormat& pformat);
  ImOutput(const helium::Posture& pos);
  void addEffect(helium::Posture& p,double weight=1);
  void scale(double w);
};



class ImFilter{
  helium::Posture maxVelocity;
  helium::Posture minVelocity;
  helium::Time elapsedTime;

  double touchThreshold;
  std::vector<int> ignoredSensor; //notch

public:
  const ImFormat& format;
  ImFilter(const ImFormat& f);
  void filterInput(ImInput& input) const;
  void filterOutput(ImOutput& output) const;
  void setElapsedTime(helium::Time t);
};

struct ImPoint {
  ImInput in;
  ImOutput out;
  ImPoint(const helium::Posture& pin, const helium::Array<double>& tsens,const helium::Posture& pout);
  double addEffect(helium::Posture& v,const ImInput& finp,const ImFilter* filter,bool filterOut);
};


struct Im {
  const std::vector<TeachIO> &teachList;
  std::vector<ImPoint> points;
  Im(const std::vector<TeachIO> &teachList);
  void getOutput(helium::Posture& out,const ImInput& inp,ImFilter* filter,bool filterSingularly,std::vector<std::pair<int,double> > *weights);
  int getSize();
};

#endif
