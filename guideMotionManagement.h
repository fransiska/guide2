/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDEMOTIONMANAGEMENT
#define GUIDEMOTIONMANAGEMENT

#include <gtkmm.h>
#include <helium/system/valueHub.h>
#include <helium/core/exception.h>
#include <helium/robot/framePolicy.h>
#include <helium/robot/motionParser.h>
#include <helium/gtk/gtkExceptionHandling.h>
#include <helium/util/mathUtil.h>
#include <app/guide2/guideState.h>
#include <app/guide2/guideLog.h>

namespace gwidget {
struct FrameSaveTo {
typedef enum {DISCARDFRAME,SELECTFRAME,THISFRAME,NUM} Type;
    static const char* strings[]; 
  };
}

inline std::pair<cfg::guide::Calib*,helium::JointPosConverter*> extractCalib(std::pair<cfg::guide::Joints*, helium::JointPosConverter*> p,int j);

class GuideMotionManagement {
public:

  /** \brief store the saved status for the motion file and calibration file
   */
  struct SavedStatus {
    helium::ValueHub<bool> motion;
    helium::ValueHub<bool> calib;
    SavedStatus();
  };

  /** \brief handles sync between frame and robot postures, for one joint
   */
  class GuideSync {
    cfg::guide::Joint &joint;

    void onRobotChange(double d); ///< on posture value change
    void onFrameChange(helium::FrameValue fv);  ///< on posture value change
    void onArrowRobotChange(cfg::guide::SyncType::Type s); ///< on arrow gui status change
    void onArrowFrameChange(cfg::guide::SyncType::Type s); ///< on arrow gui status change
    
  public:
    GuideSync(cfg::guide::Joint &pjoint);
    
    helium::StaticConnReference<double,GuideSync,double,&GuideSync::onRobotChange> CR;
    helium::StaticConnReference<helium::FrameValue,GuideSync,helium::FrameValue,&GuideSync::onFrameChange> CF;
    
    helium::StaticConnReference<cfg::guide::SyncType::Type,GuideSync,cfg::guide::SyncType::Type,&GuideSync::onArrowRobotChange> CAR;
    helium::StaticConnReference<cfg::guide::SyncType::Type,GuideSync,cfg::guide::SyncType::Type,&GuideSync::onArrowFrameChange> CAF;
  };

  GuideMotionManagement(GuideState &pgs, helium::GtkExceptionHandling &eh,
			helium::JointPosConverter& kpn,
			helium::MotionParser &pmp);

  GuideState &gs;  
  helium::GtkExceptionHandling &eh;
  helium::MotionParser &mp; 

  SavedStatus saved; ///< shows whether the files are saved or not
  //helium::TsSignal<bool> setUndo; ///< undo command
  //helium::TsSignal<bool> setRedo; ///< redo command
  helium::Posture copyPost; ///< saves posture being copied
  helium::ValueHub<bool> copying; ///< used as enable/disable in gui menu
  helium::ValueHub<std::string> motionFilename; ///< saves filename for display 
  long long t0; ///< indicating starting time for logging
  helium::FramePolicy framePolicy; ///< interpolation between frames
  
  /** @param robot play on robot or just on model, default is robot
   * @param play play or stop command, default is play
   * @param zero start from zero or current frame, default is zero
   * @param speed speed default is 1
   * @param repeat repeat default is 1
   */
  void playMotion(bool robot=true, bool play=true, bool zero=true, double speed=1, int repeat=1);
  void onMotionStateSignal(const std::pair<helium::framePlay::PlayState,helium::Time>& p);
  void insertFrame(helium::Time t);
  void insertFrameFromRobot(); ///< insert frame at 0.1 later (shift rest of frame) with current posture of robot
  /** \brief insert frame to motion with certain time, posture, and reason
   */
  template<GuideEvent::MotionChangeReason REASON> void insertFrameReason(const helium::Posture &post, helium::Time t=-1);
  /** \brief paste a posture to a frame, will change the current time to this time
   */
  template<GuideEvent::PostureChangeReason REASON, GuideEvent::JointChangeReason REASONJ> void pasteFrameReason(const helium::Posture &post, helium::Time t=-1);
  void deleteFrame(helium::Time t);
  void copyFrame();
  void copyFrame(const helium::Posture &p);
  void pasteFrame();
  void pasteFrame(helium::Time t);

  void linkFrame(helium::Time from,helium::Time to);
  void unlinkFrame(helium::Time frame);

  Glib::Dispatcher checkTicksDispatcher;
  bool adjustTime(helium::Time t, helium::Time dt);

  void newMotion();
  void openMotion(const char* filename);
  void saveMotion(const char* filename);
  void fileNotSavedWarning();
  void resetMotionParameter();
  double getPosture(int id, helium::Time t);

  void setFrameValue(helium::Time t);
  void setFrameValue(helium::Time t,int id);
  bool getLink(helium::Time t, std::set<helium::Time> &link);
  bool isLinked(helium::Time t);

