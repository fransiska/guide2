/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#include "guideMenu.h"
#include "guideWindow.h"
#include "yesNoDialog.h"

#include "jointsCalibExport.h"
#include <helium/xml/xmlLoader.h>
#include <app/guide2/inputDialog.h>

using namespace std;

GuideMenu::GuideMenu(GuideMotionManagement &pgmm, GuideWindow* pwin, std::vector<GuideSubWindow>& psubwins): //, helium::TsSignal<int> &errorUpdated):
  gmm(pgmm),
  win(pwin),
  subwins(psubwins),
  //connected(false),
  connTickChanging(false),
  coldetTickChanging(false),
  //curView(MOTION),
  callUpdateErrorNumber(this),
  callOnEditing(this),
  callEnablePaste(this),
  callSetColdetTick(this),
  callOnConnection(this,helium::FuncPlaceHolder<GuideMenu,helium::Connection,&GuideMenu::onConnection>())
{
  //connectedHost = "localhost"; //DEBUG change to guideState

  //defining view menu name and int map

  for(size_t i=0;i<subwins.size();i++){
    ViewMenuMap[subwins[i].menuLabel] = i + VIEWNUM;
  }


  for (size_t i=0;i<subwins.size();i++){
    subwins[i].win->signal_delete_event().connect(gmm.eh.wrap(*this,&GuideMenu::markWindowClosedEv,(int)i));    
    subwins[i].win->signal_hide().connect(gmm.eh.wrap(*this,&GuideMenu::markWindowClosed,(int)i));    
    subwins[i].win->signal_show().connect(gmm.eh.wrap(*this,&GuideMenu::markWindowOpened,(int)i));    
  }

  //changeHostDispatcher.connect(gmm.eh.wrap(*this,&GuideMenu::changeHostDCB));
  //connectDispatcher.connect(gmm.eh.wrap(*this,&GuideMenu::connectDCB));
  //disconnectDispatcher.connect(gmm.eh.wrap(*this,&GuideMenu::disconnectDCB));

  gmm.copying.connect(callEnablePaste);
  gmm.gs.motionInfo.editing.status.connect(callOnEditing);
  gmm.gs.conn.connect(callOnConnection);

  gmm.gs.errorn.connect(callUpdateErrorNumber);

  //errorUpdated.connect(callUpdateErrorNumber);

  gmm.gs.coldet.coldetState.connect(callSetColdetTick);

}
void GuideMenu::addTo(GuideWindow* win,Gtk::VBox& c) {
  ag = Gtk::ActionGroup::create();

  ag->add( Gtk::Action::create("MenuFile", "_File"), 
	   gmm.eh.wrap(*this, &GuideMenu::updateMenuFile) );
  ag->add( Gtk::Action::create("New", Gtk::Stock::NEW), 
	   gmm.eh.wrap(*this, &GuideMenu::newMotion) );

  ag->add( Gtk::Action::create("Open", Gtk::Stock::OPEN),
	   gmm.eh.wrap(*this,&GuideMenu::fileDialog,OPEN));
  ag->add( Gtk::Action::create("Save", Gtk::Stock::SAVE),
	   gmm.eh.wrap(*this, &GuideMenu::fileDialog,SAVE) );
  ag->add( Gtk::Action::create("SaveAs", Gtk::Stock::SAVE_AS),
	   gmm.eh.wrap(*this, &GuideMenu::fileDialog,SAVEAS) );

  ag->add( Gtk::Action::create("Export", Gtk::Stock::CONVERT,"Export As") );
  ag->add( Gtk::Action::create("Vstone", "Vstone Format") );

  ag->add( Gtk::Action::create("Quit", Gtk::Stock::QUIT), 
	   gmm.eh.wrap(*win, &GuideWindow::exitAll) );

  ag->add( Gtk::Action::create("MenuEdit", "_Edit"),
	   gmm.eh.wrap(*this, &GuideMenu::updateMenuEdit) );

  ag->add( Gtk::Action::create("Undo", Gtk::Stock::UNDO), 
	   Gtk::AccelKey("<Control>z"),gmm.eh.wrap(*this,&GuideMenu::undo)  );
  ag->add( Gtk::Action::create("Redo", Gtk::Stock::REDO), 
	   Gtk::AccelKey("<Shift><Control>z"),gmm.eh.wrap(*this,&GuideMenu::redo)  );
  ag->add( Gtk::Action::create("Copy", Gtk::Stock::COPY), 
	   gmm.eh.wrap(*this,&GuideMenu::copyFrame) );
  ag->add( Gtk::Action::create("Paste", Gtk::Stock::PASTE), 
	   gmm.eh.wrap(*this,&GuideMenu::pasteFrame)  );
  ag->add( Gtk::Action::create("CopyString",  "Cop_y Posture as String"),
	   Gtk::AccelKey("<Control>y"), gmm.eh.wrap(*this,&GuideMenu::copyPostureString) );
  ag->add( Gtk::Action::create("PasteString",  "Paste Posture _String"),
	   Gtk::AccelKey("<Control>p"), gmm.eh.wrap(*this,&GuideMenu::pastePostureString) );
  ag->add( Gtk::Action::create("PasteRobot",  "Paste Posture _Robot"),
	   Gtk::AccelKey("r"), gmm.eh.wrap(*this,&GuideMenu::pastePostureRobot) );
  ag->add( Gtk::Action::create("MirrorPosture",  "_Mirror Posture"),
	   Gtk::AccelKey("<Control>m"), gmm.eh.wrap(gmm.gs.joints,&cfg::guide::Joints::mirrorPosture) );



  ag->add(Gtk::Action::create("MenuConnect", "_Connect"));
  ag->add( Gtk::Action::create("Others", Gtk::Stock::UNDO, "_Others..."),
	   gmm.eh.wrap(*win, &GuideWindow::changeHost) );

  ag->add(Gtk::Action::create("MenuView", "_View"));
  ag->add( Gtk::Action::create("DefaultSize", Gtk::Stock::ZOOM_100, "_Default Size"),
	   gmm.eh.wrap(*win, &GuideWindow::setDefaultSize) );

  Gtk::RadioAction::Group group;
  ag->add( Gtk::RadioAction::create(group, "MotionView", "_Motion View", "Editing frames and robot with GUI"), Gtk::AccelKey("<alt>1"), gmm.eh.wrap(*this,&GuideMenu::onSetView,cfg::guide::GuideView::MOTION));
  ag->add( Gtk::RadioAction::create(group, "TeachingView", "_Teaching by Touching View", "Editing frames and robot with Teaching by Touching"), Gtk::AccelKey("<alt>2"), gmm.eh.wrap(*this,&GuideMenu::onSetView,cfg::guide::GuideView::TOUCH));
  ag->add( Gtk::RadioAction::create(group, "CalibView", "Motor _Calibration View", "Calibrating robot motors"), Gtk::AccelKey("<alt>3"), gmm.eh.wrap(*this,&GuideMenu::onSetView,cfg::guide::GuideView::CALIB));
  if(!gmm.gs.pref.hasModel)
    ag->get_action("TeachingView")->set_sensitive(false); //init as false, no copy posture available    

  ag->add( Gtk::ToggleAction::create("Coldet", "Colde_t", "Toggle Collision Detection"), Gtk::AccelKey("<control>t"), gmm.eh.wrap(*this,&GuideMenu::toggleColdet));
  if(!gmm.gs.pref.hasColdet) {
    ag->get_action("Coldet")->set_sensitive(false); //init as false, no copy posture available    
  }
  ag->get_action("Coldet")->activate();

  //ag->add(Gtk::Action::create("MenuFunction", "F_unction"));
  
  m_refUIManager=Gtk::UIManager::create();
  m_refUIManager->insert_action_group(ag);
  win->add_accel_group(m_refUIManager->get_accel_group());

  Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <menuitem action='New'/>"
    "      <menuitem action='Open'/>"
    "      <menuitem action='Save'/>"
    "      <menuitem action='SaveAs'/>"
    "      <menu action='Export'>"
    "        <menuitem action='Vstone'/>"
    "      </menu>"
    "      <separator/>"
    "      <menuitem action='Quit'/>"
    "    </menu>" 
    "    <menu action='MenuEdit'> "
    "       <menuitem action='Undo'/>"
    "       <menuitem action='Redo'/>"
    "       <menuitem action='Copy'/>"
    "       <menuitem action='Paste'/>"
    "       <menuitem action='PasteString'/>"
    "       <menuitem action='CopyString'/>"
    "       <menuitem action='MirrorPosture'/>"
    "       <separator/>"
    "       <menuitem action='PasteRobot'/>"
    "       <separator/>"
    "       <menuitem action='Coldet'/>"
    "    </menu>" 
    "    <menu action='MenuConnect'> "
    "       <menuitem action='Others'/>"
    "    </menu>" 
    "    <menu action='MenuView'>"
    "      <menuitem action='DefaultSize'/>"
    "      <separator/>"
    "      <menuitem action='MotionView'/>"
    "      <menuitem action='TeachingView'/>"
    "      <menuitem action='CalibView'/>"
    "    </menu>" 
    //    "    <menu action='MenuFunction'>"
    //    "      <separator/>"
    //    "    </menu>" 
    "  </menubar>"
    "</ui>";

  m_refUIManager->add_ui_from_string(ui_info);
  ag->get_action("Paste")->set_sensitive(false); //init as false, no copy posture available
  ag->get_action("Undo")->set_sensitive(false);
  ag->get_action("Redo")->set_sensitive(false);

  //connect toggle
  menuConnect = reinterpret_cast<Gtk::MenuItem*>(m_refUIManager->get_widget("/MenuBar/MenuConnect"));
  if(menuConnect){
    Gtk::Menu::MenuList &menulist = menuConnect->get_submenu()->items();
    stringstream ss;
    ss << gmm.gs.conn.get().host << ":" << gmm.gs.conn.get().port;
    menulist.push_front(Gtk::Menu_Helpers::CheckMenuElem(ss.str(), gmm.eh.wrap(*win, &GuideWindow::onConnectMenu)));
  }  



  
  //view toggle
  menuView = reinterpret_cast<Gtk::MenuItem*>(m_refUIManager->get_widget("/MenuBar/MenuView"));
  if(menuView){
    Gtk::Menu::MenuList &menulist = reinterpret_cast<Gtk::MenuItem*>(m_refUIManager->get_widget("/MenuBar/MenuView"))->get_submenu()->items();

    //Gtk::RadioButton::Group rbGroup;
    //menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    /*
    menulist.push_back(Gtk::Menu_Helpers::RadioMenuElem(rbGroup,"Motion View", gmm.eh.wrap
						       	<helium::TsValueHub<cfg::guide::GuideView::Type>,void,const cfg::guide::GuideView::Type&>							
							(gmm.gs.view,&helium::TsValueHub<cfg::guide::GuideView::Type>::set,  cfg::guide::GuideView::MOTION  )));
    
    menulist.push_back(Gtk::Menu_Helpers::RadioMenuElem(rbGroup,"Teaching by Touching View", gmm.eh.wrap
							<helium::TsValueHub<cfg::guide::GuideView::Type>,void,const cfg::guide::GuideView::Type&>								
							(gmm.gs.view,&helium::TsValueHub<cfg::guide::GuideView::Type>::set,cfg::guide::GuideView::TOUCH)));
    
    menulist.push_back(Gtk::Menu_Helpers::RadioMenuElem(rbGroup,"Motor Calibration View", gmm.eh.wrap	
							<helium::TsValueHub<cfg::guide::GuideView::Type>,void,const cfg::guide::GuideView::Type&>	
							(gmm.gs.view,&helium::TsValueHub<cfg::guide::GuideView::Type>::set,cfg::guide::GuideView::CALIB)));


							//this provides no accel key
    menulist.push_back(Gtk::Menu_Helpers::RadioMenuElem(rbGroup,"_Motion View", Gtk::AccelKey("<alt>1"), gmm.eh.wrap(*this,&GuideMenu::onSetView,cfg::guide::GuideView::MOTION)));
    menulist.push_back(Gtk::Menu_Helpers::RadioMenuElem(rbGroup,"_Teaching by Touching View", Gtk::AccelKey("<alt>2"), gmm.eh.wrap(*this,&GuideMenu::onSetView,cfg::guide::GuideView::TOUCH)));
    menulist.push_back(Gtk::Menu_Helpers::RadioMenuElem(rbGroup,"Motor _Calibration View", Gtk::AccelKey("<alt>3"), gmm.eh.wrap(*this,&GuideMenu::onSetView,cfg::guide::GuideView::CALIB)));
    */

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
    for (size_t i=0;i<subwins.size();i++){
      Gtk::Menu_Helpers::CheckMenuElem item=
	Gtk::Menu_Helpers::CheckMenuElem(subwins[i].menuLabel, gmm.eh.wrap(*this,&GuideMenu::toggleWindowVisibilityAdjustment,(int)(i)));
      menulist.push_back(item);
      checkItem.push_back(&dynamic_cast<Gtk::CheckMenuItem&>(menulist[menulist.size()-1])); //to save window number
    }
  }




  ////function toggle
  //menuFunction = reinterpret_cast<Gtk::MenuItem*>(m_refUIManager->get_widget("/MenuBar/MenuFunction"));
  //if(menuFunction){
  //  Gtk::Menu::MenuList &menulist = menuFunction->get_submenu()->items();
  //  menulist.clear();
  //  menulist.push_back(Gtk::Menu_Helpers::CheckMenuElem("Disable Collision Detection", gmm.eh.wrap(*this,&GuideMenu::toggleCollDet)));
  //}

  //dynamic_cast<Gtk::RadioAction&>(menuView->get_submenu()->items()[MOTIONVIEW]).set_active(true);

  Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
  c.pack_start(*pMenubar,Gtk::PACK_SHRINK);
  m_refUIManager->ensure_update();



  //temp
  dynamic_cast<Gtk::CheckMenuItem&>(menuConnect->get_submenu()->items()[0]).signal_activate_item().connect(gmm.eh.wrap(*this,&GuideMenu::onClicked));
	
}

