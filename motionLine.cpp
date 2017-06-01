/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#include "motionLine.h"
#include <iostream>
#include <helium/util/ioUtil.h>
#include <app/guide2/glarea.h>
#include <app/guide2/guideState.h>
#include "robotModels.h"
#include <app/guide2/yesNoDialog.h>
#include <app/guide2/guideUtil.h>


#define MINLENGTH 1000
#define SCROLLLIMIT 52

using namespace std;
const char* FrameEntry::FrameEntryStatus::strings[] = {"New Frame","Time Adjusted","New Frame","Speed Changed","Repeat Changed","Editing","Invalid",""}; 

TooltipWindow::TooltipWindow(RobotModels::ModelArea* pmodelArea):
  modelArea(pmodelArea)
{
  tooltipLabel.set_justify(Gtk::JUSTIFY_CENTER);
  tooltipBox.pack_start(tooltipLabel);
  if(modelArea) {
    tooltipBox.pack_start(modelArea->getBox());
  }
  add(tooltipBox);
  set_decorated(false); 
  show_all_children();
}

void TooltipWindow::show(Glib::ustring &tip) {
  tooltipLabel.set_markup(tip);
  tooltipLabel.show();
  tooltipBox.show_all_children();
  tooltipBox.queue_draw();
}

void TooltipWindow::setPosture(const double* data) {
  if(modelArea)
    modelArea->setPosture(data);
}

void TooltipWindow::setBG(const double* color) {
  if(modelArea)
    modelArea->setBG(color);
}

