/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "guideHistory.h"

using std::endl;
using std::cout;
using std::cerr;

typedef helium::InternalPresentHistoryStack<helium::Motion> MotionStack;

GuideHistory::GuideHistory(GuideState& pgs,long long pt0):
  gs(pgs),t0(pt0),
  event(Event(GuideEvent::SENTINELEVENT,0,GuideEvent::SENTINELEVENT)),
  posture(FramePosture(gs.joints.size())),
  motion(helium::Motion(gs.joints.size())),
  calibEvent(CalibEvent(GuideEvent::CALIBSENTINELEVENT,0)),
  calibPrevValue(gs.joints.size()),
  jointChangeConnRef(this,gs.joints.size()),
  calibMinChangeConnRef(this,gs.joints.size()),
  calibZeroChangeConnRef(this,gs.joints.size()),
  calibMaxChangeConnRef(this,gs.joints.size()),
  onMotionChangeConnRef(this),
  onCurTimeChangeConnRef(this),
  onPostureChangeConnRef(this)
{  
  //connect to each joint's frame changes
  for(size_t j=0;j<gs.joints.size();j++){
    helium::ConnReference<std::pair<helium::FrameValue,GuideEvent::JointChangeReason> > &r=jointChangeConnRef[j];
    gs.joints[j].frameValue.connect(r);
    gs.joints[j].calib[cfg::guide::CalibType::MIN].connect(calibMinChangeConnRef[j]);
    gs.joints[j].calib[cfg::guide::CalibType::ZERO].connect(calibZeroChangeConnRef[j]);
    gs.joints[j].calib[cfg::guide::CalibType::MAX].connect(calibMaxChangeConnRef[j]);
    for(int i=0;i<3;i++) {
      calibPrevValue[j][i] = gs.joints[j].calib[i];
    }
  }
  //connect to change of motion
  gs.motionInfo.motionChange.connect(onMotionChangeConnRef);
  gs.joints.postureChange.connect(onPostureChangeConnRef);

  gs.pref.motionLogfile.append(".log");
  std::cout << "Logging motion to " << gs.pref.motionLogfile << std::endl;
  logFile.open(gs.pref.motionLogfile.c_str(), std::ios::out | std::ios::app);
  init();
  writeLog("Starting Guide2");
}

GuideHistory::~GuideHistory() {
  logFile.close();
}



void GuideHistory::init() {
  event.clear(Event(GuideEvent::SENTINELEVENT,0,GuideEvent::SENTINELEVENT));
  motion.set<const helium::Motion&,&MotionStack::clear>(gs.motionInfo.getMotion());
  posture.clear(GuideHistory::FramePosture(gs.joints.size()));
  frameValue.clear();
  gs.motionInfo.prevMotion = &motion.get().present();
}

void GuideHistory::writeLog(GuideEvent::EventType e, std::string s) {
  std::stringstream ss;
  ss << e; //20140716 changed so that it print string instead of numbers
  if(s.length())
    ss << " " << s;
  writeLog(ss.str());
}

void GuideHistory::writeLog(std::string s) {
  logFile << helium::toString(helium::getSystemMillis()) << " "<< s << ":"; //marks the starting of data
  /*1 (22) robot joints*/ for(size_t i=0;i<gs.joints.size();i++)  logFile << gs.joints[i].displayedTarget.get()<<" ";
  /*2 (90) sensor */  for(size_t i=0;i<gs.sensors.size();i++)  logFile << gs.sensors[i].rawValue.get()<<" ";
  /*3 (5) gyro */  logFile << gs.gyro.data.get().vals << " ";
  /*4 (1) motion:currentTime */  logFile << gs.motionInfo.curTime.get() << " ";
  /*5 (22) posture enable */ for(size_t i=0;i<gs.joints.size();i++)  logFile << gs.joints[i].frameValue.get().first<<" ";
  /*5 (22) posture value */ for(size_t i=0;i<gs.joints.size();i++)  logFile << gs.joints[i].frameValue.get().second<<" ";
  logFile << endl;
}

