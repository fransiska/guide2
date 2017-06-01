/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.06.17
 * \version 0.1
 * Release_flags g
 */

#include "tbt.h"
#include <helium/vector/operations.h>

inline std::ostream& operator <<(std::ostream& o,const TbTType::TeachIOChangeReason &reason) {
  std::string s = "";
  switch(reason) {
  case TbTType::ACQUIRE: s = "Acquire"; break;
  case TbTType::ACQUIRED: s = "Acquired"; break;
  case TbTType::DELETE: s = "Delete"; break;
  case TbTType::NEW: s = "New"; break;
  case TbTType::OPEN: s = "Open"; break;
  case TbTType::UNDO: s = "Undo"; break;
  case TbTType::REDO: s = "Redo"; break;
  case TbTType::OPENNEW: s = "OpenNew"; break;
  case TbTType::OPENACQUIRE: s = "OpenAcquire"; break;
  case TbTType::OPENACQUIRED: s = "OpenAcquired"; break;
  default : s = "Event"; break;
  }
  return o << s;
}

inline std::ostream& operator <<(std::ostream& o,const TbTType::SensorUpdate &update) {
  std::string s = "";
  switch(update) {
  case TbTType::CONNECTION : s = "Connection"; break;
  case TbTType::POWER : s = "Power"; break;
  case TbTType::POT : s = "Pot"; break;
  case TbTType::TOUCH : s = "Touch"; break;
  case TbTType::GYRO : s = "Gyro"; break;
  default : s = "SensorUpdate"; break;
  }
  return o << s;
}

TbTTeach::TbTTeach(GuideState &pgs, TbTFilter *pfilter, TbTLog &plog):
  gs(pgs),
  filter(pfilter),
  log(plog),
  history(examples),
  checkPower(this,gs.teach.powers.size()),
  setPower(this,gs.teach.powers.size())
{
  teachLogfile = gs.pref.teachLogfile + ".tea";
  gs.pref.teachLogfile.append(".log");
  //open logfile
  logFile.open(gs.pref.teachLogfile.c_str(), std::ios::out | std::ios::app);
  std::cout << "Logging teachlist to " << gs.pref.teachLogfile << std::endl;
  std::cout << "Backing up teachfile to " << teachLogfile << std::endl;

  for(size_t i=0;i<gs.teach.powers.size();i++) {
    checkPower[i].signal.connect(setPower[i].conn);
  }
}

std::string TbTTeach::getTouchedSensorNames(const helium::Array<double> &ratio) {
  helium::Array<double> filteredRatio(ratio);
  std::stringstream ss;
  filter->filterTouch(filteredRatio);

  for(size_t i=0;i<filteredRatio.size();i++) {
    if(filteredRatio[i]>0)
      ss << gs.sensors[i].name << " ";
  }
  return ss.str();
}

void TbTTeach::acquire(const TeachPostureInput &p) {
  examples.push_back(TeachIO(p));
  signals.acquire(examples.back().input);
}
void TbTTeach::acquire(const TeachIO &p) {
  examples.push_back(p);
  signals.acquire(examples.back().input);
}
void TbTTeach::acquired(const TeachIO &p) {
  signals.acquired(examples.back().output);
}
void TbTTeach::acquired(const TeachPosture &p) {
  examples.back().setOutput(p);
  signals.acquired(examples.back().output);
}
void TbTTeach::del(const int id) {
  examples.erase(examples.begin()+id);
  signals.del(id);
}
bool TbTTeach::canUndo() {
  return history.canRewind();
}
bool TbTTeach::canRedo() {
  return history.canForward();
}
void TbTTeach::undo() {
  history.rewind();
  examples = history.present();
  signals.undo();
}
void TbTTeach::redo() {
  history.forward();
  examples = history.present();
  signals.redo();
}

