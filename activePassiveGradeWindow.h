#ifndef ACTIVEPASSIVEGRADEWINDOW
#define ACTIVEPASSIVEGRADEWINDOW

#include <helium/core/nan.h>
#include<app/guide2/userInfo.h>
#include<gtkmm.h>

#include <app/guide2/guideTeachWindow.h>
#include "sensorCalibWindow/sensorCalibWindow.h"
#include "sortTeachFile.h"

class ActivePassiveGradeWindow : public Gtk::Window {
  Gtk::ToggleButton bactive,bpassive,bzero;
  Gtk::Label bactivel,bpassivel,bzerol;
  Gtk::Button bsave;
  Gtk::HBox hbox;
  bool clicking;
  std::string curFile;
  int curId;
  helium::Array<bool,experiment::RSEN> setAlready;
  helium::Array<double,experiment::SEN> curValue;
  helium::Array<double,experiment::SEN> defValue;
public:
  ActivePassiveGradeWindow():
    //bactive("-active"),bpassive("+passive"),bzero("zero"),
    bactivel("<b>active</b>"),
    bpassivel("<b>passive</b>"),
    bzerol("<b>zero</b>"),
    clicking(false),    
    curFile(""),curId(-1),
    callOnFileChange(this),
    callOnIdChange(this),
    callOnValueChange(this)
  {
    hbox.pack_start(bactive,Gtk::PACK_EXPAND_WIDGET);
    hbox.pack_start(bpassive,Gtk::PACK_EXPAND_WIDGET);
    hbox.pack_start(bzero,Gtk::PACK_EXPAND_WIDGET);
    hbox.pack_start(bsave,Gtk::PACK_EXPAND_WIDGET);


    bactive.signal_toggled().connect(sigc::mem_fun(this,&ActivePassiveGradeWindow::onbactive));
    bpassive.signal_toggled().connect(sigc::mem_fun(this,&ActivePassiveGradeWindow::onbpassive));
    bzero.signal_toggled().connect(sigc::mem_fun(this,&ActivePassiveGradeWindow::onbzero));
    bsave.signal_clicked().connect(sigc::mem_fun(this,&ActivePassiveGradeWindow::onbsave));

    bzerol.set_use_markup();
    bactivel.set_use_markup();
    bpassivel.set_use_markup();

    bzero.add(bzerol);
    bactive.add(bactivel);
    bpassive.add(bpassivel);

    helium::setAll(curValue,NAN);
    helium::setAll(setAlready,false);
    setBool(-1);
    add(hbox);
    set_default_size(300,100);
    this->move(100,400);
    //enableButtons(false);
    show_all_children();
    set_title("Grading active and passive");
  }

  void onbsave() {
    std::string posture = curFile.substr(curFile.find_last_of("_")+1,curFile.size());
    std::string folder = curFile.substr(0,curFile.find_last_of("/\\"));
    std::string nama = curFile.substr(curFile.find_last_of("/\\")+1,curFile.find_last_of("_"));
    nama = nama.substr(0,nama.find_last_of("_"));

    
    std::ofstream out;
    std::stringstream sf;
    //sf << folder << "/" << nama << "/" << posture << ".sensor3.mod.dot";


    std::string folderout = curFile.substr(0,curFile.find_last_of("/\\"));
    std::string fileout = curFile.substr(curFile.find_last_of("/\\")+1,curFile.size());
    sf << folderout << "/" << fileout.substr(0,fileout.size()-9) << ".sensor3.mod.dot"; 
    out.open(sf.str().c_str());
    std::cout << "file saved to " << sf.str() << std::endl;
    size_t i=0;
    while(i<experiment::RSEN) {
      out << curValue[i] << std::endl;
      i++;
    }

    std::stringstream ss;
    ss << bsave.get_label() << std::endl << "saved!";
    bsave.set_label(ss.str());

  }

  void setBool(int id,bool b=true) {
    std::cout << setAlready << std::endl;
    if(id>=0 && id<experiment::RSEN)
      setAlready[id] = b;
    int sum=0;
    int sumnan = 0;
    int sumdiff = 0;
    std::cout << "diff ";
    for(int i=0;i<experiment::RSEN;i++) {
      if(setAlready[i] && !isnan(curValue[i]))
	sum++;
      else if(isnan(curValue[i]))
	sumnan++;
      if(!isnan(curValue[i]) && !(
	 (curValue[i]<0 && defValue[i]<0) ||
	 (curValue[i]>0 && defValue[i]>0) ||
	 (curValue[i]==0 && defValue[i]==0) ))
	{
	std::cout << i << " ";
	sumdiff++;
      }
    }
    std::cout << std::endl;
    std::stringstream ss;
    ss << sum << "/" << experiment::RSEN-sumnan << "set" << std::endl << "diff: " << sumdiff;
    bsave.set_label(ss.str());
  }