void GuideMenu::toggleColdet() {
  if(coldetTickChanging) return; //avoid loop changing
  bool status = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(ag->get_action("Coldet"))->get_active();
  gmm.gs.coldet.coldetState.set(status);
}


void GuideMenu::onSetView(cfg::guide::GuideView::Type g){ //tod DELETE
  //cout << "view was " << gmm.gs.view.get() << " is now ";
  gmm.gs.view.assure(g);
  //cout << gmm.gs.view.get() << endl;
}

void GuideMenu::addSubWin(int i) {
  Gtk::Menu_Helpers::CheckMenuElem item=
    Gtk::Menu_Helpers::CheckMenuElem(subwins[i].menuLabel,
				     gmm.eh.wrap(*this,&GuideMenu::toggleWindowVisibilityAdjustment,i));
  Gtk::Menu::MenuList &menulist = menuView->get_submenu()->items();
  menulist.push_back(item);
  checkItem.push_back(&dynamic_cast<Gtk::CheckMenuItem&>(menulist[menulist.size()-1])); //to save window number
  
  subwins[i].win->signal_delete_event().connect(gmm.eh.wrap(*this,&GuideMenu::markWindowClosedEv,(int)i));    
  subwins[i].win->signal_hide().connect(gmm.eh.wrap(*this,&GuideMenu::markWindowClosed,(int)i));    
  subwins[i].win->signal_show().connect(gmm.eh.wrap(*this,&GuideMenu::markWindowOpened,(int)i));    
  ViewMenuMap[subwins[i].menuLabel] = i + VIEWNUM;
}