TicksArea::TicksArea( Gtk::Window &pwin, GuideMotionManagement &pgmm, std::map<helium::Time,cfg::guide::TicksType::Type> &pticks, Gtk::ScrolledWindow &pscrolledWindow, RobotModels::ModelArea *pmodelArea, coldet::ColdetManager *pcoldetm):
  win(pwin),
  gmm(pgmm), 
  ticks(pticks),
  scrolledWindow(pscrolledWindow),
  coldetm(pcoldetm),
  tooltipWindow(pmodelArea),
  zoom(0),
  ticksMutex("ticks"), 
  callRefresh(this),
  callOnEditing(this),
  callSetTicks(this),
  callSetColdetTicks(this),
  callOnEnableColdet(this)
{
  set_flags(Gtk::CAN_FOCUS);
  set_size_request(1,10);
  
  set_has_tooltip();
  set_tooltip_window(tooltipWindow);
  signal_query_tooltip().connect(gmm.eh.wrap(*this,&TicksArea::onTicksTooltip));
  
  set_events(Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(gmm.eh.wrap(*this,&TicksArea::onMouseClick));
  gmm.gs.motionInfo.curTime.connect(callRefresh);  
  gmm.gs.motionInfo.editing.status.connect(callOnEditing);
  gmm.gs.motionInfo.motionChange.connect(callSetTicks);

  if(gmm.gs.pref.hasColdet) {
    gmm.gs.coldet.coldetState.connect(callOnEnableColdet);
  }

  //Fill popup menu:
  {
    Gtk::Menu::MenuList& menulist = frameMenu.items();
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_New frame",gmm.eh.wrap(*this,&TicksArea::newFrame)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Adjust time",gmm.eh.wrap(*this,&TicksArea::adjustTime)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Delete frame",gmm.eh.wrap(*this,&TicksArea::deleteFrame)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Copy frame",gmm.eh.wrap( *this, &TicksArea::copyFrame)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Copy frame to",gmm.eh.wrap( *this, &TicksArea::copyFrameTo)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Paste",gmm.eh.wrap( *this, &TicksArea::pasteFrame)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Paste frame",gmm.eh.wrap( *this, &TicksArea::pasteNearbyFrame)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Link to",gmm.eh.wrap(*this, &TicksArea::linkFrame)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Unlink",gmm.eh.wrap( *this, &TicksArea::unlinkFrame)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Rename",gmm.eh.wrap( *this, &TicksArea::renameFrame)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Fix automatically",gmm.eh.wrap( *this, &TicksArea::fixFrame)));
  }
  frameMenu.accelerate(*this);

  setTicks();
}

void TicksArea::newFrame() {
  gmm.insertFrame(clickCursorTime);
}

void TicksArea::adjustTime() {
  helium::Time dt = 0;

  //dialog window
  string adj = "";
  InputDialog inputDialog("+N to add frame time distance or -N to reduce",adj,&gmm.eh);
  inputDialog.set_transient_for(win);
  if(inputDialog.run()) {
    helium::convert(dt,adj," Invalid time adjustment");
    gmm.adjustTime(clickCursorTime,dt);
  }
}

void TicksArea::deleteFrame() {
  gmm.deleteFrame(clickFrameTime);
}

void TicksArea::copyFrame() {
  gmm.copyFrame(gmm.gs.motionInfo.getMotion()[clickFrameTime]);
  gmm.copying = true;
}

void TicksArea::copyFrameTo() {
  gmm.copyFrame(gmm.gs.motionInfo.getMotion()[clickFrameTime]);
  gmm.copying = true;

  helium::Time to;
  string frameto = "";
  InputDialog inputDialog("Copy frame to",frameto,&gmm.eh);
  inputDialog.set_transient_for(win);
  if(inputDialog.run()) {
    helium::convert(to,frameto," Invalid frame time");
    gmm.pasteFrame(to);
  }  
}

void TicksArea::pasteFrame() {
  gmm.pasteFrame(clickCursorTime);
}

void TicksArea::pasteNearbyFrame() {
  gmm.pasteFrame(clickFrameTime);
}

void TicksArea::linkFrame() { 
  helium::Time to;

  //dialog window
  string link = "";
  InputDialog inputDialog("Link frame to",link,&gmm.eh);
  inputDialog.set_transient_for(win);
  if(inputDialog.run()) {
    helium::convert(to,link," Invalid frame time");
    gmm.linkFrame(clickFrameTime,to);
  }  
}

void TicksArea::unlinkFrame() {
  gmm.unlinkFrame(clickFrameTime);
}

void TicksArea::renameFrame() {
  string name = "";
  InputDialog inputDialog("Input a posture name",name,&gmm.eh);
  inputDialog.set_transient_for(win);
  if(inputDialog.run() && name.size() != 0) { 
    gmm.gs.motionInfo.motionEdit<GuideEvent::RENAMEFRAME,void,helium::Time,std::string,&helium::Motion::rename>
      (clickFrameTime,name);
  }
}

void TicksArea::fixFrame() {
  if(coldetm) {
    coldetm->copyAutoFix(clickFrameTime);
  }
  else {
    throw helium::exc::Bug("no ColdetManager found");
  }
}

bool TicksArea::onMouseClick (GdkEventButton* event) {
  clickCursorTime = getCursorTime(event->x);
  clickFrameTime = getFrameTime(event->x);

  //mouse click to choose a frame
  if(event->button == 1) {
    gmm.gs.motionInfo.curTime.assure(clickFrameTime);
  }  
  //right click to open menu
  else if(event->button == 3) {
    onRightMouseClick(event);
  }
  return false;
}


void TicksArea::onRightMouseClick (GdkEventButton* event) {
  stringstream ss;
  bool isFrame = ticks[clickFrameTime]==cfg::guide::TicksType::KEYFRAMETICK || ticks[clickFrameTime]==cfg::guide::TicksType::COLDETFRAMETICK;

  /// new frame, 
  /// time: clickCursorTime, 
  /// enabled: no frame at cursor
  ss << "_New frame at " << clickCursorTime;
  frameMenu.items()[NEW].set_label(ss.str());
  frameMenu.items()[NEW].set_sensitive(!gmm.gs.motionInfo.getMotion().isFrame(clickCursorTime));

  /// adjust time, 
  /// time: clickCursorTime, 
  /// enabled: more than one frame in the motion
  ss.str("");
  ss << "_Adjust time here";
  frameMenu.items()[ADJUST].set_label(ss.str());
  frameMenu.items()[ADJUST].set_sensitive(gmm.gs.motionInfo.getMotion().getSize()>1);

  /// delete frame, 
  /// time: clickFrameTime, 
  /// enabled: clickFrameTime != 0 && isKeyFrame
  ss.str("");
  ss << "_Delete frame " << clickFrameTime; 
  frameMenu.items()[DEL].set_label(ss.str());
  frameMenu.items()[DEL].set_sensitive(clickFrameTime!=0 && isFrame);

  /// copy frame, 
  /// time: clickFrameTime, 
  /// enabled: isKeyFrame
  ss.str("");
  ss << "_Copy frame " << clickFrameTime; 
  frameMenu.items()[COPYFRAME].set_label(ss.str());
  frameMenu.items()[COPYFRAME].set_sensitive(isFrame);

  /// copy frame, 
  /// time: clickFrameTime, 
  /// enabled: isKeyFrame
  ss.str("");
  ss << "_Copy frame " << clickFrameTime << " to ..."; 
  frameMenu.items()[COPYFRAMETO].set_label(ss.str());
  frameMenu.items()[COPYFRAMETO].set_sensitive(isFrame);

  /// paste frame
  /// time : clickCursorTime
  /// enabled : by connection
  ss.str("");
  ss << "_Paste to " << (clickCursorTime);
  frameMenu.items()[PASTE].set_label(ss.str());
  frameMenu.items()[PASTE].set_sensitive(gmm.copying);

  /// paste frame
  /// time : clickFrameTime
  /// enabled : by connection
  ss.str("");
  ss << "_Paste to frame " << (clickFrameTime);
  frameMenu.items()[PASTEFRAME].set_label(ss.str());
  frameMenu.items()[PASTEFRAME].set_sensitive(gmm.copying);

  /// link frame
  /// time : clickFrameTime
  /// enabled : isKeyFrame
  ss.str("");
  ss << "_Link frame " << clickFrameTime << " to...";
  frameMenu.items()[LINK].set_label(ss.str());
  frameMenu.items()[LINK].set_sensitive(isFrame);

  /// unlink frame
  /// time : clickFrameTime
  /// enabled : isLinked(frame)
  ss.str("");
  ss << "_Unlink frame " << clickFrameTime; 
  frameMenu.items()[UNLINK].set_label(ss.str());
  frameMenu.items()[UNLINK].set_sensitive(gmm.isLinked(clickFrameTime));

  /// rename frame
  /// time : clickFrameTime
  /// enabled : isKeyFrame
  ss.str("");
  ss << "_Rename frame " << clickFrameTime; 
  frameMenu.items()[RENAME].set_label(ss.str());
  frameMenu.items()[RENAME].set_sensitive(isFrame);

  /// fix frame
  /// time : clickFrameTime
  /// enabled : coldet tick
  ss.str("");
  ss << "_Fix frame " << clickFrameTime; 
  frameMenu.items()[FIX].set_sensitive(ticks[clickFrameTime]!=cfg::guide::TicksType::KEYFRAMETICK);

  frameMenu.popup(event->button, event->time);
}

helium::Time TicksArea::getCursorTime(int xpos) {
  if(ticks.rbegin()->first == 0) return 0;

  return (double)xpos/(double)get_width()*(double)ticks.rbegin()->first;
}

helium::Time TicksArea::getFrameTime(int xpos) {
  if(ticks.rbegin()->first == 0) return 0;

  helium::Time ith = (double)(xpos-offset)/ ((double)(width-2*offset)/(double)ticks.rbegin()->first);
  std::map<helium::Time,cfg::guide::TicksType::Type>::iterator itup = ticks.lower_bound(ith);
  std::map<helium::Time,cfg::guide::TicksType::Type>::iterator itlow = ticks.lower_bound(ith);
  itlow--;

  if(round((double)(ith-itlow->first)/(double)(itup->first-itlow->first)))
    return itup->first;

  return itlow->first;
}

bool TicksArea::onTicksTooltip(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
  //get time
  std::stringstream ss;
  helium::Time t = getFrameTime(x);
  ss << t ;

  //set posture
  helium::Posture posture(gmm.gs.joints.size());
  gmm.getFramePosture(posture.data,t);
  tooltipWindow.setPosture(posture.data);
  //set color
  if(ticks[t] == cfg::guide::TicksType::KEYFRAMETICK)
    tooltipWindow.setBG(gmm.gs.settings.colors.defaultModelBG.data);
  else {
    tooltipWindow.setBG(gmm.gs.settings.colors.reds[0].data);
    ss <<"\n <span background=\"#B24C4C\"> Collision </span>";/// \todo change rgb converter
  }

  //set name
  if(ticks[t] == cfg::guide::TicksType::KEYFRAMETICK || ticks[t] == cfg::guide::TicksType::COLDETFRAMETICK) {
    string name = gmm.gs.motionInfo.getMotion()[t].name;
    if(name.size())
      ss << "\n" << name;
  }

  //get link
  std::set<helium::Time> st;
  if(gmm.getLink(t,st)) {
    ss << "\n" << "<i>Linked to:</i>";
    for(set<helium::Time>::iterator it=st.begin();it!=st.end();it++)
      if(*it != t)
	ss << " " << *it;
  }

  Glib::ustring tip = "<b><i>Frame</i> "+ ss.str()+"</b>";
  tooltipWindow.show(tip);

  return true; //event continues if false -- doesn't work
}

void TicksArea::zoomTicks(bool in) {  
  //return if there is only 1 frame
  if(gmm.gs.motionInfo.getMotion().getSize() < 2) return;

  if(in)
    zoom += offset;
  else {
    zoom -= offset;
  }
  zoom = std::max(0,zoom);
  queue_draw();
}

void TicksArea::drawFrameTick(Cairo::RefPtr<Cairo::Context> &cr, helium::Time time, bool red=false) {
  cr->save();
  cr->set_source_rgb((!red?0.3:0.8),0.3,(!red?0.8:0.3));
  cr->arc(offset+(ticks.rbegin()->first?((time)*(width-offset*2)/(ticks.rbegin()->first)):0), height/2, height/2-cr->get_line_width(), 0.0, 2.0 * M_PI); // full circle
  cr->fill_preserve();
  cr->restore();  // back to opaque black
  cr->stroke();
  drawFrameTime(cr,time,red);
}

void TicksArea::drawColdetTick(Cairo::RefPtr<Cairo::Context> &cr, helium::Time time) {
  cr->save();
  cr->set_font_size(14);
  cr->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  Cairo::TextExtents txext;
  Glib::ustring text;
  text = "!";
  cr->get_text_extents( text, txext );
  cr->move_to(offset+(ticks.rbegin()->first?((time)*(width-offset*2)/(ticks.rbegin()->first)):0), height/2+txext.height/2); // full circle
  cr->set_source_rgb(0, 0, 0);
  cr->text_path(text);
  cr->fill_preserve();
  cr->set_source_rgb(0.8,0.3,0.3);
  cr->stroke();
  cr->show_text(text);
  cr->stroke();
  cr->restore();
}

void TicksArea::drawFrameTime(Cairo::RefPtr<Cairo::Context> &cr, helium::Time time, bool red=false) {
  char buffer[256];
  Cairo::TextExtents te;
  sprintf(buffer,"%lld",(time));
  cr->save();
  cr->set_source_rgb(0,0,1);
  cr->set_font_size(8);
  if(red) {
    cr->set_font_size(10);
    cr->set_source_rgb(1,0,0);
    cr->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  }
  cr->get_text_extents(buffer,te);
  cr->move_to(offset+(ticks.rbegin()->first?((time)*(width-offset*2)/(ticks.rbegin()->first)):0) + (((time)!=ticks.rbegin()->first || ticks.begin()->first==ticks.rbegin()->first)?height/2:(-te.width-height/2)),+height/2+te.height/2);
  cr->show_text(buffer);
  cr->stroke();
  cr->restore();
}

bool TicksArea::on_expose_event(GdkEventExpose* event) {  
  ticksMutex.lock();
  Gtk::Allocation allocation = get_allocation();
  Cairo::RefPtr<Cairo::Context> cr = get_window()->create_cairo_context();
  cr->set_line_width(1.0);
  width = allocation.get_width();
  height = allocation.get_height();

  double defwidth = scrolledWindow.get_hadjustment()->get_page_size(); //fit to window width
  double minwidth = 0;

  //calculate the appropriate distance between ticks
  if(ticks.size() > 1) {
    //check minimum time difference between ticks
    helium::Time min = ticks.rbegin()->first;
    std::map<helium::Time,cfg::guide::TicksType::Type>::iterator it=ticks.begin();
    it++;
    for(std::map<helium::Time,cfg::guide::TicksType::Type>::iterator itb=ticks.begin();it!=ticks.end();it++,itb++){
      if(it->first - itb->first < min)
	min = it->first - itb->first;
    }
    minwidth = (double)ticks.rbegin()->first/(double)min * (double)(offset + zoom);
  }

  minwidth = std::max(minwidth,defwidth);
  set_size_request(minwidth,height);

  //show linked frames
  set<helium::Time> st;
  gmm.getLink(gmm.gs.motionInfo.curTime.get(),st);
  set<helium::Time>::iterator sti = st.begin();

  for(std::map<helium::Time,cfg::guide::TicksType::Type>::iterator it=ticks.begin();it!=ticks.end();it++){
    bool red = gmm.gs.motionInfo.curTime.get() == it->first;
    if(sti != st.end() && *sti == it->first) { //checked *sti before st.end()
      red = true;
      sti++;
    }
    switch(it->second) {
    case cfg::guide::TicksType::KEYFRAMETICK: 
      drawFrameTick(cr,it->first,red);
      break;
    case cfg::guide::TicksType::COLDETTICK: 
      drawColdetTick(cr,it->first);
      break;
    case cfg::guide::TicksType::COLDETFRAMETICK:
      drawColdetTick(cr,it->first);
      drawFrameTime(cr,it->first,red);
    }
  }
  ticksMutex.unlock();

  return false; //event continues
}

void TicksArea::refresh() {
  Gtk::Allocation allocation = get_allocation();
  Gdk::Rectangle r(0, 0, allocation.get_width(),allocation.get_height());
  Glib::RefPtr<Gdk::Window> window = get_window();
  if (window) {
    window->invalidate_rect(r, false);
  }
}

bool TicksArea::onKeyPress(GdkEventKey* event) {
  if(event->keyval == GDK_BackSpace || event->keyval == GDK_Delete) {
    helium::Time t = gmm.gs.motionInfo.curTime.get();
    if(t == 0) {
      throw helium::exc::InvalidOperation(" Cannot delete frame 0");
    }
    else if(!gmm.gs.motionInfo.getMotion().isFrame(t)) {
      throw helium::exc::InvalidOperation(" Cannot delete non-frame " + helium::toString(t));
    }      
    else {
      deleteFrame();
      return true;
    }
  }
  return false;
}

TimeSlider::TimeSlider(helium::ValueHub<helium::Time> &pcurTime, helium::GtkExceptionHandling &peh, Gtk::ScrolledWindow &pscrolledWindow, helium::ValueHub<bool> &editing):  
  curTime(pcurTime),
  eh(peh),
  scrolledWindow(pscrolledWindow),
  callSetValue(this),
  callOnEditing(this),
  callSetCurrPlayFrame(this)
{
  set_increments(1,1); //need for scroll, (arrow,pg up/down)
  set_digits(0);

  signal_value_changed().connect(eh.wrap(*this,&TimeSlider::onSetValue));
  signal_value_changed().connect(eh.wrap(*this,&TimeSlider::setScroll));
  editing.connect(callOnEditing);

  curTime.connect(callSetValue);
}

void TimeSlider::setScroll() {
  Gtk::Adjustment* adj = scrolledWindow.get_hadjustment();
  double currentCursorPosition = get_value()/get_adjustment()->get_upper()*get_width();
  double cursorOffset = 20;

  //if the cursor goes over the page view
  if(adj->get_value()+adj->get_page_size() < currentCursorPosition+cursorOffset)
    adj->set_value(std::min(get_width()-adj->get_page_size(),currentCursorPosition+cursorOffset-adj->get_page_size()));
  //if the cursor goes less than the page view
  else if(adj->get_value() > currentCursorPosition-cursorOffset)
    adj->set_value(std::max(0.0,currentCursorPosition-cursorOffset));
}

void TimeSlider::onSetValue() {
  curTime.assure(get_value());
}

void TimeSlider::setValue(helium::Time time) {  
  set_value(time);
}

void TimeSlider::setRange(helium::Time endTime) {
  get_adjustment()->set_upper((double)endTime);
}

void TimeSlider::setRange(const MotionInfo &mi) {
  setRange(mi.getMotion().getMotion().rbegin()->first);
}

MotionLine::MotionLine(GuideMotionManagement &pgmm, Gtk::Window &pguideWindow,RobotModels::ModelArea *pma, coldet::ColdetManager *pcoldetm):
  gmm(pgmm),
  guideWindow(pguideWindow),
  coldetm(pcoldetm),

  ticksArea(guideWindow,gmm,ticks,scrolledWindow,pma,coldetm),
  timeSlider(gmm.gs.motionInfo.curTime,gmm.eh,scrolledWindow,gmm.gs.motionInfo.editing.status),

  mTable(3,2,false),
  speedEntry(gmm.gs.motionInfo.mp.playSpeed,gmm.eh,gmm.gs.settings.colors.entryDefault,gmm.gs.settings.colors.entryInvalid),
  repeatEntry(gmm.gs.motionInfo.mp.iters,gmm.eh,gmm.gs.settings.colors.entryDefault,gmm.gs.settings.colors.entryInvalid),
  frameEntry(gmm,speedEntry.entry,repeatEntry.entry),

  isPlayFrame(false),
  isPlayZeroButtonClicked(false),

  playButton(gmm.eh,gmm.gs.conn,isPlayFrame,gmm.gs.motionInfo.editing.status,gmm.gs.settings.colors.cursor[cfg::guide::CursorType::ROBOT],gmm.gs.settings.colors.cursor[cfg::guide::CursorType::FRAME]),
  playZeroButton(gmm.eh,gmm.gs.conn,isPlayFrame,gmm.gs.motionInfo.editing.status,gmm.gs.settings.colors.cursor[cfg::guide::CursorType::ROBOT],gmm.gs.settings.colors.cursor[cfg::guide::CursorType::FRAME]),

  callOnEditing(this),
  callOnPlaySignal(this),
  callSetRange(&timeSlider,gmm.gs.motionInfo)
{
  //connects
  gmm.gs.motionInfo.editing.status.connect(callOnEditing);
  gmm.gs.motionInfo.framePlayState.connect(callOnPlaySignal);  
  gmm.gs.motionInfo.motionChange.connect(callSetRange);

  scrolledWindow.set_flags(Gtk::CAN_FOCUS);
  scrolledWindow.signal_button_press_event().connect(gmm.eh.wrap(*this,&MotionLine::onScrolledWindowMouseClick));

  //events
  set_events(Gdk::BUTTON_PRESS_MASK);

  //widget attitude
  scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_NEVER);

  //label
  speedLabel.set_markup("<small>speed</small>");
  repeatLabel.set_markup("<small>repeat</small>");
  speedLabel.set_alignment(0.5,0.5);
  repeatLabel.set_alignment(0.5,0.5);

  //tooltips
  zeroFrameButton.set_tooltip_markup("Go to frame 0");
  prevFrameButton.set_tooltip_markup("Go to prev frame");
  nextFrameButton.set_tooltip_markup("Go to next frame");
  zoomInSliderButton.set_tooltip_markup("Zoom in frame timeline");
  zoomOutSliderButton.set_tooltip_markup("Zoom out frame timeline");

  //control buttons
  Gtk::Widget *prevS, *nextS, *zoomInS, *zoomOutS;
  prevS = Gtk::manage(new Gtk::Image (Gtk::Stock::MEDIA_PREVIOUS, Gtk::ICON_SIZE_SMALL_TOOLBAR)); 
  prevS->show(); 
  nextS = Gtk::manage(new Gtk::Image (Gtk::Stock::MEDIA_NEXT, Gtk::ICON_SIZE_SMALL_TOOLBAR)); 
  nextS->show(); 
  prevFrameButton.add(*prevS); 
  nextFrameButton.add(*nextS); 
  zeroFrameButton.set_label("0");

  //zoom buttons
  zoomInS = Gtk::manage (new Gtk::Image (Gtk::Stock::ZOOM_IN, Gtk::ICON_SIZE_SMALL_TOOLBAR)); 
  zoomInS->show(); 
  zoomOutS = Gtk::manage (new Gtk::Image (Gtk::Stock::ZOOM_OUT, Gtk::ICON_SIZE_SMALL_TOOLBAR)); 
  zoomOutS->show();
  zoomInSliderButton.add (*zoomInS);  
  zoomOutSliderButton.add (*zoomOutS); 

  //button signals
  zeroFrameButton.signal_clicked().connect(gmm.eh.wrap(*this,&MotionLine::moveFrame,0));
  nextFrameButton.signal_clicked().connect(gmm.eh.wrap(*this,&MotionLine::moveFrame,1));
  prevFrameButton.signal_clicked().connect(gmm.eh.wrap(*this,&MotionLine::moveFrame,-1));
  zoomInSliderButton.signal_clicked().connect(gmm.eh.wrap(ticksArea,&TicksArea::zoomTicks,true));
  zoomOutSliderButton.signal_clicked().connect(gmm.eh.wrap(ticksArea,&TicksArea::zoomTicks,false));

  playZeroButton.button.signal_clicked().connect(gmm.eh.wrap(*this,&MotionLine::onPlayFrameClicked,true));
  playButton.button.signal_clicked().connect(gmm.eh.wrap(*this,&MotionLine::onPlayFrameClicked,false));

  //pack widget
  timeBox.pack_start(timeSlider,Gtk::PACK_SHRINK);
  timeBox.pack_start(ticksArea,Gtk::PACK_SHRINK); 
  scrolledWindow.add(timeBox);

  frameBox1.pack_start(zeroFrameButton,Gtk::PACK_EXPAND_WIDGET);  
  frameBox1.pack_start(prevFrameButton,Gtk::PACK_SHRINK);  
  frameBox1.pack_start(nextFrameButton,Gtk::PACK_SHRINK);  
  frameBox2.pack_start(frameEntry,Gtk::PACK_EXPAND_WIDGET);  

  zoomBox.pack_start(zoomInSliderButton,Gtk::PACK_EXPAND_WIDGET);  
  zoomBox.pack_start(zoomOutSliderButton,Gtk::PACK_EXPAND_WIDGET);   

  controlBox.pack_start(frameBox1,Gtk::PACK_EXPAND_WIDGET); 
  controlBox.pack_start(frameBox2,Gtk::PACK_SHRINK); 

  leftControlBox.pack_start(controlBox,Gtk::PACK_SHRINK); 
  leftControlBox.pack_start(vSep,Gtk::PACK_SHRINK); 
  leftControlBox.pack_start(zoomBox,Gtk::PACK_SHRINK); 

  mTable.attach(playZeroButton, 0, 1, 0, 1);
  mTable.attach(playButton, 1, 2, 0, 1);
  mTable.attach(speedLabel, 0, 1, 1, 2);
  mTable.attach(repeatLabel, 1, 2, 1, 2);
  mTable.attach(speedEntry, 0, 1, 2, 3);
  mTable.attach(repeatEntry, 1, 2, 2, 3);
  rightControlBox.pack_start(mTable,Gtk::PACK_EXPAND_WIDGET);
 
  

  pack_start(leftControlBox,Gtk::PACK_SHRINK); 
  pack_start(scrolledWindow);
  pack_start(rightControlBox,Gtk::PACK_SHRINK);
  show_all_children();
}


