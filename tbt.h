/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.06.17
 * \version 0.1
 * Release_flags g
 */

#ifndef TBT
#define TBT

#include "teachIO.h"
#include "robotModels.h"
#include "guideMotionManagement.h"
#include <helium/container/historyStack.h>

namespace TbTType {
  enum TeachIOChangeReason {ACQUIRE,ACQUIRED,DELETE,NEW,OPEN,UNDO,REDO,OPENNEW,OPENACQUIRE,OPENACQUIRED,DUMMY};
  enum State {DISCONNECTED,CONNECTED,TOUCHENABLE,NOTOUCH,TEACHING};
  enum SensorUpdate {CONNECTION,POWER,POT,TOUCH,GYRO};
};

inline std::ostream& operator <<(std::ostream& o,const TbTType::TeachIOChangeReason &reason);
inline std::ostream& operator <<(std::ostream& o,const TbTType::SensorUpdate &update);

struct TbTSignals {
  helium::Signal<const TeachPostureInput&> acquire;
  helium::Signal<const TeachPosture&> acquired;
  helium::Signal<int> del;
  helium::Signal<void> clear;
  helium::Signal<void> undo;
  helium::Signal<void> redo;
};

class TbTFilter {
  const cfg::guide::Teach &teach;
public:
  double elapsedTime;
  TbTFilter(const cfg::guide::Teach &pteach);
  virtual void filterInput(TeachPostureInput &);
  virtual void filterOutput(helium::Posture &);
  virtual void filterTouch(helium::Array<double> &ratio);
};

class TbTLog {
  GuideMotionManagement &gmm;
  RobotModels *rm;
  std::ofstream sensorLogfile; ///< for opening log file
public:
  TbTLog(GuideMotionManagement &gmm,RobotModels *rm);
  ~TbTLog();

  TeachSensorLogList teachTouchLog;
  TeachSensorLogList teachGyroLog;

  //sensor update
  template<typename T>
  void writeSensorLog(T t, std::pair<TbTType::SensorUpdate,int> s) {
    writeSensorLog(s.first,s.second);
  }
  void writeLog(std::string s);
  void writeSensorLog(TbTType::SensorUpdate,int id = -1);
  void onConnection(helium::Connection c);

private:
  helium::StaticConnReference<helium::Connection,TbTLog,helium::Connection,&TbTLog::onConnection> callOnConnection;

  //for sensor logs
  typedef helium::ParamStaticConnReference<double,TbTLog,std::pair<TbTType::SensorUpdate,int>,&TbTLog::writeSensorLog> CallWritePot; 
  void allocWritePot(CallWritePot *cwp,int i) { new (cwp) CallWritePot(this, std::make_pair(TbTType::POT,i)); }
  helium::Array<CallWritePot,helium::array::RESIZABLE,helium::array::InternalAllocator
		<helium::ClassPlacerAllocator<CallWritePot,TbTLog,&TbTLog::allocWritePot> > > callWritePot;

  typedef helium::ParamStaticConnReference<int,TbTLog,std::pair<TbTType::SensorUpdate,int>,&TbTLog::writeSensorLog> CallWriteTouch; 
  void allocWriteTouch(CallWriteTouch *cwp,int i) { new (cwp) CallWriteTouch(this, std::make_pair(TbTType::TOUCH,i)); }
  helium::Array<CallWriteTouch,helium::array::RESIZABLE,helium::array::InternalAllocator
		<helium::ClassPlacerAllocator<CallWriteTouch,TbTLog,&TbTLog::allocWriteTouch> > > callWriteTouch;

  helium::ParamStaticConnReference<helium::SensorValList<int>,TbTLog,std::pair<TbTType::SensorUpdate,int>,&TbTLog::writeSensorLog> callWriteGyro; 

  typedef helium::ParamStaticConnReference<bool,TbTLog,std::pair<TbTType::SensorUpdate,int>,&TbTLog::writeSensorLog> CallWritePower; 
  void allocWritePower(CallWritePower *cwp,int i) { new (cwp) CallWritePower(this, std::make_pair(TbTType::POWER,i)); }
  helium::Array<CallWritePower,helium::array::RESIZABLE,helium::array::InternalAllocator
		<helium::ClassPlacerAllocator<CallWritePower,TbTLog,&TbTLog::allocWritePower> > > callWritePower;
};

/*! dummy class for function structure
 */
class TbTTeachImplementation {
public:
  TbTSignals signals;
  virtual void acquire(const TeachIO &t) = 0; ///< add touch input
  virtual void acquire(const TeachPostureInput &p) = 0; ///< add touch input
  virtual void acquired(const TeachPosture &p) = 0; ///< add touch output
  virtual void acquired(const TeachIO &t) = 0; ///< add touch output
  virtual void del(const int id) = 0; ///< delete a touch
  virtual void clear() = 0;
  virtual void undo() = 0;
  virtual void redo() = 0;
  virtual void dummy() = 0;
};

