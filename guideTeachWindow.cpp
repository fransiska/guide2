/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.06.17
 * \version 0.1
 * Release_flags g
 */


#include "guideTeachWindow.h"

using std::cout;
using std::cerr;
using std::endl;

TeachTreeColumn::TeachTreeColumn() {
  add(id);
  add(input);
  add(output);
  add(name);
}

TeachTreeView::Connections::Connections(TeachTreeView* owner):
  callOnTeachAcquire(owner),
  callOnTeachAcquired(owner),
  callOnTeachDelete(owner),
  callOnTeachClear(owner),
  callOnTeachUndo(owner),
  callOnTeachRedo(owner)
{}

TeachTreeView::TeachTreeView(TbT &ptbt, helium::GtkExceptionHandling *peh, RobotModels *prm):
  tbt(ptbt),
  eh(peh),
  rm(prm),  
  snapWidth(0),snapHeight(0),
  connections(new Connections(this))
{
  //set columns
  model = Gtk::ListStore::create(column);
  set_model(model);
  append_column("ID", column.id); 
  if(rm) {
    append_column("Input", column.input);
    append_column("Output", column.output); 
  }
  nameColumnRenderer = Gtk::manage(new Gtk::CellRendererText());
  int nameColumnId = append_column("Name", *nameColumnRenderer);
  std::cout << "id " << nameColumnId << std::endl;
  Gtk::TreeViewColumn* nameColumn = get_column(nameColumnId-1);
  if(nameColumn) {
    nameColumn->add_attribute(nameColumnRenderer->property_markup(),column.name);
  }

  if(rm) {
    snapWidth = rm->mainCanvasArea.getWidth();
    snapHeight = rm->mainCanvasArea.getHeight();
  }

  //teachlist signals back from data->gui
  tbt.getSignals().acquire.connect(connections->callOnTeachAcquire);
  tbt.getSignals().acquired.connect(connections->callOnTeachAcquired);
  tbt.getSignals().del.connect(connections->callOnTeachDelete);
  tbt.getSignals().clear.connect(connections->callOnTeachClear);
  tbt.getSignals().undo.connect(connections->callOnTeachUndo);
  tbt.getSignals().redo.connect(connections->callOnTeachRedo);

  //popup menu
  {
    Gtk::Menu::MenuList& menulist = menu.items();
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Apply Input to Frame",eh->wrap(*this,&TeachTreeView::onApplytoFrame,false)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Apply Output to Frame",eh->wrap(*this,&TeachTreeView::onApplytoFrame,true)));
    //menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Copy Input to Clipboard",eh->wrap(*this,&TeachTreeView::onCopytoClipboard,false)));
    //menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Copy Output to Clipboard",eh->wrap(*this,&TeachTreeView::onCopytoClipboard,true)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Delete",eh->wrap(*this,&TeachTreeView::onDelete)));   
  }
  menu.accelerate(*this);
}

void TeachTreeView::onApplytoFrame(bool output) {
  Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
  if(refSelection)
  {
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    if(iter)
    {
      int id = (*iter)[column.id];
      tbt.applytoFrame(id,output);
    }
  }
}
void TeachTreeView::onDelete() {
  Gtk::TreeModel::iterator iter = get_selection()->get_selected();
  if(iter) {
    int id = (*iter)[column.id];
    tbt.deleteTeachList(id);
  }
}

void TeachTreeView::onTeachUndo() {
  reloadTree();
}
void TeachTreeView::onTeachRedo() {
  reloadTree();
}

bool TeachTreeView::on_button_press_event(GdkEventButton* event) {
  bool return_value = false;

  //Call base class, to allow normal handling,
  //such as allowing the row to be selected by the right-click:
  return_value = TreeView::on_button_press_event(event);

  //Then do our custom stuff:
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
  {
    //only enable menu if there is a selected item
    Gtk::TreeModel::iterator iter = get_selection()->get_selected();
    bool selected = false;
    if(iter) {
      selected = true;
    }
    for(size_t i=0;i<menu.items().size();i++) 
      menu.items()[i].set_sensitive(selected);
    menu.popup(event->button, event->time);      
  }

  return return_value;
}

void TeachTreeView::onTeachAcquire(const TeachPostureInput& p) {
  //append a row
  Gtk::TreeModel::Row row = *(model->append());
  row[column.id] = tbt.getTeachList().size()-1;
  std::stringstream ss;
  ss << p.touchedSensor << "\n" << p.posture;
  row[column.name] = ss.str();
  if(rm) {
     double black[3] = {0,0,0}; 
     postureSnapi.resize(postureSnapi.size()+1);
     rm->getPostureSnap(postureSnapi.back(),p.posture,snapWidth,snapHeight,black);
     row[column.input] = Gdk::Pixbuf::create_from_data(postureSnapi.back().data,Gdk::COLORSPACE_RGB,false,8,snapWidth,snapHeight,snapWidth*3);    
  }
}
void TeachTreeView::onTeachAcquired(const TeachPosture &p) {
  //put on name
  Gtk::TreeModel::Row row = *(model->children().rbegin());
  std::stringstream ss;
  ss << row.get_value(column.name) << "\n" << p.posture;
  row[column.name] = ss.str();
  if(rm) {
     double black[3] = {0,0,0}; 
     postureSnapo.resize(postureSnapo.size()+1);
     rm->getPostureSnap(postureSnapo.back(),p.posture,snapWidth,snapHeight,black);
     row[column.output] = Gdk::Pixbuf::create_from_data(postureSnapo.back().data,Gdk::COLORSPACE_RGB,false,8,snapWidth,snapHeight,snapWidth*3);    
  }
}

void TeachTreeView::onTeachDelete(int id){ 
  //delete a row
  Gtk::TreeModel::Children::iterator iter = model->children()[id];
  model->erase(iter);

  std::list<RobotModels::PostureSnap>::iterator iteri,itero;
  iteri = postureSnapi.begin();
  advance(iteri,id);
  postureSnapi.erase(iteri);
 
  itero = postureSnapo.begin();
  advance(itero,id);
  postureSnapo.erase(itero);

  renumber();
}

void TeachTreeView::onTeachClear() {
  //delete rows
  Gtk::TreeModel::Children::iterator iter = model->children().begin();
  while(iter != model->children().end()) {
    iter = model->erase(iter);
    postureSnapi.clear();
    postureSnapo.clear();
  }
}

void TeachTreeView::renumber() {
  Gtk::TreeModel::Children::iterator iter;
  int i;
  for(iter = model->children().begin(),i=0;iter!=model->children().end();iter++,i++){
    (*iter)[column.id] = i;
  }
}

void TeachTreeView::reloadTree() {
  onTeachClear();
  for(size_t i=0;i<tbt.getTeachList().size();i++) {
    onTeachAcquire(tbt.getTeachList()[i].input);
    onTeachAcquired(tbt.getTeachList()[i].output);
  }
  renumber();
}

GuideTeachStatusLabel::GuideTeachStatusLabel(helium::ValueHub<TbTType::State> &state):
  callOnStateChange(this,helium::FuncPlaceHolder<GuideTeachStatusLabel,TbTType::State,&GuideTeachStatusLabel::onStateChange>())
{
  state.connect(callOnStateChange); 
  onStateChange(state.get());
}

void GuideTeachStatusLabel::onStateChange(TbTType::State s) {
  switch(s) {
  case TbTType::NOTOUCH:
    set_markup("<span color=\"green\">Robot is not being touched</span>");
    break;
  case TbTType::TEACHING:
    set_markup("<span color=\"green\">Teaching sensor</span>");
    break;
  case TbTType::DISCONNECTED:
    set_markup("<span color=\"black\">Robot is not connected</span>");
    break;
  case TbTType::CONNECTED:
    set_markup("<span color=\"red\">Robot is connected</span>");
    break;
  case TbTType::TOUCHENABLE:
    set_markup("<span color=\"green\">Robot is being touched</span>");
    break;
  }
}