void TicksArea::onEnableColdet(bool enable) {  
  if(enable) {
    coldetm->checkManager.notifications.connect(callSetColdetTicks);    
  }
  else {
    coldetm->checkManager.notifications.disconnect(callSetColdetTicks);    
  }
  setTicks();  
}

void MotionLine::moveFrame(int type) {
  helium::Time newTime = gmm.gs.motionInfo.curTime.get();
  switch(type) {
  case 0:
    newTime = 0;
    break;
  case 1:
    newTime = getNextTick(newTime);
    break;
  case -1:
    newTime = getPrevTick(newTime);
    break;
  }
  gmm.gs.motionInfo.curTime.assure(newTime);
}

/*! Confirm to continue when there is collision / the coldetManager is still checking
 */
void MotionLine::confirmColdet() {
  helium::Time hasColdet = coldetm->checkManager.getFirstCollisionTime();
  if(hasColdet!=-100) {
    std::stringstream ss;
    if(hasColdet == coldet::SegmentState::CHECKING)
      ss << "Still checking for collisions";
    else
      ss << "Collisions at " << hasColdet;
    ss << ", continue?";
    YesNoDialog d("Warning",ss.str());
    d.set_transient_for(guideWindow);
    if(!d.run()) { 
      throw helium::exc::InvalidOperation("Abort due to Collision Detection");
    }
  }
}