void TbTTeach::change(TbTType::TeachIOChangeReason reason) {
  switch(reason) {

  case TbTType::ACQUIRED:
  case TbTType::DELETE:
  case TbTType::NEW:
  case TbTType::OPEN:
    history.pushPresent(examples);
    writeLog(reason,true);
    break;

  case TbTType::UNDO:
  case TbTType::REDO:
    writeLog(reason,true);
    break;

  case TbTType::ACQUIRE:
  default:
    writeLog(reason,false);
    break;


  case TbTType::OPENNEW:
  case TbTType::OPENACQUIRE:
  case TbTType::OPENACQUIRED:
    break;
  }
}

/*! reason = 
 *  write = write teachList or not
 */
void TbTTeach::writeLog(TbTType::TeachIOChangeReason reason, bool write) {
  logFile << helium::toString(helium::getSystemMillis()) << " " << reason << ": ";
  if(write) {
    for(size_t i=0;i<examples.size();i++) {
      logFile << examples[i] << " | ";
    }
  }
  logFile << std::endl;
}

TbTTeach::~TbTTeach() {
  //close logfile
  logFile.close();
}

void TbTTeach::clear() {
  examples.clear();
  signals.clear();
}
void TbTTeach::dummy() {
}

/*! set every sensor change to motor movement
 */
void TbTTeach::setSensorsConnections(bool b) {
  std::stringstream ss;
  ss << "teachchange " << b;
  log.writeLog(ss.str());

  static bool prevb = false;
  if(prevb == b) return; //avoid double (dis)connection

  for(size_t i=0;i<checkPower.size();i++) {
    checkPower[i].setSensorsConnections(b);
  }

  prevb = b;
}

const std::vector<TeachIO>& TbTTeach::get() const {
  return examples;
}

TbTFilter::TbTFilter(const cfg::guide::Teach &pteach):
  teach(pteach),
  elapsedTime(0)
{
}

void TbTFilter::filterInput(TeachPostureInput &input) {
  filterTouch(input.ratio);
}

void TbTFilter::filterOutput(helium::Posture &posture) {
  //filter if there is anything bigger than 10E-7
  bool hasValue = false;
  for(size_t i=0;i<posture.size();i++) {
    if(std::fabs(posture[i] > 10E-7)) hasValue = true; 
  }
  if(!hasValue) return;

  //get coeff
  double maxv = 0;
  double safetyCoeff = 1;
  double increaseCoeff = 1;
  
  for(size_t i=0;i<posture.size();i++) {
    double ampl = std::fabs(posture[i]);
    if(ampl > maxv) {
      maxv = ampl;
      increaseCoeff = std::max(1.0, teach.minVelocity[i] * elapsedTime / maxv);
    }
    safetyCoeff = std::min(safetyCoeff, teach.maxVelocity[i] * elapsedTime / maxv);
  }

  //filter small values
  for(size_t i=0;i<posture.size();i++) {
    if(std::fabs(posture[i]) < 0.3*maxv) 
      posture[i] = 0;
  }
  double coeff = std::min(safetyCoeff, increaseCoeff);
  for(size_t i=0;i<posture.size();i++) {
    posture[i] *= coeff;
  }
}

void TbTFilter::filterTouch(helium::Array<double> &ratio) {
  for(size_t i=0;i<teach.touch.ignored.size();i++) {
    ratio[teach.touch.ignored[i]] = 0;
  }
  for(size_t i=0;i<ratio.size();i++) {
    if(ratio[i]<teach.touch.threshold)
      ratio[i] = 0;
  }
}

TbTTouch::TbTTouch(GuideState &pgs, RobotModels *prm, const std::vector<TeachIO> &pexamples, TbTFilter *pfilter, TbTLog &plog):  
  gs(pgs),
  rm(prm),
  examples(pexamples),
  filter(pfilter),
  log(plog),
  isElapsedTimeValid(false),
  waitReturnPosition(0),
  callOnTouch(this,gs.sensors.size())
{
}