  void next(int a) {
    //write and fix file
    curValue[curId] = a;
    setBool(curId);
    reloadFile(std::make_pair(curId,curValue[curId]));
    int nextId = curId+1;
    //while(isnan(curValue[nextId]))
    //nextId++;
    moveToNextId(nextId);
    std::cout << "movetonextid " << curId << " to "<< nextId << std::endl;
  }

  void resetbutton() {
    clicking=true;
    bactive.set_active(false);
    bpassive.set_active(false);
    bzero.set_active(false);
    clicking=false;
  }
 
  void onbactive() {
    if(clicking) return;

    if(!bactive.get_active()) {
      std::cout << "turn off" << std::endl;
      curValue[curId] = defValue[curId];
      setBool(curId,false);
      reloadFile(std::make_pair(curId,defValue[curId]));
      return;
    }

    clicking = true;
    bpassive.set_active(false);
    bzero.set_active(false);
    next(-1);
    clicking = false;
  }

  void onbpassive() {
    if(clicking) return;

    if(!bpassive.get_active()) {
      std::cout << "turn off" << std::endl;
      curValue[curId] = defValue[curId];
      setBool(curId,false);
      reloadFile(std::make_pair(curId,defValue[curId]));
      return;
    }

    clicking = true;
    bactive.set_active(false);
    bzero.set_active(false);
    next(1);
    clicking = false;
  }

  void onbzero() {
    if(clicking) return;

    if(!bzero.get_active()) {
      std::cout << "turn off" << std::endl;
      curValue[curId] = defValue[curId];
      setBool(curId,false);
      reloadFile(std::make_pair(curId,defValue[curId]));
      return;
    }

    clicking = true;
    bpassive.set_active(false);
    bactive.set_active(false);
    next(0);
    clicking = false;
  }
  void setFile(std::string s) {
    std::cout << "file is now " << s << std::endl;
    curFile = s;

    helium::setAll(setAlready,false);
    helium::setAll(curValue,NAN);
    helium::setAll(defValue,NAN);
    setBool(-1);

    std::stringstream ss;
    std::string folderout = curFile.substr(0,curFile.find_last_of("/\\"));
    std::string fileout = curFile.substr(curFile.find_last_of("/\\")+1,curFile.size());
    ss << folderout << "/" << fileout.substr(0,fileout.size()-9) << ".sensor3.mod.dot"; 
    std::cout << "opening " << ss.str() << std::endl;
    std::ifstream in;
    in.open(ss.str().c_str());
    int i=0;
    std::string line;
    if(in) {
      while(getline(in,line) && i<experiment::SEN) {
	std::cout << line << std::endl;
	std::stringstream sl;
	sl << line;
	if(strcmp(line.c_str(),"nan") != 0)
	  sl >> defValue[i];
	curValue[i] = defValue[i];
	i++;
      }
    }
    std::cout << curValue << std::endl;
    
  }

  //display id value on gui
  void setId(int id) {
    curId = id;
    //if(id<0 || id>89 || isnan(curValue[id])) {
    //  enableButtons(false);
    //  return;
    //}

    enableButtons();
    
    setButtons();


    std::stringstream ss;
    std::string nama = curFile.substr(curFile.find_last_of("/\\")+1,curFile.find_last_of("_"));
    nama = nama.substr(0,nama.find_last_of("_"));
    ss << nama << " " << id ;
    set_title(ss.str());
  }
  void setButtons() {
    clicking = true;
    if(setAlready[curId]) {
      std::cout << curId << " " << curValue[curId] << std::endl;
      bzero.set_active(curValue[curId]==0);
      bpassive.set_active(curValue[curId]>0);
      bactive.set_active(curValue[curId]<0);      
    }

  

    if(defValue[curId]==0) {
      bzerol.set_markup("<b>zero</b>");
      bpassivel.set_markup("passive");
      bactivel.set_markup("active");
    }else if(defValue[curId]>0) {
      bpassivel.set_markup("<b>passive</b>");
      bzerol.set_markup("zero");
      bactivel.set_markup("active");
    }else if(defValue[curId]<0) {
      bactivel.set_markup("<b>active</b>");
      bzerol.set_markup("zero");
      bpassivel.set_markup("passive");
     
    }
    clicking = false;
  }

  void enableButtons(bool b=true) {
    clicking=true;
    bzero.set_active(false);
    bpassive.set_active(false);
    bactive.set_active(false);
    clicking=false;


    bactive.set_sensitive(b);
    bpassive.set_sensitive(b);
    bzero.set_sensitive(b);
    bsave.set_sensitive(b);

    
   
   
   
  }

  void setValue(helium::Array<double> v) {
    curValue = v;
    defValue = v;
    
  }

  helium::Signal<std::pair<int,double> > reloadFile;
  helium::Signal<int> moveToNextId;
  helium::StaticConnReference<std::string,ActivePassiveGradeWindow,std::string,&ActivePassiveGradeWindow::setFile> callOnFileChange;
  helium::StaticConnReference<int,ActivePassiveGradeWindow,int,&ActivePassiveGradeWindow::setId> callOnIdChange;
  helium::StaticConnReference<helium::Array<double>,ActivePassiveGradeWindow,helium::Array<double>,&ActivePassiveGradeWindow::setValue> callOnValueChange;
};