bool GuideMenu::markWindowClosedEv(GdkEventAny* ,int i){
  checkItem[i]->set_active(false);
  return false;
}

void GuideMenu::markWindowOpened(  int i){
  checkItem[i]->set_active(true);
}

void GuideMenu::markWindowClosed(  int i){
  checkItem[i]->set_active(false);
}

void GuideMenu::updateMenuEdit() {
  //cout << "connect " << gmm.gs.conn.get().state << endl;
  switch(gmm.gs.view.get()) {
  case cfg::guide::GuideView::CALIB:
    ag->get_action("Undo")->set_label("Undo "+ (win->gh.canUndoCalib()?win->gh.getTopCalibUndoEventString():""));
    ag->get_action("Redo")->set_label("Redo "+ (win->gh.canRedoCalib()?win->gh.getTopCalibRedoEventString():""));
    ag->get_action("Undo")->set_sensitive(win->gh.canUndoCalib());
    ag->get_action("Redo")->set_sensitive(win->gh.canRedoCalib());
    break;
  case cfg::guide::GuideView::MOTION:
  case cfg::guide::GuideView::TOUCH:
  default:
    ag->get_action("Undo")->set_label("Undo "+ (win->gh.canUndo()?win->gh.getTopUndoEventString():""));
    ag->get_action("Redo")->set_label("Redo "+ (win->gh.canRedo()?win->gh.getTopRedoEventString():""));
    ag->get_action("Undo")->set_sensitive(win->gh.canUndo());
    ag->get_action("Redo")->set_sensitive(win->gh.canRedo());
    break;
  }
}

