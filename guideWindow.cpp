/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags g
 */


#include "guideWindow.h"
//#include <app/guide2/gtkglareaBox.h>
#include <app/guide2/inputDialog.h>
#include <helium/util/mathUtil.h>
#include "yesNoDialog.h"
 
#include <math.h>
#include <iostream>

using namespace std;
using namespace cfg::guide;




struct GuideWindow::Callers{
helium::StaticConnReference<cfg::guide::GuideView::Type,GuideWindow,cfg::guide::GuideView::Type,
	  &GuideWindow::setView> callOnViewUpdate;
Callers(GuideWindow* gw):callOnViewUpdate(gw){}
};


GuideWindow::GuideWindow(GuideMotionManagement &pgmm,
			 GuideHistory &pgh,
			 RobotModels *prm,
			 std::vector<GuideSubWindow>& additionalWins,
			 coldet::ColdetManager *coldetm
):
  gmm(pgmm),
  gh(pgh),
  scale(1),
  angleEntry(&fixed,gmm.gs.joints.defaultCalib.min,gmm.gs.joints.defaultCalib.max), //angle
  hexEntry(&fixed,gmm.gs.joints.defaultCalib.calibMin,gmm.gs.joints.defaultCalib.calibMax), //hex
  
  rm(prm),
  guidePostureWindow(gmm,prm),
  subwins(guidePostureWindow,additionalWins),
  guideMenu(gmm,this, subwins.all),//,updateErrorNumberMenu),
  

  mainBox(),
  drawingFrame("",Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 1, false),
  motionLine(gmm,*this,prm?&prm->timelineArea:NULL,coldetm),

  guideCanvas(*this,&angleEntry,&hexEntry,scale,gmm,coldetm,rm),

