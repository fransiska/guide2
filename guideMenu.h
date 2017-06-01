/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef GUIDEMENU2
#define GUIDEMENU2
#include <gtkmm.h>
#include <iostream>
#include <helium/signal/tsSignal.h>
#include <app/guide2/guideMotionManagement.h>
#include <helium/compilerWorkaround/CWAinclassf.h>

class GuideWindow;
struct GuideSubWindow;

class GuideMenu {
  public:
  enum FileType{MOTION,POINTS,CALIB,VSTONE};
  enum ActionType{OPEN,ADD,SAVE,SAVEAS,EXPORT};

  std::map<std::string, int> ViewMenuMap;

  enum WindowType{X1,X2,DEFVIEW,SEP1,MOTIONVIEW,TOUCHVIEW,CALIBVIEW,SEP2,VIEWNUM};//,DEBUGWIN,HARDWIN,MODELWIN,GYROWIN,SENSORWIN,TEACHWIN};
  //enum WindowList{POSTURESW,TREEW,GYROW,TEACHW,SENSORW,ROBOTW,FRAMEW,ERRORW};//POSTWIN == POSTURESW

  //static const WindowType FirstSubWin=POSTWIN;
private:
  GuideMotionManagement &gmm; 
  GuideWindow* win;
  std::vector<GuideSubWindow>& subwins;
  std::vector<Gtk::CheckMenuItem*>  checkItem;
  /*GuideDebugWindow* gtw;
    GuideHardwareWindow* ghw;
    GuidePostureWindow* gpw;
    
    //by config
    GuideModelWindow* gmw;
    GyroCalibGlWindow* gcw;
    SensorCalibWindow* scw;
    GuideTeachWindow* gtlw;
  */
  
  
  Glib::RefPtr<Gtk::ActionGroup> ag;
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  
  bool connTickChanging; ///< avoid gtk menu tick changing loop
  bool coldetTickChanging; ///< avoid gtk menu tick changing loop
  
  //bool dcb; //set by setConnectionTick
  //bool ocm; //set by GuideWindow onConnectMenu
  
  
  //Glib::Dispatcher changeHostDispatcher;
  //void changeHostDCB();
  //Glib::Dispatcher connectDispatcher;
  //void connectDCB();
  //Glib::Dispatcher disconnectDispatcher;
  //void disconnectDCB();
  
public:

  Gtk::MenuItem* menuConnect;
  Gtk::MenuItem* menuView;
  Gtk::MenuItem* menuFunction;
  //std::string connectedHost;
  //bool connected;
  
  GuideMenu(GuideMotionManagement &pgmm, 
	    GuideWindow* pwin,
	    std::vector<GuideSubWindow>& subwins
	    //helium::TsSignal<int> &puen
	    //GuideWindow* pwin,GuideDebugWindow * pgtw,GuideHardwareWindow *pghw,GuidePostureWindow * pgpw,GuideModelWindow *pgmw,SensorCalibWindow *pscw,GyroCalibGlWindow *pgcw,GuideTeachWindow * pgtlw
	    );
  void addTo(GuideWindow* win,Gtk::VBox& c);
  void addSubWin(int i);
  void setDefaultSize();

  void onSetView(cfg::guide::GuideView::Type g); //todo DELETE

  void setConnectionTick(const helium::Connection& c);
  void setColdetTick(bool b);


  void connectTo();
  void changeHost(std::string s);
  
  //void setEnableUndo(bool enable) { ag->get_action("Undo")->set_sensitive(enable); }
  //void setEnableRedo(bool enable) { ag->get_action("Redo")->set_sensitive(enable); }
  void setEnableCopy(bool enable) { ag->get_action("Copy")->set_sensitive(enable); }
  void setEnablePaste(bool enable) { ag->get_action("Paste")->set_sensitive(enable); }
  bool getEnableCopy() { return ag->get_action("Copy")->is_sensitive(); }
  bool getEnablePaste() { return ag->get_action("Paste")->is_sensitive(); }
  void setEnablePostureList(bool enable) { ag->get_action("Undo")->set_sensitive(enable); }
  void onEditing(bool b);
  void onCollidingFrame(bool b);

  void updateMenuEdit();
  void updateMenuFile();

  void toggleColdet();

  //void toggleModelWindow();
  //bool untoggleModelWindow(GdkEventAny* event);

  //void toggleSensorCalibWindow();
  /*bool untoggleSensorCalibWindow(GdkEventAny* event);
    void untoggleSensorCalibWindow();*/

  //void toggleGyroCalibWindow();
  /*bool untoggleGyroCalibWindow(GdkEventAny* event);
  void untoggleGyroCalibWindow();*/

  //void toggleDebugWindow();
  /*bool untoggleDebugWindow(GdkEventAny* event);
    void untoggleDebugWindow();*/

  //void toggleTeachWindow();
  /*bool untoggleTeachWindow(GdkEventAny* event);
    void untoggleTeachWindow();*/

  //void togglePostureWindow();
  /*bool untogglePostureWindow(GdkEventAny* event);
    void untogglePostureWindow();*/

  //void tickHardwareWindow();
  //void toggleHardwareWindow();
  /*bool untoggleHardwareWindow(GdkEventAny* event);
  void untoggleHardwareWindow();
  */

  bool markWindowClosedEv(GdkEventAny* event,int i);
  void markWindowClosed(int i);
  void markWindowOpened(int i);
  void toggleWindowVisibilityAdjustment(int i); ///< toggle window visibility based on the status of the check menu
  void toggleWindowVisibility(int i); ///< toggle window visibility by toggling the check menu



  //menu functions
  void newMotion();
  void fileDialog(ActionType action);
  void fileDialog(ActionType action,FileType type);

  void copyFrame();
  void pasteFrame();
  void undo();
  void redo();
  void pastePostureRobot();
  void pastePostureString();
  void copyPostureString();

  //void mirrorPosture(); //moved to guideState
  void updateErrorNumber(int i);

  helium::StaticConnReference<int,GuideMenu,int,
			      &CWAinclassf(GuideMenu,updateErrorNumber)> callUpdateErrorNumber;
  helium::StaticConnReference<bool,GuideMenu,bool,
			      &CWAinclassf(GuideMenu,onEditing)> callOnEditing;
  helium::StaticConnReference<bool,GuideMenu,bool,
			      &CWAinclassf(GuideMenu,setEnablePaste)> callEnablePaste;
  helium::StaticConnReference<bool,GuideMenu,bool,&GuideMenu::setColdetTick> callSetColdetTick;
  helium::InternalConnReference<helium::Connection> callOnConnection;

  helium::TsSignal<void> onOpenCalib;

  bool isConnectTickChanging(){
    return connTickChanging;
  }
  
  friend class GuideWindow;



  //temp
  void onClicked() {
    std::cout << "clicked" << std::endl;
  }
  void onConnection(helium::Connection c);

  
};

#endif