void GuideMenu::updateMenuFile() {
  switch(gmm.gs.view.get()) {
  case cfg::guide::GuideView::CALIB:
    ag->get_action("New")->set_sensitive(false);
    ag->get_action("Export")->set_sensitive(false);
    ag->get_action("Open")->set_label("Open Calib");
    ag->get_action("SaveAs")->set_label("Save a Calib copy to");
    break;
  case cfg::guide::GuideView::MOTION:
  case cfg::guide::GuideView::TOUCH:
  default:
    ag->get_action("New")->set_sensitive(true);
    ag->get_action("Export")->set_sensitive(true);
    ag->get_action("Open")->set_label("Open Motion");
    break;
  }
}

void GuideMenu::toggleWindowVisibility(int i) {
  checkItem[i]->set_active(!checkItem[i]->get_active());
}

void GuideMenu::toggleWindowVisibilityAdjustment(int i){
  bool status = checkItem[i]->get_active();
  if(status)  { 
    subwins[i].win->show();
  }
  else
    subwins[i].win->hide();
}

// deleted 20130720
//void GuideMenu::toggleCollDet() {
//  gmm.noCollDet = !gmm.noCollDet.get();
//}

void GuideMenu::onEditing(bool b) {
  static bool pastee = false;
  if(b) {
    pastee = getEnablePaste();
    setEnableCopy(false);
    setEnablePaste(false);
  }
  else {
    setEnableCopy(true);
    setEnablePaste(pastee);
  }
}

