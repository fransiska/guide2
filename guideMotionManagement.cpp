/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include <iostream>
#include <sstream>
#include <app/guide2/guideMotionManagement.h>
#include <helium/util/ioUtil.h>
#include <helium/util/mathUtil.h>
#include <helium/signal/staticConnReference.h>
#include <queue>
#include <app/guide2/guideUtil.h>

using std::cout;
using std::cerr;
using std::endl;
using std::stringstream;
using std::string;
using std::queue;
using namespace cfg::guide;

const char* gwidget::FrameSaveTo::strings[] = {"Discard", "Select", "This", "Num"};

GuideMotionManagement::SavedStatus::SavedStatus():
  motion(true),calib(true) {
}

inline std::pair<cfg::guide::Calib*,helium::JointPosConverter*> extractCalib(std::pair<cfg::guide::Joints*, helium::JointPosConverter*> p,int j){
  return std::make_pair(&(*p.first)[j].calib,p.second);
}

void GuideMotionManagement::allocGuideSync(GuideSync *g, int i) {
  new (g) GuideSync(gs.joints[i]);
}

void GuideMotionManagement::GuideSync::onRobotChange(double d) {
  if(joint.frameValue.get().first != helium::VALUEFRAME) return; //do not change when disabled

  if(joint.prevSyncState[cfg::guide::CursorType::FRAME]<=cfg::guide::SyncType::CONNECT) {
    //avoid connection loop when frame is also synced
    if(joint.frameValue.get().second != joint.desiredTarget.get())
      joint.frameValue.assureExcept<GuideEvent::SYNCJOINTCHANGE>(std::make_pair(helium::VALUEFRAME,d),CF);
  }
  else {
    joint.frameValue.assure<GuideEvent::SYNCJOINTCHANGE>(std::make_pair(helium::VALUEFRAME,d));
  }
}
void GuideMotionManagement::GuideSync::onFrameChange(helium::FrameValue fv) {
  if(!joint.jointState.isConnected()) return; // do not change if it's not connected to robot
  if(!joint.jointPower.get()) return; //do not change if power is off
  //if(joint.frameValue.get().first != helium::VALUEFRAME) return; //only copy when enabled
  if(isnan(fv.second)) return; //do not copy invalid values

  if(joint.prevSyncState[cfg::guide::CursorType::ROBOT]<=cfg::guide::SyncType::CONNECT) {
    joint.desiredTarget.assureExcept(fv.second,CR);
  }
  else
    joint.desiredTarget.assure(fv.second);
}

void GuideMotionManagement::GuideSync::onArrowRobotChange(cfg::guide::SyncType::Type s) { 
  if(joint.frameValue.get().first != helium::VALUEFRAME) return; //do not change when disabled

  switch(s){
  case cfg::guide::SyncType::ALLCONNECT:
    if(joint.prevSyncState[cfg::guide::CursorType::ROBOT]>cfg::guide::SyncType::CONNECT) {
      joint.frameValue.assure<GuideEvent::ALLSYNCJOINTCHANGE>(std::make_pair
							      (helium::VALUEFRAME,
							    joint.desiredTarget.get()
							    ));
      joint.desiredTarget.connect(CR);
      joint.prevSyncState[cfg::guide::CursorType::ROBOT] = s;
    }
    break;
  case cfg::guide::SyncType::CONNECT:
    if(joint.prevSyncState[cfg::guide::CursorType::ROBOT]>cfg::guide::SyncType::CONNECT) {
      joint.frameValue.assure<GuideEvent::SYNCJOINTCHANGE>(std::make_pair
							   (helium::VALUEFRAME,
							    joint.desiredTarget.get()
							    ));
      joint.desiredTarget.connect(CR);
      joint.prevSyncState[cfg::guide::CursorType::ROBOT] = s;
    }
    break;
  case cfg::guide::SyncType::DISCONNECT:
    if(joint.prevSyncState[cfg::guide::CursorType::ROBOT]<=cfg::guide::SyncType::CONNECT) {
      joint.desiredTarget.disconnect(CR);
      joint.prevSyncState[cfg::guide::CursorType::ROBOT] = s;
    }
    break;
  case cfg::guide::SyncType::COPY:
    if(joint.prevSyncState[cfg::guide::CursorType::ROBOT]>cfg::guide::SyncType::CONNECT) {
      joint.frameValue.assure<GuideEvent::COPYJOINTCHANGE>(std::make_pair
							   (joint.frameValue.get().first,
							    joint.desiredTarget.get()
							    ));    
    }
    break;
  case cfg::guide::SyncType::ALLCOPY:
    if(joint.prevSyncState[cfg::guide::CursorType::ROBOT]>cfg::guide::SyncType::CONNECT) {
      joint.frameValue.assure<GuideEvent::ALLCOPYJOINTCHANGE>(std::make_pair
							   (joint.frameValue.get().first,
							    joint.desiredTarget.get()
							    ));
    }
    break;
  case cfg::guide::SyncType::NUM:
    break;
  }
}

