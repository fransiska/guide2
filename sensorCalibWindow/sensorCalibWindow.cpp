/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "sensorCalibWindow.h"
#include <helium/util/ioUtil.h>
#include <helium/xml/xmlLoader.h>

#include <iostream>

using namespace std;

struct SensorCalibWindow::Callers{
  helium::StaticConnReference<int,SensorCalibWindow,void,&SensorCalibWindow::update_info_label> callUpdateSelectedSensor;
  helium::StaticConnReference<helium::Connection,SensorCalibWindow,const helium::Connection&,&SensorCalibWindow::onConnection> callOnConnection;
  helium::StaticConnReference<const std::pair<int,helium::HwState>&,SensorCalibDrawingWidget,void,&SensorCalibDrawingWidget::on_refresh> callRefreshState;
  helium::StaticConnReference<const std::pair<int,helium::MonitorMode>&,SensorCalibWindow,const std::pair<int,helium::MonitorMode>&,&SensorCalibWindow::onMonitorModeUpdate> callRefreshMonitor;
   helium::Array<helium::StaticConnReference<int,SensorCalibWindow,void,&SensorCalibWindow::update_sensor_values>,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<
		  helium::ParamAllocator<
		    helium::StaticConnReference<int,SensorCalibWindow,void,&SensorCalibWindow::update_sensor_values>, SensorCalibWindow*
		    > 
		  > 
		>callRefreshValue;

  Callers(SensorCalibWindow* scw,size_t n):
  callUpdateSelectedSensor(scw),
  callOnConnection(scw),
  callRefreshState(&scw->sensorCalibDrawingWidget),
  callRefreshMonitor(scw),
  callRefreshValue(scw,n){}

};


SensorCalibWindow::SensorCalibWindow(cfg::guide::Sensors &psensors,cfg::guide::SensorGui &psensorgui,cfg::guide::Colors &pcolors,helium::GtkExceptionHandling *peh, helium::GtkProxy<helium::Connection> &pconn, const std::string& pfilename, SensorCalibDrawingWidget &pscdw):
  sensors(psensors),
  sensorGui(psensorgui),
  eh(peh), 
  conn(pconn),
  sensorFile(pfilename),
  sensorsConnected(false),
  visible(false),
  currentMax(std::numeric_limits<int>::min()),
  currentMin(std::numeric_limits<int>::max()),
  sensorCalibDrawingWidget(pscdw),
  //sensorCalibDrawingWidget(sensors,sensorGui,pcolors),//,active_sensor),
  label_sensor_no("sensor no."),
  label_sensor_value("value"),
  label_sensor_status("please select a sensor"),
  label_sensor_board("board no."),
  label_sensor_channel("chan. no."),
  label_sensor_min("cur. min"),
  label_sensor_max("cur. max"),
  button_max("max"),
  button_min("min"),
  button_quit(Gtk::Stock::QUIT),
  button_save(Gtk::Stock::SAVE),
  button_reset_calib(Gtk::Stock::CLEAR),
  button_ignore("ignore error"),
  hbox_outer(false,10),
  frame_button("Calibration"),
  //label, align x, align y, x/y ratio, ignore child aspect
  aspect_frame("Sensor Diagram", Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, ((double)sensorGui.bg.size.w/(double)sensorGui.bg.size.h), true),
  updating(false),
  //active_sensor(-1),
  sensorCalibMenu(),
  callers(new Callers(this,sensors.size()))
  
  //TODEL callOnRawValueUpdate(this,helium::FuncPlaceHolder<SensorCalibWindow,void,&SensorCalibWindow::update_sensor_values>()),