void GuideTeachWindow::onRowChanged(){ 
  static double prevLength = scrolledWindow.get_vadjustment()->get_upper();
  double curLength = scrolledWindow.get_vadjustment()->get_upper();
  //only if previous length is less than current length (should not scroll when deleting touch)
  if(prevLength < curLength) {
    scrolledWindow.get_vadjustment()->set_value(curLength); 
  }
  prevLength = curLength;
}

GuideTeachWindow::GuideTeachWindow(GuideMotionManagement &pgmm,RobotModels *prm):
  gmm(pgmm),
  tbt(gmm,prm),
  acquireButton(Gtk::Stock::NO, "Acquiring", Gtk::Stock::YES, "Acquire"),  
  undoButton(Gtk::Stock::UNDO,"Undo"),
  redoButton(Gtk::Stock::REDO,"Redo"),
  newButton(Gtk::Stock::NEW,"New"),
  openButton(Gtk::Stock::OPEN,"Open"),
  saveButton(Gtk::Stock::SAVE,"Save"),
  saveAsButton(Gtk::Stock::SAVE_AS,"Save As"),
  treeView(tbt,&gmm.eh,prm),
  guideTeachStatusLabel(tbt.state), 
  touchedSensorLabel(""),
  callSetAcquireOn(&acquireButton),  
  callSetAcquireOff(&acquireButton),
  connections(new Connections(this,gmm.gs.sensors.size()))
{
  //connections
  acquireButton.signal_toggled().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onAcquireButtonPress));//signal_clicked()
  tbt.state.connect(connections->callOnStateChange);
  tbt.acquireState.connect(connections->callOnAcquireStateChange);
  tbt.touchedSensor.connect(connections->callOnTouchedSensorUpdate);
  onAcquireButtonPress();
  onAcquireStateChange(tbt.acquireState.get());
  onStateChange(tbt.state.get());
  undoButton.signal_clicked().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onUndoButtonPress));
  redoButton.signal_clicked().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onRedoButtonPress));
  newButton.signal_clicked().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onNewButtonPress));
  openButton.signal_clicked().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onOpenButtonPress));
  saveButton.signal_clicked().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onSaveButtonPress));
  saveAsButton.signal_clicked().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onSaveAsButtonPress));

  scrolledWindow.get_vadjustment()->signal_changed().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onRowChanged));


  //packing
  vbox.pack_start(toolbar,Gtk::PACK_SHRINK);
  vbox.pack_start(scrolledWindow,Gtk::PACK_EXPAND_WIDGET);
  vbox.pack_start(guideTeachStatusLabel,Gtk::PACK_SHRINK);
  vbox.pack_start(touchedSensorLabel,Gtk::PACK_SHRINK);

  //scrolled window
  scrolledWindow.add(treeView);
  scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  //toolbar
  toolbar.append(acquireButton);
  toolbar.append(undoButton);
  toolbar.append(redoButton);
  toolbar.append(newButton);
  toolbar.append(openButton);
  toolbar.append(saveButton);
  toolbar.append(saveAsButton);
  
  add(vbox);

  //showing widgets
  show_all_children();
  acquireButton.on_toggled();

  
}

void GuideTeachWindow::onTouchedSensorUpdate(std::string s) {
  touchedSensorLabel.set_markup(s);
}

void GuideTeachWindow::onStateChange(TbTType::State s) {
  switch(s) {
  case TbTType::TOUCHENABLE: 
  case TbTType::TEACHING: 
    acquireButton.set_sensitive(true);
    break;
  case TbTType::DISCONNECTED:
    std::cout << "setting off" << std::endl;
    acquireButton.set_active(false);
  case TbTType::NOTOUCH: 
  case TbTType::CONNECTED:
    acquireButton.set_sensitive(false);
    break;
  }
}
void GuideTeachWindow::onUndoButtonPress() {
  tbt.undoTeachList();
}
void GuideTeachWindow::onRedoButtonPress() {
  tbt.redoTeachList();
}
void GuideTeachWindow::onNewButtonPress() {
  tbt.newTeachList();
  set_title("Teach List Window");
}
void GuideTeachWindow::onSaveButtonPress() {  
  std::string name;
  tbt.getTeachFile(name);
  if(name.size()) {
    tbt.saveTeachList(name);
  }
  else {
    onSaveAsButtonPress();
  }
}
void GuideTeachWindow::onSaveAsButtonPress(){
  std::string title = "Please input a teach file";
  Gtk::FileChooserDialog dialog(title, Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);

  setDialogFile(dialog);

  if(dialog.run()==Gtk::RESPONSE_OK) {
    std::string name = dialog.get_filename();
    tbt.saveTeachList(name);
    tbt.getTeachFile(name);
    set_title("Teach List Window " + name);
  }
}

void GuideTeachWindow::setDialogFile(Gtk::FileChooserDialog &dialog) {
  //set folder
  std::string name,folder;
  tbt.getTeachFile(name);
  if(name.length() == 0) {
    folder = g_get_current_dir();
    dialog.set_current_folder(folder);
  }
  else {
    dialog.set_filename(name);
  }

  //Add filters, so that only certain file types can be selected:
  Gtk::FileFilter filter_tea;
  filter_tea.set_name("TEA files");
  filter_tea.add_pattern("*.tea");
  dialog.add_filter(filter_tea);
}

