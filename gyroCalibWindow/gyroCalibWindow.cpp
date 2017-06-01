/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "gyroCalibWindow.h"
#include "gyroFunction.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <app/guide2/glarea.h>
//#include <heliumBased/robot/getGyroCalibration.h>

using namespace std;

struct GyroCalibWindow::Callers{
  helium::StaticConnReference<helium::Connection,GyroCalibWindow,const helium::Connection&,&GyroCalibWindow::onConnection> callOnConnection;
  helium::StaticConnReference<const std::pair<int,helium::HwState>&,GyroCalibWindow,const std::pair<int,helium::HwState>&,&GyroCalibWindow::onStateChange> callOnStateChange;
  helium::StaticConnReference<const std::pair<int,helium::MonitorMode>&,GyroCalibWindow,void,&GyroCalibWindow::onMonitorModeChange> callOnMonitorModeChange;
  helium::Array<helium::StaticConnReference<bool,GyroCalibWindow,void,&GyroCalibWindow::onPowerChange>,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<
		  helium::ParamAllocator<
		    helium::StaticConnReference<bool,GyroCalibWindow,void,&GyroCalibWindow::onPowerChange>, GyroCalibWindow*
		    > 
		  > 
		>callOnPowerChange; ///connection to power to refresh tick

  Callers(GyroCalibWindow* gcw,size_t n):
    callOnConnection(gcw),
    callOnStateChange(gcw),
    callOnMonitorModeChange(gcw),
    callOnPowerChange(gcw,n) // on or off
  {}
};


GyroCalibWindow::GyroCalibWindow(cfg::guide::Joints &pjoints,
				 cfg::guide::Gyro &pgyro,
				 helium::GtkProxy<helium::Connection> &pconn,
				 const std::string &pcalibFile,
				 cfg::guide::Colors &pcolors,
				 helium::GtkExceptionHandling *peh, 
				 int pdesample,
				 RobotModels::ModelArea *ma):
  gyroCalib(pgyro,pdesample,ma),
  joints(pjoints),gyro(pgyro),conn(pconn),eh(peh),
  gBox(this),
  frame_gl("",Gtk::ALIGN_CENTER,Gtk::ALIGN_CENTER,1.0,true),
  radio(0),
  calibFile(pcalibFile),
  gyroConnected(false),visible(false),
  callOnAcquire(this),
  callers(new GyroCalibWindow::Callers(this,joints.size()))
{
  set_title("Gyro Calibration");
  set_border_width(2);
  boxv_main.pack_start(gBox, Gtk::PACK_SHRINK);

  add(boxv_main);

  powerClicked = false;
  monitorModeClicked = false;

  acquire = false;
  acquire.connect(callOnAcquire);

#ifndef WIN32 //not available for windows
  if(ma) {
    frame_gl.add(ma->getBox());
    boxv_main.pack_end(frame_gl, Gtk::PACK_EXPAND_WIDGET);
  }
#endif





  for(int i=0;i<SETS;i++)
    dataarraysize[i]=0;


  for(int i=0;i<SETS+1;i++)
    gBox.r_button[i].signal_clicked().connect(sigc::mem_fun(*this,&GyroCalibWindow::update_radio_status));

  for(int i=0;i<SETS;i++)
    gBox.c_button[i].signal_clicked().connect(sigc::mem_fun(*this,&GyroCalibWindow::update_check_status));

  for(size_t i=0; i<joints.size();i++) 
    joints[i].jointPower.connect(callers->callOnPowerChange[i]);
 
  gBox.m_refActionGroup->get_action("Open")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::onOpen));
  gBox.m_refActionGroup->get_action("Save")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::onSave));
  gBox.m_refActionGroup->get_action("SaveAs")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::onSaveAs));
  gBox.m_refActionGroup->get_action("Acquire")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::toggleAcquire));
  gBox.m_refActionGroup->get_action("Calc")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::onCalcOne));
  gBox.m_refActionGroup->get_action("CalcAll")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::onCalcAll));
  gBox.m_refActionGroup->get_action("Mean")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::onMean));
  gBox.m_refActionGroup->get_action("Use")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::onUse));
  gBox.m_refActionGroup->get_action("Quit")->signal_activate().connect(sigc::mem_fun(*this,&GyroCalibWindow::onQuit));

  gBox.c_power.signal_clicked().connect(sigc::mem_fun(*this,&GyroCalibWindow::onPowerClick));

  gBox.c_mmode.signal_clicked().connect(sigc::mem_fun(*this,&GyroCalibWindow::onMonitorModeClick));
  gyro.monitorMode.connect(callers->callOnMonitorModeChange);

  signal_show().connect(eh->wrap(*this,&GyroCalibWindow::onShowWindow));
  signal_hide().connect(eh->wrap(*this,&GyroCalibWindow::onHideWindow));

  conn.connect(callers->callOnConnection);
  gyro.state.connect(callers->callOnStateChange);
  gBox.m_refActionGroup->get_action("Acquire")->set_sensitive(false);

  update_radio_status();
  update_check_status();
  update_calc_status();
  update_label();

  

  show_all_children();
}