void GuideMotionManagement::GuideSync::onArrowFrameChange(cfg::guide::SyncType::Type s) {
  if(!joint.jointState.isConnected()) return; // do not change if it's not connected to robot
  if(!joint.jointPower.get()) return; //do not change if power is off

  switch(s) {
  case cfg::guide::SyncType::ALLCONNECT:
  case cfg::guide::SyncType::CONNECT:
    if(joint.prevSyncState[cfg::guide::CursorType::FRAME]>cfg::guide::SyncType::CONNECT) {
      joint.desiredTarget.assure(joint.frameValue.get().second);
      joint.frameValue.connect(CF);
      joint.prevSyncState[cfg::guide::CursorType::FRAME] = s;      
    }
    break;
  case cfg::guide::SyncType::DISCONNECT:
    if(joint.prevSyncState[cfg::guide::CursorType::FRAME]<=cfg::guide::SyncType::CONNECT) {
      joint.frameValue.disconnect(CF);
      joint.prevSyncState[cfg::guide::CursorType::FRAME] = s;      
    }
    break;
  case cfg::guide::SyncType::ALLCOPY:
  case cfg::guide::SyncType::COPY:
    joint.desiredTarget.assure(joint.frameValue.get().second);
    break;
  default: break;
  }
}
GuideMotionManagement::GuideSync::GuideSync(cfg::guide::Joint &pjoint):
  joint(pjoint),CR(this),CF(this),CAR(this),CAF(this) {
  for(size_t i=0;i<joint.prevSyncState.size();i++) {
    joint.prevSyncState[i] = cfg::guide::SyncType::NUM;
  }
}

/*void GuideMotionManagement::assureCalibSaveStateFalse(AssureCalibSaveState *ccr, int i){
  new (ccr) AssureCalibSaveState(&saved.calib, false); 
}
*/


GuideMotionManagement::GuideMotionManagement(GuideState &pgs, helium::GtkExceptionHandling &peh,
		     helium::JointPosConverter &kpn,
		     helium::MotionParser &pmp):
  gs(pgs),eh(peh),mp(pmp),
  copyPost(gs.joints.size()),
  copying(false),
  motionFilename(""),
  framePolicy(gs.motionInfo.getMotion()),
  guideSync(std::make_pair(this,&GuideMotionManagement::allocGuideSync),gs.joints.size()),
  callSyncStateRobot(this),
  callSyncStateFrame(this),
  callOnFrameChanges(this,gs.joints.size()),
  callOnChangeTime(this),
  callOnMotionStateSignal(this),
  callOnChangeMotion(this),
  callOnEditingMotion(this),
  callOnFrameChangesFromEnable(this,gs.joints.size()),
  callOnViewChange(this),
  assureCalibSaveStateMin(&saved.calib,gs.joints.size()),
  assureCalibSaveStateZero(&saved.calib,gs.joints.size()),
  assureCalibSaveStateMax(&saved.calib,gs.joints.size())
{  
  for(size_t i=0; i<gs.joints.size();i++) {
    //at change of frame value or frame enable
    gs.joints[i].frameValue.connect(callOnFrameChanges[i]);
    gs.joints[i].toggleEnable.connect(callOnFrameChangesFromEnable[i]);

    gs.joints[i].syncState[CursorType::ROBOT].assure(cfg::guide::SyncType::DISCONNECT);
    gs.joints[i].syncState[CursorType::FRAME].assure(cfg::guide::SyncType::DISCONNECT);

    gs.joints[i].syncState[CursorType::ROBOT].connect(guideSync[i].CAR);
    gs.joints[i].syncState[CursorType::FRAME].connect(guideSync[i].CAF);

    gs.joints[i].calib.absLim.minv.connect(assureCalibSaveStateMin[i]);
    gs.joints[i].calib.absLim.zero.connect(assureCalibSaveStateZero[i]);
    gs.joints[i].calib.absLim.maxv.connect(assureCalibSaveStateMax[i]);
  }

  gs.motionInfo.curTime.connect(callOnChangeTime);
  gs.motionInfo.framePlayState.connect(callOnMotionStateSignal);
  gs.motionInfo.motionChange.connect(callOnChangeMotion);
  gs.motionInfo.editing.status.connect(callOnEditingMotion);
  gs.view.connect(callOnViewChange);

  gs.joints.syncState[CursorType::ROBOT].connect(callSyncStateRobot);
  gs.joints.syncState[CursorType::FRAME].connect(callSyncStateFrame);

  newMotion(); 
}