{
  //set window property
  set_title("Sensor Calibration");
  set_border_width(5);

  //put drawing area in event box for mouse events
  event_box.add(sensorCalibDrawingWidget);
  aspect_frame.add(event_box);

  //top widget add //sensor no. and value
  vbox_sensor_no.pack_start(label_sensor_no);
  vbox_sensor_no.pack_start(entry_sensor);
  vbox_sensor_value.pack_start(label_sensor_value);
  vbox_sensor_value.pack_start(entry_sensor_value);


  //board and channel
  //top widget add //sensor no. and value
  vbox_sensor_board.pack_start(label_sensor_board);
  vbox_sensor_board.pack_start(entry_sensor_board);
  vbox_sensor_channel.pack_start(label_sensor_channel);
  vbox_sensor_channel.pack_start(entry_sensor_channel);

  hbox_bottom.pack_start(vbox_sensor_board,Gtk::PACK_EXPAND_WIDGET, 5);
  hbox_bottom.pack_start(vbox_sensor_channel,Gtk::PACK_EXPAND_WIDGET, 5);


  //top widget property
  //DEBUG entry_sensor.set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
  entry_sensor.set_range(0,sensors.size()-1);
  entry_sensor.set_increments(1,10);
  entry_sensor_value.set_alignment(0.5);
  entry_sensor_value.set_max_length(5);
  entry_sensor_value.set_width_chars(5);


  int max_board = 0; 
 for(size_t i=0;i<sensors.size();i++) {
    if(max_board<sensors[i].getBoard())
      max_board = sensors[i].getBoard();
  }
  entry_sensor_board.set_range(0,max_board);
  int max_channel = 0;
  for(size_t i=0;i<sensors.size();i++) {
    if(max_channel<sensors[i].getChannel())
      max_channel = sensors[i].getChannel();
  }
  entry_sensor_channel.set_range(0,max_channel);
  entry_sensor_board.set_increments(1,5);
  entry_sensor_channel.set_increments(1,5);

  entry_sensor_min.set_range(0,3000);
  entry_sensor_min.set_increments(1,5);
  entry_sensor_max.set_range(0,3000);
  entry_sensor_max.set_increments(1,5);



  //expand the widget inside but shrink it
  hbox_top.pack_start(vbox_sensor_no,Gtk::PACK_EXPAND_WIDGET, 5);    
  hbox_top.pack_start(vbox_sensor_value,Gtk::PACK_EXPAND_WIDGET, 5);
  vbox_button.pack_start(hbox_top,Gtk::PACK_SHRINK, 5);

  //min max sensor info
  vbox_button.pack_start(separator_2,Gtk::PACK_SHRINK, 5);

  vbox_min.pack_start(label_sensor_min);
  vbox_min.pack_start(entry_sensor_min);
  vbox_min.pack_start(label_sensor_cur_min);
  vbox_min.pack_start(button_min, Gtk::PACK_SHRINK,5);
  vbox_max.pack_start(label_sensor_max);
  vbox_max.pack_start(entry_sensor_max);
  vbox_max.pack_start(label_sensor_cur_max);
  vbox_max.pack_start(button_max, Gtk::PACK_SHRINK,5);

  hbox_minmax.pack_start(vbox_min,Gtk::PACK_EXPAND_WIDGET, 5);    
  hbox_minmax.pack_start(vbox_max,Gtk::PACK_EXPAND_WIDGET, 5);    

  //buttons: 2 buttons spreaded using buttonbox
  vbox_button.pack_start(hbox_minmax, Gtk::PACK_SHRINK,5);

  //vbox_button.pack_start(hbbox_bottom, Gtk::PACK_SHRINK,5);
  //hbbox_bottom.set_layout(Gtk::BUTTONBOX_SPREAD);
  //hbbox_bottom.set_child_min_width(10);

  button_min.set_sensitive(false);
  button_max.set_sensitive(false);


  //sensor info board channel
  //vbox_button.pack_start(label_sensor_status, Gtk::PACK_SHRINK,5);
  vbox_button.pack_start(separator_1,Gtk::PACK_SHRINK,5);
  vbox_button.pack_start(hbox_bottom,Gtk::PACK_SHRINK, 5);
  label_sensor_info.set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
  vbox_button.pack_start(label_sensor_info,Gtk::PACK_SHRINK,5);

  vbox_button.pack_start(separator_3,Gtk::PACK_SHRINK, 5);
  vbox_button.pack_start(button_ignore,Gtk::PACK_SHRINK,5);
  vbox_button.pack_start(label_sensor_ignored,Gtk::PACK_SHRINK,5);
  vbox_button.pack_start(button_reset_calib,Gtk::PACK_SHRINK,5);

  //buttons: hbbox_menu: 2 buttons SAVE and QUIT
  button_save.set_use_stock(true);
  button_quit.set_use_stock(true);
  hbbox_menu.set_layout(Gtk::BUTTONBOX_END);
  hbbox_menu.pack_start(button_save, Gtk::PACK_SHRINK,5);
  hbbox_menu.pack_start(button_quit, Gtk::PACK_SHRINK,5);
  hbbox_menu.set_layout(Gtk::BUTTONBOX_SPREAD);
  hbbox_menu.set_child_min_width(10);

  //boxes layout
  frame_button.add(vbox_button);
  vbox_right.pack_start(frame_button, Gtk::PACK_EXPAND_WIDGET);
  vbox_right.pack_start(hbbox_menu, Gtk::PACK_SHRINK,5);
  frame_button.set_size_request(160,190);
  hbox_outer.pack_start( aspect_frame, Gtk::PACK_EXPAND_WIDGET);
  hbox_outer.pack_start( vbox_right, Gtk::PACK_SHRINK);

  vbox_outer.pack_end(hbox_outer);
  add(vbox_outer);

  //signal
  button_min.signal_clicked().connect(eh->wrap(*this,&SensorCalibWindow::on_button_min));
  button_max.signal_clicked().connect(eh->wrap(*this,&SensorCalibWindow::on_button_max));
  button_quit.signal_clicked().connect(eh->wrap(*this,&SensorCalibWindow::on_button_quit));
  button_save.signal_clicked().connect(eh->wrap(*this,&SensorCalibWindow::on_button_save));
  button_reset_calib.signal_clicked().connect(eh->wrap(*this,&SensorCalibWindow::on_button_reset_calib));
  //button_ignore.signal_clicked().connect(eh->wrap(*this,&SensorCalibWindow::on_button_ignore));
  button_ignore_handler = button_ignore.signal_toggled().connect(eh->wrap(*this,&SensorCalibWindow::on_button_ignore));

  //to handle the change from spinbutton
  entry_sensor.signal_value_changed().connect(eh->wrap(*this,&SensorCalibWindow::update_active_sensor,SENSORNO));
  entry_sensor_board.signal_value_changed().connect(eh->wrap(*this,&SensorCalibWindow::update_active_sensor,SENSORBOARD));
  entry_sensor_channel.signal_value_changed().connect(eh->wrap(*this,&SensorCalibWindow::update_active_sensor,SENSORCHANNEL));

  sensorCalibDrawingWidget.active_sensor.connect(callers->callUpdateSelectedSensor);

  entry_sensor_min.signal_value_changed().connect(eh->wrap(*this,&SensorCalibWindow::update_sensor_min));
  entry_sensor_max.signal_value_changed().connect(eh->wrap(*this,&SensorCalibWindow::update_sensor_max));

  //signal: event box
  event_box.set_events(Gdk::BUTTON_PRESS_MASK);
  event_box.signal_button_press_event().connect(eh->wrap(*this,&SensorCalibWindow::on_eventbox_button_press));

  show_all_children();

  signal_show().connect(eh->wrap(*this,&SensorCalibWindow::onShowWindow));
  signal_hide().connect(eh->wrap(*this,&SensorCalibWindow::onHideWindow));

  conn.connect(callers->callOnConnection);


  Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
  /* force using icons on stock buttons: */

  //settings->property_gtk_button_images() = true; 
  
  update_info_label(); //correctly set label

  set_default_size(772,480);

}

