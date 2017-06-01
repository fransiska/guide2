#ifndef TEACHVIEWERWINDOW
#define TEACHVIEWERWINDOW
#include <app/guide2/guideTeachWindow.h>
#include <fstream>
#include "sensorCalibWindow/sensorCalibWindow.h"
#include "floatingModelWindow.h"
#include <app/guide2/sensorCalibWindow/sensorPlotWindow.h> //run util/sensorVector

const std::string defaultFolder="/home/fransiska/research/touchExperiment/";
const int maxSensorId = 113;
const int sensorSize = 90;
const int motorSize = 22;







inline  void updateLastOpenedFile(std::string lof, std::string &folder, std::string &title,std::string &posture,std::string &username) {
    if(lof.size()) {
      folder = lof.substr(0,lof.find_last_of("/\\"));
      title = lof.substr(folder.find_last_of("/\\")+1,lof.size());
      username = lof.substr(folder.find_last_of("/\\")+1,title.find_last_of("/\\"));
      posture = title.substr(0,title.find('.'));
      posture = posture.substr(posture.find_last_of("/\\")+1,posture.size());
    }
  }




class TeachViewerWindow : public Gtk::Window {
  SensorCalibSubWindow &sensorCalibSubWindow;
  GuideTeachWindow &guideTeachWindow;
  FloatingFrameModelWindow &modelWindow; 
 Gtk::VBox m_VBox;
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
  std::string lastOpenedFile;
  std::string title,folder,username,posture;
public:
  TeachViewerWindow(SensorCalibSubWindow &psensorCalibSubWindow, GuideTeachWindow &pguideTeachWindow,FloatingFrameModelWindow &pmodelWindow,ActivePassiveGradeWindow *apWindow):
    sensorCalibSubWindow(psensorCalibSubWindow),guideTeachWindow(pguideTeachWindow),modelWindow(pmodelWindow),
    callOnActiveIdChange(this),
    callMoveToNextId(this)
  {

    sensorCalibSubWindow.active_sensor.connect(callOnActiveIdChange);
    guideTeachWindow.getTreeView().get_selection()->signal_changed().connect(sigc::mem_fun(this,&TeachViewerWindow::on_selection_change));



    resetPosition();

    //Create actions for menus and toolbars:
    m_refActionGroup = Gtk::ActionGroup::create(); 

    m_refActionGroup->add(Gtk::Action::create("Open", Gtk::Stock::OPEN,"_Open"));
    m_refActionGroup->add(Gtk::Action::create("OpenPos", Gtk::Stock::ORIENTATION_PORTRAIT,"_OpenPos"));
    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);
    this->add_accel_group(m_refUIManager->get_accel_group());

    
    Glib::ustring ui_info = 
      "<ui>"
      "  <toolbar name='ToolBar'>"
      "    <toolitem action='Open'/>"
      "    <toolitem action='OpenPos'/>"
      "  </toolbar>"
      "</ui>";


    m_refActionGroup->get_action("Open")->signal_activate().connect( sigc::mem_fun(this,&TeachViewerWindow::on_open_file) );
    m_refActionGroup->get_action("OpenPos")->signal_activate().connect( sigc::mem_fun(this,&TeachViewerWindow::on_open_posturefile) );

    
    try { m_refUIManager->add_ui_from_string(ui_info); }
    catch(const Glib::Error& ex) { std::cerr << "building menus failed: " <<  ex.what(); }
    
    Gtk::Toolbar* pToolbar = dynamic_cast<Gtk::Toolbar*>(m_refUIManager->get_widget("/ToolBar")) ;
    
