/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags g
 */


#ifndef MAINWINDOW
#define MAINWINDOW

#include <gtkmm.h>
#include <vector>
#include <gdkmm/pixbuf.h>


#include <app/guide2/clickable.h>

#include <app/guide2/guideCanvas.h>

#include <app/guide2/angleEntry.h>
#include <app/guide2/motionLine.h>
#include <app/guide2/guideMenu.h>
//#include <app/guide2/guideHardwareWindow.h>
#include <app/guide2/guideDebugWindow.h>
#include <app/guide2/guideTeachWindow.h>
#include <app/guide2/guidePostureWindow.h>
#include <app/guide2/guideMotionManagement.h>
#include <app/guide2/guideHistory.h>


#include <helium/compilerWorkaround/CWAinclassf.h>


struct GuideSubWindow{
  Gtk::Window* win;
  std::string menuLabel;  
  GuideSubWindow(Gtk::Window* pwin,const std::string& pmenuLabel):
    win(pwin),menuLabel(pmenuLabel)
  {
    win->set_title(menuLabel);
  }
  
};

class GuideWindow : public Gtk::Window
{
  
private:
  struct SubWins{
    std::vector<GuideSubWindow> all;

    SubWins(Gtk::Window& postureWin,std::vector<GuideSubWindow>& additional){
      all.push_back(GuideSubWindow(&postureWin,"Postures List Window"));
      all.insert(all.end(),additional.begin(),additional.end());
    }
    
  };  

public:
  
  void addSubWin(Gtk::Window& w, std::string s);

protected:
  GuideMotionManagement &gmm;
public:
  GuideHistory &gh;
protected:
  helium::ValueHub<double> scale;

  //extra window
  AngleEntry angleEntry;
  HexEntry hexEntry;
  RobotModels *rm;
  GuidePostureWindow guidePostureWindow;
  SubWins subwins;

  GuideMenu guideMenu;

  //MotionView
  Gtk::VBox mainBox;
  Gtk::Fixed fixed;
  Gtk::ScrolledWindow scroll;
  Gtk::AspectFrame drawingFrame;
  MotionLine motionLine;

public:
  GuideCanvas guideCanvas;

protected:
  void setView(cfg::guide::GuideView::Type v);
  //posture list window
  void addCurPosture();
  void applyPosture(int id);
private:
  void onCollidingFrame(bool b);

  //helium::StaticConnReference<bool,GuideMenu,bool,&GuideMenu::setEnableUndo> undoConn;
  //helium::StaticConnReference<bool,GuideMenu,bool,&GuideMenu::setEnableRedo> redoConn;

  helium::StaticConnReference<bool,GuideMenu,bool,&GuideMenu::onCollidingFrame> onCollidingFrameDisableCopyConn;
  helium::StaticConnReference<bool,GuideWindow,bool,&GuideWindow::onCollidingFrame> onCollidingFrameConn;
  
  struct  Callers;
  std::auto_ptr<Callers> callers; //vs10 bug workaround
  

  //save state for undo
  //helium::StaticConnReference<std::string,GuideMotionManagement,std::string,&GuideMotionManagement::execute> saveStateEntryConn; //DEBUG to be replaced with GuideHistory

  //helium::StaticConnReference<void,GuideWindow,void,&GuideWindow::addCurPosture> addCurPostureConn;
  //helium::StaticConnReference<int,GuideWindow,int,&GuideWindow::applyPosture> applyPostConn;
  //helium::InternalConnReference<void> addCurPostureConn;
  //helium::InternalConnReference<int> applyPostConn;

public:
  GuideWindow(GuideMotionManagement &pgmm,
	      GuideHistory &pgh,
	      RobotModels* prm,
	      std::vector<GuideSubWindow>& additionalSubwins,
	      coldet::ColdetManager *coldetm
);
	      //GuideDebugWindow &pguideDebugWindow, SensorCalibManager *pscm, GyroCalib *pgc,  GuideModelWindow *gmw, GyroCalibGlWindow *gcw, SensorCalibSubWindow *scw, GuideTeachWindow *gtw);
  void setDefaultSize();
  void exitAll();
  //void updateModel();

  //connection
  void connect();
  void disconnect();
  void changeHost();
  void changeTitle();
  void onConnectMenu();

private:
  //Signal handlers:
  void onConnection(helium::Connection c);
  void onResize();
  //bool onKeyPressModel(GdkEventKey* event);

  bool onKeyPressArrow(GdkEventKey* event);
  //bool isKeyHandledByGuideCanvas(GdkEventKey* event); //TODO:delete??

  bool onKeyPress(GdkEventKey *event);
  guint32 timePress;
  guint32 timeRelease;


  void onQuit();
  bool onDeleteEvent(GdkEventAny *event);
  void showSubWindowbyString(std::string name); ///name specified in guideMain.cpp when creating sub window
  Gtk::Window* getSubWindow(std::string name); ///name specified in guideMain.cpp when creating sub window
  bool onModelAreaClick(GdkEventButton *event); /// deal with clicks on model area, switch model (right click) or open window (else) 

  helium::StaticConnReference<std::string,GuideWindow,std::string,&GuideWindow::showSubWindowbyString> callShowSubWindowbyString;
  
  //helium::StaticConnReference<helium::Connection,GuideWindow,helium::Connection,&GuideWindow::onConnection> callOnConnection;
  helium::InternalConnReference<helium::Connection> callOnConnection;
  helium::StaticConnReference<helium::Connection,GuideMenu,const helium::Connection&,&GuideMenu::setConnectionTick> callSetConnectionTick;
  //helium::StaticConnReference<std::string,GuideWindow,void,&GuideWindow::changeTitle> callChangeTitle;
  helium::InternalConnReference<std::string> callChangeTitle;
 
  //DEBUG TEMP
  //TODEL void changeState() { std::cout << "-- changeState" << std::endl; }
  //TODEL helium::StaticConnReference<helium::Connection,GuideWindow,void,&GuideWindow::changeState> callChangeState;


  //helium::InternalConnReference<std::string> callOnSensorWidgetClick;   ///<connect with GuideCanvas
  //helium::InternalConnReference<std::string> callOnGyroWidgetClick;   ///<connect with GuideCanvas
  //helium::InternalConnReference<std::string> callOnColdetWidgetClick;   ///<connect with GuideCanvas

  helium::StaticConnReference<bool,GuideWindow,void,&GuideWindow::changeTitle> callChangeTitleonSaveStatus;
  helium::StaticConnReference<bool,GuideWindow,void,&GuideWindow::changeTitle> callChangeTitleonSaveCalibStatus;
  helium::StaticConnReference<void,GuideHistory,void,&GuideHistory::initCalib> callInitCalib;///<from calib open signal to reset calib history stack  
};


#endif
