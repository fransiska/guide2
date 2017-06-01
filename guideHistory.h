/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef HE_GUIDEHISTORY
#define HE_GUIDEHISTORY

#include <fstream>
#include <helium/container/historyStack.h>
#include <helium/util/signalChangeProtect.h>

#include "guideState.h"

//! Class for undo and redo stack events
class GuideHistory{
  GuideState& gs;
  long long t0; ///< the time guide is started, used to write time in log file

public:
  //! Overall events structure
  struct Event {
    GuideEvent::EventType type;    ///< type of event
    helium::Time time; ///< the cursor (HScale) time at which the event occurs
    int reason; ///< detailed reason using the GuideEvent::JointChangeReason or GuideEvent::MotionChangeReason
    int affectedJoint; ///< the joint that is changed in the event
    Event(GuideEvent::EventType ptype, helium::Time ptime, int reason, int paffectedJoint=-1);
  };

  //! Posture events structure
  class FramePosture:public helium::Array<helium::FrameValue>{
  public:    
    FramePosture(int n); ///< \param n number of robot's joints
  };

  struct CalibEvent{
    GuideEvent::CalibEventType type;
    int affectedJoint;
    CalibEvent(GuideEvent::CalibEventType ptype, int paffectedJoint);
  };


private:
  helium::InternalPresentHistoryStack<Event> event;  ///< internalstack means present value is saved inside the stack ///< we need to keep a copy as internal present,because we can only push the present and not the past
  helium::ExternalPresentHistoryStack<helium::FrameValue> frameValue;  ///< externalstack means present value of frameValue is taken from posture stack
  helium::InternalPresentHistoryStack<FramePosture> posture; ///< represents the whole joints robot's posture, presenting the multiple joint change at one time  
  helium::SignalChangeProtect<helium::InternalPresentHistoryStack<helium::Motion> > motion; 

  helium::InternalPresentHistoryStack<CalibEvent> calibEvent;  ///< 
  helium::ExternalPresentHistoryStack<int> calib;  ///< 
  helium::Array<helium::Array<int,3> > calibPrevValue;

  std::ofstream logFile; ///< the stream of the event log file

  void init(); ///< initialize stacks
  void writeLog(std::string s);                 ///< write event log to file, only string
  void writeLog(GuideEvent::EventType e, std::string s=""); ///< write event log to file, using GuideEvent::EventType
  template<typename CR> void writeLog(GuideEvent::EventType e, CR r, std::string s=""); ///< write event log to file, use the specific enum of the corresponding event as the reason

  void onJointChange(std::pair<helium::FrameValue,GuideEvent::JointChangeReason> v, int j); ///< push GuideHistory::frameValue stack according to reason
  void onMotionChange(GuideEvent::MotionChangeReason reason); ///< push GuideHistory::motion stack according to the reason
  void onPostureChange(GuideEvent::PostureChangeReason reason); ///< push GuideHistory::posture stack
  void onCurTimeChange(helium::Time t); ///< update current event's time if it points to the top of the GuideHistory::event stack 
  
  bool canSkip(std::pair<helium::FrameValue,GuideEvent::JointChangeReason> v, int j); ///< to limit pushing events for change on the same joint
public:
  GuideHistory(GuideState& gs,long long t0);   ///< have gs already initialized before passing it
  virtual ~GuideHistory();                     ///< close log file
  bool canUndo(); ///< see the GuideHistory::event stack
  bool canRedo(); ///< see the GuideHistory::event stack
  bool canUndoCalib(); ///< 
  bool canRedoCalib(); ///< 
  std::string getTopUndoEventString();
  std::string getTopRedoEventString();
  std::string getTopCalibUndoEventString();
  std::string getTopCalibRedoEventString();

  helium::Signal<void>& getMotionChangedSignal();

  void undo();
  void redo();
  void undoCalib();
  void redoCalib();

  void initCalib();  ///< initialize stacks
  void onCalibChange(int calibValue,std::pair<cfg::guide::CalibType::Type,int> calibId);

private:
  typedef helium::ParamStaticConnReference<std::pair
					   <helium::FrameValue, GuideEvent::JointChangeReason>,
					   GuideHistory,int,&GuideHistory::onJointChange> FVconnRef;  
  helium::Array<FVconnRef,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ParamIdxAllocator<FVconnRef,GuideHistory*> >
	> jointChangeConnRef;

  typedef helium::ParamStaticConnReference<int,GuideHistory,std::pair<cfg::guide::CalibType::Type,int>,&GuideHistory::onCalibChange> CalibConnRef; 
  void allocCalibConnRefMin(CalibConnRef *ccr, int i) { new (ccr) CalibConnRef(this, std::make_pair(cfg::guide::CalibType::MIN,i)); }
  void allocCalibConnRefZero(CalibConnRef *ccr, int i) { new (ccr) CalibConnRef(this, std::make_pair(cfg::guide::CalibType::ZERO,i)); }
  void allocCalibConnRefMax(CalibConnRef *ccr, int i) { new (ccr) CalibConnRef(this, std::make_pair(cfg::guide::CalibType::MAX,i)); }
  helium::Array<CalibConnRef,helium::array::RESIZABLE,helium::array::InternalAllocator
		<helium::ClassPlacerAllocator<CalibConnRef,GuideHistory,&GuideHistory::allocCalibConnRefMin> > > calibMinChangeConnRef;
  helium::Array<CalibConnRef,helium::array::RESIZABLE,helium::array::InternalAllocator
		<helium::ClassPlacerAllocator<CalibConnRef,GuideHistory,&GuideHistory::allocCalibConnRefZero> > > calibZeroChangeConnRef;
  helium::Array<CalibConnRef,helium::array::RESIZABLE,helium::array::InternalAllocator
		<helium::ClassPlacerAllocator<CalibConnRef,GuideHistory,&GuideHistory::allocCalibConnRefMax> > > calibMaxChangeConnRef;

  helium::StaticConnReference<GuideEvent::MotionChangeReason,GuideHistory,GuideEvent::MotionChangeReason,&GuideHistory::onMotionChange> onMotionChangeConnRef;
  helium::StaticConnReference<helium::Time,GuideHistory,helium::Time,&GuideHistory::onCurTimeChange> onCurTimeChangeConnRef;
  helium::StaticConnReference<GuideEvent::PostureChangeReason,GuideHistory,GuideEvent::PostureChangeReason,&GuideHistory::onPostureChange> onPostureChangeConnRef;

  
};//GuideHistory

inline std::ostream& operator<<( std::ostream& os, const GuideHistory::Event&  e);


#include "guideHistory.hpp"


#endif