void GuideTeachWindow::onOpenButtonPress() {
  std::string title = "Please choose a teach file";
  Gtk::FileChooserDialog dialog(title, Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  setDialogFile(dialog);

  //add option to append file  
  Gtk::CheckButton replaceButton("Replace current list");
  replaceButton.show();
  replaceButton.set_active(true);
  dialog.set_extra_widget(replaceButton);
  
  if(dialog.run()==Gtk::RESPONSE_OK) {
    std::string name = dialog.get_filename();
    std::cout << "dialog is ok" << std::endl;
    tbt.openTeachList(name,replaceButton.get_active());
    tbt.getTeachFile(name);
    set_title("Teach List Window " + name);
  }
}

void GuideTeachWindow::onAcquireButtonPress() {
  if(acquireButton.get_active()) {
    std::cout << "acquirestate active, is now " << tbt.acquireState.get() << std::endl;
    tbt.acquireState.assure(true);
  }
  else {
    std::cout << "acquirestate off, is now " << tbt.acquireState.get() << std::endl;
    tbt.acquireState.assure(false);
  }
}

void GuideTeachWindow::onAcquireStateChange(bool b) {
  acquireButton.set_active(b);
  //set sensitive buttons
  undoButton.set_sensitive(!b);
  redoButton.set_sensitive(!b);
  newButton.set_sensitive(!b);
  openButton.set_sensitive(!b);
  saveButton.set_sensitive(!b);
  saveAsButton.set_sensitive(!b);
  treeView.get_selection()->set_mode(b?Gtk::SELECTION_NONE:Gtk::SELECTION_SINGLE);
}

GuideTeachWindow::Connections::Connections(GuideTeachWindow* owner, size_t sensorsSize):
  callOnStateChange(owner),
  callOnAcquireStateChange(owner),
  callOnTouchedSensorUpdate(owner)
{}

// #include <iostream>
// #include <sstream>
// #include "yesNoDialog.h"
// #include <time.h>
// #include "inputDialog.h"
// 
// 
// using std::cout;
// using std::cerr;
// using std::endl;
// 
// inline std::ostream& operator <<(std::ostream& o,const SensorUpdate::UpdateType& u){
//   std::string s = "";
//   switch(u) {
//   case SensorUpdate::POT: s = "Potentiometer"; break;
//   case SensorUpdate::SENSOR: s = "Sensor"; break;
//   case SensorUpdate::GYRO: s = "Gyro"; break;
//   case SensorUpdate::POWER: s = "Power"; break;
//   }
//   return o << s;
// }
// 
// inline std::ostream& operator <<(std::ostream& o,const TeachListChange::Reason &e){
//   std::string s = "";
//   switch(e) {
//   case TeachListChange::ADD: s = "Add"; break;    
//   case TeachListChange::ACQUIRE: s = "Acquire"; break;
//   case TeachListChange::ACQUIRED: s = "Acquired"; break;
//   case TeachListChange::DELETETEACH: s = "Delete"; break;
//   case TeachListChange::CLEAR: s = "Clear"; break;
//   case TeachListChange::OPEN: s = "Open"; break;
//   case TeachListChange::UNDO: s = "Undo"; break;
//   case TeachListChange::REDO: s = "Redo"; break;
//   case TeachListChange::NONE: s = "None"; break;
//   case TeachListChange::USE: s = "USE"; break;
//   }
//   return o << s;
// }
// 
// inline std::ostream& operator <<(std::ostream& o,const GuideTeachList &gtl){
//   std::stringstream ss;
//   //motor in [22], sensor in [90], gyro [5],motor out [22]
//   for(size_t i=0;i<gtl.gettl().size();i++) {
//     ss << gtl.gettl()[i].input;
//     ss << gtl.gettl()[i].sensor;
//     ss << gtl.gettl()[i].gyro;
//     ss << gtl.gettl()[i].output;    
//     ss << ":";
//   }
//   return o << ss.str();
// }
// 
// inline std::ostream& operator <<(std::ostream& o,const helium::SensorValList<int>& g) {
//   return o << g.vals;
// }
// 
// 
// bool operator==(const helium::SensorValList<int>& lhs, const double& rhs) {
//   return false; /* your comparison code goes here */
// }
// 
// 
// const char* SensorSelection::strings[] = {"nothing","list id","description"};
// 
// void GuideTeachListInfo::writeLog(TeachListChange::Reason e,std::string s,bool writeSensor) {
//   logFile << helium::toString(helium::getSystemMillis()-t0) << " "<< e;
//   if(s.length())
//     logFile <<" "<< s;
//   logFile << ":";     //marks the starting of data
// 
//   if(writeSensor) {
//     //1 robot joints [22]
//     //2 sensor value [90]
//     //3 gyro [5]
// 
// 
//     
//     std::stringstream ss;
//     for(size_t i=0;i<gs.joints.size();i++) {
//       ss << gs.joints[i].displayedTarget.get() << " ";
//     }
//     for(size_t i=0;i<gs.sensors.size();i++) {
//       ss << gs.sensors[i].rawValue.get() << " ";
//     }
//     ss << gs.gyro.data.get();
//     logFile << ss.str();
// 
//   }
// 
//   logFile << endl;
// }
// 
// template <typename T>
// void GuideTeachListInfo::writeLog(TeachListChange::Reason e, T t, bool writeSensor) {
//   writeLog(e,helium::toString(t),writeSensor);
// }
// 
// template <typename T>
// void GuideTeachListInfo::writeLog(T t, std::pair<SensorUpdate::UpdateType,int> u) {
//   std::stringstream ss;
// 
//   struct OldData{
//     helium::Array<double> prevPot;
//     helium::Array<int> prevSensor;
//     helium::Array<bool> prevPower;
//     
//     OldData(int m,int s):
//       prevPot(m),
//       prevSensor(s),
//       prevPower(m)      
//     {
//       zero(prevPot);
//       zero(prevSensor);
//       zero(prevPower);
//     }
//     
//   };
// 
//   
//   static OldData old(gs.joints.size(),gs.sensors.size());
//   
//   static std::string prevGyro="";
// 
//   switch(u.first) {
//   case SensorUpdate::POT: 
//     if(t == old.prevPot[u.second]) return;
//     if(t == helium::motor::Interface::MOVINGPOS) { return; }
//     for(size_t i=0;i<gs.joints.size();i++) {
//       ss << gs.joints[i].displayedTarget.get() << " ";
//     }
//     helium::convert(old.prevPot,ss.str());
//     break;
//   case SensorUpdate::SENSOR: 
//     if(t == old.prevSensor[u.second]) return;
//     for(size_t i=0;i<gs.sensors.size();i++) {
//       ss << gs.sensors[i].rawValue.get() << " ";
//     }
//     helium::convert(old.prevSensor,ss.str());
//     break;
//   case SensorUpdate::GYRO: 
//     ss << t;
//     if(strcmp(prevGyro.c_str(),ss.str().c_str())==0) return;
//     prevGyro = ss.str();
//     break;
//   case SensorUpdate::POWER:     
//     if(t == old.prevPower[u.second]) return;
//     for(size_t i=0;i<gs.joints.size();i++) {
//       ss << gs.joints[i].jointPower.get() << " ";
//     }
//     helium::convert(old.prevPower,ss.str());
//     
//     break;
//   }
// 
//   logFile << helium::toString(helium::getSystemMillis()-t0) << " " << u.first;
//   logFile << ":";     //marks the starting of data
//   logFile << ss.str();
//   logFile << endl;
// }
// 
// void GuideTeachListInfo::writeLogTeachList() {
//   logFile << helium::toString(helium::getSystemMillis()-t0) << " teachList "<< guideTeachList.gettl().size();
//   logFile << ":"; 
//   logFile << guideTeachList;
//   logFile << endl;
// }
// 
// 
// 
// void GuideTeachListInfo::tlChange(TeachListChange::Reason reason,std::string s) {
//   //cout << "tlchange " << reason << endl;
//   switch(reason) {
//   case TeachListChange::ACQUIRE:  //write log and sensor
//     writeLog(reason,guideTeachList.gettl().size()-1,true);
//     break;        
//   case TeachListChange::UNDO:     
//   case TeachListChange::REDO:
//     writeLog(reason);             //write log
//     writeLogTeachList();          //write current teachList
//     break;
//   case TeachListChange::CLEAR:
//     tlHistory.pushPresent(guideTeachList.gettl());
//     writeLog(reason,"",false);       //write log
//     break;
//   case TeachListChange::ACQUIRED: //write log size and sensor, current teachlist
//     tlHistory.pushPresent(guideTeachList.gettl());
//     writeLog(reason,guideTeachList.gettl().size()-1,true);
//     writeLogTeachList(); 
//     break;    
//   case TeachListChange::OPEN:     //write log size of teachlist
//     tlHistory.pushPresent(guideTeachList.gettl());
//     writeLog(reason,guideTeachList.gettl().size(),false);
//     writeLogTeachList(); 
//     break;
//   case TeachListChange::ADD:      //write log size, current teachlist
//     tlHistory.pushPresent(guideTeachList.gettl());
//     writeLog(reason,guideTeachList.gettl().size()-1,false);
//     writeLogTeachList(); 
//     break;    
//   case TeachListChange::DELETETEACH:   //write log id
//     tlHistory.pushPresent(guideTeachList.gettl());
//     writeLog(reason,s,false);
//     writeLogTeachList(); 
//     break;
//   default:
//     break;
//   }
// }
// 
// bool GuideTeachListInfo::canUndo() {
//   return tlHistory.canRewind();
// }
// 
// bool GuideTeachListInfo::canRedo() {
//   return tlHistory.canForward();
// }
// 
// void GuideTeachListInfo::undo() {
//   tlHistory.rewind();
//   edit<TeachListChange::UNDO,void,const std::vector<TeachIO>&,&GuideTeachList::replaceTeachList>(tlHistory.present());
// }
// 
// void GuideTeachListInfo::redo() {
//   tlHistory.forward();
//   edit<TeachListChange::REDO,void,const std::vector<TeachIO>&,&GuideTeachList::replaceTeachList>(tlHistory.present());
// }
// 
// TeachTreeView::TeachTreeView(Gtk::Window &pwin, GuideTeachListInfo &pgtl,helium::GtkExceptionHandling *peh,RobotModels *prm):
//   win(pwin),
//   w(150),h(105),
//   gtl(pgtl),eh(peh),rm(prm)
// {
//   //Create the Tree model:
//   m_refTreeModel = Gtk::ListStore::create(m_Columns);
//   set_model(m_refTreeModel);
// 
//   //Add the TreeView's view columns:
//   //This number will be shown with the default numeric formatting.
//   append_column("ID", m_Columns.m_col_id);
// 
//   if(rm) append_column("Input", m_Columns.m_pixbuf_input);
//   if(rm) append_column("Output", m_Columns.m_pixbuf_output);
// 
//   Gtk::CellRendererText *renderer1 = Gtk::manage(new Gtk::CellRendererText());
//   append_column("Name", *renderer1);
// 
//   Gtk::TreeViewColumn *col = get_column(rm?3:1);
//   col->add_attribute(renderer1->property_markup(), m_Columns.m_col_name);
// 
// 
//   signal_button_press_event().connect(eh->wrap(*this, &TeachTreeView::on_button_press_event), false);
// 
//   //Fill popup menu:
//   {
//     Gtk::Menu::MenuList& menulist = m_Menu_Popup.items();
//     menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Apply Input to Frame",eh->wrap(*this,&TeachTreeView::onApplytoFrame,false)));
//     menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Apply Output to Frame",eh->wrap(*this,&TeachTreeView::onApplytoFrame,true)));
//     menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Copy Input to Clipboard",eh->wrap(*this,&TeachTreeView::onCopytoClipboard,false)));
//     menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Copy Output to Clipboard",eh->wrap(*this,&TeachTreeView::onCopytoClipboard,true)));
//     menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Delete",eh->wrap(*this,&TeachTreeView::onDelete)));
//     
//   }
//   m_Menu_Popup.accelerate(*this);
// 
//   for(size_t i=0;i<m_Menu_Popup.items().size();i++) 
//     m_Menu_Popup.items()[i].set_sensitive(false);
// }
// 
// void TeachTreeView::onCopytoClipboard(bool output) {
//   Glib::RefPtr<Gtk::Clipboard> clip(Gtk::Clipboard::get());
//   Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
//   if(refSelection) {
//     Gtk::TreeModel::iterator iter = refSelection->get_selected();
//     
//     if(iter) {
//       int id = (*iter)[m_Columns.m_col_id];
//        
//       std::stringstream ss;
//       if(output)
// 	ss <<gtl.get().gettl()[id].output;
//       else
// 	ss << gtl.get().gettl()[id].input;
//       clip->set_text(ss.str());
//     }
//   }
// }
// 
// bool TeachTreeView::on_button_press_event(GdkEventButton* event) {
//   bool return_value = false;
// 
//   //Call base class, to allow normal handling,
//   //such as allowing the row to be selected by the right-click:
//   return_value = Gtk::TreeView::on_button_press_event(event);
// 
//   //Then do our custom stuff:
//   if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
//   {
//     m_Menu_Popup.popup(event->button, event->time);
//   }
// 
//   return return_value;
// }
// 
// 
// void GuideTeachWindow::clearTree() {
//   m_TreeView.m_refTreeModel->clear();
// 
//   for(size_t i=0;i<m_TreeView.m_Menu_Popup.items().size();i++) 
//     m_TreeView.m_Menu_Popup.items()[i].set_sensitive(false);
// 
// 
//   m_TreeView.postureSnapi.clear();
//   m_TreeView.postureSnapo.clear();
// 
//   set_title("Teach List Window");
// }
// 
// void TeachTreeView::onApplytoFrame(bool output) {
//   Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
//   if(refSelection)
//   {
//     Gtk::TreeModel::iterator iter = refSelection->get_selected();
//     if(iter)
//     {
//       int id = (*iter)[m_Columns.m_col_id];
// 
//       if(output)
// 	applytoFrame(gtl.get().gettl()[id].output);
//       else
// 	applytoFrame(gtl.get().gettl()[id].input);
// 
//     }
//   }
// }
// 
// int TeachTreeView::getSelectedId() {
//  Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
//   if(refSelection)
//   {
//     Gtk::TreeModel::iterator iter = refSelection->get_selected();
//     if(iter)
//     {
//       return (*iter)[m_Columns.m_col_id];
//     }
//   }
//   return -1;
// }
// 
// void TeachTreeView::onDelete() {
//   Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
//   if(refSelection)
//   {
//     Gtk::TreeModel::iterator iter = refSelection->get_selected();
//     if(iter)
//     {
//       int id = (*iter)[m_Columns.m_col_id];
//       YesNoDialog d("Warning","Deleted teach input will be lost forever, continue?");
//       d.set_transient_for(win);
//       if(d.run()) {
// 	m_refTreeModel->erase( iter );
// 	gtl.edit<TeachListChange::DELETETEACH,void,int,&GuideTeachList::deleteTeach>(id,helium::toString(id));
// 
// 
// 	std::list<RobotModels::PostureSnap>::iterator iteri,itero;
// 	iteri = postureSnapi.begin();
// 	advance(iteri,id);
// 	postureSnapi.erase(iteri);
// 
// 	itero = postureSnapo.begin();
// 	advance(itero,id);
// 	postureSnapo.erase(itero);
// 
// 	//std::cout << "Deleted selected ID " << id << " remaining " << gtl.get().gettl().size()<< std::endl;
// 	if(gtl.get().gettl().size() < 1) {
// 	  //cout << "no more entry" << endl;
// 	  for(size_t i=0;i<m_Menu_Popup.items().size();i++) 
// 	    m_Menu_Popup.items()[i].set_sensitive(false);
// 	}
//       }
//     }
//   }
//   updateId();
// }
// 
// void TeachTreeView::updateId() {
//   typedef Gtk::TreeModel::Children type_children; //minimise code length.
//   type_children children = m_refTreeModel->children();
//   int i=0;
//   for(type_children::iterator iter = children.begin();
//       iter != children.end(); ++iter)
//     {
//       Gtk::TreeModel::Row row = *iter;
//       row[m_Columns.m_col_id] = i;
//       i++;
//     }
// }
// 
// void TeachTreeView::addEntry(const int id) {//const int id, const std::string sensor, const helium::Posture& anglei, const helium::Posture& angleo) {
//   addEntryInput(id);
//   updateEntryOutput(id);
// 
//   for(size_t i=0;i<m_Menu_Popup.items().size();i++) 
//     m_Menu_Popup.items()[i].set_sensitive(true);
// }
// 
// void TeachTreeView::addEntryInput(const int id) {
//   Gtk::TreeModel::Row row = *(m_refTreeModel->append());
//   row[m_Columns.m_col_id] = id;
// 
//   std::stringstream ss;
//   if(gtl.get().gettl()[id].name.size() > 0)
//     ss << gtl.get().gettl()[id].name << "\n";
//   ss << gtl.get().gettl()[id].input << "\n" << gtl.get().getSensorString(id) << "\n";
// 
//   row[m_Columns.m_col_name] = ss.str();
// 
//   if(rm) {
//     double black[3] = {0,0,0};
// 
//     postureSnapi.resize(postureSnapi.size()+1);
//     rm->getPostureSnap(postureSnapi.back(),gtl.get().gettl()[id].input,w,h,black);
// 
//     row[m_Columns.m_pixbuf_input] = Gdk::Pixbuf::create_from_data(postureSnapi.back().data,Gdk::COLORSPACE_RGB,false,8,w,h,w*3);
// 
//   }
// }
// 
// void TeachTreeView::updateEntryOutput(const int id) {
//   typedef Gtk::TreeModel::Children type_children;
//   type_children children = m_refTreeModel->children();
//   type_children::iterator iter = children.end();
//   iter--;
//   Gtk::TreeModel::Row row = *iter;
// 
//   std::stringstream ss;
//   ss << row[m_Columns.m_col_name] << gtl.get().gettl()[id].output << "\n";
//   row[m_Columns.m_col_name] = ss.str();
// 
//   if(rm) {
//     double black[3] = {0,0,0};
// 
//     postureSnapo.resize(postureSnapo.size()+1);
//     rm->getPostureSnap(postureSnapo.back(),gtl.get().gettl()[id].output,w,h,black);
// 
//     row[m_Columns.m_pixbuf_output] = Gdk::Pixbuf::create_from_data(postureSnapo.back().data,Gdk::COLORSPACE_RGB,false,8,w,h,w*3);
//   }
// }
// 
// 
// 
// GuideTeachWindow::~GuideTeachWindow() {
//   if(lg)
//     delete lg;
//   if(ep)
//     delete ep;
// }
// 
// struct  GuideTeachWindow::Connections{
//   helium::StaticConnReference<helium::Connection,GuideTeachWindow,helium::Connection,&GuideTeachWindow::onConnection> callOnConnection;
//   helium::StaticConnReference<cfg::guide::GuideView::Type,GuideTeachWindow,cfg::guide::GuideView::Type,&GuideTeachWindow::onViewChange> callOnViewChange;
//   helium::StaticConnReference<const helium::Posture&,GuideTeachWindow,const helium::Posture&,&GuideTeachWindow::onApplytoFrame> callOnApplytoFrame;
//   helium::ParamStaticConnReference<void,helium::GtkSignal<bool, bool,false,true>,
// 				   bool,& helium::GtkSignal<bool, bool,false,true>::operator() > callStartAcquire; 
//   helium::ParamStaticConnReference<void,helium::GtkSignal<bool, bool,false,true>,
// 				 bool,& helium::GtkSignal<bool, bool,false,true>::operator()> callStopAcquire; 
//   helium::StaticConnReference<bool,Gtk::ToggleToolButton,bool,&Gtk::ToggleToolButton::set_active> callSetAcquire; 
//   helium::StaticConnReference<void,GuideTeachWindow,void,&GuideTeachWindow::setPowerOff> callSetPowerOff; 
//  
//   Connections(GuideTeachWindow* owner):
//     callOnConnection(owner),
//     callOnViewChange(owner),
//     callOnApplytoFrame(owner),
//     callStartAcquire(&owner->setAcquireSignal,true),
//     callStopAcquire(&owner->setAcquireSignal,false),
//     callSetAcquire(&owner->b_acquire),
//     callSetPowerOff(owner)
//   {}
//  
// };
// 
// GuideTeachStatusLabel::GuideTeachStatusLabel(helium::ValueHub<TbTState::State> &state):
//   callOnStateChange(this,helium::FuncPlaceHolder<GuideTeachStatusLabel,TbTState::State,&GuideTeachStatusLabel::onStateChange>())
// {
//   state.connect(callOnStateChange); 
//   set_markup("<span color=\"black\">No robot connection</span>");
// }
// 
// void GuideTeachStatusLabel::onStateChange(TbTState::State s) {
//   switch(s) {
//   case TbTState::DISCONNECTED:
//     set_markup("<span color=\"black\">Robot is not connected</span>");
//     break;
//   case TbTState::CONNECTED:
//     set_markup("<span color=\"red\">Robot is connected</span>");
//     break;
//   case TbTState::TOUCHENABLE:
//     set_markup("<span color=\"green\">Robot can be touched</span>");
//     break;
//   }
// }
// 
// void GuideTeachWindow::onViewChange(cfg::guide::GuideView::Type v) {
//   helium::Connection::State connState = gmm.gs.conn.get().state;
//   switch(connState) {
//   case helium::Connection::CONNECTED :
//     if(v == cfg::guide::GuideView::TOUCH)
//       tbtState.assure(TbTState::TOUCHENABLE);
//     else 
//       tbtState.assure(TbTState::CONNECTED);
//     break;
//   default : 
//     tbtState.assure(TbTState::DISCONNECTED);
//     break;
//   }
// }
// 
// GuideTeachWindow::GuideTeachWindow(GuideMotionManagement &pgmm,RobotModels *prm) :
//   gmm(pgmm),//pcw(ppcw),
//   lg(NULL),
//   ep(NULL),
//   beforeAcquirePost(gmm.gs.joints.size()),
//   pedalConn(false),
//   writeLogConn(false),
//   tbtState(TbTState::DISCONNECTED),
//   gtl(gmm),
//   teachPower(gmm),
//   teachUse(gmm,gtl.get().gettl()),
//   copyTeachOutputToFrameButton("copy to frame"),
//   selectSensorLabel("select sensor by"),
//   statusLabel(tbtState),
//   m_TreeView(*this,gtl,&gmm.eh,prm),
//   //m_Button_Quit(Gtk::Stock::QUIT),
//   //m_Button_Open(Gtk::Stock::OPEN),
//   //m_Button_Clear(Gtk::Stock::CLEAR),
//   conns(new GuideTeachWindow::Connections(this)),
//   callSelectSensor(this),
//   callSetSelectedSensor(this),
//   callCopyTeachOutputToFrame(&m_TreeView,true),
//   callOnStateChange(this,helium::FuncPlaceHolder<GuideTeachWindow,TbTState::State,&GuideTeachWindow::onStateChange>()),
//   callWriteBuffer(this,gmm.gs.joints.size())	
//    
//   
// {
//   set_border_width(5);
//   set_default_size(350, 600);
// 
//   if(gmm.gs.pref.pedal.dev.size()) {
//     lg = new helium::LogitecGTForce(gmm.gs.pref.pedal.dev.c_str(),false,gmm.gs.pref.pedal.checksum1,gmm.gs.pref.pedal.checksum2,gmm.gs.pref.pedal.rdownThreshold,
// 				    gmm.gs.pref.pedal.rupThreshold, gmm.gs.pref.pedal.ldownThreshold,gmm.gs.pref.pedal.lupThreshold);
//     //ep = new helium::LogitecGTEventPrinter(*lg);
//     lg->start();
//     
// 
//   }
// 
//   add(m_VBox);
// 
//   Gtk::Widget* imAcquire = new Gtk::Image(Gtk::Stock::MEDIA_RECORD, Gtk::ICON_SIZE_LARGE_TOOLBAR);
//   b_acquire.set_icon_widget(*imAcquire);
//   b_acquire.set_label("Acquire");
//   b_acquire.signal_toggled().connect(gmm.eh.wrap(*this,&GuideTeachWindow::on_acquire));//signal_clicked()
//   gmm.gs.conn.connect(conns->callOnConnection); 
//   gmm.gs.view.connect(conns->callOnViewChange);
//   tbtState.connect(callOnStateChange);
//   setAcquireSignal.connect(conns->callSetAcquire);
// 
//   Gtk::Widget* imUse = new Gtk::Image(Gtk::Stock::MEDIA_PLAY, Gtk::ICON_SIZE_LARGE_TOOLBAR);
//   b_use.set_icon_widget(*imUse);
//   b_use.set_label("Use");
//   b_use.signal_toggled().connect(gmm.eh.wrap(*this,&GuideTeachWindow::on_use));//signal_clicked()
// 
// 
// 
// 
//   //Create actions for menus and toolbars:
//   m_refActionGroup = Gtk::ActionGroup::create();
// 
//   //toolbar button
//   m_refActionGroup->add(Gtk::Action::create("Open", Gtk::Stock::OPEN,"_Open"));
//   m_refActionGroup->add(Gtk::Action::create("SaveAs", Gtk::Stock::SAVE_AS,"_Save As"));
//   m_refActionGroup->add(Gtk::Action::create("Comment", Gtk::Stock::PROPERTIES,"Co_mment"));
//   m_refActionGroup->add(Gtk::Action::create("Clear", Gtk::Stock::CLEAR,"Clea_r"));
//   m_refActionGroup->add(Gtk::Action::create("Undo", Gtk::Stock::UNDO,"_Undo"));
//   m_refActionGroup->add(Gtk::Action::create("Redo", Gtk::Stock::REDO,"_Redo"));
//   m_refActionGroup->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT,"_Close"));
// 
//   m_refUIManager = Gtk::UIManager::create();
//   m_refUIManager->insert_action_group(m_refActionGroup);
//   this->add_accel_group(m_refUIManager->get_accel_group());
// 
//   Glib::ustring ui_info = 
//     "<ui>"
//     "  <toolbar name='ToolBar'>"
//     "    <toolitem action='Open'/>"
//     "    <toolitem action='SaveAs'/>"
//     "    <toolitem action='Comment'/>"
//     "      <separator/>"
//     "    <toolitem action='Clear'/>"
//     "    <toolitem action='Undo'/>"
//     "    <toolitem action='Redo'/>"
//     "      <separator/>"
//     "    <toolitem action='Quit'/>"
//     "  </toolbar>"
//     "</ui>";
// 
//   try { m_refUIManager->add_ui_from_string(ui_info); }
//   catch(const Glib::Error& ex) { std::cerr << "building menus failed: " <<  ex.what(); }
// 
//   pToolbar = dynamic_cast<Gtk::Toolbar*>(m_refUIManager->get_widget("/ToolBar")) ;
//   
//   m_VBox.pack_start(statusLabel, Gtk::PACK_SHRINK);
// 
//   if(pToolbar) {
//     m_VBox.pack_start(*pToolbar, Gtk::PACK_SHRINK);
//     pToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH);
//     pToolbar->insert(b_acquire,0); //pToolbar->get_n_items()
//     pToolbar->insert(b_use,1); //pToolbar->get_n_items()
//     pToolbar->insert(separator,2);
//   }
//   else 
//     std::cerr << "error: GuideTeachWindow no toolbar" << endl;
// 
// 
// 
// 
// 
//   //Add the TreeView, inside a ScrolledWindow, with the button underneath:
//   m_ScrolledWindow.add(m_TreeView);
// 
//   //Only show the scrollbars when they are necessary:
//   m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
// 
//   m_VBox.pack_start(m_ScrolledWindow);
//   m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);
// 
// 
//   //m_ButtonBox.pack_start(m_Button_Open, Gtk::PACK_SHRINK);
//   //m_ButtonBox.pack_start(m_Button_Clear, Gtk::PACK_SHRINK);
//   //m_ButtonBox.pack_start(m_Button_Quit, Gtk::PACK_SHRINK);
// 
//   m_ButtonBox.set_border_width(5);
//   m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
//   m_refActionGroup->get_action("Open")->signal_activate().connect( gmm.eh.wrap(*this,&GuideTeachWindow::on_button_file,OPEN) );
//   m_refActionGroup->get_action("SaveAs")->signal_activate().connect( gmm.eh.wrap(*this,&GuideTeachWindow::on_button_file,SAVEAS) );
//   m_refActionGroup->get_action("Comment")->signal_activate().connect( gmm.eh.wrap(*this,&GuideTeachWindow::on_comment) );
//   m_refActionGroup->get_action("Clear")->signal_activate().connect( gmm.eh.wrap(*this,&GuideTeachWindow::on_button_clear) );
//   m_refActionGroup->get_action("Quit")->signal_activate().connect( gmm.eh.wrap(*this,&GuideTeachWindow::on_button_quit) );
//   m_refActionGroup->get_action("Undo")->signal_activate().connect( gmm.eh.wrap(*this,&GuideTeachWindow::on_button_undo) );
//   m_refActionGroup->get_action("Redo")->signal_activate().connect( gmm.eh.wrap(*this,&GuideTeachWindow::on_button_redo) );
// 
//   m_TreeView.applytoFrame.connect(conns->callOnApplytoFrame);
// 
//   //sensor select
//   m_VBox.pack_start(sensorSelectionHBox, Gtk::PACK_SHRINK);
//   sensorSelectionHBox.pack_start(copyTeachOutputToFrameButton, Gtk::PACK_EXPAND_PADDING,0);
//   sensorSelectionHBox.pack_start(sensorSelectionHBoxSub, Gtk::PACK_SHRINK);
//   sensorSelectionHBoxSub.pack_start(selectSensorLabel,Gtk::PACK_EXPAND_PADDING,0);
//   sensorSelectionHBoxSub.pack_start(selectSensor,Gtk::PACK_EXPAND_PADDING,0);
// 
//   copyTeachOutputToFrameButton.signal_clicked().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onCopyTeachOutputToFrameButtonClick));
//   selectSensor.signal_changed().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onSelectionChange));				 
// 
//   m_TreeView.signal_cursor_changed().connect(gmm.eh.wrap(*this,&GuideTeachWindow::onSelectionChange));
//   for(int i=0;i<(int)SensorSelection::NUM;i++)
//     selectSensor.append(SensorSelection::strings[i]);
//   selectSensor.set_active_text(SensorSelection::strings[SensorSelection::BYDESC]);
// 
//   selectionChangeSignal.connect(callSetSelectedSensor);
// 
// 
//   show_all_children();
// }
// 
// void GuideTeachWindow::onApplytoFrame(const helium::Posture& p) {
//   for(size_t i=0;i<gmm.gs.joints.size();i++)
//     gmm.gs.joints[i].frameValue.assure<GuideEvent::PASTETEACHPOSTURE>(std::make_pair(helium::VALUEFRAME,p[i]));
//   gmm.gs.joints.postureChange(GuideEvent::PASTETEACHPOSTURECHANGE);
// }
// 
// void GuideTeachWindow::on_button_undo() {
//   if(!gtl.canUndo()) return; 
//   if(b_acquire.get_active()) return;
//   
//   gtl.undo();
//   clearTree();
//   reloadTree();
//   
// }
// void GuideTeachWindow::on_button_redo() {
//   if(!gtl.canRedo()) return; 
//   if(b_acquire.get_active()) return;
// 
//   gtl.redo();
//   clearTree();
//   reloadTree();
//   
// }
// 
// void GuideTeachWindow::reloadTree() {
//   for(size_t i=0;i<gtl.get().gettl().size();i++) {
//     //m_TreeView.addEntry(i,gtl.get().getSensorString(i),gtl.get().gettl()[i].input,gtl.get().gettl()[i].output);
//     m_TreeView.addEntry(i);
//   }
// }
// 
// void GuideTeachWindow::on_button_file(ActionType action) {
//   std::string title = "";
//   switch(action) {
//   case OPEN: title="Please choose a teach file";break;
//   case SAVEAS: title="Please input a teach file";break;
//   }
// 
//   Gtk::FileChooserDialog dialog(title, action==OPEN?Gtk::FILE_CHOOSER_ACTION_OPEN:Gtk::FILE_CHOOSER_ACTION_SAVE);
// 
//   dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
//   dialog.add_button(action==OPEN?Gtk::Stock::OPEN:Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);
// 
//   //making sure tilda is replaced by user's home folder
//   std::string f = gtl.get().getFilename().substr(0,gtl.get().getFilename().find_last_of("/\\"));
//   if(lastOpenedFile.size())
//     f = lastOpenedFile.substr(0,lastOpenedFile.find_last_of("/\\"));
//   dialog.set_current_folder(f);
// 
// 
//   Gtk::CheckButton replace("Replace current list");
//   if(action==OPEN) {
//     replace.show();
//     replace.set_active(true);
//     dialog.set_extra_widget(replace);
//   }
// 
//   //Add filters, so that only certain file types can be selected:
//   Gtk::FileFilter filter_tea;
//   filter_tea.set_name("TEA files");
//   filter_tea.add_pattern("*.tea");
//   dialog.add_filter(filter_tea);
// 
//   Gtk::FileFilter filter_txt;
//   filter_txt.set_name("TXT files");
//   filter_txt.add_pattern("*.txt");
//   
//   if (action==SAVEAS) {
//     dialog.add_filter(filter_txt);
//     dialog.set_do_overwrite_confirmation(true);
//   }
//   dialog.set_current_name("teach.tea");
// 
//   if(dialog.run()==Gtk::RESPONSE_OK) {
//     lastOpenedFile = dialog.get_filename();
//     if(action==SAVEAS) {
//       //cout << dialog.get_filter()->get_name() << endl;
//       if(dialog.get_filter()->get_name() == filter_tea.get_name())
// 	gtl.get().saveTeachList(dialog.get_filename().c_str());
//       else if(dialog.get_filter()->get_name() == filter_txt.get_name())
// 	gtl.get().saveTeachListTxt(dialog.get_filename());
//     }
//     else {//OPEN file (replace current)
//       openFile(replace.get_active(),dialog.get_filename());
//       fileChangeSignal(dialog.get_filename());
//     }
//     set_title("Teach List Window " + lastOpenedFile);
//   }
// }
// 
// void GuideTeachWindow::openFile(bool toReplace,std::string openedFile) {
//   std::vector<TeachIO> tl;
//   gtl.get().loadTeachList(openedFile.c_str(),tl);
//   if(toReplace) {
//     gtl.edit<TeachListChange::CLEAR,void,&GuideTeachList::clear>();
//     gtl.edit<TeachListChange::OPEN,void,const std::vector<TeachIO> &,&GuideTeachList::replaceTeachList>(tl);	
//   }
//   else {
//     gtl.edit<TeachListChange::OPEN,void,const std::vector<TeachIO> &,&GuideTeachList::appendTeachList>(tl);	
//   }
//   clearTree();
//   reloadTree();
// }
// 
// void GuideTeachWindow::on_button_clear() {
//   YesNoDialog d("Warning","Delete all teach inputs?");
//   if(d.run()) {
//     gtl.edit<TeachListChange::CLEAR,void,&GuideTeachList::clear>();
//     clearTree();
//   }
// }
// 
// 
// /*void GuideTeachWindow::setAcquire(bool b) {
//   cout << "acquire " << b << endl;
//   setAcquireButton(b);
//   }*/
// 
// void GuideTeachWindow::selectEntry(int id) {
// 
//   //only select entry if select sensor is by id
//   if(selectSensor.get_active_row_number() != SensorSelection::BYID) return;
// 
//   typedef Gtk::TreeModel::Children type_children; //minimise code length.
//   type_children children = m_TreeView.m_refTreeModel->children();
//   type_children::iterator iter = children.begin();
// 
//   if (id > (int)children.size())
//     return;
// 
//   for(int i=0;i<id;i++)
//     iter++;
//   m_TreeView.set_cursor(m_TreeView.get_model()->get_path(*iter));
// 
// }
// 
// void GuideTeachWindow::on_use() {
//   if(b_use.get_active()) {
//     b_acquire.set_sensitive(false);      
//     teachUse.setTeachActive(true);   
//   }
//   //button unpressed
//   else {
//     teachUse.setTeachActive(false);
//     b_acquire.set_sensitive(true);
//   }
// }
// 
// void GuideTeachWindow::on_acquire() {
// 
//   if(b_use.get_active()) {
//     return;
//   }
// 
//   if(b_acquire.get_active()) {
//     b_use.set_sensitive(false);
//     //cout << "start acquiring..." << endl;
//     helium::Posture pin(gmm.gs.joints.size());
//     helium::Array<int> sensor(gmm.gs.sensors.size());
//     helium::Array<int> gyro(5);
// 
//     helium::zero(pin);
// 
//     //saves robot pos
//     if(gmm.gs.conn.get().state == helium::Connection::CONNECTED) {
//       for(size_t i=0;i<gmm.gs.joints.size();i++) {
// 	pin[i] = gmm.gs.joints[i].displayedTarget.get();
// 	
// 	//check error
// 	if(pin[i] < -3 || pin[i] > 3) {
// 	  helium::zero(pin);
// 	  cerr << "error: faulty in pin, reset pin to zero"<<endl;
// 	  break;
// 	}
//       }
//     }
//     else
//       for(size_t i=0;i<gmm.gs.joints.size();i++) {
// 	pin[i] = gmm.gs.joints[i].frameValue.get().second;
//       }
// 
//     beforeAcquirePost = pin;
// 
//     //saves sensors
//     for(size_t i=0;i<gmm.gs.sensors.size();i++) {
//       sensor[i] = gmm.gs.sensors[i].rawValue.get();
//     }
// 
//     gyro = gmm.gs.gyro.data.get().vals;
// 
//     helium::Time tin = helium::getSystemMillis()-gmm.t0;
//     gtl.edit<TeachListChange::ACQUIRE,void,const helium::Array<int>&, const helium::Posture&,const helium::Array<int>&, const helium::Time&,&GuideTeachList::acquireTeach>(sensor,pin,gyro,tin);
//     m_TreeView.addEntryInput(gtl.get().gettl().size()-1);
// 
//     //enable turning power off
//     teachPower.setTeachActive(true);
// 
//     //select the last taught point
//     selectEntry(gtl.get().gettl().size()-1);
// 
//     for(size_t i=0;i<m_TreeView.m_Menu_Popup.items().size();i++) 
//       m_TreeView.m_Menu_Popup.items()[i].set_sensitive(false);
// 
//     //setting menu state
//     m_refActionGroup->get_action("Open")->set_sensitive(false);
//     m_refActionGroup->get_action("SaveAs")->set_sensitive(false);
//     m_refActionGroup->get_action("Clear")->set_sensitive(false);
//     m_refActionGroup->get_action("Undo")->set_sensitive(false);
//     m_refActionGroup->get_action("Redo")->set_sensitive(false);
//     m_refActionGroup->get_action("Quit")->set_sensitive(false);
// 
// 
// 
// 
//     cout << "will return to " <<  gtl.get().gettl()[gtl.get().gettl().size()-1].input << endl;
//   }
//   else {
//     helium::Posture pout(gmm.gs.joints.size());
//     helium::zero(pout);
// 
//     /*
//     for(size_t i=0;i<gmm.gs.joints.size();i++) {
//       pout[i] = gmm.gs.joints[i].displayedTarget.get();
//     }
//     */
// 
//     //saves robot pos
// 
//     if(gmm.gs.conn.get().state == helium::Connection::CONNECTED) {
//       for(size_t i=0;i<gmm.gs.joints.size();i++) {
// 	pout[i] = gmm.gs.joints[i].displayedTarget.get();
// 	
// 	//check error
// 	if(pout[i] < -3 || pout[i] > 3) {
// 	  pout[i] = gmm.gs.joints[i].displayedTargetBuffer;
// 	  cerr << "error: faulty in pout, reset pout ["<<i <<"] to "<< pout[i]<<endl;
// 	}
//       }
//     }
//     else
//       for(size_t i=0;i<gmm.gs.joints.size();i++) {
// 	pout[i] = gmm.gs.joints[i].frameValue.get().second;
//       }
// 
//     helium::Time tout = helium::getSystemMillis()-gmm.t0;
//     gtl.edit<TeachListChange::ACQUIRED,void,const helium::Posture&,const helium::Time&,&GuideTeachList::acquiredTeach>(pout,tout);
//     m_TreeView.updateEntryOutput(gtl.get().gettl().size()-1);
// 
// 
//     for(size_t i=0;i<m_TreeView.m_Menu_Popup.items().size();i++) 
//       m_TreeView.m_Menu_Popup.items()[i].set_sensitive(true);
// 
// 
//     //disable turning power off
//     teachPower.setTeachActive(false);
// 
//     helium::Posture resetPost = gtl.get().gettl()[gtl.get().gettl().size()-1].input;
//     cout << "returning to " << resetPost << endl;
//     //return robot to previous posture
//     if(gmm.gs.conn.get().state == helium::Connection::CONNECTED) {
//       for(size_t i=0;i<gmm.gs.joints.size();i++) {
// 	gmm.gs.joints[i].desiredTarget.assure(resetPost[i]);      
//       }
//     }
// 
//     selectEntry(gtl.get().gettl().size()-1);
// 
//     //setting menu state
//     m_refActionGroup->get_action("Open")->set_sensitive(true);
//     m_refActionGroup->get_action("SaveAs")->set_sensitive(true);
//     m_refActionGroup->get_action("Clear")->set_sensitive(true);
//     m_refActionGroup->get_action("Undo")->set_sensitive(true);
//     m_refActionGroup->get_action("Redo")->set_sensitive(true);
//     m_refActionGroup->get_action("Quit")->set_sensitive(true);
// 
//     if(gmm.gs.conn.get().state == helium::Connection::CONNECTED)
//       b_use.set_sensitive(true);
//   }
// }
// 
// void GuideTeachWindow::writeBuffer(double d,int i) {
//   if(d<-3 || d>3) return;
//   
//   gmm.gs.joints[i].displayedTargetBuffer = gmm.gs.joints[i].displayedTarget.get();
// }
// 
// void GuideTeachWindow::onConnection(helium::Connection c) {
//   onViewChange(gmm.gs.view.get());
// 
//   //logging robot state
//   helium::Connection::State connState = c.state;
//   switch(connState) {
//   case helium::Connection::DISCONNECTED : 
//     //connection for logging
//     if(writeLogConn) {
//       cout << "writelogconn off" << endl;
//       for(size_t i=0;i<gmm.gs.joints.size();i++) {
// 	gmm.gs.joints[i].displayedTarget.disconnect(gtl.callWritePot[i]);
// 	gmm.gs.joints[i].displayedTarget.disconnect(callWriteBuffer[i]);
// 	gmm.gs.joints[i].jointPower.disconnect(gtl.callWritePower[i]);
//       }
//       for(size_t i=0;i<gmm.gs.sensors.size();i++)
// 	gmm.gs.sensors[i].rawValue.disconnect(gtl.callWriteSensor[i]);
//       gmm.gs.gyro.data.disconnect(gtl.callWriteGyro);
//       writeLogConn = false;
//     }
//     break;
//   case helium::Connection::CONNECTED : 
//     //connection for logging
//     if(!writeLogConn) {
//       cout << "writelogconn on" << endl;
//       for(size_t i=0;i<gmm.gs.joints.size();i++) {
// 	gmm.gs.joints[i].displayedTarget.connect(gtl.callWritePot[i]);
// 	gmm.gs.joints[i].displayedTarget.connect(callWriteBuffer[i]);
// 	gmm.gs.joints[i].jointPower.connect(gtl.callWritePower[i]);
//       }
//       for(size_t i=0;i<gmm.gs.sensors.size();i++)
// 	gmm.gs.sensors[i].rawValue.connect(gtl.callWriteSensor[i]);
//       gmm.gs.gyro.data.connect(gtl.callWriteGyro);
//       writeLogConn = true;
//     }
//     break;
//   default:
//     break;
//   }
// }
// 
// void GuideTeachWindow::onStateChange(TbTState::State s) {
//   switch(s) {
//   case TbTState::TOUCHENABLE : 
//     b_acquire.set_sensitive(true);
//     if(!b_acquire.get_active())
//        b_use.set_sensitive(true);
// 
//     //pedal
//     if(lg && !pedalConn) {
//       lg->rightPedalDown.connect(conns->callStartAcquire);
//       lg->rightPedalUp.connect(conns->callStopAcquire);
//       lg->leftPedalDown.connect(conns->callSetPowerOff);
//       pedalConn = true;
//     }
// 
//     break;
//   default:
// 
//     b_acquire.set_sensitive(false);
//     b_use.set_sensitive(false);
// 
//     //pedal
//     if(lg && pedalConn) {
//       lg->rightPedalDown.disconnect(conns->callStartAcquire);
//       lg->rightPedalUp.disconnect(conns->callStopAcquire);
//       lg->leftPedalDown.disconnect(conns->callSetPowerOff);
//       pedalConn = false;
//     }
// 
//   }
// }
// 
// void GuideTeachWindow::on_comment() {
//   std::string c;
//   InputDialog inputDialog("Comment:",c,&gmm.eh);
//   inputDialog.set_transient_for(*this);
//   if(inputDialog.run()) {
//     gtl.writeLog(TeachListChange::NONE,c);
//   }  
// }
// 
// void GuideTeachWindow::setPowerOff() {
//   for(size_t i=0;i<gmm.gs.joints.size();i++)
//     gmm.gs.joints[i].jointPower.set(false);
// }
// 
// 
// void GuideTeachWindow::onSelectionChange() {
//   static bool changing = false;
//   Glib::RefPtr<Gtk::TreeView::Selection> refSelection = m_TreeView.get_selection();
//   if(!changing && refSelection) {
//     Gtk::TreeModel::iterator iter = refSelection->get_selected();
//     
//     if(iter) {
//       changing = true;
//       selectionChangeSignal((*iter)[m_TreeView.m_Columns.m_col_id]);
//       changing = false;
//     }
//   }
// }
// 
// void GuideTeachWindow::setSelectedSensor() {
//   Glib::RefPtr<Gtk::TreeView::Selection> refSelection = m_TreeView.get_selection();
//   if(refSelection) {
//     Gtk::TreeModel::iterator iter = refSelection->get_selected();
//     if(iter) {
//       switch((SensorSelection::SENSORSELECTION)selectSensor.get_active_row_number()) {
//       case SensorSelection::BYDESC: {
// 	std::stringstream ss;
// 	ss << (*iter)[m_TreeView.m_Columns.m_col_name];
// 	int sid = -1;
// 	ss >> sid;
// 	if(sid > -1)
// 	  sensorSelectionSignal(sid);
// 	break;
//       }
//       case SensorSelection::BYID: {
// 	sensorSelectionSignal((*iter)[m_TreeView.m_Columns.m_col_id]);
// 	break;
//       }
//       default: break;
//       }
//     }
//   }
// }
// 
// 
// void GuideTeachWindow::onCopyTeachOutputToFrameButtonClick() {
//   if(copyTeachOutputToFrameButton.get_active()) {
//     cout << "connecting" << endl;
//     selectionChangeSignal.connect(callCopyTeachOutputToFrame);
//   }
//   else {
//     cout << "disconnecting" << endl;
//     selectionChangeSignal.disconnect(callCopyTeachOutputToFrame);
//   }
// }