//undoConn(&guideMenu),
//redoConn(&guideMenu),
  onCollidingFrameDisableCopyConn(&guideMenu),
  onCollidingFrameConn(this),
  //setViewConn(this,helium::FuncPlaceHolder<GuideWindow,cfg::guide::GuideView::Type,&GuideWindow::setView>()),
  //saveStateEntryConn(&gmm),
  //addCurPostureConn(this,helium::FuncPlaceHolder<GuideWindow,void,&GuideWindow::addCurPosture>()),
  //applyPostConn(this,helium::FuncPlaceHolder<GuideWindow,int,&GuideWindow::applyPosture>()),
  callers(new GuideWindow::Callers(this)),
  callShowSubWindowbyString(this),
  callOnConnection(this,helium::FuncPlaceHolder<GuideWindow,helium::Connection,&GuideWindow::onConnection>()),
  callSetConnectionTick(&guideMenu), 
  callChangeTitle(this,helium::FuncPlaceHolder<GuideWindow,void,&GuideWindow::changeTitle>()),
  //callOnSensorWidgetClick(this,helium::FuncPlaceHolder<GuideWindow,std::string,&GuideWindow::showSubWindowbyString>()),
  //callOnGyroWidgetClick(this,helium::FuncPlaceHolder<GuideWindow,std::string,&GuideWindow::showSubWindowbyString>()),
  //callOnColdetWidgetClick(this,helium::FuncPlaceHolder<GuideWindow,std::string,&GuideWindow::showSubWindowbyString>()),
  callChangeTitleonSaveStatus(this),
  callChangeTitleonSaveCalibStatus(this),
  callInitCalib(&gh)
{

  //TODEL gmm.gs.conn.connect(callChangeState);

  //default
  set_position(Gtk::WIN_POS_CENTER);
  set_title("Guide2");
  set_border_width(10);
  set_default_size(guideCanvas.getWidth0()+gmm.gs.settings.sizes.windowOff.w,guideCanvas.getHeight0()+gmm.gs.settings.sizes.windowOff.h); //26,70 //22,52
  guideMenu.addTo(this,mainBox);

  //default connections
  signal_check_resize().connect(gmm.eh.wrap(*this,&GuideWindow::onResize));
  signal_key_press_event().connect(gmm.eh.wrap(*this,&GuideWindow::onKeyPress),false); //call before default one

  //gmm.setUndo.connect(undoConn);
  //gmm.setRedo.connect(redoConn);
  //gmm.playSpeed.connect(motionLine.callUpdatePlaySpeed);
  //gmm.playRepeat.connect(motionLine.callUpdatePlayRepeat);
  gmm.motionFilename.connect(callChangeTitle);

  //connection with robot

  gmm.gs.conn.connect(callSetConnectionTick); 
  gmm.gs.conn.connect(callOnConnection); 

  //gmm.gs.conn.signalCache();

  guideCanvas.showSubWindowSignal.connect(callShowSubWindowbyString);

  //onConnection(gmm.gs.conn.get());

  //guidePostureWindow.signalAddCurPost.connect(addCurPostureConn);
  //guidePostureWindow.signalApplyPost.connect(applyPostConn);
  

  /// do in its own class: guidePostureWindow.signal_realize().connect(gmm.eh.wrap(guidePostureWindow,&GuidePostureWindow::onRealize));//SUBWINDOW
  //guideCanvas.setShowSensorWindow.connect(callOnSensorWidgetClick);
  //guideCanvas.setShowGyroWindow.connect(callOnGyroWidgetClick);
  //guideCanvas.setShowColdetWindow.connect(callOnColdetWidgetClick);

  //widgets
  fixed.put(guideCanvas,0,0);
  fixed.put(angleEntry,0,0); 
  scroll.add(fixed);
  scroll.set_shadow_type(Gtk::SHADOW_NONE);
  scroll.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  scroll.set_border_width(0);

  mainBox.pack_start(scroll,Gtk::PACK_EXPAND_WIDGET);
  mainBox.pack_start(motionLine,Gtk::PACK_SHRINK);

  add(mainBox);

  //by config
  if(gmm.gs.pref.hasModel) {
    rm->mainCanvasArea.getWidget()->signal_button_press_event().connect(gmm.eh.wrap(*this,&GuideWindow::onModelAreaClick));
    rm->mainCanvasArea.getWidget()->signal_button_press_event().connect_notify(gmm.eh.wrap(guideCanvas,&GuideCanvas::onClickOther));
    fixed.put(rm->mainCanvasArea.getBox(),gmm.gs.settings.sizes.stroke,gmm.gs.settings.sizes.stroke);
    //rm->setCaptureWindowTransient(*this);
  }
  //motionLine.add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);

  //motionLine.signalButtonPress.connect(guideCanvas.callOnClickOther);  /// \todo signal 

  gmm.gs.view.connect(callers->callOnViewUpdate); //guideMenu.setViewSignal.connect(setViewConn);
  gmm.gs.view.set(cfg::guide::GuideView::MOTION);
  //guideCanvas.setView(cfg::guide::GuideView::MOTION);

  if(gmm.gs.pref.hasColdet) {
    coldetm->isOnCollidingFrame.connect(onCollidingFrameDisableCopyConn);
    coldetm->isOnCollidingFrame.connect(onCollidingFrameConn);
  }

  ///show all
  show_all_children();

  ///hide entry
  angleEntry.hide();  
  angleEntry.set_range(gmm.gs.joints.defaultCalib.min,gmm.gs.joints.defaultCalib.max);
  //DEBUG guideHistory angleEntry.setSaveState.connect(saveStateEntryConn);

  signal_delete_event().connect(gmm.eh.wrap(*this,&GuideWindow::onDeleteEvent));

  gmm.checkTicksDispatcher(); //insert tick at frame 0

  gmm.saved.motion.connect(callChangeTitleonSaveStatus);
  gmm.saved.calib.connect(callChangeTitleonSaveCalibStatus);
  guideMenu.onOpenCalib.connect(callInitCalib);

  //cout << "GuideWindow load OK" << endl;
  //cout << "---" << endl;
}