void GuideMenu::onCollidingFrame(bool b) {
  setEnableCopy(!b);
}

void GuideMenu::changeHost(std::string s) {
  std::vector<std::string> tokens;
  helium::tokenize(tokens,s,":");
  
  gmm.gs.conn.setCache(helium::Connection(tokens[0],
					 tokens.size() > 1? helium::convert<int>(tokens[1]):gmm.gs.conn.get().port,
					 helium::Connection::DISCONNECTING));
		      
  dynamic_cast<Gtk::CheckMenuItem&>(menuConnect->get_submenu()->items()[0]).set_label(s);
}

void GuideMenu::setColdetTick(bool b) {
  coldetTickChanging = true;
  Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(ag->get_action("Coldet"))->set_active(b);
  coldetTickChanging = false;
}

//set the tick according to connection status change
void GuideMenu::setConnectionTick(const helium::Connection& c) {
  connTickChanging=true;
  Gtk::CheckMenuItem& cmi=*static_cast<Gtk::CheckMenuItem*>(&menuConnect->get_submenu()->items()[0]);

  
  switch( gmm.gs.conn.get().state){
  case helium::Connection::DISCONNECTED:
    cmi.set_active(false);
    cmi.set_inconsistent(false);
    break;
  case helium::Connection::CONNECTED:
    cmi.set_active(true);
    cmi.set_inconsistent(false);
    break;
  default:
    cmi.set_inconsistent(true);
    break;
  }

  /*dcb = true; //visited setconnectiontick flag
  if(c.state.val == cfg::guide::ConnState::CONNECTED) 
    connectDispatcher();
  else 
  disconnectDispatcher();*/
  connTickChanging=false;
}

