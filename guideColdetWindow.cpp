/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include <helium/xml/xmlLoader.h>
#include "guideDescription.h"
#include "guideColdetWindow.h"

using std::cout;
using std::endl;

const char* GuideColdetWindow::CpuEnum::strings[] = {"Idle","Busy"};

GuideColdetWindow::TreeView::TreeView() {
  //Create the Tree model:
  listStore = Gtk::ListStore::create(modelColumns);
  set_model(listStore);

  append_column("start", modelColumns.m_col_start);
  append_column("end", modelColumns.m_col_end);

  //adding column with icon
  Gtk::TreeView::Column* pColumn = Gtk::manage( new Gtk::TreeView::Column("state") );
  pColumn->pack_start(modelColumns.m_col_icon, false); //false = don't expand.
  pColumn->pack_start(modelColumns.m_col_state);
  append_column(*pColumn);

  show_all_children();
}

GuideColdetWindow::GuideColdetWindow(cfg::guide::Joints &pjoints, cfg::guide::Coldet &pcoldet,coldet::ColdetManager &pcoldetm,helium::GtkExceptionHandling *peh,const std::string& pfilename):
  joints(pjoints),
  coldet(pcoldet),
  coldetm(pcoldetm),
  eh(peh),
  filename(pfilename),
  label_cpu_idle("CPU idle"),
  label_cpu_busy("CPU busy"),
  button_default("Default"),

  vbbox_idle(Gtk::BUTTONBOX_CENTER,1),
  vbbox_busy(Gtk::BUTTONBOX_CENTER,1),
  frame_cpu("CPU settings"),
  frame_turbo("Turbo settings"),
  callUpdateSegment(this),
  callOnEnableColdet(this)
{

  set_title("Collision Detection Window");
  std::stringstream ss;
  ss <<"idle " << coldet.idle.def << ", "<< "busy " << coldet.busy.def;
  label_default.set_text(ss.str());


  vbox_cpu.pack_start(hbox_outer,Gtk::PACK_EXPAND_PADDING,5);
  frame_cpu.add(vbox_cpu);
  frame_cpu.set_border_width(3);

  Gtk::RadioButton::Group group = rbutton[0].get_group();
  for(int i=1;i<3;i++)
    rbutton[i].set_group(group);
  rbutton[0].set_label("Normal");
  rbutton[1].set_label("Auto");
  rbutton[2].set_label("Turbo");

  for(int i=0;i<3;i++) {
    hbox_turbo.pack_start(rbutton[i]);
    rbutton[i].signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(*this,&GuideColdetWindow::setTurbo),i));
  }

  vbox_turbo.pack_start(hbox_turbo,Gtk::PACK_EXPAND_PADDING,5);

  frame_turbo.add(vbox_turbo);
  frame_turbo.set_border_width(3);

  //vbox_main.pack_start(label_default);

  vbox_main.pack_start(frame_turbo, Gtk::PACK_SHRINK);
  vbox_main.pack_start(frame_cpu, Gtk::PACK_SHRINK);
  vbox_main.pack_start(scrolledWindow, Gtk::PACK_EXPAND_WIDGET);


  scrolledWindow.add(treeView);
  scrolledWindow.set_size_request(-1,100);

  hbox_outer.pack_start(vbbox_idle);
  hbox_outer.pack_start(vbbox_busy);
  hbox_outer.pack_start(button_default);
  
  vbbox_idle.pack_start(label_cpu_idle,Gtk::PACK_SHRINK);
  vbbox_idle.pack_start(entry_cpu_idle,Gtk::PACK_SHRINK);
  vbbox_busy.pack_start(label_cpu_busy,Gtk::PACK_SHRINK);
  vbbox_busy.pack_start(entry_cpu_busy,Gtk::PACK_SHRINK);

  entry_cpu_idle.set_range(1,100);
  entry_cpu_busy.set_range(1,100);

  entry_cpu_idle.set_increments(1,10);
  entry_cpu_busy.set_increments(1,10);

  entry_cpu_idle.set_value(coldet.idle.cur);
  entry_cpu_busy.set_value(coldet.busy.cur);

  

  add(vbox_main);
  show_all_children();


  //signal
  /// create signal to update file when value changed
  entry_cpu_idle.signal_value_changed().connect(eh->wrap(*this,&GuideColdetWindow::updateCurCpu,CpuEnum::IDLE));
  entry_cpu_busy.signal_value_changed().connect(eh->wrap(*this,&GuideColdetWindow::updateCurCpu,CpuEnum::BUSY));
  button_default.signal_clicked().connect(eh->wrap(*this,&GuideColdetWindow::setToDefault));


  coldet.coldetState.connect(callOnEnableColdet);

  set_size_request(250,450);

  show_all_children();


}

void GuideColdetWindow::onEnableColdet(bool enable) {  
  treeView.set_sensitive(enable);  
  if(enable)
    coldetm.checkManager.notifications.connect(callUpdateSegment);
  else
    coldetm.checkManager.notifications.disconnect(callUpdateSegment);
}

/// \todo confirm setTurbo correct?
void GuideColdetWindow::setTurbo(int id) {
  if(rbutton[id].get_active()) {
    coldetm.setTurbo((coldet::TurboSetting)id);
  }
}

void GuideColdetWindow::setToDefault() {
  entry_cpu_idle.set_value(coldet.idle.def);
  entry_cpu_busy.set_value(coldet.busy.def);
}

void GuideColdetWindow::updateCurCpu(CpuEnum::Type t) {
  /// updating current value
  switch(t) {
  case CpuEnum::IDLE :  
    coldet.idle.cur = entry_cpu_idle.get_value_as_int();
  break;
  case CpuEnum::BUSY :
    coldet.busy.cur = entry_cpu_busy.get_value_as_int();
    break;
  default:break;
  }
  /// and saving to file
  save(); 
  coldet.refresh();
}

/**
 * saving with the format of the xml
 * <guide>
 *   <coldet>
 *     <idle default="4" current="3" />
 *     <busy default="2" current="2" />
 *   </coldet>
 * </guide>
 */
void GuideColdetWindow::save() {
  helium::Memory outmem;
  helium::XmlLoader out;
  dexport(outmem,coldet,helium::IDPath("guide.coldet","."));
  out.save(filename.c_str(),outmem);  
}

/** \brief clear and add segment into tree view, triggered by signal from coldet::coldetManager
 */
void GuideColdetWindow::updateSegment(const coldet::SegmentNotification& s) {
  treeView.listStore->clear();
  for(size_t i=0;i<s.size();i++) {
    Gtk::TreeModel::Row row = *(treeView.listStore->append());
    row[treeView.modelColumns.m_col_start] = s[i].start;
    row[treeView.modelColumns.m_col_end] = s[i].end;
    row[treeView.modelColumns.m_col_icon] = render_icon((s[i].coll == coldet::SegmentState::NOCOLLISION?Gtk::Stock::APPLY:(s[i].coll == coldet::SegmentState::CHECKING?Gtk::Stock::REFRESH:Gtk::Stock::CANCEL)), Gtk::ICON_SIZE_MENU);
    std::stringstream ss;
    if(s[i].coll == coldet::SegmentState::NOCOLLISION)
      ss << "ok";
    else if(s[i].coll == coldet::SegmentState::CHECKING)
      ss << "checking";
    else 
      ss << "collide at " << s[i].coll;

    row[treeView.modelColumns.m_col_state] = ss.str();
  }

}