/*! Confirm if the play speed is valid */
void MotionLine::confirmPlaySpeed() {
  //warn if play speed is invalid
  if(gmm.gs.motionInfo.mp.playSpeed <= 0) {
    Gtk::MessageDialog dialog("Please fix play speed to > 0");
    dialog.set_transient_for(guideWindow);
    dialog.run();
    throw helium::exc::InvalidDataValue("Abort due to Play Speed value");
  }
}

/*! Confirm if the play repeat is valid */
void MotionLine::confirmPlayRepeat() {
  //warn if play speed is invalid
  if(gmm.gs.motionInfo.mp.iters < 1) {
    Gtk::MessageDialog dialog("Please fix play repeat to >= 1");
    dialog.set_transient_for(guideWindow);
    dialog.run();
    throw helium::exc::InvalidDataValue("Abort due to Play Repeat value");
  }  
}

/*! Confirm if the start and end frame is valid */
void MotionLine::confirmSameStartEndFrame() {
  //fix start and end frame when repeat is > 1 for smooth looping play
  if(gmm.gs.motionInfo.mp.iters > 1) {
    helium::Time t = gmm.gs.motionInfo.getMotion().getEndTime();
    //if start and end frame is different, ask user to agree linking starting frame to end of motion

    //std::cout<<"@0 "<<gmm.gs.motionInfo.getMotion().getPosture(0)<<std::endl;
    //std::cout<<"@t "<<gmm.gs.motionInfo.getMotion().getPosture(t)<<std::endl;

    if(helium::cmpn(gmm.gs.motionInfo.getMotion().getPosture(0),gmm.gs.motionInfo.getMotion().getPosture(t),gmm.gs.joints.size()) != 0) {
      string time = "";
      //InputDialog inputDialog("Link start frame to",time,&gmm.eh,"To repeat a motion more than once,\n you may create a frame at the end of the motion linked to the first frame.\n specify the time:");
      SameStartEndFrameDialog inputDialog(time,&gmm.eh);
      inputDialog.set_transient_for(guideWindow);
      inputDialog.setEntry(helium::toString(t));
      int ans = inputDialog.run();
      helium::Time to;
      
      cout << "answer is " << ans << endl;
      //if link      
      if(ans==1) {
	//convert to number
	helium::convert(to,time," Frame time must be a number");
	//should be >= last frame
	if(to>=t)
	  gmm.linkFrame(0,to);
	else
	  throw helium::exc::InvalidDataValue(" Frame time", "must be >= " + helium::toString(t));
      }
      else if(ans==0) {
	throw helium::exc::InvalidDataValue("Frame 0 ","has to be the same as the last frame when looping.");
      }
      else {
	
      }
    }
  }//play repeat
}