void GyroCalibWindow::update_radio_status() {
  for(int i=0;i<SETS+1;i++)
    if(gBox.r_button[i].get_active()) 
      radio=i;

  bool b5;
  if(radio==5)
    b5=false;
  else
    b5=true;
  //gBox.m_refActionGroup->get_action("Acquire")->set_sensitive(b5);
  gBox.m_refActionGroup->get_action("CalcAll")->set_sensitive(b5);
  gBox.m_refActionGroup->get_action("Calc")->set_sensitive(b5);
  update_calc_status();
}

void GyroCalibWindow::update_check_status() {
  check.clear();

  for(int i=0;i<SETS;i++)
    if(gBox.c_button[i].get_active())
      check.push_back(make_pair(i,gBox.entry[i].get_text()));

  if(check.size() < 2)
      gBox.m_refActionGroup->get_action("Mean")->set_sensitive(false);
  else
      gBox.m_refActionGroup->get_action("Mean")->set_sensitive(true);
}



void GyroCalibWindow::calc() {
  helium::Array<double,SETS> c;//  double c[SETS];
  int n=dataarraysize[radio];

  vector<pair<int,int> > idx;

  if (all){
    getAllCouples(idx,n);
  }else{
    getRandomCouples(idx,n);      
  }
  if (dataarraysize[radio]==0){
    cerr<<"Data not acquired for set "<<radio<<endl;
    return;
  }

  gyroCalib.getGyroCalibration(c.data,dataarray[radio],idx); //gets gyro calib from data
  //gyroCalib.getGyroCalibration(c.data,dataarray[radio],idx); //array version

  stringstream ss;
  for(size_t i=0;i<(size_t)SETS;i++)
    ss << c[i] << " ";

  gBox.entry[radio].set_text(ss.str());   //put 5 double to entry

  //put extra info on label
  double vars[3],rm,rvar;
  getStatistics(vars,rm,rvar,dataarray[radio],n,c.data);
  ss.str("");
  ss<<"x="<<sqrt(vars[0])<<" y="<<sqrt(vars[1])<<" z="<<sqrt(vars[2])<<" r="<<rm<<" var r="<<rvar;
  //cout<<"std x= "<<sqrt(vars[0])<<" std y= "<<sqrt(vars[1])<<" std z= "<<sqrt(vars[2])<<" r= "<<rm<<" var r= "<<rvar<<endl;
  gBox.label[radio].set_label(ss.str());

}

void GyroCalibWindow::onCalcOne() {
  all = false;
  calc();
}

void GyroCalibWindow::onCalcAll() {
  all = true;
  calc();
}

void GyroCalibWindow::setCurrentLabel(double* c){
  stringstream ss;
  for (int i=0;i<5;i++)
    ss<<c[i]<<" ";
  gBox.entry[radio].set_text(ss.str());
}

void GyroCalibWindow::toggleAcquire() {  
  acquire.set(!acquire.get());
}