//on change of framesaveto: get action on how to save the current posture
void GuideMotionManagement::onSaveFrame(gwidget::FrameSaveTo::Type type,helium::Time t) {
  helium::Time ct = gs.motionInfo.curTime.get();

  //saving frame
  switch(type) {
  case gwidget::FrameSaveTo::NUM : 
  default: return; break;
  case gwidget::FrameSaveTo::THISFRAME :
    {
      ///get the name of the frame previously
      std::stringstream ss;
      ss << "Frame " << ct;
      string name = ss.str();
      if(gs.motionInfo.getMotion().isFrame(ct)) {
	name = gs.motionInfo.getMotion()[ct].name;
      }
      helium::Posture p(gs.getCurrentPosture());
      p.name = name;
      insertFrameReason<GuideEvent::SAVETHISFRAME>(p);
    }
    break;
  case gwidget::FrameSaveTo::SELECTFRAME : 
    if(t<0) return; //the user did not save/discard the temp. frame
    { 
      helium::Posture cp(gs.getCurrentPosture());
    
      if(!(*gs.motionInfo.prevMotion).isFrame(ct))
	gs.motionInfo.motionEdit<GuideEvent::DISCARDTEMPFRAME,void,helium::Time,&helium::Motion::deleteFrame> (ct); //deleteFrame(config.motionInfo.curTime.get());    
      else {
	helium::Posture p((*gs.motionInfo.prevMotion)[ct]);
	insertFrameReason<GuideEvent::REPLACEBACKFRAME>(p);
      }    
      insertFrameReason<GuideEvent::SAVESELECTFRAME>(cp,t);
      ct = t;
    }
    break;
  case gwidget::FrameSaveTo::DISCARDFRAME :
    if(!(*gs.motionInfo.prevMotion).isFrame(ct)) {
      gs.motionInfo.motionEdit<GuideEvent::DISCARDFRAME,void,helium::Time,&helium::Motion::deleteFrame> (ct); //deleteFrame(config.motionInfo.curTime.get());    
    }
    else {
      helium::Posture p((*gs.motionInfo.prevMotion)[ct]);
      insertFrameReason<GuideEvent::DISCARDFRAME>(p,ct);
    }
    break;
  }
  gs.motionInfo.editing.status = false; //save frame is the only way to confirm motion
  gs.motionInfo.curTime.set(ct);
  //gs.motionInfo.editing.temporary = false; //tempFrame = false;
}

void GuideMotionManagement::onChangeMotion(GuideEvent::MotionChangeReason reason) {
  saved.motion.assure(false);
}

//on change of curTime: set the display to the frame's value, 
void GuideMotionManagement::onChangeTime(helium::Time t) {
  setFrameValue(t); //set frame value from motion 
}