void SensorCalibWindow::update_active_sensor(SensorEntry s) {
  if(updating) return;

  int cur = -1;
  switch(s) {
  case SENSORNO:
    cur = entry_sensor.get_value_as_int();
    break;
  case SENSORBOARD:
  case SENSORCHANNEL:
    cur = getSensorNumber(entry_sensor_board.get_value_as_int(),entry_sensor_channel.get_value_as_int());
    break;
  default: break;
  };
  sensorCalibDrawingWidget.active_sensor.assure(cur);

}

int SensorCalibWindow::getSensorNumber(int board, int channel) {
  for(size_t i=0;i<sensors.size();i++)
    if(sensors[i].getBoard() == board && sensors[i].getChannel() == channel)
      return i;
  return -1;
}



void SensorCalibWindow::onShowWindow() {
  visible = true;
  connect(visible);
}

void SensorCalibWindow::onHideWindow() {
  visible = false;
  connect(visible);
}



void SensorCalibWindow::connect(bool b) {
  if(b) {
    if(sensorsConnected || conn.get().state != helium::Connection::CONNECTED) return;


    //sensors.startMassiveOperation();
    for(size_t i=0;i<sensors.size();i++) {
      sensors[i].rawValue.connect(callers->callRefreshValue[i]);
    }
    sensors.state.connect(callers->callRefreshState);
    sensors.monitorMode.connect(callers->callRefreshMonitor);

        
    //sensors.endMassiveOperation();
    
    sensorCalibDrawingWidget.on_refresh();
    sensorsConnected = true;
  }
  else {
    if(!sensorsConnected) return;
    //cout << "hiding " << endl;
    //sensors.startMassiveOperation();
    //std::cout<<"massive"<<std::endl;
    for(size_t i=0;i<sensors.size();i++) {
      //cout << i << " ";
      sensors[i].rawValue.disconnect(callers->callRefreshValue[i]);
    }
    sensors.state.disconnect(callers->callRefreshState);
    sensors.monitorMode.disconnect(callers->callRefreshMonitor);

    
    //sensors.endMassiveOperation();
    sensorCalibDrawingWidget.on_refresh();
    sensorsConnected = false;
    //std::cout<<"disconnecting, spooler up"<<std::endl;
  }
}