void GyroCalibWindow::onAcquire(bool acq) {  
  //if(gBox.m_refActionGroup->get_action("Acquire")->get_label().compare("Stop")) {
  if(acq) {
    gBox.m_refActionGroup->get_action("Acquire")->set_label("Stop");
    gBox.m_refActionGroup->get_action("Acquire")->set_stock_id(Gtk::Stock::DISCONNECT);

    //if (dataarray[radio]!=NULL){
    //delete[] dataarray[radio];
    //dataarray[radio]=NULL;
    //}
    gyroCalib.clearAcquiredData();
    gyroCalib.setDataCollection(true);

    gBox.m_refActionGroup->get_action("Open")->set_sensitive(false);
    gBox.m_refActionGroup->get_action("Save")->set_sensitive(false);
    gBox.m_refActionGroup->get_action("SaveAs")->set_sensitive(false);

    gBox.m_refActionGroup->get_action("Mean")->set_sensitive(false);
    gBox.m_refActionGroup->get_action("Calc")->set_sensitive(false);
    gBox.m_refActionGroup->get_action("CalcAll")->set_sensitive(false);
  }
  else {
    gBox.m_refActionGroup->get_action("Acquire")->set_label("Acquire");
    gBox.m_refActionGroup->get_action("Acquire")->set_stock_id(Gtk::Stock::CONNECT);
    gyroCalib.setDataCollection(false);
    dataarraysize[radio] = gyroCalib.getDataSize(); 
    stringstream ss;
    ss << "Collected " << dataarraysize[radio] << " on data " << radio;
    gBox.label[SETS].set_label(ss.str());

    gyroCalib.getData(dataarray[radio]);     //copy acquired data pointer to dataarray
    //gyroCalib.getData(dataarrayh[radio]); //array version

    gBox.m_refActionGroup->get_action("Open")->set_sensitive(true);
    gBox.m_refActionGroup->get_action("Save")->set_sensitive(true);
    gBox.m_refActionGroup->get_action("SaveAs")->set_sensitive(true);

    update_check_status();
    update_calc_status();
  }

  //set buttons not clickable
  gBox.m_refActionGroup->get_action("Use")->set_sensitive(!acq);

  for(int i=0;i<SETS+1;i++)
    gBox.r_button[i].set_sensitive(!acq);
  for(int i=0;i<SETS;i++)
    gBox.c_button[i].set_sensitive(!acq);
}

void GyroCalibWindow::onMean() {
  update_check_status();
  meanData.clear();
  stringstream ss;

  unsigned int i=0;
  while(i<check.size()) {
    ss.clear();
    ss.str("");
    ss << check[i].second;

    double n;
    vector<double> nn;
    while(ss >> n)
      nn.push_back(n);

    if(nn.size()<5) {
      cerr << "didn't find 5 items in entry " << check[i].first << endl;
    }
    else
      meanData.push_back(nn);
    i++;
  }
  if(meanData.size()>1) {
    double n[SETS];
    stringstream ss;
    getMean(meanData,n);
    ss << n[0] <<" " << n[1] <<" " << n[2] <<" " << n[3] <<" " << n[4];
    gBox.entry[SETS].set_text(ss.str());
  }
}

void GyroCalibWindow::onUse() {
  stringstream ss;
  ss << gBox.entry[radio].get_text();

  double n[SETS];
  if(ss >> n[0] >> n[1] >> n[2] >> n[3] >> n[4]) {
    gyro.calib.setCalib(n);
    update_label();
  }
  else
    cerr << "didn't find 5 items" << endl;

}

void GyroCalibWindow::update_label() {
  //show initial calib data 
  //gyroCalib.getCalib();
  stringstream ss;
  helium::Array<double> c;
  gyroCalib.getCalib(c);

  for(size_t i=0;i<c.size();i++)
    ss << c[i] << " ";
  gBox.l_data.set_text(ss.str());
}

void GyroCalibWindow::update_calc_status() {
  if(!dataarraysize[radio]) {
    gBox.m_refActionGroup->get_action("Calc")->set_sensitive(false);
    gBox.m_refActionGroup->get_action("CalcAll")->set_sensitive(false);
  }
  else {
    gBox.m_refActionGroup->get_action("Calc")->set_sensitive(true);
    gBox.m_refActionGroup->get_action("CalcAll")->set_sensitive(true);
  }
}