//on change of frame joint value: set editing, save current posture
//   if it's different from initial state, remind user (editing=true)
void GuideMotionManagement::onFrameChange(helium::FrameValue fv, int id) { //signals
  //make sure there is a keyframe for the posture being edited
  if(!gs.motionInfo.getMotion().isFrame(gs.motionInfo.curTime.get()))
    insertFrameReason<GuideEvent::INSERTTEMPFRAME>(gs.getCurrentPosture());

  //update motion first
  if(fv.first == helium::INTERPOLATEFRAME) {
    gs.motionInfo.motionEdit<GuideEvent::DIRECTFRAMECHANGE,void,helium::Time,int,double,&helium::Motion::modifyFrame>(gs.motionInfo.curTime.get(),id,NAN);
  }
  else {
    gs.motionInfo.motionEdit<GuideEvent::DIRECTFRAMECHANGE,void,helium::Time,int,double,&helium::Motion::modifyFrame>(gs.motionInfo.curTime.get(),id,fv.second);
  }

  //make sure the correct framevalue is displayed (including enable/disable frame)
  setFrameValue(gs.motionInfo.curTime.get(),id);
  //gs.motionInfo.curTime.signalCache();
  gs.motionInfo.editing.status.assure(true);  
}

// activated by signal from widget
//   handle enable change here
void GuideMotionManagement::updateFrameChangeFromEnable(helium::FrameValueType fv, int id) { //signals
  onFrameChange(std::make_pair
		(fv,framePolicy.getPosture(id,gs.motionInfo.curTime.get())),
		id);
}

/// \brief move current time to t and create new frame with current posture
void GuideMotionManagement::insertFrame(helium::Time t) {
  insertFrameReason<GuideEvent::INSERTFRAME>(gs.getCurrentPosture(),t);
  gs.motionInfo.curTime.set(t);
}


void GuideMotionManagement::deleteFrame(helium::Time t) {
  ///cannot delete frame 0
  if(t==0)
    return;
  ///cannot delete frame while editing
  if(gs.motionInfo.editing.status.get())
    return;
  gs.motionInfo.motionEdit<GuideEvent::DELETEFRAME,void,helium::Time,&helium::Motion::deleteFrame>(t); //deleteFrame(config.motionInfo.curTime.get());    

  /// set curTime to the last point 
  helium::Time now = gs.motionInfo.curTime.get();
  helium::Time last = gs.motionInfo.getMotion().getEndTime();
  if(now > last)
    gs.motionInfo.curTime.set(last);
  else
    gs.motionInfo.curTime.signalCache(); //signal the time update to refresh the frameValue //onChangeTime();
}


/** \brief copy current posture (guideMenu)
 */
void GuideMotionManagement::copyFrame() { 
  gs.getCurrentPosture(copyPost);
  if(gs.motionInfo.getMotion().isFrame(gs.motionInfo.curTime.get()))
    copyPost.name = (gs.motionInfo.getMotion().getPosture(gs.motionInfo.curTime.get()).name);
}

/** \brief copy specified posture (ticksArea)
 */
void GuideMotionManagement::copyFrame(const helium::Posture &p) { 
  copyPost = p;
}

/** \brief copy to current time (guideMenu)
 */
void GuideMotionManagement::pasteFrame() {
  insertFrameReason<GuideEvent::PASTEFRAME>(copyPost);
  gs.motionInfo.curTime.signalCache();
}

/** \brief paste copyframe to cursor time t (ticksArea)
 */
void GuideMotionManagement::pasteFrame(helium::Time t) {
  insertFrameReason<GuideEvent::PASTEFRAME>(copyPost,t);
  gs.motionInfo.curTime.set(t);
}

/** \brief check to time and link frame 
 */
void GuideMotionManagement::linkFrame(helium::Time from,helium::Time to) {  
  if(to != from && to>=0) {
    gs.motionInfo.curTime.setCache(to);
    gs.motionInfo.motionEdit<GuideEvent::LINKFRAME,void,helium::Time,helium::Time,&helium::Motion::linkFrame>(from,to);
    gs.motionInfo.curTime.signalCache();
  }
  else 
    cerr << "Destination frame is less than 0 or the same as source frame" << endl;
}