void MotionLine::onPlayFrameClicked(bool zero) {
  try {
    //check if playing and there's collision when play button is clicked, warn user
    if(playZeroButton.button.get_active() || playButton.button.get_active()) {
      if(gmm.gs.pref.hasColdet)
	confirmColdet();
      confirmPlaySpeed();
      confirmPlayRepeat();
      confirmSameStartEndFrame();	
    }//if play
    cout << "continuing playing" << endl;

    //get if play/stop is clicked
    bool play = zero?playZeroButton.button.get_active():playButton.button.get_active(); 
    gmm.gs.motionInfo.mp.startTime = zero?0:gmm.gs.motionInfo.curTime.get();
    gmm.playMotion(!isPlayFrame.get(),play,zero,gmm.gs.motionInfo.mp.playSpeed,gmm.gs.motionInfo.mp.iters);
  }
  catch(helium::exc::Exception &e) {
    cout << "catching exception" << endl;
    disableClicksOnPlay(false); // turn off the already clicked/disabled buttons
    e.rethrow();
  }
}

///change play icon to stop and vice versa
void MotionLine::disableClicksOnPlay(bool play) { 
  //play
  if(play) {
    if(playButton.button.get_active()) {
      playZeroButton.button.set_sensitive(false);
    }
    else {
      playButton.button.set_sensitive(false);
    }
  }
  //stop
  else {
    playButton.button.set_active(false);
    playZeroButton.button.set_active(false);
    playButton.button.set_sensitive(true);
    playZeroButton.button.set_sensitive(true);
  }

  disableClicks(play);
  timeSlider.disableClicks(play);
  ticksArea.disableClicks(play);
  frameEntry.disableClicks(play);
}