class TbTCheckPower {
  helium::Array<std::vector<cfg::guide::Sensor*>,2> sensors;
  double threshold;
  bool isTurnedOff;
public:
  TbTCheckPower(cfg::guide::Sensors &sensors,cfg::guide::Teach::Power &power,const double& pthreshold);
  void setSensorsConnections(bool b);

  helium::Signal<bool> signal;
private:  
  void checkPower();
  typedef helium::StaticConnReference<int,TbTCheckPower,void,&TbTCheckPower::checkPower> CallCheckPower;
  helium::Array<CallCheckPower,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ParamAllocator<CallCheckPower,TbTCheckPower*> > 
		>callCheckPower1;
  helium::Array<CallCheckPower,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ParamAllocator<CallCheckPower,TbTCheckPower*> > 
		>callCheckPower2;
};

class TbTSetPower {
  helium::Array<cfg::guide::Joint*> joints;
public:
  TbTSetPower(cfg::guide::Joints &joints,cfg::guide::Teach::Power &power);  
  void setPower(bool b);
  helium::StaticConnReference<bool,TbTSetPower,bool,&TbTSetPower::setPower> conn;
};

/*! rules to add/delete/save/undo touch list
  (old guideTeachList)
 */
class TbTTeach : public TbTTeachImplementation {
private:
  GuideState &gs;
  TbTFilter *filter;
  TbTLog &log;
  std::vector<TeachIO> examples;
  helium::InternalPresentHistoryStack<std::vector<TeachIO> > history; ///< saves teachlist changes history
  std::ofstream logFile;

  void allocCheckPower(TbTCheckPower *t,int i) { new(t) TbTCheckPower(gs.sensors,gs.teach.powers[i],gs.teach.touch.threshold); }
  helium::Array<TbTCheckPower,helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ClassPlacerAllocator<TbTCheckPower,TbTTeach,&TbTTeach::allocCheckPower> > > checkPower;
  void allocSetPower(TbTSetPower *t,int i) { new(t) TbTSetPower(gs.joints,gs.teach.powers[i]); }
  helium::Array<TbTSetPower,helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ClassPlacerAllocator<TbTSetPower,TbTTeach,&TbTTeach::allocSetPower> > > setPower;
  
  /*! TbTTeachImplementation functions are made private and implemented here
   */
  virtual void acquire(const TeachIO &t); ///< add touch input
  virtual void acquired(const TeachIO &t); ///< add touch output

  virtual void acquire(const TeachPostureInput &p); ///< add touch input
  virtual void acquired(const TeachPosture &p); ///< add touch output
  virtual void del(const int id); ///< delete a touch
  virtual void clear();
  virtual void undo();
  virtual void redo();
  virtual void dummy();

  void change(TbTType::TeachIOChangeReason reason);
  void writeLog(TbTType::TeachIOChangeReason reason,bool write);

public:
  std::string getTouchedSensorNames(const helium::Array<double> &ratio);

  std::string teachLogfile; ///< file to save temporary teach file (.tea)

  TbTTeach(GuideState &pgs, TbTFilter* filter, TbTLog &log);
  ~TbTTeach();
  /*! edit() is the only way to edit the examples outside class
   */
  template<TbTType::TeachIOChangeReason reason, typename R,R (TbTTeachImplementation::*f)()> 
  void edit(){
    (this->*f)();
    change(reason);
  } 
  template<TbTType::TeachIOChangeReason reason, typename R,typename P,R (TbTTeachImplementation::*f)(P p)> 
  void edit(typename helium::constReference<P>::value p){
    (this->*f)(p);
    change(reason);
  } 
  template<TbTType::TeachIOChangeReason reason, typename R,typename P1,typename P2,R (TbTTeachImplementation::*f)(P1 p1,P2 p2)> 
  void edit(typename helium::constReference<P1>::value p1,typename helium::constReference<P2>::value p2){
    (this->*f)(p1,p2);
    change(reason);
  } 
  template<TbTType::TeachIOChangeReason reason, typename R,typename P1,typename P2,typename P3,R (TbTTeachImplementation::*f)(P1 p1,P2 p2,P3 p3)> 
  void edit(typename helium::constReference<P1>::value p1,typename helium::constReference<P2>::value p2,typename helium::constReference<P3>::value p3){
    (this->*f)(p1,p2,p3);
    change(reason);
  }
  template<TbTType::TeachIOChangeReason reason, typename R,typename P1,typename P2,typename P3,typename P4,R (TbTTeachImplementation::*f)(P1 p1,P2 p2,P3 p3,P4 p4)> 
  void edit(typename helium::constReference<P1>::value p1,typename helium::constReference<P2>::value p2,typename helium::constReference<P3>::value p3,typename helium::constReference<P4>::value p4){
    (this->*f)(p1,p2,p3,p4);
    change(reason);
  }