//done every sensor value update
void TbTTouch::onTouch() {
  //wait for robot to return to first position
  if(helium::getSystemMillis() < waitReturnPosition) {
    //std::cout << "now is " << helium::getSystemMillis() << " wait till time is " << waitReturnPosition << std::endl;
    return;
  }
  //std::cout << "ok " << std::endl;

  //prevent processing onTouch too fast
  static helium::Time prevT=helium::getSystemMillis();
  if (helium::getElapsedMillis(prevT)<10) return;
  prevT=helium::getSystemMillis();

  std::stringstream ss;

  //get current posture, touch sensor, gyro

  //* posture
  TeachPostureInput touch(0,gs.joints.size(),gs.sensors.size(),gs.gyro.calib, log.teachTouchLog, log.teachGyroLog);
  gs.getCurrentPot(touch.posture);
  //std::cout << "current pot " << touch.posture << std::endl;
  //std::cout << touch.rototra << std::endl;
  
  if(gs.isRobotMoving(touch.posture)) {
    ss << "ontouch robot is moving";
    log.writeLog(ss.str());
    return;
  } 

  ss.str("");
  ss << "ontouch input " << touch.posture;
  log.writeLog(ss.str());

  //time elapsed
  static helium::Time prevTime = helium::getSystemMillis();
  helium::Time newTime = helium::getSystemMillis();
  filter->elapsedTime = isElapsedTimeValid?newTime-prevTime:0;

  //set sensor ratio
  gs.getSensorMax(touch.touchMax);
  gs.getSensorMin(touch.touchMin);
  touch.calcTouchRatio();

  //get output by checking teach examples
  helium::Posture output(gs.joints.size());
  helium::setAll(output,0);
  getOutput(output,touch);
  //std::cout << "output " << output  << std::endl;
  helium::sumn(output,touch.posture,output.size());

  ss.str("");
  ss << "ontouch output " << output;
  log.writeLog(ss.str());
  //std::cout << ss.str() << std::endl;
  
  //filter by coldet
  if(filterColdet(output)) {
    for (size_t i=0;i<gs.joints.size();i++) {
      gs.joints[i].desiredTarget.assure(output[i]);
    }
  }  
  
  prevTime = newTime;
  isElapsedTimeValid = true;
}

void TbTTouch::getOutput(helium::Posture &output,const TeachPostureInput &touch) {
  TeachPostureInput filteredTouch(touch);
  filter->filterInput(filteredTouch);

  for(size_t e=0;e<examples.size();e++) {
    //example diff = example.output - example.input
    helium::Posture exampleDiff(output.size());
    helium::subn(exampleDiff,examples[e].output.posture,examples[e].input.posture,output.size());

    //copy and filter current example
    TeachPostureInput filteredExample(examples[e].input);
    filter->filterInput(filteredExample);

    //get weight from example.input and touch
    double weight = getWeight(filteredExample,filteredTouch);
    //std::cout << weight << " " ;

    //weight * example diff
    helium::mulScalar(exampleDiff,weight);

    //filter the diff
    filter->filterOutput(exampleDiff);

    //add to the output
    helium::sumn(output,exampleDiff,output.size());
  }			  
  //std::cout << " moving to " << output << std::endl;
}

double TbTTouch::getWeight(const TeachPostureInput &example, const TeachPostureInput &touch) {
  //std::cout << "e " << example.ratio << std::endl;
  //std::cout << "t " << touch.ratio << std::endl;
                    
  //check with threshold
  bool atLeastOne = false;
  for(size_t i=0;i<example.ratio.size();i++) {
    if(example.ratio[i] > 0) {
      atLeastOne = true;
      break;
    }    
  }
  if(!atLeastOne) return 0;

  //gamma
  for(size_t i=0;i<example.ratio.size();i++) {
    if(example.ratio[i] > 0 && touch.ratio[i] == 0) {
      return 0;
    }
  }

  double alpha = 1;
  double beta = 1;

  //std::cout << "ab";
  for(size_t i=0;i<example.ratio.size();i++) {
    if(example.ratio[i] > 0) {
      alpha *= touch.ratio[i]/example.ratio[i];
    }
    else {
      beta += touch.ratio[i]*touch.ratio[i];
    }
  }

  for(size_t i=0;i<touch.posture.size();i++) {
    double diff = touch.posture[i]-example.posture[i];
    beta += diff*diff;
  }
  
  for(size_t i=0;i<touch.getGyro().size();i++) {
    double diff = touch.getGyro()[i]-example.getGyro()[i];
    beta += diff*diff;
  }

  //std::cout << "beta " << beta << " | ";

  return alpha/(1+sqrt(beta));
}