bool GuideHistory::canSkip(std::pair<helium::FrameValue,GuideEvent::JointChangeReason> v, int j) {
  //skips frame change at same joint at same time
  return
    ((event.present().type==GuideEvent::JOINTEVENT) && 
     (event.present().affectedJoint==j) && 
     (event.present().time==gs.motionInfo.curTime.get()));
}

void GuideHistory::onJointChange(std::pair<helium::FrameValue,GuideEvent::JointChangeReason> v, int j) {
  if(gs.verbose)
    cout << "onJointChange " << v.second << " " << j << " at " << gs.motionInfo.curTime.get()<< endl;
  helium::Time ct = gs.motionInfo.curTime.get();
  switch(v.second){
  case GuideEvent::DIRECTJOINTCHANGE:
  case GuideEvent::ENABLEJOINTCHANGE:
  case GuideEvent::SYNCJOINTCHANGE:
  case GuideEvent::COPYJOINTCHANGE:
    {
      if(!canSkip(v,j)) {
	std::stringstream ss;
	ss << "frame " << ct << " joint " << j;
	frameValue.pushPast(posture.present()[j]); //push the current posture[j] to the past	
	event.pushPresent(Event(GuideEvent::JOINTEVENT,ct,v.second,j));
	writeLog(GuideEvent::JOINTEVENT,v.second,ss.str());
      }
    }
  case GuideEvent::TIMESELECTIONJOINTCHANGE:
    //update current posture to keep track of the posture before joint change
    gs.getCurrentPosture(posture.present());     
    break;    
    //ignored due to posture event
  case GuideEvent::FIXPOSTUREJOINTCHANGE:
  case GuideEvent::PASTESTRINGJOINTCHANGE:
  case GuideEvent::UNDOJOINT:
  case GuideEvent::REDOJOINT:
  case GuideEvent::MIRRORJOINTS:
  case GuideEvent::ALLZEROJOINTCHANGE:
  case GuideEvent::ALLENABLEJOINTCHANGE:
  case GuideEvent::ALLSYNCJOINTCHANGE:
  case GuideEvent::ALLCOPYJOINTCHANGE:
  case GuideEvent::INITIALIZEJOINT:
  case GuideEvent::PASTELISTJOINT:
  case GuideEvent::PASTETEACHPOSTURE:

    break;
  default:
    {
      std::stringstream ss;
      ss << "Undefined JointChange action " << v.second;
      throw helium::exc::InvalidOperation(ss.str());
    }
    break;
  } 
}

void GuideHistory::onPostureChange(GuideEvent::PostureChangeReason reason) {
  if(gs.verbose)
    cout << "onPostureChange " << reason << endl;
  if(reason < GuideEvent::UNDOPOSTURE) {
    helium::Time ct = gs.motionInfo.curTime.get();
    posture.pushPast();
    gs.getCurrentPosture(posture.present());  //replace current posture with actual frameValue
    event.pushPresent(Event(GuideEvent::POSTUREEVENT,ct,reason)); 
    writeLog(GuideEvent::POSTUREEVENT,reason);
  } 
}