bool SensorCalibWindow::getVisible() {
  //return get_visible(); //only available on gtkmm 2.18 above
  return visible; 
}

void SensorCalibWindow::onConnection(const helium::Connection &c) {
  switch(c.state){
  case helium::Connection::DISCONNECTED:
    connect(false);
    break;
  case helium::Connection::CONNECTED:
    //cout << "sensor connected " << get_visible()  << " " << sensorsConnected<< endl;
    if(getVisible()) connect(true);
    break;
  default:
    break;
  }
}

void SensorCalibWindow::on_button_min() {
  int t_sensorval;

  //output from get_text() is Glib::ustring
  //to convert to string, need the raw()
  //to convert string to int, need stringstream
  //and output it to an int
  //stringstream ss(entry_sensor.get_text().raw());
  //ss >> t_sensorno;

  //need to clear stringstream before reuse
  //ss.clear(stringstream::goodbit);

  //convert string to stringstream
  std::stringstream ss;
  ss.str(entry_sensor_value.get_text().raw());
  ss >> t_sensorval;

  //sensors[t_sensorno].setMin(t_sensorval);
  entry_sensor_min.set_value(t_sensorval);

  sensorCalibDrawingWidget.on_refresh(); //refresh circles
  update_status_label();
}

void SensorCalibWindow::on_button_max() {
  int t_sensorval;
  std::stringstream ss;
  ss.str(entry_sensor_value.get_text().raw());
  ss >> t_sensorval;

  //sensors[t_sensorno].setMin(t_sensorval);
  entry_sensor_max.set_value(t_sensorval);

  sensorCalibDrawingWidget.on_refresh(); //refresh circles
  update_status_label();
}


void SensorCalibWindow::update_sensor_min() {
  if(sensorCalibDrawingWidget.active_sensor.get()<0) return;
  if(updating) return;
  sensors[sensorCalibDrawingWidget.active_sensor.get()].setMin(entry_sensor_min.get_value());
}

void SensorCalibWindow::update_sensor_max() {
  if(sensorCalibDrawingWidget.active_sensor.get()<0) return;
  if(updating) return;
  sensors[sensorCalibDrawingWidget.active_sensor.get()].setMax(entry_sensor_max.get_value());
}