  bool canUndo();
  bool canRedo();

  const std::vector<TeachIO>& get() const; ///< get protected touch list
  void setSensorsConnections(bool b);
};

/*! rules use the touch
 */
class TbTTouch {
  GuideState &gs;
  RobotModels *rm;
  const std::vector<TeachIO> &examples;
  TbTFilter *filter;
  TbTLog &log;
  bool isElapsedTimeValid;
public:
  helium::Time waitReturnPosition;
  TbTTouch(GuideState &pgs, RobotModels *rm, const std::vector<TeachIO> &examples,TbTFilter *filter, TbTLog &log);
  void setSensorsConnections(bool b);  
private:
  void onTouch();
  void getOutput(helium::Posture &output,const TeachPostureInput &touch);
  virtual double getWeight(const TeachPostureInput &example, const TeachPostureInput &touch);
  bool filterColdet(const helium::Posture &output); ///< returns true when the posture is not colliding
  void moveMotors(helium::Array<double> &weight);
  typedef helium::StaticConnReference<int,TbTTouch,void,&TbTTouch::onTouch> CallOnTouch;
  helium::Array<CallOnTouch,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ParamAllocator<CallOnTouch,TbTTouch*> > 
		>callOnTouch;
};

/*! contains all of tbt non gui
  - takes care of TbT states according to views
 */

class TbT {
private:
  GuideMotionManagement &gmm;
  RobotModels *rm;

  TbTFilter filter;
  TbTLog log; ///< handles logging of tbt
  TbTTeach teach; ///< acquire touch meanings
  TbTTouch touch; ///< move robot by touch
  std::string teachfile; ///< save destination of current teach file

public:
  helium::ValueHub<TbTType::State> state;
  helium::GtkProxy<bool> acquireState;
  helium::GtkProxy<std::string> touchedSensor;
  bool acquiring; ///< make sure not to change to same state two times when the previous acquire/acquired process has not been finished

  TbT(GuideMotionManagement &gmm,RobotModels *prm);
private:
  void onConnection(helium::Connection c);
  void onViewChange(cfg::guide::GuideView::Type v);
  void onStateChange(TbTType::State s);
  void onAcquireChange(bool acquire);
  void onAcquireChangeProceed();

public:
  void newTeachList();          ///< [gui]
  void applytoFrame(int id, bool output); ///< [gui]
  void deleteTeachList(const int id); ///< [gui]
  void openTeachList(const std::string &filename, bool replace); ///< [gui]
  void saveTeachList(const std::string &filename); ///< [gui]
  void undoTeachList();
  void redoTeachList();
  void onTouch();
  void updateTouchedSensor();
  void updateTbTState();

  std::string getTouchedSensorNames(const helium::Array<double> &ratio);

  //interfacing teach
  const std::vector<TeachIO>& getTeachList() const;
  TbTSignals& getSignals();
  void getTeachFile(std::string &s);

  struct Connections {
    helium::StaticConnReference<helium::Connection,TbT,helium::Connection,&TbT::onConnection> callOnConnection;
    helium::StaticConnReference<cfg::guide::GuideView::Type,TbT,cfg::guide::GuideView::Type,&TbT::onViewChange> callOnViewChange;
    helium::StaticConnReference<TbTType::State,TbT,TbTType::State,&TbT::onStateChange> callOnStateChange;
    helium::StaticConnReference<bool,TbT,bool,&TbT::onAcquireChange> callOnAcquireChange;
    typedef helium::StaticConnReference<int,TbT,void,&TbT::onTouch> CallOnTouch;
    helium::Array<CallOnTouch,
		  helium::array::RESIZABLE,
		  helium::array::InternalAllocator<helium::ParamAllocator<CallOnTouch,TbT*> > 
		  >callOnTouch; ///<for changing touched sensor label

    //for saving temporary tea files
    helium::ReplaceParamStaticConnReference<const TeachPostureInput&,TbT,const std::string&,&TbT::saveTeachList> callOnTeachAcquire;
    helium::ReplaceParamStaticConnReference<const TeachPosture&,TbT,const std::string&,&TbT::saveTeachList> callOnTeachAcquired;
    helium::ReplaceParamStaticConnReference<int,TbT,const std::string&,&TbT::saveTeachList> callOnTeachDelete;
    helium::ParamStaticConnReference<void,TbT,const std::string&,&TbT::saveTeachList> callOnTeachClear;
    helium::ParamStaticConnReference<void,TbT,const std::string&,&TbT::saveTeachList> callOnTeachUndo;
    helium::ParamStaticConnReference<void,TbT,const std::string&,&TbT::saveTeachList> callOnTeachRedo;

    Connections(TbT *owner,const GuideState &gs,const std::string& teachLogfile);
  };
  std::auto_ptr<Connections> connections;
};

#endif