void GuideWindow::onCollidingFrame(bool b) {
  gmm.gs.joints.syncState[CursorType::ROBOT].assure(cfg::guide::SyncType::DISCONNECT);
  gmm.gs.joints.syncState[CursorType::FRAME].assure(cfg::guide::SyncType::DISCONNECT);
}

void GuideWindow::addSubWin(Gtk::Window& w, std::string s) {
  subwins.all.push_back(GuideSubWindow(&w,s));
  guideMenu.addSubWin(subwins.all.size()-1);
}

bool GuideWindow::onDeleteEvent(GdkEventAny *event) {
  //cout <<"attempting to close window" << endl;
  onQuit();
  return true;
}


void GuideWindow::onResize() {
  static double w0 = get_width()-gmm.gs.settings.sizes.windowOff.w;
  static double h0 = get_height()-gmm.gs.settings.sizes.windowOff.h;
  double wn = get_width()-gmm.gs.settings.sizes.windowOff.w;
  double hn = get_height()-gmm.gs.settings.sizes.windowOff.h;
  Point2D canvasSize = Point2D(guideCanvas.getWidth0(),guideCanvas.getHeight0());
  //if(motionLine.is_visible()) 
  //hn -= 4;
    
  //cout << wn << " " << w0 << " " << hn << " " << h0 << endl;

  scroll.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);

  if(wn <= w0 || hn <= h0) 
    scale = 1;
  else 
    if(wn/w0 < hn/h0) {
      scale = wn/w0;
    }
    else {
      scale = hn/h0;
    }
  helium::mulScalar(canvasSize,scale.get());
  //cout << canvasSize.x << " " << canvasSize.y << endl;
  guideCanvas.set_size_request(floor(canvasSize.x),floor(canvasSize.y));
  //motionLine.updateTicks();


  //by config
  if(gmm.gs.pref.hasModel) {
    ///resize and move the model so that it aligns with the line of nocoldet
    rm->mainCanvasArea.setScale(scale.get());   
    rm->mainCanvasArea.getBox().unparent();
    fixed.put(rm->mainCanvasArea.getBox(),gmm.gs.settings.sizes.stroke*scale.get(),gmm.gs.settings.sizes.stroke*scale.get());
  }

  angleEntry.hide();
  hexEntry.hide();

}

void GuideWindow::setDefaultSize() {
  //cout << "resize" << motionLine.get_height() << endl;
  if(motionLine.is_visible()) 
    resize(guideCanvas.getWidth0()+gmm.gs.settings.sizes.windowOff.w,guideCanvas.getHeight0()+gmm.gs.settings.sizes.windowOff.h); //26,70 //22,52
  else
    resize(guideCanvas.getWidth0()+gmm.gs.settings.sizes.windowOff.w,guideCanvas.getHeight0()+gmm.gs.settings.sizes.windowOff.h-motionLine.get_height()); //26,70 //22,52
}

void GuideWindow::exitAll() {
  onQuit();
}

void GuideWindow::changeHost() {
  stringstream ss;
  ss << gmm.gs.conn.get().host << ":" << gmm.gs.conn.get().port;
  string host = ss.str();
  InputDialog inputDialog("Input a different host",host,&gmm.eh);
  inputDialog.set_transient_for(*this);
  if(inputDialog.run()) {
    guideMenu.changeHost(host);
    gmm.gs.conn=helium::Connection::connect( gmm.gs.conn.get().host,gmm.gs.conn.get().port);
  }
}