void MotionLine::onPlaySignal(std::pair<helium::framePlay::PlayState,helium::Time> state) {
  bool b = (state.first == helium::framePlay::PLAYING);
  //when there's a signal from the robot that it's playing/stopping a motion
  //update current play frame / disconnect
  if(b){
    assureConnection(gmm.gs.motionInfo.framePlayState,timeSlider.callSetCurrPlayFrame); 
    cerr << "DEBUG: assureconnection motionlineonplaysignal" << endl;
  }else{
    assureDisconnection(gmm.gs.motionInfo.framePlayState,timeSlider.callSetCurrPlayFrame);         
    cerr << "DEBUG: assureconnection motionlineonplaysignal" << endl;
    timeSlider.setValue(state.second);
  }
  disableClicksOnPlay(b);
}

bool MotionLine::onKeyPress(GdkEventKey* event) {
  if(!(frameEntry.is_focus() || speedEntry.entry.is_focus() || repeatEntry.entry.is_focus())) {
    if(event->keyval == GDK_BackSpace || event->keyval == GDK_Delete) {
      return ticksArea.onKeyPress(event);
    }
  }
  return false;
}

helium::Time MotionLine::getNextTick(helium::Time time) {
  if(time == ticks.rbegin()->first) return time;
  return ticks.upper_bound(time)->first;
}