void GuideHistory::onMotionChange(GuideEvent::MotionChangeReason reason) {
  if(gs.verbose)
    cout << "onMotionChange " << reason<< endl;
  switch(reason) {
  case GuideEvent::SAVETHISFRAME:
  case GuideEvent::SAVESELECTFRAME:
  case GuideEvent::DISCARDFRAME:
  case GuideEvent::INSERTFRAME:
  case GuideEvent::INSERTFRAMEFROMROBOT:
  case GuideEvent::DELETEFRAME:
  case GuideEvent::PASTEFRAME:
  case GuideEvent::ADJUSTFRAME:
  case GuideEvent::RENAMEFRAME:
  case GuideEvent::LINKFRAME:
  case GuideEvent::UNLINKFRAME:
    {
      helium::Time ct = gs.motionInfo.curTime.get();
      motion.set<const helium::Motion&,&MotionStack::pushPresent>(gs.motionInfo.getMotion());
      posture.pushPast(); //also create a new stack for posture 
      gs.getCurrentPosture(posture.present());
      gs.motionInfo.prevMotion = &motion.get().present();   //the previous motion is moved
      event.pushPresent(Event(GuideEvent::MOTIONEVENT,ct,reason));
      std::stringstream ss;
      ss << "frame " << ct;
      writeLog(GuideEvent::MOTIONEVENT,reason,ss.str());
      break;
    }
 
  case GuideEvent::NEWMOTION:
  case GuideEvent::OPENMOTION:
    init();
    writeLog(GuideEvent::SENTINELEVENT,reason);    
    break;

    //ignored    
  case GuideEvent::UNDOMOTION:
  case GuideEvent::REDOMOTION:
  case GuideEvent::DIRECTFRAMECHANGE:
  case GuideEvent::INSERTTEMPFRAME:
  case GuideEvent::REPLACEBACKFRAME:
  case GuideEvent::UNDOINSERTTEMPFRAME:
  case GuideEvent::UNDOFRAME:
  case GuideEvent::DISCARDTEMPFRAME:
    //cerr << "ignored" << endl;
    break;
  default:
    {
      std::stringstream ss;
      ss << "Undefined MotionChange action " << reason;
      throw helium::exc::InvalidOperation(ss.str());
    }
    break;
  }
}

void GuideHistory::onCurTimeChange(helium::Time t) {
  if(!event.canForward())
    event.present().time = t;
}


bool GuideHistory::canUndo(){
  return event.canRewind();
}

bool GuideHistory::canRedo(){
  return event.canForward();
}

void GuideHistory::undo(){
  gs.joints.syncState[cfg::guide::CursorType::ROBOT].assure(cfg::guide::SyncType::DISCONNECT);    
  gs.joints.syncState[cfg::guide::CursorType::FRAME].assure(cfg::guide::SyncType::DISCONNECT);

  gs.motionInfo.curTime.setCache(event.previous().time); //so that frame value change will be done in this time

  switch(event.present().type){
  case GuideEvent::JOINTEVENT:
    {
      int j=event.present().affectedJoint;
      frameValue.rewind(posture.present()[j]); 

      gs.joints[j].frameValue.set<GuideEvent::UNDOJOINT>(posture.present()[j]);  

      writeLog(GuideEvent::JOINTEVENT,GuideEvent::UNDOJOINT);
    }
    break;
  case GuideEvent::POSTUREEVENT:
    {
      posture.rewind();

      for(size_t j=0;j<gs.joints.size();j++)
	gs.joints[j].frameValue.set<GuideEvent::UNDOJOINT>(posture.present()[j]);  

      writeLog(GuideEvent::POSTUREEVENT,GuideEvent::UNDOPOSTURE);
    }
    break;
  case GuideEvent::MOTIONEVENT:
    {     
      gs.motionInfo.motionEdit<GuideEvent::UNDOMOTION,void,const helium::Motion&,&helium::Motion::replace>(motion.get().previous());       

      for(size_t j=0;j<gs.joints.size();j++)
	gs.joints[j].frameValue.set<GuideEvent::UNDOJOINT>(posture.previous()[j]);

      motion.set<&MotionStack::rewind>(); //moves back the motion, make sure the motion is correct before rewinding
      posture.rewind(); //moves back the posture
      gs.motionInfo.prevMotion = &(motion.get()).present();

      writeLog(GuideEvent::MOTIONEVENT,GuideEvent::UNDOMOTION); 
    }
    break;
  default:
    throw helium::exc::InvalidOperation("Undo operation not defined");
  }
  event.rewind();

  if(event.present().type == GuideEvent::MOTIONEVENT || event.present().type == GuideEvent::SENTINELEVENT) {
    gs.motionInfo.motionEdit<GuideEvent::UNDOMOTION,void,const helium::Motion&,&helium::Motion::replace>(motion.get().present()); 
    gs.motionInfo.editing.status.assure(false);
  }
  gs.motionInfo.curTime.signalCache();
}