///\brief handles key press events for all widgets inside GuideWindow, do not connect key signals elsewhere
///key press event for guide window includes
bool GuideWindow::onKeyPress(GdkEventKey *event) { 
  //if have shift mask, continue with window
  if ((event->state & GDK_MOD1_MASK) || (event->state & GDK_CONTROL_MASK)) return false;

  //key handled here: a,s,w,d,t,1,2,3,4,5,6,0
  if(guideCanvas.is_focus()) {
    /// keypress only when focused on guidecanvas, arrow keys and RobotModels::mainCanvasArea (was not possible to grab focus directly for this widget)
    if(guideCanvas.onKeyPressArrow(event)) 
      return true;
    else if(rm && rm->mainCanvasArea.onGdkKeyPress(event)) {
      return true;	
    }
  }
  else if(angleEntry.is_focus() || hexEntry.is_focus()) {
    return false; //continue on with processing keys
  }
  else if(gmm.gs.view == cfg::guide::GuideView::MOTION) {
    /// Make sure that it is on the motion view window
    /// if ScrolledWindow is in focus, keypress on motion line (delete ...)
    if(motionLine.onKeyPress(event)) { //if returning false, continue process
      return true;
    }
    //else if(event->keyval==GDK_r) {
    //  gmm.insertFrameFromRobot();
    //  return true;
    //}
  }
  ///no more key processing allowed after the event is processed
  ///process key events for other widgets
  guideMenu.updateMenuEdit(); //for updating shortcuts
  return false; //continue on with processing key
}

void GuideWindow::changeTitle() {
  string title = "Guide2";
  if(gmm.motionFilename.get().length()>0)
    title = title + " " + gmm.motionFilename.get();

  //not saved, print the warning
  if(!(gmm.saved.motion.get() && gmm.saved.calib.get())) {
    title = title + " *";
    title = title + (gmm.saved.motion.get()?"":"m");
    title = title + (gmm.saved.calib.get()?"":"c");
    title = title + "*";
  }
  set_title(title);
}

//when the tick of the menu connection is changed
void GuideWindow::onConnectMenu() {
  if (guideMenu.isConnectTickChanging()){
    return;
  }

  //cout << "==onConnectMenu " << endl;
  //helium::printTrace();
  //  dynamic_cast<Gtk::CheckMenuItem&>(guideMenu.menuConnect->get_submenu()->items()[0]).set_inconsistent(true);


  switch( gmm.gs.conn.get().state){
  case helium::Connection::DISCONNECTED:
    //std::cout<<"==is disc, setting connect"<<std::endl;
    gmm.gs.conn=helium::Connection::connect(gmm.gs.conn.get().host,helium::CELLPORT);
    //std::cout<<"==did connect, now "<<helium::en::io<<gmm.gs.conn.get().state<<std::endl;
    break;
  case helium::Connection::CONNECTED:
    //std::cout<<"==is conn, setting disconnect"<<std::endl;
    gmm.gs.conn=helium::Connection::disconnect();
    //std::cout<<"==did disc, now "<<helium::en::io<<gmm.gs.conn.get().state<<std::endl;
    break;
  default:
    //std::cout<<"==none of them"<<helium::en::io<<gmm.gs.conn.get().state<<std::endl;
    break;
  }
  //cout << "==endonConnectmentu" << endl;
}

void GuideWindow::onConnection(helium::Connection c) {
  //helium::printTrace();
  //std::cout<<"hey this is thread "<<helium::getThreadId()<<std::endl;

  
  try {
  switch(c.state) {
  case helium::Connection::CONNECTED : 


    

    guideCanvas.refresh(); 

    break;
  case helium::Connection::DISCONNECTED :
    //cout << "DISCONNECTED----------------------" << endl;
    for(size_t i=0; i<gmm.gs.joints.size(); i++) { 
      gmm.gs.joints[i].jointState.setCache(helium::UNKNOWNHWSTATE); //change color when disconnected      
      gmm.gs.joints[i].jointState.signalCache();//sending signal of the state
    }


    gmm.gs.joints.syncState[CursorType::ROBOT].assure(cfg::guide::SyncType::DISCONNECT);
    gmm.gs.joints.syncState[CursorType::FRAME].assure(cfg::guide::SyncType::DISCONNECT);
    guideCanvas.refresh(); 

    gmm.playMotion(false); //stop playing

    break;
  case helium::Connection::DISCONNECTING : 
  case helium::Connection::CONNECTING : 
  default:
    break;
  }
  }catch(...) {}
}

