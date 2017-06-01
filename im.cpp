/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "im.h"
using std::endl;
using std::cout;
ImFormat::ImFormat(int p,int t):
  posSize(p),tsensSize(t) {
}

ImInput::ImInput(const ImFormat& pformat):
  pos(pformat.posSize),
  tsens(pformat.tsensSize) {
  helium::zero(pos);
  helium::zero(tsens);
  helium::zero(rototra);
}

ImInput::ImInput(const helium::Posture& ppos, const helium::Array<double>& ptsens):
  pos(ppos),tsens(ptsens)
{
}

double ImInput::weight(const ImInput& from) {
  //threshold
  bool atLeastOne=false;
  for (size_t i=0;i<tsens.size();i++){
    if (tsens[i]>pushThreshold){
      atLeastOne=true;
      continue;
    }
  }
  if (!atLeastOne) return 0;

  //gamma
  for (size_t i=0;i<tsens.size();i++){
    if (tsens[i]>pushThreshold&&from.tsens[i]<pushThreshold) return 0;
  }

  double weight=1; double dist=0;
  for (size_t i=0;i<tsens.size();i++){
    //alpha
    if (tsens[i]>pushThreshold) 
      weight*=from.tsens[i]/tsens[i];
    //beta F[s]
    else 
      dist+=from.tsens[i]*from.tsens[i];
  }
  //beta P
  for (size_t i=0;i<pos.size();i++){
    double diff=from.pos[i]-pos[i];
    dist+=diff*diff;
  }

  /*for (int i=0;i<format->gyroSize;i++){
    double diff=from.gyro[i]-gyro[i];
    dist+=diff*diff;
    }*/

  //beta O
  for (int i=0;i<16;i++){
    double diff=from.rototra[i]-rototra[i];
    dist+=diff*diff;
  }

  //alpha * beta
  return weight/(1+sqrt(dist));
}

ImOutput::ImOutput(const helium::Posture& ppos):
  pos(ppos)
{
}
ImOutput::ImOutput(const ImFormat& pformat):
  pos(pformat.posSize) {
  helium::zero(pos);
}

void ImOutput::scale(double w) {
  for (size_t i=0;i<pos.size();i++){
    pos[i]*=w;
  }
}

void ImOutput::addEffect(helium::Posture& v,double weight) {
  for(size_t i=0;i<pos.size();i++) 
    v[i] += weight*pos[i]; 
}

ImFilter::ImFilter(const ImFormat& f):
  maxVelocity(f.posSize),
  minVelocity(f.posSize),
  elapsedTime(1),
  touchThreshold(0.2),
  format(f)
{
  helium::setAll(maxVelocity,3E-4); //0.1745 per second
  helium::setAll(minVelocity,1E-4);
}

void ImFilter::setElapsedTime(helium::Time t) {
  elapsedTime = t;
}

void ImFilter::filterInput(ImInput& input) const {
  for(size_t i=0;i<ignoredSensor.size();i++) {
    input.tsens[ignoredSensor[i]] = 0;
  }
  for(size_t i=0;i<input.tsens.size();i++) {
    if(input.tsens[i]<touchThreshold)
      input.tsens[i] = 0;
  }
}

void ImFilter::filterOutput(ImOutput& o) const {

  //sth in the output
  bool hasValue = false;
  for(size_t i=0;i<o.pos.size();i++){
    if(fabs(o.pos[i])>10E-7) 
      hasValue = true;
  }
  if (!hasValue) return;

  double maxv=0;
  double safetyCoeff=1;
  double increaseCoeff=1;

  //minvelo always zero
  //increasecoeff always 1

  //cout << "ti" << elapsedTime << endl;

  for (size_t i=0;i<o.pos.size();i++){
    double ampl=fabs(o.pos[i]);
    if (ampl>maxv){
      increaseCoeff=std::max(1.0,minVelocity[i]*elapsedTime/ampl); //the max velocity must be at least minDelta
      maxv=ampl;
    }
    safetyCoeff=std::min(safetyCoeff,maxVelocity[i]*elapsedTime/ampl);
  }

  //small changes will be ignored
  for (size_t i=0;i<o.pos.size();i++){
    if (fabs(o.pos[i])<0.3*maxv)
      o.pos[i]=0;
  }

  //o.pos will be o.pos * safetyCoeff
  double coeff=std::min(safetyCoeff,increaseCoeff);
  for (size_t i=0;i<o.pos.size();i++){
    o.pos[i]=coeff*o.pos[i];
  }

  //o.pos is only filtered out of the small changes
}

Im::Im(const std::vector<TeachIO> &pteachList):
  teachList(pteachList)
{
}
  

int Im::getSize() {
  return teachList.size();
}


void Im::getOutput(helium::Posture& out,const ImInput& inp,ImFilter* filter,bool filterSingularly,std::vector<std::pair<int,double> > *weights) {
  //filter current pressed sensor
  ImInput finp(inp);
  filter->filterInput(finp);

  //initializing
  ImOutput fout(filter->format);
  weights->clear();
  
  //calc
  double w;
  for(size_t i=0;i<teachList.size();i++) {
    w = points[i].addEffect(fout.pos,finp,filter,filterSingularly); //calculate weights, manipulate fout
    if (w>1E-4){
      weights->push_back(std::pair<int,double>(i,w));
    }
    //cout << "fout " << fout.pos << endl;
  }
  if(!filterSingularly){
    filter->filterOutput(fout);
  }
  fout.addEffect(out,1);
}

ImPoint::ImPoint(const helium::Posture& pin, const helium::Array<double>& tsens,const helium::Posture& pout):
  in(pin,tsens),out(pout) {
}

double ImPoint::addEffect(helium::Posture& v,const ImInput& finp,const ImFilter* filter,bool filterOutput) {
  //get this teach's input
  ImInput fin(in);
  filter->filterInput(fin);

  //calculate weight for this example
  double w = fin.weight(finp);  

  //calculate output (v)
  if (filterOutput){
    ImOutput fout(out);
    fout.scale(w);               //scale fout
    filter->filterOutput(fout); //filter fout
    fout.addEffect(v);           //v += fout.pos
  }else{
    out.addEffect(v,w);
  }
  return w;
}

/*
void ImInput::getStatus(const GuideMotionManagement& gmm) {
  gmm.gs.getCurrentPosture(pos);
  gmm.gs.getCurrentSensor(tsens);
}
*/