helium::Time MotionLine::getPrevTick(helium::Time time) {
  if(time == 0) return 0;
  std::map<helium::Time,cfg::guide::TicksType::Type>::iterator lower = ticks.lower_bound(time);
  lower--;
  return lower->first;
}

void TicksArea::setTicks() {
  ticksMutex.lock();

  ticks.clear(); //always clear 

  const helium::Motion::MotionMap &motMap = gmm.gs.motionInfo.getMotion().getMotion();
  for(helium::Motion::MotionMap::const_iterator it=motMap.begin();it!=motMap.end();++it) {
    ticks[it->first] = cfg::guide::TicksType::KEYFRAMETICK;
  }  

  ticksMutex.unlock();
  queue_draw();   
}

void TicksArea::setColdetTicks(coldet::SegmentNotification sm) {
  ticksMutex.lock();

  for(size_t i=0;i<sm.size();i++) {    
    if(sm[i].start > ticks.rbegin()->first) continue;
    if(sm[i].coll<0) {
      ticks[sm[i].start] = cfg::guide::TicksType::KEYFRAMETICK;
    }
    else if(gmm.gs.motionInfo.getMotion().isFrame(sm[i].coll))  //set for colliding frame
      ticks[sm[i].coll] = cfg::guide::TicksType::COLDETFRAMETICK;
    else //set for colliding non frame
      ticks[sm[i].coll] = cfg::guide::TicksType::COLDETTICK;
  }

  ticksMutex.unlock();
  queue_draw();                     //make sure ticks are queued to draw
}

bool MotionLine::onScrolledWindowMouseClick (GdkEventButton* event) {
  scrolledWindow.grab_focus();
  return false;
}

void MotionLine::disableClicks(bool disable) {
  prevFrameButton.set_sensitive(!disable);
  nextFrameButton.set_sensitive(!disable);
  zeroFrameButton.set_sensitive(!disable);
  zoomInSliderButton.set_sensitive(!disable);
  zoomOutSliderButton.set_sensitive(!disable);
  repeatEntry.entry.set_sensitive(!disable);
  speedEntry.entry.set_sensitive(!disable);  
}

void TimeSlider::disableClicks(bool disable) {
  set_sensitive(!disable);
}
void TicksArea::disableClicks(bool disable) {
  set_sensitive(!disable);
}
void FrameEntry::disableClicks(bool disable) {
  set_sensitive(!disable);
}

FrameEntry::FrameEntry(GuideMotionManagement &pgmm, Gtk::Entry &pspeedEntry, Gtk::Entry &prepeatEntry):
  gmm(pgmm),
  repeatEntry(prepeatEntry),
  speedEntry(pspeedEntry),
  entryStatus(FrameEntry::FrameEntryStatus::ESHELP),
  callOnEditing(this)
{
  set_width_chars(9);
  set_flags(Gtk::CAN_FOCUS);
  set_tooltip_markup("<b>1234:</b> insert frame at time 1234\n<b>+100:</b> set the next frame a 100ms further \n<b>-100:</b> set the next frame a 100ms closer\n<b>2x:</b> repeat the motion 2 times\n<b>x2:</b> play with 2 times faster speed");

  //set default frame entry text
  Gdk::Color color;
  color.set_rgb_p(gmm.gs.settings.colors.entryInvalid[0],gmm.gs.settings.colors.entryInvalid[1],gmm.gs.settings.colors.entryInvalid[2]);
  set_text(FrameEntry::FrameEntryStatus::strings[entryStatus]);
  modify_text(Gtk::STATE_NORMAL, color);

  gmm.gs.motionInfo.editing.status.connect(callOnEditing);
  signal_key_press_event().connect(gmm.eh.wrap(*this,&FrameEntry::onFrameEntryKey),false); //called before the default gtkmm handler
  signal_activate().connect(gmm.eh.wrap(*this,&FrameEntry::onFrameEntryActivate));
  signal_focus_in_event().connect(gmm.eh.wrap(*this,&FrameEntry::onFrameEntryFocusIn));
  signal_focus_out_event().connect(gmm.eh.wrap(*this,&FrameEntry::onFrameEntryFocusOut));
}