  //action from widget
  void onChangeTime(helium::Time t);
  void onSaveFrame(gwidget::FrameSaveTo::Type type, helium::Time t=-1);
  void onEditing(); ///<add remove ticks when editing a temporary frame
  void discardFrame();
  void onFrameChange(helium::FrameValue fv,int id);

  void getFramePosture(double *d, helium::Time t);
  void zeroAll(cfg::guide::CursorType::Type t);
  void enableAll();
  void setJointValue(cfg::guide::CursorType::Type t, helium::Posture p);

private:
  void allocGuideSync(GuideSync *g, int i);
  helium::Array<GuideSync,helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::DynamicClassPlacerAllocator
						 <GuideSync, GuideMotionManagement> 
						 > > guideSync;

  template<cfg::guide::CursorType::Type c> void onSyncStateUpdate(cfg::guide::SyncType::Type t);

#ifdef _WIN32
  friend helium::StaticConnReference<cfg::guide::SyncType::Type,GuideMotionManagement,cfg::guide::SyncType::Type,&onSyncStateUpdate<cfg::guide::CursorType::ROBOT> >;
  friend helium::StaticConnReference<cfg::guide::SyncType::Type,GuideMotionManagement,cfg::guide::SyncType::Type,&onSyncStateUpdate<cfg::guide::CursorType::FRAME> >;
  helium::StaticConnReference<cfg::guide::SyncType::Type,GuideMotionManagement,cfg::guide::SyncType::Type,&onSyncStateUpdate<cfg::guide::CursorType::ROBOT> > callSyncStateRobot;
  helium::StaticConnReference<cfg::guide::SyncType::Type,GuideMotionManagement,cfg::guide::SyncType::Type,&onSyncStateUpdate<cfg::guide::CursorType::FRAME> > callSyncStateFrame;
#else
  helium::StaticConnReference<cfg::guide::SyncType::Type,GuideMotionManagement,cfg::guide::SyncType::Type,&GuideMotionManagement::onSyncStateUpdate<cfg::guide::CursorType::ROBOT> > callSyncStateRobot;
  helium::StaticConnReference<cfg::guide::SyncType::Type,GuideMotionManagement,cfg::guide::SyncType::Type,&GuideMotionManagement::onSyncStateUpdate<cfg::guide::CursorType::FRAME> > callSyncStateFrame;
#endif
  
  void updateFrameChangeFromEnable(helium::FrameValueType, int id);
  typedef helium::ParamStaticConnReference<helium::FrameValue,GuideMotionManagement,int,&GuideMotionManagement::onFrameChange> CallOnFrameChange; ///< refresh from jwp->bigjointstate
  helium::Array<CallOnFrameChange,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ParamIdxAllocator<CallOnFrameChange,GuideMotionManagement*> > 
		>callOnFrameChanges;
  void onChangeMotion(GuideEvent::MotionChangeReason reason); ///< check ticks (call checkTicksDispatcher)

  helium::StaticConnReference<helium::Time,GuideMotionManagement,helium::Time,&GuideMotionManagement::onChangeTime> callOnChangeTime;  
  helium::StaticConnReference<std::pair<helium::framePlay::PlayState,helium::Time>,GuideMotionManagement,const std::pair<helium::framePlay::PlayState,helium::Time>&,&GuideMotionManagement::onMotionStateSignal> callOnMotionStateSignal;
  helium::StaticConnReference<GuideEvent::MotionChangeReason,GuideMotionManagement,GuideEvent::MotionChangeReason,&GuideMotionManagement::onChangeMotion> callOnChangeMotion;

  void onEditingMotion(bool editing);
  helium::StaticConnReference<bool,GuideMotionManagement,bool,&GuideMotionManagement::onEditingMotion> callOnEditingMotion;

  typedef helium::ParamStaticConnReference<helium::FrameValueType,GuideMotionManagement,int,&GuideMotionManagement::updateFrameChangeFromEnable> CallOnFrameChangeFromEnable;
  helium::Array< CallOnFrameChangeFromEnable,
		 helium::array::RESIZABLE,
		 helium::array::InternalAllocator<helium::ParamIdxAllocator<CallOnFrameChangeFromEnable,GuideMotionManagement*> > >
  callOnFrameChangesFromEnable;

  void onViewChange(cfg::guide::GuideView::Type v);
  helium::StaticConnReference<cfg::guide::GuideView::Type,GuideMotionManagement,cfg::guide::GuideView::Type,&GuideMotionManagement::onViewChange> callOnViewChange;

  //array connection to set calib saved to false on change
  typedef helium::ReplaceTConstStaticConnReference<int,helium::ValueHub<bool>,const bool&,&helium::ValueHub<bool>::assure,bool,false> AssureCalibSaveState; 

  typedef   helium::Array<AssureCalibSaveState,helium::array::RESIZABLE,helium::array::InternalAllocator
			  <helium::ParamAllocator<AssureCalibSaveState,helium::ValueHub<bool>* > >
			  > AssureCalibSaveStateArray;

AssureCalibSaveStateArray assureCalibSaveStateMin,assureCalibSaveStateZero, assureCalibSaveStateMax;


  

};

#include "guideMotionManagement.hpp"

#endif