void GuideMotionManagement::unlinkFrame(helium::Time frame) {    
  gs.motionInfo.motionEdit<GuideEvent::UNLINKFRAME,void,helium::Time,&helium::Motion::unlinkFrame>(frame);
}

void GuideMotionManagement::newMotion() {
  motionFilename.set("");

  //clear ticks and previous motions
  gs.joints.syncState[CursorType::ROBOT] = (cfg::guide::SyncType::DISCONNECT);//syncFromFrame(gwidget::SyncType::DISCONNECT);
  gs.joints.syncState[CursorType::FRAME] = (cfg::guide::SyncType::DISCONNECT);//syncFromRobot(gwidget::SyncType::DISCONNECT);

  for(size_t i=0;i<gs.joints.size();i++)
    gs.joints[i].frameValue.assure<GuideEvent::INITIALIZEJOINT>(std::make_pair(helium::VALUEFRAME,0.0));

  gs.motionInfo.motionEdit<GuideEvent::NEWMOTION,void,&helium::Motion::clear>();   //gs.motion.clear();
  resetMotionParameter();
}

void GuideMotionManagement::openMotion(const char* name) {
  helium::Motion motion(gs.joints.size());
  motion.robot = gs.pref.robot;
  mp.openMotion(motion,name);
  gs.motionInfo.motionEdit<GuideEvent::OPENMOTION,void,const helium::Motion&,&helium::Motion::replace>(motion);
  resetMotionParameter();
  
  motionFilename = name;
  if(gs.verbose)
    cout << "Opened motion " << motionFilename << endl;
}

void GuideMotionManagement::saveMotion(const char* name) {
  mp.saveMotion(gs.motionInfo.getMotion(),name);
  if(gs.verbose)
    cout << "Saved motion to " << name << endl;
}


void GuideMotionManagement::resetMotionParameter() {
  gs.motionInfo.editing.status.assure(false);
  //gs.motionInfo.editing.temporary = false;
  
  zero(copyPost);

  gs.motionInfo.mp.playSpeed = 1;
  gs.motionInfo.mp.iters = 1;
  gs.motionInfo.mp.startTime = 0;
  gs.motionInfo.curTime = 0; // will call onchangetime

  saved.motion.assure(true);
}

double GuideMotionManagement::getPosture(int id, helium::Time t) {
  //call the framepolicy function
  return framePolicy.getPosture(id,t);
}

bool GuideMotionManagement::getLink(helium::Time t, std::set<helium::Time> &link) {
  gs.motionInfo.getMotion().getLink(link,t);
  return link.size()>1;
}

bool GuideMotionManagement::isLinked(helium::Time t) {
  return gs.motionInfo.getMotion().isLinked(t);
}

void GuideMotionManagement::setFrameValue(helium::Time t) {
  for(size_t i=0;i<gs.joints.size();i++) {
    //set frameValue without calling frameValue
    gs.joints[i].frameValue.assureExcept<GuideEvent::TIMESELECTIONJOINTCHANGE>
      (std::make_pair
       ((gs.motionInfo.getMotion().isFrame(t) && 
	 isnan(gs.motionInfo.getMotion()[t][i]))?
	helium::INTERPOLATEFRAME:
	helium::VALUEFRAME,
	getPosture(i,t)),
       callOnFrameChanges[i]);
  }
}

void GuideMotionManagement::setFrameValue(helium::Time t, int i) {
  //set frameValue without calling frameValue
  gs.joints[i].frameValue.assureExcept<GuideEvent::TIMESELECTIONJOINTCHANGE>
    (std::make_pair
     ((gs.motionInfo.getMotion().isFrame(t) && 
       isnan(gs.motionInfo.getMotion()[t][i]))?
      helium::INTERPOLATEFRAME:
      helium::VALUEFRAME,
      getPosture(i,t)),
     callOnFrameChanges[i]);
}


/** \brief do checking, make sure the adjustment of time is feasible before calling adjustTime, update curTime appropriately
 * \param t the point to adjust time
 * \param dt the delta time to be added (can be negative)
 */