bool TbTTouch::filterColdet(const helium::Posture &output){
  if(rm) {
    rm->coldetModel.rotate(output);
    if(rm->coldetModel.isColliding()) {
      log.writeLog("colliding position");
      return false;
    }
  }
  return true;
}

void TbTTouch::setSensorsConnections(bool b) {
  std::stringstream ss;
  ss << "touchchange " << b;
  log.writeLog(ss.str());
  //std::cout << ss.str() << std::endl;

  static bool prevb = false;
  if(prevb == b) return; //avoid double (dis)connection

  if(b) {
    for(size_t s=0;s<gs.sensors.size();s++) {
      gs.sensors[s].rawValue.connect(callOnTouch[s]);
    }
  }
  else {
    for(size_t s=0;s<gs.sensors.size();s++) {
      gs.sensors[s].rawValue.disconnect(callOnTouch[s]);
    }
    isElapsedTimeValid = false;
  }

  prevb = b;
}

std::string TbT::getTouchedSensorNames(const helium::Array<double> &ratio) {
  return teach.getTouchedSensorNames(ratio);
}

void TbT::onTouch() {
  updateTouchedSensor();
  updateTbTState();
}

void TbT::updateTouchedSensor() {
  static helium::Time prevT=helium::getSystemMillis();
  if (helium::getElapsedMillis(prevT)<10) return;
  prevT=helium::getSystemMillis();
 
  helium::Array<double> ratio(gmm.gs.sensors.size());
  gmm.gs.getCurrentSensorRatio(ratio);
  touchedSensor = getTouchedSensorNames(ratio);  
}

TbT::TbT(GuideMotionManagement &pgmm,RobotModels *prm):
  gmm(pgmm),
  rm(prm),
  filter(gmm.gs.teach),
  log(gmm,rm),
  teach(gmm.gs,&filter,log),
  touch(gmm.gs,prm,teach.get(),&filter,log),
  state(TbTType::DISCONNECTED),
  acquireState(false),
  touchedSensor(""),
  acquiring(false),
  connections(new Connections(this,gmm.gs,teach.teachLogfile))
{
  gmm.gs.conn.connect(connections->callOnConnection);
  gmm.gs.view.connect(connections->callOnViewChange);
  state.connect(connections->callOnStateChange);
  acquireState.connect(connections->callOnAcquireChange);

  teach.signals.acquire.connect(connections->callOnTeachAcquire);
  teach.signals.acquired.connect(connections->callOnTeachAcquired);
  teach.signals.del.connect(connections->callOnTeachDelete);
  teach.signals.clear.connect(connections->callOnTeachClear);
  teach.signals.undo.connect(connections->callOnTeachUndo);
  teach.signals.redo.connect(connections->callOnTeachRedo);

  //to update touched sensor label
  for(size_t s=0;s<gmm.gs.sensors.size();s++) {
    gmm.gs.sensors[s].rawValue.connect(connections->callOnTouch[s]);    
  }  
}

void TbT::onConnection(helium::Connection c) {
  //onViewChange(gmm.gs.view.get());
  updateTbTState();
}

void TbT::onViewChange(cfg::guide::GuideView::Type v) {
  updateTbTState();
}

