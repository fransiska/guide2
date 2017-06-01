/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDE_COLDETMANAGER
#define GUIDE_COLDETMANAGER
#include <helium/gtk/gtkSignal.h>
#include <helium/core/time.h>
#include <helium/robot/frame.h>
#include "guideMotionManagement.h"
#include "guideEvents.h"
#include <helium/convex/checkManager.h>
#include <helium/convex/convexModel.h>

namespace coldet{
  
  class	ColdetManager{
    GuideMotionManagement &gmm;
    helium::Signal<void>& motionChangedSignal;

  public:    
    helium::CheckManager checkManager;

  private:


    class Worker:public helium::Thread{
      bool running;
      helium::CheckManager& check;
      helium::convex::ConvexCollisionModel ccm;
      helium::Array<double> posture;           
      void run();
    public:
      Worker(helium::CheckManager& pcheck,helium::convex::ConvexCollisionModel& pccm,int n);
      ~Worker();
    };

    Worker worker; ///< calculation thread


  public:
    ColdetManager(GuideMotionManagement &pgmm,helium::Signal<void>& pmotionChangedSignal,helium::convex::ConvexCollisionModel& pccm);
    ~ColdetManager();

    void setTurbo(TurboSetting s); ///< \todo set to fasten coldet calculation

    //ColdetAutoFixWidget
    void copyAutoFix(); ///< copy the suggested non colliding posture to current time
    void copyAutoFix(helium::Time t); ///< \todo make sure the autofixposture is for time t

    helium::ValueHub<bool> isOnCollidingFrame; ///< signal if current frame is colliding
    void calculateIsOnColliding(); ///< call ColdetManager::calculateIsOnColliding() with previous coldet result
    void calculateIsOnColliding(coldet::SegmentNotification segment); ///< calculate on colliding upon coldet result update

    helium::TsSignal<const helium::Posture&> rotateModelToAutoFixPostureSignal; ///signal to rotate fix model to autoFixPosture

    /* ???
    void connect(helium::ConnReference<void>& c,helium::Time t);
    void disconnect(helium::ConnReference<void>& c);
    void setMotion(GuideEvent::MotionChangeReason, const helium::Motion& m);

    helium::TsSignal<void> autoReach; ///< \todo connected to button
    helium::TsSignal<void> retryReach; ///< \todo connected to button
    */

  private:
    helium::Posture autoFixPosture; ///< \todo dummy posture, need to be set by checkManager

    //calculate isOnCollidingFrame when: 1) curTime change, 2) coldet notification
    helium::StaticConnReference<helium::Time,ColdetManager,void,&ColdetManager::calculateIsOnColliding> callOnChangeCurTimeCalculateIsOnColliding;
    helium::StaticConnReference<coldet::SegmentNotification,ColdetManager,coldet::SegmentNotification,&ColdetManager::calculateIsOnColliding> callOnNotificationCalculateIsOnColliding;

    void setActive(bool active);
    helium::StaticConnReference<bool,ColdetManager,bool,&ColdetManager::setActive> callSetActive;    

    void checkMotion();
    helium::StaticConnReference<void,ColdetManager,void,&ColdetManager::checkMotion> callCheckMotion;    

    //onIsCollidingFrame rotate autoFixPosture
    helium::ReplaceParamStaticConnReference<bool,helium::TsSignal<const helium::Posture&>,const helium::Posture&,&helium::TsSignal<const helium::Posture&>::operator()> callRotateFixModel;

    /* ???
    helium::ValueHub<bool> robotCollide; ///< \todo signal that robot is going to a colliding frame

    void turnOffRobotCollide();
    void turnOffRobotCollideAuto();
    helium::StaticConnReference<void,ColdetManager,void,&ColdetManager::turnOffRobotCollideAuto> callTurnOffRobotCollideOnAuto;    /// \todo DELETE ?
    helium::StaticConnReference<void,ColdetManager,void,&ColdetManager::turnOffRobotCollide> callTurnOffRobotCollideOnRetry;    /// \todo DELETE ?

    void setSync(bool collision);
    helium::StaticConnReference<bool,ColdetManager,bool,&ColdetManager::setSync> callOnRobotCollideChangeSync;
    helium::StaticConnReference<bool,ColdetManager,bool,&ColdetManager::setSync> callOnFrameCollideChangeSync;

    void checkRobotCollision();    
    helium::Array<helium::StaticConnReference<double,ColdetManager,void,&ColdetManager::checkRobotCollision>,
		  helium::array::RESIZABLE,
		  helium::array::InternalAllocator<
		    helium::ParamAllocator<
		      helium::StaticConnReference<double,ColdetManager,void,&ColdetManager::checkRobotCollision>, ColdetManager*
		      > 
		    > 
		  >callCheckRobotCollision;
    */

  };						

}


#endif