void GyroCalibWindow::onPowerClick() {
  //cout << "power()" << endl;
  powerClicked = true;
  if(gBox.c_power.get_active())
    setPower(true);
  else
    setPower(false);
}

void GyroCalibWindow::setPower(bool b) {
  for(size_t i=0; i<joints.size();i++)
    joints[i].jointPower.assure(b);
}

void GyroCalibWindow::onPowerChange() { //update ticks on power change
  if(powerClicked) { //avoid loop
    powerClicked = false;
    return;
  }
  int p = 0;
  for(size_t i=0; i<joints.size();i++)
    if(joints[i].jointPower.get())
      p++;
  //cout << "powerchange " <<p<< endl;
  if(p==0) {//turn check off
    //cout << "power off" << endl;
    gBox.c_power.set_active(false);
    gBox.c_power.set_inconsistent(false);
  } 
  else if(p<(int)joints.size()) {//set halfway 
    //cout << "power half" << endl;
    gBox.c_power.set_inconsistent(true);
  }
  else {
    //cout << "power on" << endl;
    gBox.c_power.set_active(true);
    gBox.c_power.set_inconsistent(false);
  }
}

void GyroCalibWindow::saveSelectedCalib(std::string filename) {
  if(filename.compare(filename.size()-4,4,".xml"))
    filename += ".xml";

  std::stringstream ss;
  ss << gBox.entry[radio].get_text();
  helium::Array<double,5> d;
  ss >> d;
  gyro.calib.setCalib(d.data); //sets selected calib as current
  gyroCalib.saveGyroCalib(filename); //saves to file
  cout << "saved to " << filename << " : " << d<< endl;
  update_label();
}

void GyroCalibWindow::onOpen() {
  file_chooser = new Gtk::FileChooserDialog(*this, "FileChoose", Gtk::FILE_CHOOSER_ACTION_OPEN);
  file_chooser->add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
  file_chooser->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  //Add filters, so that only certain file types can be selected:
  Gtk::FileFilter filter_xml;
  filter_xml.set_name("XML files");
  filter_xml.add_pattern("*.xml");
  file_chooser->add_filter(filter_xml);

  std::string f = calibFile.substr(0,calibFile.find_last_of("/\\"));
  file_chooser->set_current_folder(f);
  file_chooser->set_current_name(calibFile.substr(calibFile.find_last_of("/\\")+1,calibFile.length()));

  if(file_chooser->run()==Gtk::RESPONSE_OK) {
    helium::Array<double> c;
    try{
      gyroCalib.openGyroCalib(file_chooser->get_filename(),c);
      setCurrentLabel(c.data);

      /*
      //deletes so that it doesn't mix with current data
      if (dataarray[radio]!=NULL){
	delete[] dataarray[radio];
	dataarray[radio]=NULL;
	dataarraysize[radio]=0;
      }
      update_check_status();
      update_calc_status();
      */
      if(dataarraysize[radio]) {
	gBox.label[radio].set_label("* "+gBox.label[radio].get_label());//star indicates opened file
      }
      gBox.label[SETS].set_label("Opened a calib file");

    }catch(helium::exc::Exception& e){
      std::cerr << "error: cannot open gyroCalib file" << std::endl;
    }

  }
  delete file_chooser;
}

void GyroCalibWindow::onSave() {
  saveSelectedCalib(calibFile);
}

void GyroCalibWindow::onSaveAs() {
  file_chooser = new Gtk::FileChooserDialog(*this, "FileSaveAs", Gtk::FILE_CHOOSER_ACTION_SAVE);
  file_chooser->add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
  file_chooser->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  //Add filters, so that only certain file types can be selected:
  Gtk::FileFilter filter_xml;
  filter_xml.set_name("XML files");
  filter_xml.add_pattern("*.xml");
  file_chooser->add_filter(filter_xml);

  std::string f = calibFile.substr(0,calibFile.find_last_of("/\\"));
  file_chooser->set_current_folder(f);
  file_chooser->set_current_name(calibFile.substr(calibFile.find_last_of("/\\")+1,calibFile.length()));

  file_chooser->set_do_overwrite_confirmation(true);
  int result = file_chooser->run();
  if(result==Gtk::RESPONSE_OK) {
    std::string s = file_chooser->get_filename();
    saveSelectedCalib(s);
  }

  delete file_chooser;
}