void GuideHistory::redo(){
  gs.joints.syncState[cfg::guide::CursorType::ROBOT].assure(cfg::guide::SyncType::DISCONNECT);    
  gs.joints.syncState[cfg::guide::CursorType::FRAME].assure(cfg::guide::SyncType::DISCONNECT);

  gs.motionInfo.curTime.setCache(event.future().time); //so that frame value change will be done in this time

  switch(event.future().type){
  case GuideEvent::JOINTEVENT:
    {
      int j=event.future().affectedJoint;
      frameValue.forward(posture.present()[j]); 

      //change frame
      gs.joints[j].frameValue.set<GuideEvent::REDOJOINT>(posture.present()[j]);  

      writeLog(GuideEvent::JOINTEVENT,GuideEvent::REDOJOINT);
    }
    break;
  case GuideEvent::POSTUREEVENT:
    {
      posture.forward();

      for(size_t j=0;j<gs.joints.size();j++)
	gs.joints[j].frameValue.set<GuideEvent::REDOJOINT>(posture.present()[j]);  

      writeLog(GuideEvent::POSTUREEVENT,GuideEvent::REDOPOSTURE);
    }
    break;
  case GuideEvent::MOTIONEVENT:
    {      
      gs.motionInfo.motionEdit<GuideEvent::REDOMOTION,void,const helium::Motion&,&helium::Motion::replace>(motion.get().future()); 
      gs.motionInfo.editing.status.assure(false);

      motion.set<&MotionStack::forward>();
      posture.forward(); //moves back the posture
      gs.motionInfo.prevMotion = &motion.get().present();

      writeLog(GuideEvent::MOTIONEVENT,GuideEvent::REDOMOTION); 
    }
    break;
  default:
    throw helium::exc::InvalidOperation("Redo operation not defined");
  }
  event.forward();
  gs.motionInfo.curTime.signalCache();
}

GuideHistory::FramePosture::FramePosture(int n):
  helium::Array<helium::FrameValue>(n) 
{
  setAll(*this,std::make_pair(helium::VALUEFRAME,0.0));      
};

bool GuideHistory::canRedoCalib() {
  return calibEvent.canForward();
}
bool GuideHistory::canUndoCalib() {
  return calibEvent.canRewind();
}

void GuideHistory::undoCalib() {
  int j = calibEvent.present().affectedJoint;
  cfg::guide::CalibType::Type c = (cfg::guide::CalibType::Type)(calibEvent.present().type-GuideEvent::CALIBMINCHANGEEVENT);

  calib.rewind(calibPrevValue[j][c]); //swap with most recent value

  switch(c) {
  case cfg::guide::CalibType::MIN: gs.joints[j].calib[c].assureExcept(calibPrevValue[j][c],calibMinChangeConnRef);break;
  case cfg::guide::CalibType::MAX: gs.joints[j].calib[c].assureExcept(calibPrevValue[j][c],calibMaxChangeConnRef);break;
  case cfg::guide::CalibType::ZERO: gs.joints[j].calib[c].assureExcept(calibPrevValue[j][c],calibZeroChangeConnRef);break;
  };
  calibEvent.rewind();
  writeLog(GuideEvent::CALIBEVENT,GuideEvent::CALIBUNDOEVENT);
}

void GuideHistory::redoCalib() {
  int j = calibEvent.future().affectedJoint;
  cfg::guide::CalibType::Type c = (cfg::guide::CalibType::Type)(calibEvent.future().type-GuideEvent::CALIBMINCHANGEEVENT);

  calib.forward(calibPrevValue[j][c]); //swap with most recent value

  calibEvent.forward(); //avoid onCalibChange called
  switch(c) {
  case cfg::guide::CalibType::MIN: gs.joints[j].calib[c].assureExcept(calibPrevValue[j][c],calibMinChangeConnRef);break;
  case cfg::guide::CalibType::MAX: gs.joints[j].calib[c].assureExcept(calibPrevValue[j][c],calibMaxChangeConnRef);break;
  case cfg::guide::CalibType::ZERO: gs.joints[j].calib[c].assureExcept(calibPrevValue[j][c],calibZeroChangeConnRef);break;
  };
  writeLog(GuideEvent::CALIBEVENT,GuideEvent::CALIBREDOEVENT);
}