class ActivePassiveModelWindow {
  helium::convex::ConvexModel modelDimensions;
public:
  RobotModels::SignallingConvexCollisionModel model;
  RobotModels::ModelWindow window;

  ActivePassiveModelWindow(GuideState &gs):
    modelDimensions(gs.joints.modelFilename.c_str()),
    model(&modelDimensions),
    window(model)
  {
  }
};

class ActivePassiveGradeMainWindow : public Gtk::Window {
  SensorCalibSubWindow &sensorCalibSubWindow;
  GuideTeachWindow &guideTeachWindow;
  ActivePassiveGradeWindow &activePassiveGradeWindow;
  ActivePassiveModelWindow &modelWindow;

  helium::ValueHub<int> activeId;

  void setTeachViewId(int id) {
    if(guideTeachWindow.getTreeView().getSelectedId() != id)
      guideTeachWindow.selectEntry(id);    
  }
  void setSensorViewId(int id) {
    sensorCalibSubWindow.sensorCalibDrawingWidget.active_sensor.assure(id);
  }
  void setGradeViewId(int id) {
    activePassiveGradeWindow.setId(id);
  }
  void setId(int id) {
    while(guideTeachWindow.gtl.get().gettl()[id].output == guideTeachWindow.gtl.get().gettl()[id].input)
      id++;
    if(id<(int)guideTeachWindow.gtl.get().gettl().size()) 
      activeId.assure(id);
  }
  void setToFirstId() {
    std::cout << "fuirst id "  << std::endl;
    activeId.set(0);
  }
  void setPosture(int id) {
    modelWindow.model.rotate(guideTeachWindow.gtl.get().gettl()[id].output.data);
  }
  helium::StaticConnReference<int,ActivePassiveGradeMainWindow,int,&ActivePassiveGradeMainWindow::setTeachViewId> callSetTeachViewId;
  helium::StaticConnReference<int,ActivePassiveGradeMainWindow,int,&ActivePassiveGradeMainWindow::setSensorViewId> callSetSensorViewId;
  helium::StaticConnReference<int,ActivePassiveGradeMainWindow,int,&ActivePassiveGradeMainWindow::setGradeViewId> callSetGradeViewId;
  helium::StaticConnReference<int,ActivePassiveGradeMainWindow,int,&ActivePassiveGradeMainWindow::setId> callOnTeachIdChange;
  helium::StaticConnReference<int,ActivePassiveGradeMainWindow,int,&ActivePassiveGradeMainWindow::setId> callOnSensorIdChange;
  helium::StaticConnReference<int,ActivePassiveGradeMainWindow,int,&ActivePassiveGradeMainWindow::setId> callSetNextId;
  helium::StaticConnReference<std::string,ActivePassiveGradeMainWindow,void,&ActivePassiveGradeMainWindow::setToFirstId> callSetToFirstId;
  helium::StaticConnReference<int,ActivePassiveGradeMainWindow,int,&ActivePassiveGradeMainWindow::setPosture> callSetPosture;

public:
  ActivePassiveGradeMainWindow(SensorCalibSubWindow &psensorCalibSubWindow, GuideTeachWindow &pguideTeachWindow, ActivePassiveGradeWindow &pactivePassiveGradeWindow, ActivePassiveModelWindow &pmodelWindow):
    sensorCalibSubWindow(psensorCalibSubWindow),
    guideTeachWindow(pguideTeachWindow),
    activePassiveGradeWindow(pactivePassiveGradeWindow), 
    modelWindow(pmodelWindow),
    callSetTeachViewId(this),
    callSetSensorViewId(this),
    callSetGradeViewId(this),
    callOnTeachIdChange(this),
    callOnSensorIdChange(this),
    callSetNextId(this),
    callSetToFirstId(this),
    callSetPosture(this)
  {
    //change id
    activeId.connect(callSetSensorViewId);     //activePassiveGradeWindow.moveToNextId.connect(callSetSensorViewId);
    activeId.connect(callSetTeachViewId);      //activePassiveGradeWindow.moveToNextId.connect(callSetTeachViewId);
    activeId.connect(callSetGradeViewId);
    activeId.connect(callSetPosture);
    activePassiveGradeWindow.moveToNextId.connect(callSetNextId);
    guideTeachWindow.selectionChangeSignal.connect(callOnTeachIdChange);
    guideTeachWindow.fileChangeSignal.connect(activePassiveGradeWindow.callOnFileChange);
    guideTeachWindow.fileChangeSignal.connect(callSetToFirstId);
    sensorCalibSubWindow.sensorCalibDrawingWidget.active_sensor.connect(callOnSensorIdChange);

    //show windows
    sensorCalibSubWindow.show();
    guideTeachWindow.show();
    activePassiveGradeWindow.show();
    modelWindow.window.show();
  }

};

#endif