void GyroCalibWindow::onQuit() {
  hide();
}

void GyroCalibWindow::onShowWindow() {
  visible = true;
  gyroDataConnect(true);
}

void GyroCalibWindow::onHideWindow() {
  visible = false;
  gyroDataConnect(false);
}

void GyroCalibWindow::gyroDataConnect(bool b) {
  if(b) {
    if(gyroConnected || conn.get().state != helium::Connection::CONNECTED) return;
    gyro.data.connect(gyroCalib.callOnGyroUpdate);
    gyroConnected = true;
  }
  else {
    if(!gyroConnected) return;
    gyro.data.disconnect(gyroCalib.callOnGyroUpdate);
    gyroConnected = false;
  }
}

void GyroCalibWindow::onMonitorModeChange() {
  if(monitorModeClicked) {
    monitorModeClicked = false;
    return;
  }
  int j = 0;
  for(size_t i=0;i<gyro.monitorMode.get().size();i++)
    if(gyro.monitorMode.get().data[i] != helium::FAULTYHWMON)
      j++;

  if(j==0) {//turn check off
    gBox.c_mmode.set_active(false);
    gBox.c_mmode.set_inconsistent(false);
  } 
  else if(j<(int)gyro.monitorMode.get().size()) {//set halfway 
    gBox.c_mmode.set_inconsistent(true);
  }
  else {
    gBox.c_mmode.set_active(true);
    gBox.c_mmode.set_inconsistent(false);
  }
}
void GyroCalibWindow::onMonitorModeClick() {
  monitorModeClicked = true;

  if(gBox.c_mmode.get_active())
    setMonitorMode(true);
  else
    setMonitorMode(false);

}

void GyroCalibWindow::setMonitorMode(bool b) {
  helium::Array<std::pair<int,helium::MonitorMode> > update(gyro.monitorMode.get().size());

  for(size_t i=0;i<gyro.monitorMode.get().size();i++)
    update[i] = std::make_pair(i,b?helium::NOHWMON:helium::FAULTYHWMON);

  gyro.monitorMode.set(update);
}

void GyroCalibWindow::onStateChange(const std::pair<int,helium::HwState>& hwpair) {
  //update contains not working pair, directly quit
  if(hwpair.second != helium::WORKINGHWSTATE) {
    gBox.label[SETS].set_label("Gyro["+helium::toString(hwpair.first)+"] is not working");
    return;
  }

  //update contains working pair, check all gyro
  for(size_t i=0;i<gyro.state.get().size();i++)
    if(gyro.state.get()[i] != helium::WORKINGHWSTATE) {
      gBox.m_refActionGroup->get_action("Acquire")->set_sensitive(false);
      gBox.label[SETS].set_label("Gyro["+helium::toString(i)+"] is not working");
      return;
    }

  //check succeeded, gyro working
  gBox.m_refActionGroup->get_action("Acquire")->set_sensitive(true);
  gBox.label[SETS].set_label("Gyro is now working");
}

void GyroCalibWindow::onConnection(const helium::Connection &c) {
  switch(c.state){
  case helium::Connection::DISCONNECTED:
    //if(acquire.get()) {
      //if(gBox.m_refActionGroup->get_action("Acquire")->get_label().compare("Stop") == 0) {
      //onAcquire(); //force stop acquire if connection drops
      //}
    acquire.assure(false);
    gBox.m_refActionGroup->get_action("Acquire")->set_sensitive(false);
    gyroDataConnect(false);
    gBox.c_power.set_sensitive(false);
    gBox.c_mmode.set_sensitive(false);
    break;
  case helium::Connection::CONNECTED:
    //cout << "gyro connected " << get_visible()  << " " << gyroConnected<< endl;
    gBox.m_refActionGroup->get_action("Acquire")->set_sensitive(true);
    if(visible) gyroDataConnect(true);
    gBox.c_power.set_sensitive(true);
    gBox.c_mmode.set_sensitive(true);
    break;
  default:
    break;
  }
}