void GuideHistory::initCalib() {
  for(size_t j=0;j<gs.joints.size();j++){
    for(int i=0;i<3;i++) {
      calibPrevValue[j][i] = gs.joints[j].calib[i];
    }
  }
  calibEvent.clear(CalibEvent(GuideEvent::CALIBSENTINELEVENT,0));
  calib.clear();
}

void GuideHistory::onCalibChange(int calibValue,std::pair<cfg::guide::CalibType::Type,int> calibId) {    
  int type = (GuideEvent::CALIBMINCHANGEEVENT+calibId.first);

  //only done for each new max/min/zero joint change
  if(!(calibEvent.present().type==(GuideEvent::CalibEventType)type && calibEvent.present().affectedJoint==calibId.second)) {
    calib.pushPast(calibPrevValue[calibId.second][calibId.first]);
    calibEvent.pushPresent(CalibEvent((GuideEvent::CalibEventType)type,calibId.second));
    writeLog(GuideEvent::CALIBEVENT,(GuideEvent::CalibEventType)type);
  }
  calibPrevValue[calibId.second][calibId.first] = calibValue;
}

std::string GuideHistory::getTopUndoEventString() {
  std::stringstream ss;
  switch(event.present().type) {
  case GuideEvent::JOINTEVENT: ss << "Frame "<< (GuideEvent::JointChangeReason)event.present().reason; break;
  case GuideEvent::POSTUREEVENT: ss << "Posture " << (GuideEvent::PostureChangeReason)event.present().reason; break;
  case GuideEvent::MOTIONEVENT: ss << "Motion " << (GuideEvent::MotionChangeReason)event.present().reason; break;
  default: ss << " "; break;
  }
  return ss.str();
}

std::string GuideHistory::getTopRedoEventString() {
  std::stringstream ss;
  switch(event.future().type) {
  case GuideEvent::JOINTEVENT: ss << "Frame "<< (GuideEvent::JointChangeReason)event.future().reason; break;
  case GuideEvent::POSTUREEVENT: ss << "Posture " << (GuideEvent::PostureChangeReason)event.future().reason; break;
  case GuideEvent::MOTIONEVENT: ss << "Motion " << (GuideEvent::MotionChangeReason)event.future().reason; break;
  default: ss << " "; break;
  }
  return ss.str();
}

std::string GuideHistory::getTopCalibUndoEventString() {
  std::stringstream ss;
  ss << calibEvent.present().type << " " << calibEvent.present().affectedJoint;
  return ss.str();
}

std::string GuideHistory::getTopCalibRedoEventString() {
  std::stringstream ss;
  ss << calibEvent.future().type << " " << calibEvent.future().affectedJoint;
  return ss.str();
}


GuideHistory::Event::Event(GuideEvent::EventType ptype, helium::Time ptime, int preason, int paffectedJoint):
  type(ptype),
  time(ptime),  
  reason(preason),
  affectedJoint(paffectedJoint)
{
}   

inline std::ostream& operator<<( std::ostream& os, const GuideHistory::Event&  e) {
  os << e.type << ": ";
  switch(e.type) {
  case GuideEvent::SENTINELEVENT:
    os << (GuideEvent::EventType)e.reason; break;
  case GuideEvent::JOINTEVENT:
    os << (GuideEvent::JointChangeReason)e.reason; break;
  case GuideEvent::POSTUREEVENT:
    os << (GuideEvent::PostureChangeReason)e.reason; break;
  case GuideEvent::MOTIONEVENT:
    os << (GuideEvent::MotionChangeReason)e.reason; break;
  default:
    os << "error";break;
  }
  os << " at " << e.time << " for " << e.affectedJoint;
  return os;
}

inline std::ostream& operator<<( std::ostream& os, const GuideHistory::CalibEvent&  e) {
  os << (GuideEvent::CalibEventType)e.type << " for " << e.affectedJoint;
  return os;
}

GuideHistory::CalibEvent::CalibEvent(GuideEvent::CalibEventType ptype, int paffectedJoint):
   type(ptype),affectedJoint(paffectedJoint)
{
}   

helium::Signal<void>& GuideHistory::getMotionChangedSignal() {
  return motion.changed;
}
