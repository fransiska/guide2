/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef MOTIONINFO
#define MOTIONINFO

#include <helium/gtk/gtkProxy.h>
#include <helium/robot/frame.h> 
#include <helium/robot/motionAdapter.h>
#include <helium/system/valueHub.h>
#include "guideEvents.h"
struct MotionInfo {
 
public:
  helium::GtkProxy<helium::framePlay::FramePlayControlSignature> framePlayState;
  helium::ValueProxy<helium::TargetCellBaseSignature<
		       helium::framePlay::Target > > framePlayTarget; //guideLogic writes here

  //operated by guide
  helium::Signal<const helium::Motion&> target; //guidelogic listens here, and then sets framePlayTarget
  

  //guimotion
 
  helium::ValueHub<helium::Time> curTime;
  struct Editing {
    Editing(bool s, bool t):status(s) {} 
    helium::ValueHub<bool> status;
  } editing;


  helium::MotionParameter mp;
 

  MotionInfo();

  const helium::Motion& getMotion() const{
    return motion;
  }

  bool isPlaying(){
    return framePlayState.get().first==helium::framePlay::PLAYING;  
  }
  
  helium::Signal<GuideEvent::MotionChangeReason > motionChange;

  
  template<GuideEvent::MotionChangeReason reason,typename R,R (helium::Motion::*f)()> 
  void motionEdit(){
    (motion.*f)();
    motionChange(reason);
    
  } 

  template<GuideEvent::MotionChangeReason reason,typename R,typename P,R (helium::Motion::*f)(P p)> 
  void motionEdit(typename helium::constReference<P>::value p){
    (motion.*f)(p);
    motionChange(reason);
  } 

  template<GuideEvent::MotionChangeReason reason,typename R, typename P,typename P2,R (helium::Motion::*f)(P p,P2 p2)> 
  void motionEdit(typename helium::constReference<P>::value p,typename helium::constReference<P2>::value p2){
    (motion.*f)(p,p2);
    motionChange(reason);
  } 

  template<GuideEvent::MotionChangeReason reason,typename R, typename P,typename P2,typename P3,R (helium::Motion::*f)(P p,P2 p2,P3 p3)> 
  void motionEdit(typename helium::constReference<P>::value p,typename helium::constReference<P2>::value p2,typename helium::constReference<P3>::value p3){
    (motion.*f)(p,p2,p3);
    motionChange(reason);
  } 

  template<GuideEvent::MotionChangeReason reason,typename R,typename P,typename P2,R (helium::Motion::*f)(P p,P2 p2)> 
  R motionEditR(typename helium::constReference<P>::value p,typename helium::constReference<P2>::value p2){
    R r = (motion.*f)(p,p2);
    motionChange(reason);
    return r;
  } 

  /** \brief 1 reference parameter, return bool, signal only when returned true
   * \deprecated not used
   */
  template<GuideEvent::MotionChangeReason reason,typename P,typename P2,typename N,bool (helium::Motion::*f)(P p,P2 p2,N n)>
  bool motionEditBN(typename helium::constReference<P>::value p,typename helium::constReference<P2>::value p2,typename helium::natReference<N>::value n){
    bool b = (motion.*f)(p,p2,n);
    if(b)
      motionChange(reason);    
    return b;
  } 


private:
  helium::Motion motion;
public:
  const helium::Motion *prevMotion; ///< used to save the previous posture to be used when discarding temporary frame / select different frame
};//MotionInfo


#endif