void TbT::updateTbTState() {
  helium::Connection::State connState = gmm.gs.conn.get().state;
  cfg::guide::GuideView::Type view = gmm.gs.view.get();
  
  switch(connState) {
  case helium::Connection::CONNECTED :
    if(view == cfg::guide::GuideView::TOUCH) {
      if(acquiring) {
	state.assure(TbTType::TEACHING);
      }
      else {
	if(touchedSensor.get().size()) {
	  state.assure(TbTType::TOUCHENABLE);
	}
	else {
	  state.assure(TbTType::NOTOUCH);
	}
      }
    }
    else 
      state.assure(TbTType::CONNECTED);
    break;
  default : 
    state.assure(TbTType::DISCONNECTED);
    break;
  }
}

/*! on button acquire is toggled/untoggled disconnect all sensors, get postures, then connect sensors again
 */
void TbT::onAcquireChange(bool acquire) {
  TeachPostureInput p(helium::getSystemMillis(), gmm.gs.joints.size(), gmm.gs.sensors.size(), gmm.gs.gyro.calib, log.teachTouchLog, log.teachGyroLog);  
  gmm.gs.getSensorMax(p.touchMax);
  gmm.gs.getSensorMin(p.touchMin);
  p.calcTouchRatio();
  p.touchedSensor = getTouchedSensorNames(p.ratio);

  //to make sure that a sensor is touched
  if(acquire && p.touchedSensor.size() == 0) return;

  if(acquire) {
    touch.setSensorsConnections(false);
  }
  else {
    teach.setSensorsConnections(false);
  }

  if(state != TbTType::DISCONNECTED) {
    helium::mlin::HaltTarget<helium::RelMotorSign,helium::motor::IFACELEVEL> halt(gmm.gs.joints.size());
    helium::TargetEndReason r;
    gmm.gs.joints.remoteLinearNormWholeTarget.set(r,halt);
    if (r!=helium::TARGETREACHED){
      std::cerr << "target not reached" << std::endl;
      return;
    }

    std::cerr << "robot is still moving " << p.posture << std::endl;
    std::cout << "start " << helium::getSystemMillis()  << std::endl;
    gmm.gs.getActualCurrentPot(p.posture);
    std::cout << "stop " << helium::getSystemMillis()  << std::endl;

    while(gmm.gs.isRobotMoving(p.posture)) {
      gmm.gs.getActualCurrentPot(p.posture);
      helium::milliSleep(100);
    }
  }

  if(acquire && !acquiring) {
    teach.edit<TbTType::ACQUIRE,void,const TeachPostureInput&,&TbTTeachImplementation::acquire>(p);
    teach.setSensorsConnections(true);
    acquiring = true;
  }
  else if(!acquire && acquiring) {
    teach.edit<TbTType::ACQUIRED,void,const TeachPosture&,&TbTTeachImplementation::acquired>(p);

    //move robot back
    std::stringstream ss;
    ss.str("");
    ss << (acquire?"acquire":"acquired") << " returning to " << teach.get().back().input.posture;
    log.writeLog(ss.str());
    std::cout << ss.str() << std::endl;
    if(gmm.gs.conn.get().state == helium::Connection::CONNECTED) {
      for(size_t i=0;i<gmm.gs.joints.size();i++) {
 	gmm.gs.joints[i].desiredTarget.assure(teach.get().back().input.posture[i]);
      }
    }

    helium::Array<double> postureDifference(teach.get().back().output.posture);
    helium::absSubn(postureDifference,teach.get().back().input.posture.data,gmm.gs.joints.size());
    helium::Time timeNeededToReturn = helium::getMax(postureDifference)/gmm.gs.behaviorSettings.targetReachingSpeed;
    //usleep(timeNeededToReturn * 1000 * 1.5);
    
    touch.waitReturnPosition = helium::getSystemMillis() + (timeNeededToReturn * 15) / 10;
    touch.setSensorsConnections(true);
    acquiring = false;
  }
}