bool SensorCalibWindow::on_eventbox_button_press (GdkEventButton* event) {
  button_ignore_handler.block();
  sensorCalibDrawingWidget.active_sensor.assure(sensorCalibDrawingWidget.update_active_sensor_no(event->x,event->y,sensorCalibDrawingWidget.get_pr()));
  //cout << " " << sensorCalibDrawingWidget.get_active_sensor_no() << endl;
  //for DEBUG only TODEL
  //helium::Array<int> a;
  //a.resize(sensors.size());
  //static int d = 1;
  //for(size_t i = 0;i<sensors.size();i++) {
  //  a[i] = i*d;
  //}
  //d++;

  //sensors.rawValue = a; DEBUGSENSOR
  //for(size_t i=0;i<sensors.size();i++) 
  //cout << "sensor " << i << " " << sensors[i].pos << " v=" << sensors.rawValue.get().vals[i]  << endl;
  ///end
  //update_info_label();

  button_ignore_handler.unblock();
  return true;
}

void SensorCalibWindow::update_info_label() {
  if(updating) return;

  if(sensorCalibDrawingWidget.get_active_sensor_no()<0 || sensorCalibDrawingWidget.get_active_sensor_no()>(int)sensors.size()-1) {
    entry_sensor_min.set_sensitive(false);
    entry_sensor_max.set_sensitive(false);
    entry_sensor_min.set_text("");
    entry_sensor_max.set_text("");
    button_min.set_sensitive(false);
    button_max.set_sensitive(false);
    entry_sensor.set_text("");
    label_sensor_info.set_text("");
    entry_sensor_value.set_text("");
    label_sensor_status.set_text("please select a sensor\n");
    button_ignore.set_sensitive(false);
  }
  else {
    button_min.set_sensitive(true);
    button_max.set_sensitive(true);
    entry_sensor_min.set_sensitive(true);
    entry_sensor_max.set_sensitive(true);

    currentMax = std::numeric_limits<int>::min();
    currentMin = std::numeric_limits<int>::max();
    label_sensor_cur_max.set_text("");
    label_sensor_cur_min.set_text("");


    updating = true;
    entry_sensor.set_value(sensorCalibDrawingWidget.get_active_sensor_no());
    entry_sensor_board.set_value(sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getBoard());
    entry_sensor_channel.set_value(sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getChannel());
    update_status_label();
    updating = false;




    std::stringstream ss;
    ss << "raw " << sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getRawSensorID();
    //ss << " \nboard " << sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getBoard();
    //ss << " channel " << sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getChannel();
    ss<<" ("<<sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getName()<<")";
    label_sensor_info.set_text(ss.str());
    
    //cout << "sensor monitor is " << sensors.monitorMode.isConnected() << endl;
    if(sensors.monitorMode.isConnected()){
      button_ignore.set_sensitive(true);
      
      if(sensors.monitorMode.get()[sensorCalibDrawingWidget.get_active_sensor_no()]==helium::FAULTYHWMON) {       //if(true) {
	button_ignore.set_active(false);
	button_ignore.set_label("ignore error");
      }
      else {
	button_ignore.set_active(true);
	button_ignore.set_label("ignoring error");
      }
    }else{
      button_ignore.set_sensitive(false);
      button_ignore.set_active(false);
      button_ignore.set_label("ignore error");
    }
    
  }
  sensorCalibDrawingWidget.on_refresh(); //refresh circles
}

void SensorCalibWindow::update_status_label() {


  entry_sensor_min.set_value(sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getMin());
  entry_sensor_max.set_value(sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getMax());


  /*
  string status,min,max;
  helium::convert(min,sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getMin());
  helium::convert(max,sensors[sensorCalibDrawingWidget.get_active_sensor_no()].getMax());
  status  = "current min: " + min + "\ncurrent max: " + max;   
  label_sensor_status.set_text(status);
  */

  if(sensors.monitorMode.isConnected()) {
    std::stringstream ss;
    int j = 0;
    for(size_t i=0;i<sensors.size();i++) {
      if(sensors.monitorMode.get()[i] != helium::FAULTYHWMON) {
	j++;
	ss << i << " ";
	if(j%8 == 0)
	  ss << "\n";
      }
    }
    
    string str = j>0?"ignoring sensors: \n":"";
    

    label_sensor_ignored.set_text(str + ss.str());
  }

  //show current value of sensor
  update_sensor_values();
}