void FrameEntry::onFrameEntryActivate() {
  std::string entry = get_text();
  if(entry.size() < 1) return; //return if it has nothing
  GuideUtil::trim(entry);
  entryStatus = FrameEntry::FrameEntryStatus::ESHELP;

  try{
    //speed
    if(entry[0] == 'x') {
      entry.erase(0,1);
      speedEntry.set_text(entry);
      entryStatus = FrameEntry::FrameEntryStatus::ESSPEED; 
    }
    //repeat
    else if(entry[entry.size()-1] == 'x') {
      entry.erase(entry.size()-1,1);
      repeatEntry.set_text(entry);
      entryStatus = FrameEntry::FrameEntryStatus::ESREPEAT; 
    }
    //adjust frame distance
    else if(entry[0] == '+' || entry[0] == '-') {
      helium::Time t = 0;
      helium::convert(t,entry," Invalid time");
      gmm.adjustTime(gmm.gs.motionInfo.curTime.get(),t);
      entryStatus = FrameEntry::FrameEntryStatus::ESADJUST;
    }
    //new frame
    else { 
      helium::Time t = 0;
      helium::convert(t,entry," Invalid frame time");
      gmm.insertFrame(t);
      entryStatus = FrameEntry::FrameEntryStatus::ESNEWFRAME;
    } 
  }
  catch(helium::exc::Exception &e) {
    entryStatus = FrameEntry::FrameEntryStatus::ESINVALID;

    Gdk::Color color;
    color.set_rgb_p(gmm.gs.settings.colors.entryInvalid[0],gmm.gs.settings.colors.entryInvalid[1],gmm.gs.settings.colors.entryInvalid[2]);
    set_text(FrameEntry::FrameEntryStatus::strings[entryStatus]);
    modify_text(Gtk::STATE_NORMAL, color);

    e.rethrow();
  }
  Gdk::Color color;
  color.set_rgb_p(gmm.gs.settings.colors.entryInvalid[0],gmm.gs.settings.colors.entryInvalid[1],gmm.gs.settings.colors.entryInvalid[2]);
  set_text(FrameEntry::FrameEntryStatus::strings[entryStatus]);
  modify_text(Gtk::STATE_NORMAL, color);
}



bool FrameEntry::onFrameEntryFocusOut(GdkEventFocus* event) {
  std::string entry = get_text();
  if(entryStatus != FrameEntry::FrameEntryStatus::ESEDITING || entry == "") {
    entryStatus = FrameEntry::FrameEntryStatus::ESHELP;
    std::stringstream ss;
    ss << "<span color=\"red\">" << FrameEntry::FrameEntryStatus::strings[entryStatus] <<  "</span>";
    get_layout()->set_markup(ss.str());
  }
  return false;
}

bool FrameEntry::onFrameEntryFocusIn(GdkEventFocus* event) {
  deleteHelpText();
  return false;
}

void FrameEntry::deleteHelpText() {
  Gdk::Color color;
  color.set_rgb_p(gmm.gs.settings.colors.entryDefault[0],gmm.gs.settings.colors.entryDefault[1],gmm.gs.settings.colors.entryDefault[2]);
  modify_text(Gtk::STATE_NORMAL, color);

  if(entryStatus != FrameEntry::FrameEntryStatus::ESEDITING) {
    set_text("");
    entryStatus = FrameEntry::FrameEntryStatus::ESEDITING;
  }    
}

bool FrameEntry::onFrameEntryKey(GdkEventKey* event)  {
  deleteHelpText();
  return false;
}

RepeatEntry::RepeatEntry(int& value, helium::GtkExceptionHandling &eh, const cfg::guide::Color &valid, const cfg::guide::Color &invalid):
  ValidatingEntry<int>(value,eh,valid,invalid) 
{
  set_tooltip_markup("Number of times the motion will be repeated");
}
SpeedEntry::SpeedEntry(double& value, helium::GtkExceptionHandling &eh, const cfg::guide::Color &valid, const cfg::guide::Color &invalid):
  ValidatingEntry<double>(value,eh,valid,invalid) 
{  
  set_tooltip_markup("Speed of played motion");
}
void RepeatEntry::setValue(std::string s) {  
  double repeat;  
  if(s.length()<1) {
    throw helium::exc::InvalidDataValue(helium::toString("Play repeat"), "Cannot be empty");
  }
  GuideUtil::trim(s);
  helium::convert(repeat,s,"Only numbers are accepted");  
  if(repeat<1) {
    throw helium::exc::InvalidDataValue(helium::toString("Play repeat"), "Has to be >= 1");
  }
  else if(((double)repeat-(int)repeat) > (double)0.0) {
    throw helium::exc::InvalidDataValue(helium::toString("Play repeat"), "Has to be integer");
  }
  value = repeat;
}
void SpeedEntry::setValue(std::string s) {
  double speed;  
  if(s.length()<1) {
    throw helium::exc::InvalidDataValue(helium::toString("Play repeat"), "Cannot be empty");
  }
  GuideUtil::trim(s);
  helium::convert(speed,s,"Only numbers are accepted");  
  if(speed<=0) {
    throw helium::exc::InvalidDataValue(helium::toString("Play speed"), "Has to be > 0");
  }
  value = speed;
}
  
MotionLine::SameStartEndFrameDialog::SameStartEndFrameDialog(std::string &pstring,helium::GtkExceptionHandling *eh):
  InputDialog("Link start frame to",pstring,eh,"To repeat a motion more than once,\n you may create a frame at the end of the motion linked to the first frame.\n specify the time:")
{
  button1->set_label("Link Frame and Play");
  button0->set_label("Cancel Play");
  button2 = add_button("Play Anyway",2);
  show_all_children();
}