/*! in and out of TOUCHENABLE state sensor connections must be turned on and off accordingly
 *  DISCONNECTED requires acquireState to be turned off
 */
void TbT::onStateChange(TbTType::State s) {
  switch(s) {
  case TbTType::DISCONNECTED:
    acquireState.assure(false);
    break;
  case TbTType::TOUCHENABLE:    
  case TbTType::NOTOUCH:
    if(!acquiring) touch.setSensorsConnections(true);
    break;
  case TbTType::TEACHING:
    if(acquiring) teach.setSensorsConnections(true);
    break;
  default:
    touch.setSensorsConnections(false);
    teach.setSensorsConnections(false);
    break;
  }

//   if(s == TbTType::DISCONNECTED){
//     acquireState.assure(false);
//   }
//   if(s == TbTType::TOUCHENABLE) {
//     if(s ) {
//       teach.setSensorsConnections(true);
//     }
//     else {
//       touch.setSensorsConnections(true);
//     }
//   }
//   else {
//     touch.setSensorsConnections(false);
//     teach.setSensorsConnections(false);
//   }
}

void TbT::newTeachList() {
  teach.edit<TbTType::NEW,void,&TbTTeachImplementation::clear>();
  teachfile = "";
}

void TbT::applytoFrame(int id, bool output) {
  helium::Posture p(gmm.gs.joints.size());
  if(output) {
    p = teach.get()[id].output.posture;
  }
  else {
    p = teach.get()[id].input.posture;
  }

  for(size_t i=0;i<gmm.gs.joints.size();i++)
    gmm.gs.joints[i].frameValue.assure<GuideEvent::PASTETEACHPOSTURE>(std::make_pair(helium::VALUEFRAME,p[i]));
  gmm.gs.joints.postureChange(GuideEvent::PASTETEACHPOSTURECHANGE);
}

void TbT::deleteTeachList(const int id) {
  teach.edit<TbTType::DELETE,void,const int,&TbTTeachImplementation::del>(id);
}
void TbT::openTeachList(const std::string &filename, bool replace) {
  std::cout << "opening teach list" << std::endl;
  //open xml file into teachio
  std::vector<TeachIO> openedio;

  helium::XmlLoader xml;
  helium::Memory inmem;
  xml.loadDataAndModel(inmem,filename);
  //std::cout << inmem << std::endl;
  helium::dimport(openedio,inmem);

  if(replace) {
    teach.edit<TbTType::OPENNEW,void,&TbTTeachImplementation::clear>();
    helium::convert(teachfile,filename);
  }

  //for(size_t i=0;i<openedio.size();i++) {
  //  std::cout << "size of teachio " << i << std::endl;
  //  std::cout << "posture " << openedio[i].output.posture << std::endl;
  //  std::cout << "size of gyro of first teachio " << openedio[i].input.gyro.size() << std::endl;
  //  std::cout << "size of gyro of first teachio " << openedio[i].input.gyro[0].second.size() << std::endl;
  //}
  //throw;

  for(size_t i=0;i<openedio.size();i++) {
    teach.edit<TbTType::OPENACQUIRE,void,const TeachIO&, &TbTTeachImplementation::acquire>(openedio[i]);
    teach.edit<TbTType::OPENACQUIRED,void,const TeachIO&, &TbTTeachImplementation::acquired>(openedio[i]);
  }  
  teach.edit<TbTType::OPEN,void,&TbTTeachImplementation::dummy>();
}
void TbT::saveTeachList(const std::string &filename) {
  helium::Memory outmem;    
  helium::XmlLoader out;    
  helium::dexport(outmem,const_cast<std::vector<TeachIO>&>(teach.get()));    
  out.save(filename.c_str(),outmem);

  //only change currently open tea file if it's not saving the current tea log file
  if(std::strcmp(filename.c_str(),teach.teachLogfile.c_str()) != 0) {
    helium::convert(teachfile,filename);
  }
}
const std::vector<TeachIO>& TbT::getTeachList() const {
  return teach.get();
}