void GuideWindow::onQuit() {
  //cout << "onquit" << endl;
  bool b = true;
  if(!gmm.saved.motion.get()) {
    YesNoDialog d("Warning","File has not been saved, continue?");
    d.set_transient_for(*this);
    b = d.run();
  }
  if(!gmm.saved.calib.get()) { 
    YesNoDialog d("Warning","Calib file has not been saved, continue?");
    d.set_transient_for(*this);
    b = d.run();
  }

  if(b) {
    for (size_t i=0;i<subwins.all.size();i++){
      subwins.all[i].win->hide();
    }
    hide();
  }
}

///toggle the particular window
void GuideWindow::showSubWindowbyString(std::string name) {
  if(guideMenu.ViewMenuMap.find(name) == guideMenu.ViewMenuMap.end()) {
    throw helium::exc::Bug("error: guideWindow cannot find subWindow "+name);
  }
  else {
    guideMenu.toggleWindowVisibility(guideMenu.ViewMenuMap[name]-GuideMenu::VIEWNUM);
  }
}

Gtk::Window* GuideWindow::getSubWindow(std::string name) {
  if(guideMenu.ViewMenuMap.find(name) == guideMenu.ViewMenuMap.end())
    throw helium::exc::Bug("error: guideWindow cannot find subWindow "+name);

  return subwins.all[guideMenu.ViewMenuMap[name]-GuideMenu::VIEWNUM].win;
}


void GuideWindow::setView(cfg::guide::GuideView::Type v) {
  //cout << "change view to " << v << " " << gmm.gs.view.get() << endl;
  if(v == cfg::guide::GuideView::CALIB) {
    motionLine.hide();
    setDefaultSize();
    guidePostureWindow.enableApplyMenu(false);
    if(gmm.gs.pref.hasModel)
      getSubWindow("Teach List Window")->hide();
    getSubWindow("Postures List Window")->hide();
  }
  else if(v == cfg::guide::GuideView::MOTION || v == cfg::guide::GuideView::TOUCH) {
    //cout << "motion view" << endl;
    motionLine.show();
    setDefaultSize();
    guidePostureWindow.enableApplyMenu(true);
    //dynamic_cast<Gtk::CheckMenuItem&>(guideMenu.menuView->get_submenu()->items()[GuideMenu::POSTWIN]).set_sensitive(true);
  }
  if(v == cfg::guide::GuideView::TOUCH) {
    //show teach windows
    int x; int y;
    this->get_position(x,y);
    getSubWindow("Teach List Window")->show();
    getSubWindow("Teach List Window")->resize(300,this->get_height());
    getSubWindow("Teach List Window")->move(x+this->get_width(),y);
  }
}

bool GuideWindow::onModelAreaClick(GdkEventButton *event) {
  ///since it is clicked, grab focus on the RobotModels::mainCanvasArea if the focus is not grabbed by the frame view window
  rm->mainCanvasArea.getWidget()->grab_focus(); 
  if(event->button == 3){ /// switch model on right click
    rm->mainCanvasArea.modelid++;
  }
  else {
    switch(rm->mainCanvasArea.modelid.get()) {
    case cfg::guide::SwitchingModelArea::FRAME: showSubWindowbyString("Frame View"); break;
    case cfg::guide::SwitchingModelArea::ROBOT: showSubWindowbyString("Robot View"); break;
    case cfg::guide::SwitchingModelArea::FIX: showSubWindowbyString("Fix View"); break;
    default: break;
    }
  }
  return false; 
}


