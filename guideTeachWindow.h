/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.06.17
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDETEACHWINDOW
#define GUIDETEACHWINDOW

#include "tbt.h"
#include <helium/device/pedal.h>
#include "robotModels.h"
#include "iconLabelButton.h"

class TeachTreeColumn : public Gtk::TreeModelColumnRecord {
public:
  enum Column {ID,INPUT,OUTPUT,NAME,INPUTSNAP,OUTPUTSNAP};
  TeachTreeColumn();
  Gtk::TreeModelColumn<int> id;
  Gtk::TreeModelColumn<RobotModels::PixbufPtr> input;
  Gtk::TreeModelColumn<RobotModels::PixbufPtr> output;
  Gtk::TreeModelColumn<std::string> name;  
};

class TeachTreeView : public Gtk::TreeView {
  TbT &tbt;
  helium::GtkExceptionHandling *eh;
  RobotModels *rm;

  int snapWidth;
  int snapHeight;
  std::list<RobotModels::PostureSnap> postureSnapi; ///< saves snaps of input
  std::list<RobotModels::PostureSnap> postureSnapo; ///< saves snap of output, has to be list, since vector changes the address of the array that breaks the pixbuf

  TeachTreeColumn column;
  Glib::RefPtr<Gtk::ListStore> model;
  Gtk::CellRendererText *nameColumnRenderer;
  Gtk::Menu menu;

public:
  TeachTreeView(TbT &ptbt, helium::GtkExceptionHandling *peh, RobotModels *prm);
private:
  //teachlist functions, action to be done according to change to teach list
  void onTeachAcquire(const TeachPostureInput&);
  void onTeachAcquired(const TeachPosture &);
  void onTeachDelete(int id); 
  void onApplytoFrame(bool output);
  void onTeachClear();
  void onTeachUndo();
  void onTeachRedo();

  void renumber(); ///< renumber id consecutively from 0
  void reloadTree(); ///< upon undo/redo, clear and reload the whole tree

  //menu
  bool on_button_press_event(GdkEventButton* event);
  void onDelete(); //menu delete is pressed

  struct Connections {
    helium::StaticConnReference<const TeachPostureInput&,TeachTreeView,const TeachPostureInput&,&TeachTreeView::onTeachAcquire> callOnTeachAcquire;
    helium::StaticConnReference<const TeachPosture&,TeachTreeView,const TeachPosture&,&TeachTreeView::onTeachAcquired> callOnTeachAcquired;
    helium::StaticConnReference<int,TeachTreeView,int,&TeachTreeView::onTeachDelete> callOnTeachDelete;
    helium::StaticConnReference<void,TeachTreeView,void,&TeachTreeView::onTeachClear> callOnTeachClear;
    helium::StaticConnReference<void,TeachTreeView,void,&TeachTreeView::onTeachUndo> callOnTeachUndo;
    helium::StaticConnReference<void,TeachTreeView,void,&TeachTreeView::onTeachRedo> callOnTeachRedo;
    Connections(TeachTreeView *owner);
  };
  std::auto_ptr<Connections> connections;  
};

class GuideTeachStatusLabel: public Gtk::Label {
public:
  GuideTeachStatusLabel(helium::ValueHub<TbTType::State> &s);
  void onStateChange(TbTType::State s);
  helium::InternalConnReference<TbTType::State> callOnStateChange;
};

template<typename T>
class GuideValueHubLabel: public Gtk::Label {
public:
  GuideValueHubLabel(const helium::ValueHub<T> &value):
    callSetValue(this)
  {
    value.connect(callSetValue);
  }
  void setValue(T t) {
    std::string text;
    helium::convert(text,t);
    set_text(text);
  }
  helium::StaticConnReference<T,GuideValueHubLabel,T,&GuideValueHubLabel::setValue> callSetValue;
};

class GuideTeachWindow : public Gtk::Window {
  GuideMotionManagement &gmm;
  TbT tbt;

  helium::ValueHub<std::string> touchedSensor;