TbTSignals& TbT::getSignals() {
  return teach.signals;
}

void TbT::getTeachFile(std::string &s) {
  s = teachfile;
}

TbT::Connections::Connections(TbT *owner,const GuideState &gs,const std::string &teachLogfile):
  callOnConnection(owner),
  callOnViewChange(owner),
  callOnStateChange(owner),
  callOnAcquireChange(owner),
  callOnTouch(owner,gs.sensors.size()),
  callOnTeachAcquire(owner,teachLogfile),
  callOnTeachAcquired(owner,teachLogfile),
  callOnTeachDelete(owner,teachLogfile),
  callOnTeachClear(owner,teachLogfile),
  callOnTeachUndo(owner,teachLogfile),
  callOnTeachRedo(owner,teachLogfile)
{
}

void TbT::undoTeachList() {
  if(teach.canUndo())
    teach.edit<TbTType::UNDO,void,&TbTTeachImplementation::undo>();
}
void TbT::redoTeachList() {
  if(teach.canRedo())
    teach.edit<TbTType::REDO,void,&TbTTeachImplementation::redo>();
}

TbTLog::TbTLog(GuideMotionManagement &pgmm, RobotModels *prm):
  gmm(pgmm),
  rm(prm),
  callOnConnection(this),
  callWritePot(this,gmm.gs.joints.size()),
  callWriteTouch(this,gmm.gs.sensors.size()),
  callWriteGyro(this,std::make_pair(TbTType::GYRO,-1)),
  callWritePower(this,gmm.gs.joints.size())
{
  gmm.gs.conn.connect(callOnConnection);
  gmm.gs.pref.sensorLogfile.append(".log");
  sensorLogfile.open(gmm.gs.pref.sensorLogfile.c_str(), std::ios::out | std::ios::app);
}

TbTLog::~TbTLog() {
  sensorLogfile.close();
}

void TbTLog::onConnection(helium::Connection c) {
  static bool connected = false;
  helium::Connection::State connState = c.state;
  switch(connState) {
  case helium::Connection::CONNECTED :
    if(!connected) {
      for(size_t i=0;i<gmm.gs.joints.size();i++) {
	gmm.gs.joints[i].displayedTarget.connect(callWritePot[i]);
	gmm.gs.joints[i].jointPower.connect(callWritePower[i]);
      }
      for(size_t i=0;i<gmm.gs.sensors.size();i++) {
	gmm.gs.sensors[i].rawValue.connect(callWriteTouch[i]);
      }
      gmm.gs.gyro.data.connect(callWriteGyro);
    }
    connected = true;
    break;    
  case helium::Connection::DISCONNECTED :
    if(connected) {
      for(size_t i=0;i<gmm.gs.joints.size();i++) {
	gmm.gs.joints[i].displayedTarget.disconnect(callWritePot[i]);
	gmm.gs.joints[i].jointPower.disconnect(callWritePower[i]);
      }
      for(size_t i=0;i<gmm.gs.sensors.size();i++) {
	gmm.gs.sensors[i].rawValue.disconnect(callWriteTouch[i]);
      }
      gmm.gs.gyro.data.disconnect(callWriteGyro);
    }
    connected = false;
    break;    
  default:
    break;
  }
  writeSensorLog(TbTType::CONNECTION);
}

void TbTLog::writeLog(std::string s) {
  sensorLogfile << helium::toString(helium::getSystemMillis()) << " " << s << std::endl;
}