void SensorCalibWindow::on_button_quit() {
  hide();
}

void SensorCalibWindow::on_button_save() {
  Gtk::FileChooserDialog dialog("Please input a sensor config file", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);
  dialog.set_current_folder(sensorFile.substr(0,sensorFile.find_last_of("/\\")));


  //Add filters, so that only certain file types can be selected:
  Gtk::FileFilter filter_xml;
  filter_xml.set_name("XML files");
  filter_xml.add_pattern("*.xml");
  dialog.add_filter(filter_xml);
  
  dialog.set_do_overwrite_confirmation(true);
  dialog.set_current_name(sensorFile.substr(sensorFile.find_last_of("/\\")+1,sensorFile.length()));

  if(dialog.run()==Gtk::RESPONSE_OK) {
    helium::Memory outmem;
    helium::XmlLoader out;      
    helium::cexport<helium::SensorsExport>(outmem,sensors);
    out.save(dialog.get_filename().c_str(),outmem);  
  }
}

void SensorCalibWindow::on_button_ignore() {
  //cout << "sensorcalib setting monmode to ";
  helium::Array<std::pair<int,helium::MonitorMode> > update(1);
  if (sensors.monitorMode.isConnected()){

    if(sensors.monitorMode.get()[sensorCalibDrawingWidget.active_sensor] == helium::FAULTYHWMON) {
      update[0] = std::make_pair(sensorCalibDrawingWidget.active_sensor.get(),helium::NOHWMON);
    }
    else {
      update[0] = std::make_pair(sensorCalibDrawingWidget.active_sensor.get(),helium::FAULTYHWMON);
    }
    sensors.monitorMode.set(update);
    //cout << sensors.monitorMode.get().data[active_sensor] << endl;
  }else{
    //cout<<"disabled!"<<endl;
  }
  update_status_label();
}

void SensorCalibWindow::update_sensor_values(){
  string curr;

  if(sensorCalibDrawingWidget.get_active_sensor_no()>=0) { 
    int currRawVal = sensors[sensorCalibDrawingWidget.get_active_sensor_no()].rawValue.get();
    //sensors.rawValue.get().vals[sensorCalibDrawingWidget.get_active_sensor_no()];

    //helium::Array<int> currRawVal2 = sensors.rawValue.get().vals;
    //cout << "currrawval " << currRawVal2 << endl;

    helium::convert(curr,currRawVal);
    entry_sensor_value.set_text(currRawVal==-1?"":curr);    
    if(currRawVal != -1 && currRawVal < currentMin) {
      currentMin = currRawVal;
      label_sensor_cur_min.set_text(curr);
    }
    if(currRawVal != -1 && currRawVal > currentMax) {
      currentMax = currRawVal;
      label_sensor_cur_max.set_text(curr);
    }
  }
  
  sensorCalibDrawingWidget.on_refresh();
}

void SensorCalibWindow::on_button_reset_calib() {
  //cout << "sensor calib window ";
  //cout << sensors.monitorMode.get();
  //cout << endl;

  Gtk::MessageDialog confirmReset("Clear calibration marks?",true,Gtk::MESSAGE_QUESTION ,Gtk::BUTTONS_YES_NO);
  if(confirmReset.run()) {
    for(size_t i=0;i<sensors.size();i++) {
      sensors[i].limitCalibrated.first = false;
      sensors[i].limitCalibrated.second = false;
    }
  }
  sensorCalibDrawingWidget.on_refresh();
}

void SensorCalibWindow::onMonitorModeUpdate(const std::pair<int,helium::MonitorMode>& hm) {
  sensorCalibDrawingWidget.on_refresh();
  if(sensorCalibDrawingWidget.get_active_sensor_no() == hm.first)
    update_info_label();
}