    if(pToolbar) {
      m_VBox.pack_start(*pToolbar, Gtk::PACK_EXPAND_WIDGET);
      pToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH);
    }
    else 
      std::cerr << "error: TeachViewer no toolbar" << std::endl;

    guideTeachWindow.getToolbar()->hide();
    add(m_VBox);
    set_default_size(140,-1);
    set_title("Teach Viewer");
    show_all_children();
    if(apWindow)
      apWindow->show();

    


  }

  void resetPosition() {
    int x,y;
    this->move(0,400);
    this->get_position(x,y);

    sensorCalibSubWindow.show();
    guideTeachWindow.show();
    modelWindow.getModelWindow().show();

    sensorCalibSubWindow.move(x,y+this->get_height());
    guideTeachWindow.move(x+sensorCalibSubWindow.get_width(),y+this->get_height());
    modelWindow.getModelWindow().move(x,y+300+modelWindow.getModelWindow().get_height());

    guideTeachWindow.resize(450,600);

    readSensorVector(); //read file for plot
    modelWindow.getModelWindow().set_title(username);
  }

  //open file and convert to sort by sensor id
  void on_open_file() {

    try{
      Gtk::FileChooserDialog dialog("Open and Convert", Gtk::FILE_CHOOSER_ACTION_OPEN);
      dialog.set_transient_for(*this);
      dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
      dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
      dialog.set_current_folder(defaultFolder);
      /*
	if(lastOpenedFile.size()) {
	string f = lastOpenedFile.substr(0,lastOpenedFile.find_last_of("/\\"));
	dialog.set_current_folder(f);
	}
      */
      Gtk::FileFilter filter_tea;
      filter_tea.set_name("TEA files");
      filter_tea.add_pattern("*.tea");
      dialog.add_filter(filter_tea);
      
      if(dialog.run()==Gtk::RESPONSE_OK) {
	lastOpenedFile = dialog.get_filename();
	//guideTeachWindow.lastOpenedFile = lastOpenedFile;
	updateLastOpenedFile(lastOpenedFile,folder,title,posture,username);
	guideTeachWindow.set_title(title);
	
	std::string conver = lastOpenedFile;
	conver = conver.substr(0,conver.find_last_of("."));
	conver.append(".conv.tea");
	
	try {
	  convertFile(lastOpenedFile,conver);
	  guideTeachWindow.set_title(title);
	  guideTeachWindow.openFile(true,conver);
	}
	catch(std::string &e) {
	  std::cerr <<"error reading file " << e << std::endl;
	}catch(...) {
	  std::cerr << "error reading file " << lastOpenedFile << std::endl;
	}
      }
    }//try
    catch(helium::exc::Exception& e){
      resetPosition();
      e.rethrow();
    }

    resetPosition();
  }
public:
  void on_open_posturefile() {
    guideTeachWindow.on_button_file(GuideTeachWindow::OPEN);
    lastOpenedFile = guideTeachWindow.lastOpenedFile;
    updateLastOpenedFile(lastOpenedFile,folder,title,posture,username);
    guideTeachWindow.set_title(title);
    resetPosition();
  }
  void readSensorVector() {
    updateSensorPlot(folder+"_"+posture);
    updateSensorPie(folder+"_"+posture);
  }
  void on_selection_change() {
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = guideTeachWindow.getTreeView().get_selection();
    if(refSelection) {
      Gtk::TreeModel::iterator iter = refSelection->get_selected();
      if(iter) {
	int id = (*iter)[guideTeachWindow.getTreeView().getColumn().m_col_id];
	sensorCalibSubWindow.active_sensor = id;

	//or input?
	guideTeachWindow.onApplytoFrame(guideTeachWindow.gtl.get().gettl()[id].output);

	if(modelWindow.getCCM().isColliding(false))
	  std::cout << id << " isColliding " << guideTeachWindow.gtl.get().gettl()[id].output << std::endl;

	changeAPid(id);


	/*
	helium::Array<double,22> delta;
	delta = guideTeachWindow.gtl.get().gettl()[id].input;

	bool bothleg = false;
	bool left = false;
	bool right = false;
	for(int i=9;i<22;i+=2) {
	    if(fabs(delta[i]) > 0.15) {
	    left = true;
	    break;
	  }
	}
	for(int i=8;i<22;i+=2) {
	  if(fabs(delta[i]) > 0.15) {
	    right = true;
	    break;
	  }
	}
	
	if((left && right))
	  bothleg = true;
	
	//body zero
	
	std::cout << id << 	((bothleg)?" both ":"") << std::endl;
	*/

      }
    }
  }
  void on_active_id_change(int id) {
    guideTeachWindow.selectEntry(id);    
  }
  void moveToNextId(int id) {
    sensorCalibSubWindow.active_sensor.set(id);
  }
  helium::StaticConnReference<int,TeachViewerWindow,int,&TeachViewerWindow::on_active_id_change> callOnActiveIdChange;
  helium::StaticConnReference<int,TeachViewerWindow,int,&TeachViewerWindow::moveToNextId> callMoveToNextId;
  helium::Signal<std::string> updateSensorPlot;
  helium::Signal<std::string> updateSensorPie;
  helium::Signal<int> changeAPid;
};
#endif