void TbTLog::writeSensorLog(TbTType::SensorUpdate s, int id) {
  sensorLogfile << helium::toString(helium::getSystemMillis()) << " " << s << " " << id << ": ";
  switch(s) {
  case TbTType::CONNECTION: 
    sensorLogfile << gmm.gs.conn.get();
    break;
  case TbTType::POT: 
    for(size_t i=0;i<gmm.gs.joints.size();i++) {
      sensorLogfile << gmm.gs.joints[i].displayedTarget.get() << " ";
    }
    break;
  case TbTType::POWER: 
    for(size_t i=0;i<gmm.gs.joints.size();i++) {
      sensorLogfile << gmm.gs.joints[i].jointPower.get() << " ";
    }
    break;
  case TbTType::TOUCH: 
    teachTouchLog.push_front(std::make_pair(helium::getSystemMillis(),helium::Array<int>(gmm.gs.sensors.size())));
    for(size_t i=0;i<gmm.gs.sensors.size();i++) {
      sensorLogfile << gmm.gs.sensors[i].rawValue.get() << " ";
      teachTouchLog.front().second[i] = gmm.gs.sensors[i].rawValue.get();
    }
    if(teachTouchLog.size() > 10) {
      teachTouchLog.pop_back();
    }    
    break;
  case TbTType::GYRO: 
    {
      sensorLogfile << gmm.gs.gyro.data.get();
      teachGyroLog.push_front(std::make_pair(helium::getSystemMillis(),helium::Array<int>(gmm.gs.gyro.data.get().vals)));
      if(teachGyroLog.size() > 10) {
	teachGyroLog.pop_back();
      }      
    }
    break;
  default:    
    break;
  }
  sensorLogfile << std::endl;
}

TbTSetPower::TbTSetPower(cfg::guide::Joints &pjoints,cfg::guide::Teach::Power &power):
  joints(power.motor.size()),
  conn(this)
{
  for(size_t j=0;j<power.motor.size();j++) {
    joints[j] = &pjoints[power.motor[j]];
  }
}

void TbTSetPower::setPower(bool b) {
  //std::cout << "set ";
  for(size_t j=0;j<joints.size();j++) {
    //std::cout << joints[j]->getId() << " ";
    joints[j]->jointPower.assure(b);
  }
  //std::cout << (b?" on":" off") << std::endl;
}

TbTCheckPower::TbTCheckPower(cfg::guide::Sensors &psensors,cfg::guide::Teach::Power &power,const double& pthreshold):
  threshold(pthreshold),
  isTurnedOff(false),
  callCheckPower1(this,power.sensor[0].size()),
  callCheckPower2(this,power.sensor[0].size())
{
  for(size_t s=0;s<power.sensor.size();s++) {
    for(size_t ss=0;ss<power.sensor[s].size();ss++) {
      sensors[s].push_back(&psensors[power.sensor[s][ss]]);
    }
  }
}

void TbTCheckPower::setSensorsConnections(bool b) {
  if(b) {
    for(size_t s=0;s<sensors[0].size();s++) {
      sensors[0][s]->rawValue.connect(callCheckPower1[s]);
    }
    for(size_t s=0;s<sensors[1].size();s++) {
      sensors[1][s]->rawValue.connect(callCheckPower2[s]);
    }
  }
  else {
    for(size_t s=0;s<sensors[0].size();s++) {
      sensors[0][s]->rawValue.disconnect(callCheckPower1[s]);
    }
    for(size_t s=0;s<sensors[1].size();s++) {
      sensors[1][s]->rawValue.disconnect(callCheckPower2[s]);
    }
  }
}

void TbTCheckPower::checkPower() {
  bool sensoron[2];
  sensoron[0] = sensoron[1] = false;
  for(size_t s=0;s<sensors.size();s++) {
    for(size_t ss=0;ss<sensors[s].size();ss++) {
      if(sensors[s][ss]->getRatio() > threshold) {
	sensoron[s] = true;
	break;
      }
    }
  }
  bool shouldBeOn = !(sensoron[0] && sensoron[1]);
  if(shouldBeOn) {
    //check if it was turned off by sensor
    if(isTurnedOff) {
      signal(shouldBeOn);
      isTurnedOff = false;
    }
  }
  else {
    isTurnedOff = true;
    signal(shouldBeOn);
  }

}