void GuideMenu::newMotion() {
  //bool b = true;
  if(!gmm.saved.motion.get()) {
    YesNoDialog d("Warning","File has not been saved, continue?");
    d.set_transient_for(*win);
    if(!d.run()) return;
  }

  gmm.newMotion();
}

void GuideMenu::fileDialog(ActionType action) {
  if(gmm.gs.view.get() == cfg::guide::GuideView::CALIB)
    fileDialog(action,CALIB);
  else
    fileDialog(action,MOTION);
}

void GuideMenu::fileDialog(ActionType action,FileType type) {
  const char* title;

  switch(type){
  case MOTION:
    title="Please choose a motion file";   
    break;
  case POINTS:
    title="Please choose a point file";
    break;
  case CALIB:
    title="Please choose a calib file";
    break;
  case VSTONE:
    title="Please choose a vstone motion file";
    break;
  default:
    title="Please choose a file"; break;
  }

  Gtk::FileChooserDialog dialog(title,action==ADD||action==OPEN?Gtk::FILE_CHOOSER_ACTION_OPEN:Gtk::FILE_CHOOSER_ACTION_SAVE); 

  dialog.set_transient_for(*win);

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(action==SAVE||action==SAVEAS||action==EXPORT?Gtk::Stock::SAVE : Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
  Gtk::CheckButton loop("loop",false);
  if(type==VSTONE)
    dialog.set_extra_widget(loop);

  //Add filters, so that only certain file types can be selected:  
  if (type==MOTION){
    Gtk::FileFilter filter_xide;
    filter_xide.set_name("XIDE files");
    filter_xide.add_pattern("*.xide");
    if(gmm.motionFilename.get().length())
      dialog.set_current_folder(gmm.motionFilename.get().substr(0,gmm.motionFilename.get().find_last_of("/\\")));
    else
      dialog.set_current_folder("motion");
    dialog.add_filter(filter_xide);
  }
  if (type==CALIB){
    Gtk::FileFilter filter_xml;
    filter_xml.set_name("XML files");
    filter_xml.add_pattern("*.xml");
    //dialog.set_current_folder(gmm.gs.pref.motorCalibFilename.substr(0,gmm.gs.pref.motorCalibFilename.find_last_of("/\\")));
    dialog.add_filter(filter_xml);
  }
  if (type==VSTONE){
    Gtk::FileFilter filter_vstone;
    filter_vstone.set_name("VSTONE files");
    filter_vstone.add_pattern("*.vs.txt");
    dialog.set_current_folder("motion");
    dialog.add_filter(filter_vstone);
  }
  if (type==POINTS) {
    Gtk::FileFilter filter_pnt;
    filter_pnt.set_name("PNT files");
    filter_pnt.add_pattern("*.pnt");
    dialog.set_current_folder("mapping");
    dialog.add_filter(filter_pnt);
  }
  Gtk::FileFilter filter_text;
  filter_text.set_name("Text files");
  filter_text.add_mime_type("text/plain");
  dialog.add_filter(filter_text);

  //set to confirm overwriting files
  if (action==SAVE||action==SAVEAS) {
    dialog.set_do_overwrite_confirmation(true);
  }



  if(action==EXPORT) {
    std::string exportName ="export";
    int startOfName = (int)gmm.motionFilename.get().find_last_of("/")+1;
    if(gmm.motionFilename.get().size()>0) 
      exportName = gmm.motionFilename.get().substr(startOfName,gmm.motionFilename.get().size()-startOfName-4);
    if(type==VSTONE)
      dialog.set_current_name(exportName.append(".vs.txt"));
  }

  //Show the dialog and wait for a user response:
  int result = 0;
  std::string filename;

  //getting filename
  if (action==SAVE){ //for motion, save command
    //skip dialog if already have filename
    if(type==MOTION&&gmm.motionFilename.get().length()>0) {
      result=Gtk::RESPONSE_OK;
      filename=gmm.motionFilename.get(); 
    }
    else if(type==CALIB&&gmm.gs.pref.motorCalibFilename.length()>0) {
      result=Gtk::RESPONSE_OK;
      filename=gmm.gs.pref.motorCalibFilename;
    }
  }
  //run dialog
  if(result == 0) {
    bool b = true;
    if(type==MOTION&&!gmm.saved.motion.get()&&(action==OPEN)) { //opening a motion file when the motion has not been saved
      YesNoDialog d("Warning","File has not been saved, continue?");
      d.set_transient_for(*win);
      b = d.run();
    }
    else if(type==CALIB&&!gmm.saved.calib.get()&&(action==OPEN)) { //opening a motion file when the motion has not been saved
      YesNoDialog d("Warning","Calib file has not been saved, continue?");
      d.set_transient_for(*win);
      b = d.run();
    }
    if(!b) return; //return if the user cancel opening motion file

    //set default name
    switch(type) {
    case MOTION:
      if(gmm.motionFilename.get().length())
	dialog.set_filename(gmm.motionFilename.get());
      else if(action==SAVE || action==SAVEAS)
	dialog.set_current_name("motion.xide");
      break;
    case POINTS:    
      dialog.set_filename("point.pnt");
      break;
    case CALIB:
      dialog.set_filename(gmm.gs.pref.motorCalibFilename);
    default:
      break;
    }

    result=dialog.run();
    filename=dialog.get_filename();
  }

  if (result==Gtk::RESPONSE_OK){    
    switch(action){
    case OPEN:
      switch(type) {
      case MOTION: 
	gmm.openMotion(filename.c_str()); 
	break;
      case POINTS: break;
      case CALIB:{ 
	helium::Memory inmem;
	helium::XmlLoader in;
	in.loadDataAndModel(inmem,filename);
	helium::cimport<helium::JointsCalibExport>(gmm.gs.joints,inmem);
	onOpenCalib();
	gmm.saved.calib.assure(true);
      }
	break;
      default: break;
      }
      break;
    case SAVE: {
      //todo before saving file
      switch(type) {
      case CALIB: {
	//backup old file
	std::ifstream  src(filename.c_str(), std::ios::binary);
	std::stringstream ss;

	size_t dot = filename.find_last_of(".");
	std::string name = filename;
	std::string ext = ".xml";
	if (dot != std::string::npos) {
	  name = filename.substr(0, dot);
	  ext  = filename.substr(dot, filename.size() - dot);
	}


	//time for backup file
	struct tm * timeinfo;
	time_t rawtime;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char buffer[80];
	strftime(buffer,80,"%Y%m%d%H%M%S",timeinfo);


	ss << name << std::string(buffer) << ext;

	std::ofstream  dst(ss.str().c_str(), std::ios::binary);
	dst << src.rdbuf();

	break;
      }
      default: break;
      }
    }
    case SAVEAS:
      switch(type) {
      case MOTION:
      gmm.motionFilename = filename;
      gmm.saveMotion(filename.c_str()); 
      gmm.saved.motion.assure(true);
      break;
      
      case POINTS:break;
      case CALIB: {
	helium::Memory outmem;
	helium::XmlLoader out;      
	helium::cexport<helium::JointsCalibExport>(outmem,gmm.gs.joints);
	out.save(filename.c_str(),outmem);  
	gmm.saved.calib.assure(true);
	break;
      }
      default: break;
      }
      
      break;
    case ADD:
      switch(type) {
      case POINTS: break;
      default: break;
      }
      break;
    case EXPORT:
      switch(type) {
      case VSTONE: break;
      default: break;
      }
      break;
    }
  }//response=ok
}

void GuideMenu::pastePostureRobot() {
  gmm.insertFrameFromRobot();
}

void GuideMenu::pastePostureString() {
  //dialog window
  string pos = "";
  InputDialog inputDialog("Paste posture string",pos,&gmm.eh);
  inputDialog.set_transient_for(*win);
  bool ans = inputDialog.run();
  helium::Posture temp(gmm.gs.joints.size());
  if(ans) {
    helium::convert(temp,pos," Invalid posture string");
    gmm.pasteFrameReason<GuideEvent::PASTESTRING,GuideEvent::PASTESTRINGJOINTCHANGE>(temp);
  }
}

void GuideMenu::copyPostureString() {
  Glib::RefPtr<Gtk::Clipboard> refClipboard = Gtk::Clipboard::get();
  std::string s;
  helium::Posture p(gmm.gs.joints.size());
  gmm.getFramePosture(p.data,gmm.gs.motionInfo.curTime.get());
  helium::convert(s,p);
  refClipboard->set_text(s);
}

void GuideMenu::copyFrame() {
  gmm.copyFrame();
  gmm.copying = true;
}

void GuideMenu::pasteFrame() {
  gmm.pasteFrame(gmm.gs.motionInfo.curTime.get());
}

void GuideMenu::undo() {
  switch(gmm.gs.view.get()) {
  case cfg::guide::GuideView::CALIB:
    if(win->gh.canUndoCalib()) {
      win->gh.undoCalib();
      gmm.saved.calib.assure(false);   
    }
    break;
  case cfg::guide::GuideView::MOTION:
  case cfg::guide::GuideView::TOUCH:
  default:
    if(win->gh.canUndo()) {
      win->gh.undo();
      gmm.saved.motion.assure(false);  
    }
    break;
  }
  updateMenuEdit();
}

void GuideMenu::redo() {
  switch(gmm.gs.view.get()) {
  case cfg::guide::GuideView::CALIB:
    if(win->gh.canRedoCalib()) {
      win->gh.redoCalib();
      gmm.saved.calib.assure(false);
    }
    break;
  case cfg::guide::GuideView::MOTION:
  case cfg::guide::GuideView::TOUCH:
  default:
    if(win->gh.canRedo()) {
      win->gh.redo();
      gmm.saved.motion.assure(false);      
    }
    break;
  }
  updateMenuEdit();
}

void GuideMenu::updateErrorNumber(int i) {
  std::stringstream ss;
  ss << "Error List Window";
  if(i)
    ss << " (" << i <<")";
  dynamic_cast<Gtk::CheckMenuItem&>(menuView->get_submenu()->items()[ViewMenuMap["Error List Window"]]).set_label(ss.str());
}

void GuideMenu::onConnection(helium::Connection c) {
  switch(c.state) {
  case helium::Connection::CONNECTED : 
    ag->get_action("PasteRobot")->set_sensitive(true);    
    break;
  default:
    ag->get_action("PasteRobot")->set_sensitive(false);    
    break;
  }
}