bool GuideMotionManagement::adjustTime(helium::Time t, helium::Time dt) {
  //moved to robot/frame.h
  // ///check feasibility of adjust time
  // if(gs.motionInfo.getMotion().getSize()<2) 
  //   throw helium::exc::InvalidDataValue("Cannot adjust time of motion with 1 frame");
  // 
  // if(dt == 0)
  //   throw helium::exc::InvalidDataValue("Cannot adjust time of 0");

  ///gets prev and next frame off the given time <B>t</B>, prev is self if t is frame
  helium::Time prev = (gs.motionInfo.getMotion().isFrame(t) && gs.motionInfo.getMotion().getEndTime() != t)?t:gs.motionInfo.getMotion().getPrevTime(t);
  helium::Time next = gs.motionInfo.getMotion().getNextTime(t);

  //moved to robot/frame.h
  // ///cannot adjust time if prev and next overlaps
  // if(prev >= next + dt) {
  //   throw helium::exc::InvalidDataValue("Overlapping existing frame time");//return false;
  // }

  /// \todo saves curTime to base off, is this needed?
  helium::Time ct = gs.motionInfo.curTime.get();

  ///call to adjust time
  gs.motionInfo.motionEdit<GuideEvent::ADJUSTFRAME,void,helium::Time,helium::Time,&helium::Motion::adjustTime>(t,dt);

  ///adjust current time if it is shifted so that the robot posture is not much changed
  if(ct > prev) {
    if(ct < next) {
      ///interpolate for in between 
      gs.motionInfo.curTime = prev + (ct-prev) * (double)(gs.motionInfo.getMotion().getNextTime(ct)-prev)/(double)(next-prev);    
    }
    else
      ///just add if curTime is a frame
      gs.motionInfo.curTime = ct+dt;
  }
  return true;
}

void GuideMotionManagement::getFramePosture(double *d, helium::Time t) {
  for(size_t i=0;i< gs.joints.size();i++) {
    d[i] = framePolicy.getPosture(i,t);
  }
}

void GuideMotionManagement::playMotion(bool robot, bool play, bool zero, double speed, int repeat) {
  if(!robot) return; /// \TODO do play for frame
  if(play) {
    gs.motionInfo.mp.playSpeed = speed;
    gs.motionInfo.mp.iters = repeat;

    gs.joints.syncState[CursorType::ROBOT].assure(cfg::guide::SyncType::DISCONNECT);
    gs.joints.syncState[CursorType::FRAME].assure(cfg::guide::SyncType::DISCONNECT);

    gs.motionInfo.target(gs.motionInfo.getMotion()); //playing = true;
    if(gs.verbose)
      cout << "playing started x" << speed << " " << repeat << "x, from " << (zero?0:gs.motionInfo.curTime.get()) <<   " playing "  << gs.motionInfo.framePlayState.get().first<< endl;
  }
  else {
    if (gs.motionInfo.framePlayState.get().first==helium::framePlay::PLAYING){
    gs.motionInfo.framePlayState.set(helium::framePlay::RequestStop());     //gs.motionInfo.playing.assureExcept(false,callOnStopPlayMotionSignal);
    }
  }
}

/// \todo delete?
void GuideMotionManagement::onMotionStateSignal(const std::pair<helium::framePlay::PlayState,helium::Time>& p) {
}

void GuideMotionManagement::onEditingMotion(bool editing) {
}

void GuideMotionManagement::insertFrameFromRobot() {
  helium::Posture p(gs.joints.size()); 
  gs.getCurrentRobotPosture(p);
  gs.motionInfo.motionEdit<GuideEvent::INSERTFRAMEFROMROBOT,void,helium::Time,helium::Time,const helium::Posture&,&helium::Motion::insertShiftFrame>(gs.motionInfo.curTime.get(),100,p);
  gs.motionInfo.curTime = gs.motionInfo.curTime.get()+100;
}

void GuideMotionManagement::onViewChange(cfg::guide::GuideView::Type v) {
  gs.joints.syncState[CursorType::ROBOT].assure(cfg::guide::SyncType::DISCONNECT);
  gs.joints.syncState[CursorType::FRAME].assure(cfg::guide::SyncType::DISCONNECT);  
}
