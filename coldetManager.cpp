/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "coldetManager.h"
using namespace coldet;
using std::endl;
using std::cout;


ColdetManager::ColdetManager(GuideMotionManagement &pgmm,helium::Signal<void>& pmotionChangedSignal,helium::convex::ConvexCollisionModel &coldetModel):
  gmm(pgmm),
  motionChangedSignal(pmotionChangedSignal),
  checkManager(gmm.gs.joints.size(),coldetModel),
  worker(checkManager,coldetModel,gmm.gs.joints.size()),
  isOnCollidingFrame(false),
  autoFixPosture(gmm.gs.joints.size()),
  callOnChangeCurTimeCalculateIsOnColliding(this),
  callOnNotificationCalculateIsOnColliding(this),
  callSetActive(this),
  callCheckMotion(this),
  callRotateFixModel(&rotateModelToAutoFixPostureSignal,autoFixPosture)
{
  setAll(autoFixPosture,0.1); ///< \todo set to zero

  gmm.gs.coldet.coldetState.connect(callSetActive); //turn on or off coldet state
}


ColdetManager::~ColdetManager(){
  checkManager.stop();
}


/// connections and set to check motion when coldet is activated
void ColdetManager::setActive(bool active) {
  //connection to checkmotion
  if(active) {
    motionChangedSignal.connect(callCheckMotion); //check coldet on motion change

    checkManager.notifications.connect(callOnNotificationCalculateIsOnColliding); //
    gmm.gs.motionInfo.curTime.connect(callOnChangeCurTimeCalculateIsOnColliding); // 

    isOnCollidingFrame.connect(callRotateFixModel);
    checkManager.checkMotion(gmm.gs.motionInfo.getMotion().getMotion()); //run coldet now
  }
  else {
    motionChangedSignal.disconnect(callCheckMotion);

    checkManager.notifications.disconnect(callOnNotificationCalculateIsOnColliding); //
    gmm.gs.motionInfo.curTime.disconnect(callOnChangeCurTimeCalculateIsOnColliding); // 

    setAll(autoFixPosture,0.0);
    isOnCollidingFrame = false;
    isOnCollidingFrame.disconnect(callRotateFixModel);
    //robotCollide.assure(false); ///\todo
  }
}

void ColdetManager::checkMotion() {
  checkManager.checkMotion(gmm.framePolicy.getInterpolatedMotionMap());
}

ColdetManager::Worker::Worker(helium::CheckManager& pcheck,helium::convex::ConvexCollisionModel& pccm,int n):
  running(true),check(pcheck),ccm(pccm,NULL),posture(n) {
  start();
}

ColdetManager::Worker::~Worker(){
  running=false;
  join();
}


void ColdetManager::Worker::run() {
  std::auto_ptr<helium::convexCheck::Work> myWork;
  while(running){      

    if (check.getWork(myWork)){
      double alpha;
      while((alpha=myWork->getNext())!=helium::convexCheck::NONE){
	for (size_t i=0;i<myWork->s->begin.size();i++){
	  posture[i]=myWork->s->begin[i]*(1-alpha)
	    + alpha*myWork->s->end[i];
	}
	ccm.rotate(posture);
	if (ccm.isColliding()){
	  myWork->collTime=alpha;
	  break;
	}
      }
      if (myWork->collTime==helium::convexCheck::UNKNOWN){
      	myWork->collTime=helium::convexCheck::NONE;
      }
      myWork.reset(); 
    }else{
      return;
    }
  }//while run
}


/** \brief copy the calculated solution of the colliding frame
 */
void ColdetManager::copyAutoFix() {
  gmm.pasteFrameReason<GuideEvent::FIXPOSTURE,GuideEvent::FIXPOSTUREJOINTCHANGE>(autoFixPosture);
}

void ColdetManager::copyAutoFix(helium::Time t) {
  gmm.pasteFrameReason<GuideEvent::FIXPOSTURE,GuideEvent::FIXPOSTUREJOINTCHANGE>(autoFixPosture, t);
}

/** \brief calculate the value of isOnCollidingFrame, find the segment with col == t
 */
void ColdetManager::calculateIsOnColliding() {
  calculateIsOnColliding(checkManager.getMotionNotification());
}

void ColdetManager::calculateIsOnColliding(coldet::SegmentNotification segment) {
  helium::Time t = gmm.gs.motionInfo.curTime.get();

  for(size_t i=0;i<segment.size();i++) {
    if(segment[i].coll == t) {
      autoFixPosture = gmm.gs.motionInfo.getMotion().getPosture(t);
      helium::mulScalar(autoFixPosture,0.9); ///< \todo set to proper auto fix posture
      isOnCollidingFrame = true;
      return;
    }
  }
  setAll(autoFixPosture,0.0);
  isOnCollidingFrame = false;
}

void ColdetManager::setTurbo(TurboSetting s) {
  throw helium::exc::Bug("ColdetManager::setTurbo not yet implemented");
}

/*
  callTurnOffRobotCollideOnAuto(this),
  callTurnOffRobotCollideOnRetry(this),
  callOnRobotCollideChangeSync(this),
  callOnFrameCollideChangeSync(this),
  callCheckRobotCollision(this,gmm.gs.joints.size()),

  robotCollide.set(false);
  //check collision bool when displayedtarget is changed
  for(size_t i=0;i<gmm.gs.joints.size();i++) {
    gmm.gs.joints[i].displayedTarget.connect(callCheckRobotCollision[i]);
  }
  //autoReach.connect(callTurnOffRobotCollideOnAuto);
  //retryReach.connect(callTurnOffRobotCollideOnRetry);

  //turn off sync on collision
  //robotCollide.connect(callOnRobotCollideChangeSync);
  //isOnCollidingFrame.connect(callOnFrameCollideChangeSync);

void ColdetManager::setSync(bool collision) {
  if(collision) {
    gmm.gs.joints.syncState[0].assure(cfg::guide::SyncType::DISCONNECT);
    gmm.gs.joints.syncState[1].assure(cfg::guide::SyncType::DISCONNECT);
  }
}

void ColdetManager::turnOffRobotCollideAuto() {
  //cout << "auto turn off" << endl;
 for(size_t i=0;i<gmm.gs.joints.size();i++) {
   gmm.gs.joints[i].desiredTarget = 0;
 }
}

void ColdetManager::turnOffRobotCollide() {
 for(size_t i=0;i<gmm.gs.joints.size();i++) {
   gmm.gs.joints[i].desiredTarget.signalCache();
 }
}

void ColdetManager::checkRobotCollision() {
  bool allOk = true;
  for(size_t i=0;i<gmm.gs.joints.size();i++) {
    if(gmm.gs.joints[i].displayedTarget.get() == helium::motor::Interface::COLLISIONPOS) {
      allOk = false;
      break;
    }
  }
  robotCollide.set(!allOk);
}
 */