  Gtk::VBox vbox;
  Gtk::Toolbar toolbar;
  IconLabelToggleToolButton acquireButton;
  IconLabelToolButton undoButton;
  IconLabelToolButton redoButton;
  IconLabelToolButton newButton;
  IconLabelToolButton openButton;
  IconLabelToolButton saveButton;
  IconLabelToolButton saveAsButton;
  TeachTreeView treeView;
  Gtk::ScrolledWindow scrolledWindow;
  GuideTeachStatusLabel guideTeachStatusLabel;
  Gtk::Label touchedSensorLabel;

  //button functions
  void onAcquireButtonPress();
  void onUndoButtonPress();
  void onRedoButtonPress();
  void onNewButtonPress();
  void onOpenButtonPress();
  void onSaveButtonPress();
  void onSaveAsButtonPress();
  void setDialogFile(Gtk::FileChooserDialog &dialog); ///< set teach file setting to dialog box

  //valuehub functions
  void onStateChange(TbTType::State s);
  void onAcquireStateChange(bool b);
  void onTouchedSensorUpdate(std::string s);
  void onRowChanged(); ///< on adding row in scrolledwindow (adding/deleting teach samples)

public:
  GuideTeachWindow(GuideMotionManagement &gmm,RobotModels *prm);  
  helium::ReplaceTConstStaticConnReference<void,IconLabelToggleToolButton,bool,&IconLabelToggleToolButton::set_active,bool,true> callSetAcquireOn; 
  helium::ReplaceTConstStaticConnReference<void,IconLabelToggleToolButton,bool,&IconLabelToggleToolButton::set_active,bool,false> callSetAcquireOff; 
private:
  struct Connections {
    helium::StaticConnReference<TbTType::State,GuideTeachWindow,TbTType::State,&GuideTeachWindow::onStateChange> callOnStateChange;
    helium::StaticConnReference<bool,GuideTeachWindow,bool,&GuideTeachWindow::onAcquireStateChange> callOnAcquireStateChange;
    helium::StaticConnReference<std::string,GuideTeachWindow,std::string,&GuideTeachWindow::onTouchedSensorUpdate> callOnTouchedSensorUpdate;
    Connections(GuideTeachWindow *owner,size_t sensorsSize);
  };
  std::auto_ptr<Connections> connections;  
};

// #include <fstream>
// #include <gtkmm.h>
// #include <helium/device/pedal.h>
// #include "guideTeachList.h"
// #include "robotModels.h"
// #include "teachPower.h"
// #include "teachUse.h"
// 
// class GuidePostureWindow;
// 
// namespace TeachListChange {
//   enum Reason {ADD,ACQUIRE,ACQUIRED,DELETETEACH,CLEAR,OPEN,UNDO,REDO,USE,NONE};
// }
// 
// namespace SensorUpdate {
//   enum UpdateType {POT,SENSOR,GYRO,POWER};
// }
// 
// struct SensorSelection {
//   typedef enum SENSORSELECTION {NONE,BYID,BYDESC,NUM} Type;
//   static const char* strings[];
// };
// 
// namespace TbTState {
//   enum State {DISCONNECTED,CONNECTED,TOUCHENABLE};
// };
// 
// 
// inline std::ostream& operator <<(std::ostream& o,const SensorUpdate::UpdateType& u);
// inline std::ostream& operator <<(std::ostream& o,const helium::SensorValList<int>& g);
// 
// //privately store guideteachlist to enable history
// struct GuideTeachListInfo {
//   const GuideTeachList& get() { return guideTeachList; }
//   bool canUndo();
//   bool canRedo();
//   void undo();
//   void redo();
//   
//   void writeLog(TeachListChange::Reason e, std::string s="", bool writeSensor=false);
// 
//   template <typename T>
//   void writeLog(TeachListChange::Reason e, T t,bool writeSensor=false);
// 
//   /**
//    * @param t update value
//    * @param u pair of update type and array index (if available, else -1)
//    */
//   template <typename T>
//   void writeLog(T t, std::pair<SensorUpdate::UpdateType,int> u);
// 
//   void writeLogTeachList();
// 
//   std::ofstream logFile; //saves logs
// 
//   template<TeachListChange::Reason reason,typename R,R (GuideTeachList::*f)()> 
//   void edit(){
//     (guideTeachList.*f)();
//     tlChange(reason);
//   } 
// 
//   template<TeachListChange::Reason reason,typename R,typename P,R (GuideTeachList::*f)(P p)> 
//   void edit(typename helium::constReference<P>::value p){
//     (guideTeachList.*f)(p);
//     tlChange(reason);
//   } 
// 
//   /**
//    * string is passed as a comment for log
//    */
//   template<TeachListChange::Reason reason,typename R,typename P,R (GuideTeachList::*f)(P p)> 
//   void edit(typename helium::constReference<P>::value p,std::string s){
//     (guideTeachList.*f)(p);
//     tlChange(reason,s);
//   } 
// 
//   template<TeachListChange::Reason reason,typename R,typename P1,typename P2,R (GuideTeachList::*f)(P1 p1,P2 p2)> 
//   void edit(typename helium::constReference<P1>::value p1,typename helium::constReference<P2>::value p2){
//     (guideTeachList.*f)(p1,p2);
//     tlChange(reason);
//   } 
// 
//   template<TeachListChange::Reason reason,typename R,typename P1,typename P2,typename P3,R (GuideTeachList::*f)(P1 p1,P2 p2,P3 p3)> 
//   void edit(typename helium::constReference<P1>::value p1,typename helium::constReference<P2>::value p2,typename helium::constReference<P3>::value p3){
//     (guideTeachList.*f)(p1,p2,p3);
//     tlChange(reason);
//   } 
//   
//   template<TeachListChange::Reason reason,typename R,typename P1,typename P2,typename P3,typename P4,R (GuideTeachList::*f)(P1 p1,P2 p2,P3 p3,P4 p4)> 
//   void edit(typename helium::constReference<P1>::value p1,typename helium::constReference<P2>::value p2,typename helium::constReference<P3>::value p3,typename helium::constReference<P4>::value p4){
//     (guideTeachList.*f)(p1,p2,p3,p4);
//     tlChange(reason);
//   } 
// 
//   GuideTeachListInfo(GuideMotionManagement &gmm):
//     gs(gmm.gs),
//     guideTeachList(gmm),
//     tlHistory(guideTeachList.gettl()),
//     t0(gmm.t0),
//     callWritePot(this,gmm.gs.joints.size()),
//     callWriteSensor(this,gmm.gs.sensors.size()),
//     callWriteGyro(this,std::make_pair(SensorUpdate::GYRO,-1)),
//     callWritePower(this,gmm.gs.joints.size())
//   {
//     //fixing filename
//     std::string logf = gmm.gs.pref.tlFilename;
//     size_t found = logf.rfind(".tea");
//     logf = logf.substr(0,found);
//     logf.append(".log");
// 
//     logFile.open(logf.c_str(), std::ios::out | std::ios::app);
//     std::cout << "teachList log " << logf << std::endl;
//   }
// 
//   ~GuideTeachListInfo() {
//     logFile.close();
//   }
// private:
//   //void tlChange(TeachListChange::Reason reason);
//   void tlChange(TeachListChange::Reason reason,std::string s="");
//   GuideState &gs;
//   GuideTeachList guideTeachList;
//   helium::InternalPresentHistoryStack<std::vector<TeachIO> > tlHistory;
//   long long t0;
// 
// public:
//   ///connections  
//   typedef helium::ParamStaticConnReference<double,GuideTeachListInfo,std::pair<SensorUpdate::UpdateType,int>,&GuideTeachListInfo::writeLog> CallWritePot; 
//   void allocWritePot(CallWritePot *cwp,int i) { new (cwp) CallWritePot(this, std::make_pair(SensorUpdate::POT,i)); }
//   helium::Array<CallWritePot,helium::array::RESIZABLE,helium::array::InternalAllocator
// 		<helium::ClassPlacerAllocator<CallWritePot,GuideTeachListInfo,&GuideTeachListInfo::allocWritePot> > > callWritePot;
// 
//   typedef helium::ParamStaticConnReference<int,GuideTeachListInfo,std::pair<SensorUpdate::UpdateType,int>,&GuideTeachListInfo::writeLog> CallWriteSensor; 
//   void allocWriteSensor(CallWriteSensor *cwp,int i) { new (cwp) CallWriteSensor(this, std::make_pair(SensorUpdate::SENSOR,i)); }
//   helium::Array<CallWriteSensor,helium::array::RESIZABLE,helium::array::InternalAllocator
// 		<helium::ClassPlacerAllocator<CallWriteSensor,GuideTeachListInfo,&GuideTeachListInfo::allocWriteSensor> > > callWriteSensor;
// 
//   helium::ParamStaticConnReference<helium::SensorValList<int>,GuideTeachListInfo,std::pair<SensorUpdate::UpdateType,int>,&GuideTeachListInfo::writeLog> callWriteGyro; 
// 
//   typedef helium::ParamStaticConnReference<bool,GuideTeachListInfo,std::pair<SensorUpdate::UpdateType,int>,&GuideTeachListInfo::writeLog> CallWritePower; 
//   void allocWritePower(CallWritePower *cwp,int i) { new (cwp) CallWritePower(this, std::make_pair(SensorUpdate::POWER,i)); }
//   helium::Array<CallWritePower,helium::array::RESIZABLE,helium::array::InternalAllocator
// 		<helium::ClassPlacerAllocator<CallWritePower,GuideTeachListInfo,&GuideTeachListInfo::allocWritePower> > > callWritePower;
// 
// 
// 
// };
// 
// 
// class TeachTreeView: public Gtk::TreeView {
//   Gtk::Window &win;
//   int w,h;
//   friend class GuideTeachWindow;
//   // Types and classes
// 
//   GuideTeachListInfo &gtl;
//   helium::GtkExceptionHandling *eh;
//   RobotModels *rm;
//   std::list<RobotModels::PostureSnap> postureSnapi; ///< saves snaps of input
//   std::list<RobotModels::PostureSnap> postureSnapo; ///< saves snap of output
// 
// public:
//   TeachTreeView(Gtk::Window &pwin, GuideTeachListInfo &pgtl,helium::GtkExceptionHandling *peh,RobotModels *prm);
// protected:
//   class ModelColumns : public Gtk::TreeModel::ColumnRecord {
//   public:
//     ModelColumns() {
//       add(m_col_id); add(m_pixbuf_input); add(m_pixbuf_output); add(m_col_name); 
//     }
//     Gtk::TreeModelColumn<unsigned int> m_col_id;
//     Gtk::TreeModelColumn<RobotModels::PixbufPtr> m_pixbuf_input;
//     Gtk::TreeModelColumn<RobotModels::PixbufPtr> m_pixbuf_output;
//     Gtk::TreeModelColumn<Glib::ustring> m_col_name;
//     //Gtk::TreeModelColumn<Glib::ustring> m_col_angle;
//   };
//   ModelColumns m_Columns;
// public:
//   ModelColumns& getColumn() {return m_Columns;}
//   int getSelectedId();
// protected:
//   Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
//   Gtk::Menu m_Menu_Popup;
//   virtual bool on_button_press_event(GdkEventButton* event);
//   void onDelete();
//   void onApplytoFrame(bool output=true);
//   void onCopytoClipboard(bool b);
//   void updateId();
// 
// 
//   /** 
//    * require teachList to be firstly updated with the data (which is done in GuideTeachWindow::on_acquire)
//    * @param id the index of GuideTeachList::teachList vector to be added to tree
//    */
//   void addEntry(const int id);
//   void addEntryInput(const int id);
//   void updateEntryOutput(const int id);
// 
//   /** doesn't require referring to teachList 
//    * deprecated
//    * problem: may result in error when deleting if id doesn't fit gtl
//    */
//   //void addEntry(const int id, const std::string sensor, const helium::Posture& anglei, const helium::Posture& angleo);
// 
// protected:
//   helium::Signal<const helium::Posture&> applytoFrame;
// };
// 
// class GuideTeachStatusLabel: public Gtk::Label {
// public:
//   GuideTeachStatusLabel(helium::ValueHub<TbTState::State> &s);
//   void onStateChange(TbTState::State s);
//   helium::InternalConnReference<TbTState::State> callOnStateChange;
// };
// 
// class GuideTeachWindow: public Gtk::Window {
//   GuideMotionManagement &gmm; //helium::GtkExceptionHandling *eh;
//   helium::LogitecGTForce *lg;
//   helium::LogitecGTEventPrinter *ep;
//   helium::Posture beforeAcquirePost;
//   bool pedalConn;
//   bool writeLogConn;
// public:
//   bool onuse;
//   helium::ValueHub<TbTState::State> tbtState;
//   enum ActionType{OPEN,SAVEAS};
//   GuideTeachListInfo gtl;
//   TeachPower teachPower;
//   TeachUse teachUse;
//   GuideTeachWindow(GuideMotionManagement &gmm,RobotModels *prm);
//   virtual ~GuideTeachWindow();
//   //void addEntry(const int id, const std::string sensor, const helium::Posture& angle);
//   void on_button_file(ActionType action);
//   void openFile(bool toReplace,std::string openedFile);
//   void selectEntry(int id);  
//   TeachTreeView& getTreeView() {return m_TreeView;}
// protected:
//   virtual void on_button_quit() {hide();}
//   void on_button_clear();
//   void on_button_redo();
//   void on_button_undo();
//   void on_acquire();
//   void on_use();
//   void on_comment();
// 
//   void addTeach();
//   void clearTree();
//   void reloadTree();
// 
//   //Child widgets:
//   Gtk::VBox m_VBox;
//   Gtk::CheckButton copyTeachOutputToFrameButton;
//   Gtk::ComboBoxText selectSensor;
//   Gtk::Label selectSensorLabel;
//   Gtk::HBox sensorSelectionHBox;
//   Gtk::HBox sensorSelectionHBoxSub;
//   GuideTeachStatusLabel statusLabel;
// 
//   void onCopyTeachOutputToFrameButtonClick();
// 
//   Gtk::ScrolledWindow m_ScrolledWindow;
//   TeachTreeView m_TreeView;
// 
//   Gtk::HButtonBox m_ButtonBox;
//   //Gtk::Button m_Button_Quit;
//   //Gtk::Button m_Button_Open;
//   //Gtk::Button m_Button_Clear;
// 
//   Gtk::Toolbar* pToolbar;
//   Glib::RefPtr<Gtk::UIManager> m_refUIManager;
//   Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
//   Gtk::ToggleToolButton b_acquire,b_use;
//   Gtk::SeparatorToolItem separator;
//   struct Connections;
//   friend struct Connections;
//   void onConnection(helium::Connection c);
//   void onViewChange(cfg::guide::GuideView::Type);
//   void onStateChange(TbTState::State s);
//   void setAcquire(bool b);
//   void setPowerOff();
//  
//   std::auto_ptr<Connections> conns;//VS is bugged and cannot have templates referring the funcs here
// 
//   
//   helium::GtkSignal<bool, bool,false,true> setAcquireSignal;
// public:
//   void onApplytoFrame(const helium::Posture& p);
//   std::string lastOpenedFile;
//   Gtk::Toolbar* getToolbar() {return pToolbar;}
// 
//   //extra signals
//   helium::TsSignal<int> selectionChangeSignal;
//   void onSelectionChange();
//   void setSelectedSensor();
//   helium::TsSignal<int> sensorSelectionSignal;
//   helium::TsSignal<std::string> fileChangeSignal;  
// 
//   helium::StaticConnReference<int,GuideTeachWindow,int,&GuideTeachWindow::selectEntry> callSelectSensor;
//   helium::StaticConnReference<int,GuideTeachWindow,void,&GuideTeachWindow::setSelectedSensor> callSetSelectedSensor;
//   helium::ReplaceParamStaticConnReference<int,TeachTreeView,bool,&TeachTreeView::onApplytoFrame> callCopyTeachOutputToFrame;
//   helium::InternalConnReference<TbTState::State> callOnStateChange;
// 
// protected:
// 
//   void writeBuffer(double,int);
//   typedef helium::ParamStaticConnReference<double,GuideTeachWindow,int,&GuideTeachWindow::writeBuffer> CallWriteBuffer; 
//   helium::Array<CallWriteBuffer,
// 		helium::array::RESIZABLE,
// 		helium::array::InternalAllocator
// 		<helium::ParamIdxAllocator<CallWriteBuffer,GuideTeachWindow*> > > callWriteBuffer;
// 
// };



#endif
