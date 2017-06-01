/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "widgetDrawer.h"
#include <helium/util/mathUtil.h>
#include <helium/util/ioUtil.h>
#include "angleEntry.h"

#include <iomanip>
#include <helium/robot/motorInterface.h>

#include <cstdlib>
#include <ctime>

using namespace std;
using namespace gwidget;
using helium::convert;
using namespace  cfg::guide;

const char* JointElementType::strings[] = {"MOTORW","BMOTORW","ROBOTEW","FRAMEEW","BROBOTEW","BFRAMEEW","ROBOTCW","FRAMECW","POWERSW","ENABLESW","ROBOTCCW","MINCW","ZEROCW","MAXCW","MINEW","ZEROEW","MAXEW","NOTCLICKABLE"};

static const double MINTEMP = 20;
static const double MAXTEMP = 70;




GWidgetState::GWidgetState():
  visible(false),enable(true),warning(false),mouseState(NORMAL) //,warning(false)
{
}

WidgetGlobalProperty::WidgetGlobalProperty(cfg::guide::Settings &psettings, helium::ValueHub<double> &pscale):
  settings(psettings),
  scale(pscale)
{
}

WidgetProperty::WidgetProperty(const Point2D &panchor,int pwidth,int pheight/*, bool pcornerOrigin*/):
  anchor(panchor),
  width(pwidth),
  height(pheight),
  name("widget")
{
}

inline std::ostream& operator<<(std::ostream& o,const WidgetProperty& p){
  o<<"WP "<< p.name  << " : "<< p.width << " x " << p.height<<std::endl;
  return o;
}


GWidget::GWidget(const WidgetProperty& pprop, WidgetGlobalProperty& pglobalProp):
  prop(pprop),
  globalProp(pglobalProp),
  onMouseOverConn(this),
  onResizeConn(this)
{
  isMouseOver.setCache(false);
  isMouseOver.connect(onMouseOverConn);
  globalProp.scale.connect(onResizeConn);
}

void GWidget::setAnchor(int x, int y) {
  prop.anchor().x = x; prop.anchor().y = y; 
}
Point2D& GWidget::getAnchor() { 
  return prop.anchor(); 
}

double GWidget::getWidth() {
  return prop.width();
}

double GWidget::getHeight() {
  return prop.height();
}

bool GWidget::isCanvasResponsible(const Point2D &e) {
  if(!state.visible) return false;
  if(!state.enable) return false;
  return isResponsible(e);
}

GWidgetState GWidget::getState() {
  return state;
}

void GWidget::setVisible(bool v) {
  state.visible = v;
  if(v) {    
    onVisible();
    onCanvasResize(globalProp.scale.get());
  }
  else {
    onInvisible();
  }
  refresh();
}

void GWidget::setEnable(bool e) {
  state.enable = e;
  refresh();
}

void GWidget::setWarning(bool w) {
  state.warning = w;
  refresh();
}

void GWidget::setMouseState(GWidgetState::MouseState m) {
  state.mouseState = m;
  refresh();
}

void GWidget::refresh() {
  invalidate(getRefreshColor());      
}

void GWidget::canvasDraw() {
  if(state.visible) {
    draw();
  }
}

void GWidget::onCanvasResize(double scale) {
  if(state.visible) {
    onResize(scale);
    setSize(scale);
    setLocation(scale);
    refresh();
  }
}

bool GWidget::isResponsible(const Point2D &e) {
  bool ew = fabs(e.x - prop.anchor().x) < (prop.width()/2);
  bool eh = fabs(e.y - prop.anchor().y) < (prop.height()/2);
  return ew && eh;
}

void GWidget::invalidate(double *bgColor) {
  if (!getCtx().getwin()) return;

  Gdk::Rectangle r;
  int rx, ry, rw, rh;
  rx = (floor)(prop.anchor().x-prop.width()/2-globalProp.settings.sizes.stroke/2);
  ry = (floor)(prop.anchor().y-prop.height()/2-globalProp.settings.sizes.stroke/2);
  rw = (ceil)(prop.width()+globalProp.settings.sizes.stroke);
  rh = (ceil)(prop.height()+globalProp.settings.sizes.stroke);
  setCustomInvalidate(rx,ry,rw,rh);
  getCtx().getda()->queue_draw_area(rx,ry,rw,rh);
}

double* GWidget::getRefreshColor() {
  return globalProp.settings.colors.bgs[0].data;
}

void GWidget::setLocation(double scale) {
  prop.anchor() = prop.anchor.def;
  helium::mulScalar(prop.anchor(),scale);
}

void GWidget::setSize(double scale) {
  prop.width() = ceil(prop.width.def * scale);
  prop.height() = ceil(prop.height.def * scale);
}

bool GWidget::onCanvasClick(GdkEventButton * event){
  clickState = Clickable::State::NONE;
  if(isCanvasResponsible(Point2D(event->x,event->y))) {
    clickState = Clickable::State::CLICK;
    onClick(event);
    onCanvasDrag();
    return true;
  }
  return false;
}

void GWidget::onCanvasRelease(GdkEventButton * event){
  //release for previously clicked widget
  if(clickState == Clickable::State::CLICK) { //release the previously clicked widget, regardless position
    onRelease(event);
  }

  //release is on the widget 
  if(isCanvasResponsible(Point2D(event->x,event->y))) { 
    //clicked = click + release
    if(clickState == Clickable::State::CLICK) {
      clickState = Clickable::State::CLICKED;
      onClicked(event);
      return;
    }
    //drop = release only
    else {
      clickState = Clickable::State::RELEASE; 
      onRelease(event);
      onCanvasDrop();
      return;
    }
  }
  clickState = Clickable::State::NONE;
}

void GWidget::onCanvasMotion(GdkEventMotion * event) {
  if(isCanvasResponsible(Point2D(event->x,event->y))) {
    isMouseOver.assure(true);
  }
  else
    isMouseOver.assure(false);
  onMotion(event);
}

void GWidget::onCanvasDrag() {
  if(getDragType()) {
    globalProp.dragData.type = getDragType();
    onDrag();
  }
}

void GWidget::onCanvasDrop() { 
  if(getDragType() && getDragType() == globalProp.dragData.type) { // && clickState == GWidgetClickState::RELEASE) {
    onDrop(); 
    globalProp.dragData.type = DragData::NONE;
  }
  
}


GWidgets::GWidgets(const WidgetProperty& pprop, WidgetGlobalProperty& pglobalProp, int widgetsNumber):
  GWidget(pprop,pglobalProp),
  widgets(widgetsNumber)
{
}

void GWidgets::onClick(GdkEventButton * event) {
  for(int i=widgets.size()-1;i>=0;i--) {
    if(widgets[i]->onCanvasClick(event)) {
      return;
    }
  }
}
void GWidgets::onRelease(GdkEventButton * event){
  for(int i=widgets.size()-1;i>=0;i--) {
    widgets[i]->onCanvasRelease(event);
  }
}

void GWidgets::onMotion(GdkEventMotion * event){
  for(size_t i=0;i<widgets.size();i++)
    widgets[i]->onCanvasMotion(event);
}
void GWidgets::draw() {
  for(size_t i=0;i<widgets.size();i++)
    widgets[i]->canvasDraw();
}
void GWidgets::refresh() {
  for(size_t i=0;i<widgets.size();i++)
    widgets[i]->refresh();
}

//void GWidgets::setVisible(bool b) {
//  for(size_t i=0;i<widgets.size();i++)
//    widgets[i]->setVisible(b);
//}

bool GWidgets::isResponsible(const Point2D &p) {
  for(size_t i=0;i<widgets.size();i++)
    if(widgets[i]->isCanvasResponsible(p))
      return true;
  return false;
}

JointGlobalProperty::JointGlobalProperty(helium::GtkProxy<helium::Connection>& pconnection):
  connection(pconnection) {
  bigJointId = -1;
}


ConnectionDependent::ConnectionDependent(helium::GtkProxy<helium::Connection> &connection):
  onConnectionChangeConn(this) 
{
  connection.connect(onConnectionChangeConn);
}

void ConnectionDependent::onConnectionChange(helium::Connection connection) {
  //std::cout<<" ConnectionDependent::onConnectionChange"<<connection<<std::endl;
  if(connection.state == helium::Connection::CONNECTED)
    onConnected();
  else
    onDisconnected();
}

JointElement::JointElement(cfg::guide::Joint* pjointProp, JointGlobalProperty& pjointGlobalProp):
  jointProp(pjointProp),jointGlobalProp(pjointGlobalProp)
{
}

JointElementWidget::JointElementWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  GWidget(pprop,pglobalProp),
  JointElement(pjointProp,pjointGlobalProp),
  ConnectionDependent(pjointGlobalProp.connection)
{
}

JointElementWidgets::JointElementWidgets(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, int widgetsNumber):
  GWidgets(pprop,pglobalProp,widgetsNumber),
  JointElement(pjointProp,pjointGlobalProp),
  ConnectionDependent(pjointGlobalProp.connection)
{
}



//** Basic Entry


BasicEntry::BasicEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  JointElementWidget(pprop,pjointProp,pglobalProp,pjointGlobalProp),overLimit(false)
{  
}

Point2D BasicEntry::getOffset() {
  Point2D u;
  jointProp->getU(u);
  u.x *= (prop.width()/2 + globalProp.settings.sizes.stroke/2);
  u.y *= (prop.height()/2 + globalProp.settings.sizes.stroke/2);
  u.x = -abs(u.x);
  u.y = -abs(u.y);

  modifyOffset(u);

  return u;
}

void BasicEntry::onClick(GdkEventButton *event) {
  setMouseState(GWidgetState::MOUSEDOWN);
}
void BasicEntry::onRelease(GdkEventButton *event){
  setMouseState(GWidgetState::NORMAL);
}


//** end of Basic Entry



//** Entry Dest RobotAll
EntryTypeAdapter<EntryDestination::RobotAll>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp): 
  BasicEntry(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callOnPowerChange(this)
{
  this->prop.name = "entry robot all";
}

void EntryTypeAdapter<EntryDestination::RobotAll>::onPowerChange(bool power) {
  setEnable(jointGlobalProp.connection.get().state == helium::Connection::CONNECTED && power);
}

void EntryTypeAdapter<EntryDestination::RobotAll>::onConnected() {
  if(this->getState().visible)
    setEnable(jointProp->jointPower.get());
}

void EntryTypeAdapter<EntryDestination::RobotAll>::onDisconnected() {
  setEnable(false);
}

//** end of Entry Dest RobotAll





//** Entry Dest RobotOnly
EntryTypeAdapter<EntryDestination::RobotOnly>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp): 
  EntryTypeAdapter<EntryDestination::RobotAll>(pprop,pjointProp,pglobalProp,pjointGlobalProp)
{
}

double* EntryTypeAdapter<EntryDestination::RobotOnly>::getColor() {
  if(this->getState().warning)
    return this->globalProp.settings.colors.coldet.data;
  return globalProp.settings.colors.cursor[cfg::guide::CursorType::ROBOT].data;
}


void EntryTypeAdapter<EntryDestination::RobotOnly>::modifyOffset(Point2D &u) {  
  u.x = -u.x;
  u.y = -u.y;
}
//** end of Entry Dest RobotOnly










//** Entry Dest Robot
EntryTypeAdapter<EntryDestination::Robot>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp): 
  EntryTypeAdapter<EntryDestination::RobotOnly>(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callOnValueChange(this),
  callOnDisplayedValueChange(this)
{
}
void EntryTypeAdapter<EntryDestination::Robot>::onDisplayedValueChange(double val) {
  setWarning(val == helium::motor::Interface::COLLISIONPOS);
}

std::string EntryTypeAdapter<EntryDestination::Robot>::getValueString() {
  return helium::toString(getValue());
}

double EntryTypeAdapter<EntryDestination::Robot>::getValue() {
  double v = jointProp->desiredTarget.get();
  if(v == helium::motor::Interface::INVALIDPOS || v == helium::motor::Interface::FREEPOS || v == helium::motor::Interface::MOVINGPOS || v == helium::motor::Interface::COLLISIONPOS) return NAN;
  helium::radtoDeg(v);
  if(v<0) return ceil(v*100.0)/100.0;
  return floor(v*100.0)/100.0;
}

void EntryTypeAdapter<EntryDestination::Robot>::onVisible() {
  jointProp->desiredTarget.connect(callOnValueChange);
  jointProp->displayedTarget.connect(callOnDisplayedValueChange);
  jointProp->jointPower.connect(callOnPowerChange);
  onValueChange(jointProp->desiredTarget.get());
  onDisplayedValueChange(jointProp->displayedTarget.get());
  onPowerChange(jointProp->jointPower.get());
}

void EntryTypeAdapter<EntryDestination::Robot>::onInvisible() {
  jointProp->desiredTarget.disconnect(callOnValueChange);
  jointProp->displayedTarget.disconnect(callOnDisplayedValueChange);
  jointProp->jointPower.disconnect(callOnPowerChange);
}

/** \brief set state to overlimit / no
 */
void EntryTypeAdapter<EntryDestination::Robot>::onValueChange(double d) {
  //cout << "robot entry onvaluechange " << std::setprecision(10) << d << " is overlimit? " << jointProp->isOverLimit(d) << " limit is " << this->jointProp->getLimit(cfg::guide::CalibType::MIN) << " " <<this->jointProp->getLimit(cfg::guide::CalibType::MAX) <<endl;
  this->overLimit = jointProp->isOverLimit(d);
  refresh();
}

//** end of Entry Dest Robot



//** Entry Dest Frame
EntryTypeAdapter<EntryDestination::Frame>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  BasicEntry(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callOnValueChange(this),
  callOnEnableChange(this)
{
}

std::string EntryTypeAdapter<EntryDestination::Frame>::getValueString() {
  return helium::toString(getValue());
}

double EntryTypeAdapter<EntryDestination::Frame>::getValue() {
  double v = jointProp->frameValue.get().second;
  helium::radtoDeg(v);
  return round(v*100.0)/100.0;
}

void EntryTypeAdapter<EntryDestination::Frame>::modifyOffset(Point2D &u) {  
  u.x = 0;
  u.y = 0;
}

double* EntryTypeAdapter<EntryDestination::Frame>::getColor() {
  return globalProp.settings.colors.cursor[cfg::guide::CursorType::FRAME].data;
}

void EntryTypeAdapter<EntryDestination::Frame>::onVisible() {
  jointProp->frameValue.connect(callOnValueChange);
  jointProp->frameValue.connect(callOnEnableChange);
  onValueChange(jointProp->frameValue.get());
  onEnableChange(jointProp->frameValue.get());
}

void EntryTypeAdapter<EntryDestination::Frame>::onInvisible() {
  jointProp->frameValue.disconnect(callOnValueChange);
  jointProp->frameValue.disconnect(callOnEnableChange);
}

void EntryTypeAdapter<EntryDestination::Frame>::onValueChange(helium::FrameValue f) {
  this->overLimit = jointProp->isOverLimit(f.second);
  refresh();
}

void EntryTypeAdapter<EntryDestination::Frame>::onEnableChange(helium::FrameValue f) {
  setWarning(f.first != helium::VALUEFRAME);
}
//** end of Entry Dest Frame


EntryTypeAdapter<EntryDestination::PotAll>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  EntryTypeAdapter<EntryDestination::RobotAll>(pprop,pjointProp,pglobalProp,pjointGlobalProp)
{}
double* EntryTypeAdapter<EntryDestination::PotAll>::getColor() {
  return globalProp.settings.colors.cursor[cfg::guide::CursorType::POT].data;
}


//** Entry Dest Pot

EntryTypeAdapter<EntryDestination::Pot>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  EntryTypeAdapter<EntryDestination::PotAll>(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callOnValueChange(this)
{
}

std::string EntryTypeAdapter<EntryDestination::Pot>::getValueString() {
  double now = jointProp->displayedTarget.get();
  if(now == helium::motor::Interface::INVALIDPOS) return "inv"; 
  if(now == helium::motor::Interface::FREEPOS) return "free"; 
  if(now == helium::motor::Interface::MOVINGPOS) return "mov";

  if(now == helium::motor::Interface::COLLISIONPOS) return "col";

  return helium::toString(getValue());
}

double EntryTypeAdapter<EntryDestination::Pot>::getValue() {
  double now = jointProp->displayedTarget.get();
  if(now == helium::motor::Interface::INVALIDPOS || now == helium::motor::Interface::FREEPOS || now == helium::motor::Interface::MOVINGPOS || now == helium::motor::Interface::COLLISIONPOS) 
    return now;
  helium::radtoDeg(now);
  return round(now*100.0)/100.0;
}


void EntryTypeAdapter<EntryDestination::Pot>::onVisible() {
  jointProp->displayedTarget.connect(callOnValueChange);
  jointProp->jointPower.connect(callOnPowerChange);
  onValueChange(jointProp->displayedTarget.get());
  onPowerChange(jointProp->jointPower.get());
}

void EntryTypeAdapter<EntryDestination::Pot>::onInvisible() {
  jointProp->displayedTarget.disconnect(callOnValueChange);
  jointProp->jointPower.disconnect(callOnPowerChange);
}


void EntryTypeAdapter<EntryDestination::Pot>::onValueChange(double val) {
  this->overLimit = jointProp->isOverLimit(val);
  refresh();
}

//** end of Entry Dest Pot

//** Entry Dest CalibPot

EntryTypeAdapter<EntryDestination::CalibPot>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  EntryTypeAdapter<EntryDestination::PotAll>(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callOnValueChange(this)
{
}



std::string EntryTypeAdapter<EntryDestination::CalibPot>::getValueString() {
  if(getValue() == helium::AbsMotorSign::INVALIDPOS) return "inv"; 
  if(getValue() == helium::AbsMotorSign::FREEPOS) return "free"; 
  if(getValue() == helium::AbsMotorSign::MOVINGPOS) return "mov";
  stringstream ss;
  ss << std::hex << getValue();
  return ss.str();
}

int EntryTypeAdapter<EntryDestination::CalibPot>::getValue() {
  return jointProp->calibCurrentPosition.get();
}

//void EntryTypeAdapter<EntryDestination::CalibPot>::onValueChange(int val) {
//}
void EntryTypeAdapter<EntryDestination::CalibPot>::onVisible() {
  jointProp->calibCurrentPosition.connect(callOnValueChange);
  jointProp->jointPower.connect(callOnPowerChange);
  onPowerChange(jointProp->jointPower.get());
  //onValueChange(jointProp->calibCurrentPosition.get());
  refresh();
}

void EntryTypeAdapter<EntryDestination::CalibPot>::onInvisible() {
  jointProp->calibCurrentPosition.disconnect(callOnValueChange);
  jointProp->jointPower.disconnect(callOnPowerChange);
}

//** end of Entry Dest CalibPot




//** Entry Dest CalibRobot

EntryTypeAdapter<EntryDestination::CalibRobot>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  EntryTypeAdapter<EntryDestination::RobotOnly>(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callOnValueChange(this)
{
}

std::string EntryTypeAdapter<EntryDestination::CalibRobot>::getValueString() {
  //if(!jointProp) return "-1";
  stringstream ss;
  ss << std::hex << getValue();
  return ss.str();
}

int EntryTypeAdapter<EntryDestination::CalibRobot>::getValue() {
  return jointProp->absTarget.get();
}


void EntryTypeAdapter<EntryDestination::CalibRobot>::onVisible() {
  jointProp->absTarget.connect(callOnValueChange);
  jointProp->jointPower.connect(callOnPowerChange);
  onPowerChange(jointProp->jointPower.get());
}

void EntryTypeAdapter<EntryDestination::CalibRobot>::onInvisible() {
  jointProp->absTarget.disconnect(callOnValueChange);
  jointProp->jointPower.disconnect(callOnPowerChange);
}

//** end of Entry Dest CalibRobot


template<>
void FrameEntryWidget<false>::onRightClicked() {
  jointProp->toggleEnable(jointProp->frameValue.get().first==helium::INTERPOLATEFRAME?
			  helium::VALUEFRAME:
			  helium::INTERPOLATEFRAME);
}



EntryTypeAdapter<EntryDestination::CalibMin>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  CalibEntryTypeAdapter(pprop,pjointProp,pglobalProp,pjointGlobalProp) {
}
EntryTypeAdapter<EntryDestination::CalibZero>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  CalibEntryTypeAdapter(pprop,pjointProp,pglobalProp,pjointGlobalProp) {
}
EntryTypeAdapter<EntryDestination::CalibMax>::EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  CalibEntryTypeAdapter(pprop,pjointProp,pglobalProp,pjointGlobalProp) {
}



MotorWidget::MotorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  JointElementWidget(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callRefreshonPower(this),
  callRefreshonFrameValue(this),
  callRefreshonTemp(this),
  callRefreshonState(this),
  callRefreshonMonitor(this)
{
}

void MotorWidget::draw(){
  getCtx().getcr()->save();
  getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);

  //draw shape
  getCtx().getcr()->move_to(prop.width()/2,0);
  jointProp->getType()==helium::Rotation::ROLL?drawCircle():drawDiamond();

  double *rgb = NULL;

  //color  
  //mouse selected
  if(getState().mouseState == GWidgetState::MOUSEDOWN) {
    rgb=globalProp.settings.colors.selected.data;     
  }
  else {
    //disconnected
    if(jointGlobalProp.connection.get().state != helium::Connection::CONNECTED) {
      rgb=globalProp.settings.colors.disable.data;
    }
    else {
      //have temperature color
      if(jointProp->jointTemp.get()!=helium::motor::Interface::INVALIDPROP) {
	double r,g,b;
	r=g=b=0;//compiler warning
	double h=2.0/3* (1-min(max(0.0,(jointProp->getTemp()-MINTEMP)/(MAXTEMP-MINTEMP)),1.0));       //if has temp
	helium::hsv2rgb(r,g,b,h,0.6,1);
	getCtx().getcr()->set_source_rgb(r,g,b);
      }
      //no temperature color
      else 
	rgb=globalProp.settings.colors.nanTemp.data; 
    }
  }

  //if rgb not yet set
  if(rgb)
    getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);
  getCtx().getcr()->fill_preserve();

  //joint line
  if(jointProp->getType() != helium::Rotation::ROLL) {
    Point2D u;
    jointProp->getU(u);
    helium::mulScalar(u,prop.width()/2);
    getCtx().getcr()->move_to(u.x,u.y);
    getCtx().getcr()->line_to(u.x/2,u.y/2);
    getCtx().getcr()->move_to(-u.x,-u.y);
    getCtx().getcr()->line_to(-u.x/2,-u.y/2);
  }

  //dash
  std::vector<double> dashes;
  dashes.push_back(7);
  dashes.push_back(2);
  double offset = -50;
  getCtx().getcr()->set_source_rgb(0,0,0);

  if(jointProp->frameValue.get().first != helium::VALUEFRAME)//(!jointProp->getEnable()) {
    getCtx().getcr()->set_dash (dashes, offset);

  if(jointGlobalProp.connection.get().state == helium::Connection::CONNECTED) { //jointState.isConnected())
    if(!jointProp->getPower())
      getCtx().getcr()->set_source_rgb(globalProp.settings.colors.disable[0],globalProp.settings.colors.disable[1],globalProp.settings.colors.disable[2]);
      //getCtx().getcr()->set_source_rgb(r/2,g/2,b/2); //darken base color so that the line is viewable
  }
  getCtx().getcr()->stroke();
  getCtx().getcr()->restore(); 
  getCtx().getcr()->set_line_width(globalProp.settings.sizes.stroke);

  //draw cross when the motor is faulty when connected to robot
  if(helium::isFaulty(jointProp->jointState.get()) && jointGlobalProp.connection.get().state == helium::Connection::CONNECTED) //jointProp->jointState.isConnected())
    drawCross();

  drawAdditional();
}

void MotorWidget::drawDiamond(){
  getCtx().getcr()->move_to(0,-prop.width()/2+globalProp.settings.sizes.stroke/2);
  getCtx().getcr()->line_to(prop.width()/2-globalProp.settings.sizes.stroke/2,0);
  getCtx().getcr()->line_to(0,prop.width()/2-globalProp.settings.sizes.stroke/2);
  getCtx().getcr()->line_to(-prop.width()/2+globalProp.settings.sizes.stroke/2,0);
  getCtx().getcr()->close_path();
}

void MotorWidget::drawCircle(){
  getCtx().getcr()->arc(0, 0, prop.width()/2, 0.0, 2.0 * M_PI); // full circle
}

void MotorWidget::drawCross() {
  getCtx().getcr()->save();
  getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);
  getCtx().getcr()->move_to(-prop.width()/2+globalProp.settings.sizes.stroke,-prop.width()/2+globalProp.settings.sizes.stroke);
  getCtx().getcr()->line_to(prop.width()/2-globalProp.settings.sizes.stroke,prop.width()/2-globalProp.settings.sizes.stroke);
  getCtx().getcr()->move_to(prop.width()/2-globalProp.settings.sizes.stroke,-prop.width()/2+globalProp.settings.sizes.stroke);
  getCtx().getcr()->line_to(-prop.width()/2+globalProp.settings.sizes.stroke,prop.width()/2-globalProp.settings.sizes.stroke);

  if(jointProp->jointMonitor.isConnected()&&jointProp->jointMonitor.get() == helium::NOHWMON)//different color when the error is ignored
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.disable[0],globalProp.settings.colors.disable[1],globalProp.settings.colors.disable[2]);
  else 
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.powerState[0][0],globalProp.settings.colors.powerState[0][1],globalProp.settings.colors.powerState[0][2]);

  getCtx().getcr()->set_line_width(globalProp.settings.sizes.stroke*3);
  getCtx().getcr()->stroke();
  getCtx().getcr()->restore();
  getCtx().getcr()->set_line_width(globalProp.settings.sizes.stroke);
}

bool MotorWidget::isResponsible(const Point2D &e) {
  return infinityNormDistance(prop.anchor(),e) < (prop.width()/2);
}

void MotorWidget::onConnected() { refresh(); }
void MotorWidget::onDisconnected() { refresh(); }

void BigMotorWidget::setSize(double scale) {
  prop.width() = prop.width.def * globalProp.scale.get();
  prop.height() = prop.height.def * globalProp.scale.get();
  if(jointProp->getType()==helium::Rotation::ROLL) {
    prop.width() *= 0.9;
    prop.height() *= 0.9;
  }
}

void BigMotorWidget::setCustomInvalidate(int &rx, int &ry, int &rw, int &rh) {
  rx = (ceil)(prop.anchor().x-prop.width.def*globalProp.scale/2.0-globalProp.settings.sizes.stroke/2.0);
  ry = (ceil)(prop.anchor().y-prop.width.def*globalProp.scale/2.0-globalProp.settings.sizes.bigJointTagHeight-globalProp.settings.sizes.stroke/2.0);
  rw = prop.width.def*globalProp.scale+globalProp.settings.sizes.stroke;
  rh = (ceil)(prop.width.def*globalProp.scale+globalProp.settings.sizes.stroke+globalProp.settings.sizes.bigJointTagHeight);
}


BigMotorWidget::BigMotorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  MotorWidget(pprop,pjointProp,pglobalProp,pjointGlobalProp)
{
}

void BigMotorWidget::onClicked(GdkEventButton * event) {
  if(jointProp->jointMonitor.isConnected() && helium::isFaulty(jointProp->jointState.get()) ) { // DEBUG only allow change mode when faulty? helium::isFaulty(jointProp->jointState.get()) 
    if(jointProp->jointMonitor.get() == helium::FAULTYHWMON) 
      jointProp->jointMonitor.set(helium::NOHWMON);
    else 
      jointProp->jointMonitor.set(helium::FAULTYHWMON);
  }
}

void BigMotorWidget::drawAdditional() {
  Cairo::TextExtents txext;
  Glib::ustring text;
  helium::convert(text,jointProp->getId());
  text += "  " + jointProp->getJointName();
  getCtx().getcr()->set_font_size(12); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL );
  getCtx().getcr()->get_text_extents( text, txext );

  //draw name
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x),ceil(prop.anchor().y-prop.width.def*globalProp.scale/2));
  getCtx().getcr()->rectangle(-txext.width/2-globalProp.settings.sizes.bigJointTagHeight/2,-globalProp.settings.sizes.bigJointTagHeight,
			      txext.width+globalProp.settings.sizes.bigJointTagHeight,globalProp.settings.sizes.bigJointTagHeight);
  getCtx().getcr()->set_source_rgb(1,1,1);
  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->restore();
  getCtx().getcr()->stroke();

  //insert text
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x),prop.anchor().y-prop.width.def*globalProp.scale/2-globalProp.settings.sizes.bigJointTagHeight/2);
  getCtx().getcr()->move_to(-txext.width/2,6);
  getCtx().getcr()->set_source_rgb(0,0,0);
  getCtx().getcr()->show_text(text);
  getCtx().getcr()->stroke();
  getCtx().getcr()->restore();
}

void BigMotorWidget::onVisible() {
  jointProp->jointPower.connect(callRefreshonPower);
  jointProp->frameValue.connect(callRefreshonFrameValue);
  jointProp->jointTemp.connect(callRefreshonTemp);
  jointProp->jointState.connect(callRefreshonState);
  jointProp->jointMonitor.connect(callRefreshonMonitor);
}

void BigMotorWidget::onInvisible() {
  jointProp->jointPower.disconnect(callRefreshonPower);
  jointProp->frameValue.disconnect(callRefreshonFrameValue);
  jointProp->jointTemp.disconnect(callRefreshonTemp);
  jointProp->jointState.disconnect(callRefreshonState);
  jointProp->jointMonitor.disconnect(callRefreshonMonitor);
}

SmallMotorWidget::SmallMotorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  MotorWidget(pjointProp->getType()==helium::Rotation::ROLL?WidgetProperty(pprop.anchor.def,pprop.width.def*0.9,pprop.height.def*0.9):pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callOnBigJointChange(this),
  callRefreshonFrameValue(this)
{
  jointGlobalProp.bigJointId.connect(callOnBigJointChange);
  std::stringstream ss;
  ss << "motorWidget " << jointProp->getId();
  prop.name = ss.str();

}

double* SmallMotorWidget::getRefreshColor() {
  return globalProp.settings.colors.bgs[1].data;
}

void SmallMotorWidget::onVisible() {
  jointProp->frameValue.connect(callRefreshonFrameValue);
  jointProp->jointPower.connect(callRefreshonPower);
  jointProp->jointTemp.connect(callRefreshonTemp);
  jointProp->jointState.connect(callRefreshonState);
  jointProp->jointMonitor.connect(callRefreshonMonitor);
}

void SmallMotorWidget::onInvisible() {
  jointProp->frameValue.disconnect(callRefreshonFrameValue);
  jointProp->jointPower.disconnect(callRefreshonPower);
  jointProp->jointTemp.disconnect(callRefreshonTemp);
  jointProp->jointState.disconnect(callRefreshonState);
  jointProp->jointMonitor.disconnect(callRefreshonMonitor);
}



void SmallMotorWidget::onBigJointChange(int id) {
  if(id == jointProp->getId())
    setMouseState(GWidgetState::MOUSEDOWN);
  else
    setMouseState(GWidgetState::NORMAL);
}

void SmallMotorWidget::onClicked(GdkEventButton * event) {
  //change big joint
  if(event->button == 1) {
    if(jointGlobalProp.bigJointId == jointProp->getId())
      jointGlobalProp.bigJointId = -1;
    else
      jointGlobalProp.bigJointId = jointProp->getId();
  }
  //right mouse = on off power
  else if(event->button == 3) {
    if(jointGlobalProp.connection.get().state == helium::Connection::CONNECTED)     //if(jointProp->jointState.get()==helium::WORKINGHWSTATE)
      jointProp->setPower(!jointProp->getPower());
  }
}

void SmallMotorWidget::drawAdditional() {
  //insert text: joint temperature
  Cairo::TextExtents txext;
  Glib::ustring text = "nan";
  
  getCtx().getcr()->save();
  getCtx().getcr()->set_font_size(12*globalProp.scale.get()); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL );
  
  //the text to print
  if(jointProp->getTemp() != helium::motor::Interface::INVALIDPROP) {
    convert(text,jointProp->getTemp()*100/100);
  }
    
  getCtx().getcr()->get_text_extents( text, txext );
  getCtx().getcr()->translate(prop.anchor.def.x*globalProp.scale.get(),prop.anchor.def.y*globalProp.scale.get());
  getCtx().getcr()->move_to(-txext.width/2,+txext.height/2);
  getCtx().getcr()->show_text( text );
  getCtx().getcr()->restore();
  getCtx().getcr()->stroke();
}


TextWidget::TextWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  JointElementWidget(pprop,pjointProp,pglobalProp,pjointGlobalProp),callRefresh(this)
{
}

void TextWidget::draw() {
  //text properties
  getCtx().getcr()->set_font_size(12); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  Cairo::TextExtents txext;
  Glib::ustring text;
  convert(text,jointProp->getId());
  getCtx().getcr()->get_text_extents( text, txext );

  /*if(jointProp->isMoving()) {
    getCtx().getcr()->save();
    getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);
    getCtx().getcr()->translate(jointProp->isLeftDescription()?prop.width()-txext.width:0,0);
    getCtx().getcr()->rectangle(globalProp.settings.sizes.stroke/2,0,txext.width,12-globalProp.settings.sizes.stroke/2);
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.moving[0],
				     globalProp.settings.colors.moving[1],
				     globalProp.settings.colors.moving[2]);
    getCtx().getcr()->fill_preserve();
    getCtx().getcr()->restore();
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.moving[0],
				     globalProp.settings.colors.moving[1],
				     globalProp.settings.colors.moving[2]);
    getCtx().getcr()->set_line_width(globalProp.settings.sizes.stroke/2);
    getCtx().getcr()->stroke();
    getCtx().getcr()->set_line_width(globalProp.settings.sizes.stroke);
    getCtx().getcr()->set_source_rgb(0,0,0);
    }*/

  getCtx().getcr()->save();
  getCtx().getcr()->translate(prop.anchor().x-prop.width()/2,prop.anchor().y+prop.height()/2);

  //text properties
  getCtx().getcr()->set_font_size(12); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  getCtx().getcr()->get_text_extents( text, txext );

  //getCtx().getcr()->move_to(jointProp->isLeftDescription()?prop.width()-txext.width:0,0);
  getCtx().getcr()->move_to(jointProp->isLeftDescription()?-txext.width:0,0);
  getCtx().getcr()->show_text( text );

  //insert text: joint name
  text = jointProp->getJointName();
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL );
  getCtx().getcr()->get_text_extents( text, txext );
  getCtx().getcr()->move_to(jointProp->isLeftDescription()?-txext.width-20:20,0);
  //getCtx().getcr()->move_to(jointProp->isLeftDescription()?-20:20,0);
  //getCtx().getcr()->move_to(jointProp->isLeftDescription()?(prop.width()-txext.width-20):20,0);
  /*if(jointProp->isMoving()) 
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.moving[0],
				     globalProp.settings.colors.moving[1],
				     globalProp.settings.colors.moving[2]);*/
  getCtx().getcr()->show_text( text );
  getCtx().getcr()->restore();
  /*if(jointProp->isMoving())
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.moving[0],
				     globalProp.settings.colors.moving[1],
				     globalProp.settings.colors.moving[2]);*/
  getCtx().getcr()->stroke();
  getCtx().getcr()->set_source_rgb(0,0,0);
}

void TextWidget::setLocation(double scale) {
  prop.anchor() = prop.anchor.def;
  helium::mulScalar(prop.anchor(),scale);
  prop.anchor().y -= prop.height();
  prop.anchor().x += (jointProp->isLeftDescription()?-1:1) * (globalProp.settings.sizes.entrysOffset * scale);
  prop.anchor().x += prop.width()/2;
}

CalibTextWidget::CalibTextWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  JointElementWidget(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callRefreshonMin(this),  callRefreshonZero(this),  callRefreshonMax(this)
{
}

void CalibTextWidget::onVisible() {
  jointProp->calib.absLim.minv.connect(callRefreshonMin);
  jointProp->calib.absLim.maxv.connect(callRefreshonMax);
  jointProp->calib.absLim.zero.connect(callRefreshonZero);
}

void CalibTextWidget::onInvisible() {
  jointProp->calib.absLim.minv.disconnect(callRefreshonMin);
  jointProp->calib.absLim.maxv.disconnect(callRefreshonMax);
  jointProp->calib.absLim.zero.disconnect(callRefreshonZero);
}

void CalibTextWidget::onClicked(GdkEventButton * event) {
  jointGlobalProp.bigJointId.assure(jointProp->getId());
}

void CalibTextWidget::draw() {
  //insert text: joint id
  getCtx().getcr()->save();
  getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);

  //text properties
  getCtx().getcr()->set_font_size(12); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  Cairo::TextExtents txext;
  Glib::ustring text;

  for(int i=0;i<3;i++) {
    int d = jointProp->calib[(cfg::guide::CalibType::Type)i].get();
    stringstream ss;
    ss << std::hex << d;
    convert(text,ss.str());

    getCtx().getcr()->get_text_extents( text, txext );
    getCtx().getcr()->move_to((jointProp->isLeftDescription()?(-(2-i)*globalProp.settings.sizes.calibOffset-txext.width)+prop.width()*0.5- globalProp.settings.sizes.stroke:i*globalProp.settings.sizes.calibOffset-prop.width()*0.5+ globalProp.settings.sizes.stroke),0);
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.calibs[i][0],
				     globalProp.settings.colors.calibs[i][1],
				     globalProp.settings.colors.calibs[i][2]);
    getCtx().getcr()->show_text( text );
  }
  getCtx().getcr()->restore();
  getCtx().getcr()->stroke();
}

void CalibTextWidget::setLocation(double scale) {
  prop.anchor() = prop.anchor.def;
  helium::mulScalar(prop.anchor(),globalProp.scale.get());

  prop.anchor().x += (jointProp->isLeftDescription()?-1:1) * (globalProp.settings.sizes.smallJoint* globalProp.scale.get() + prop.width()*0.5 - globalProp.settings.sizes.stroke);
  prop.anchor().y = prop.anchor().y + prop.height()/2;  
}


JointWidget::JointWidget(cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, LinkableEntry<double>&pentry,helium::ValueHub<cfg::guide::GuideView::Type> &view): 
  JointElementWidgets(WidgetProperty(pjointProp->getPos(),
				     (double)pglobalProp.settings.sizes.smallJoint,//*(double)(pjointProp->getType()==helium::Rotation::ROLL?0.9:1.0),
				     (double)pglobalProp.settings.sizes.smallJoint),//*(double)(pjointProp->getType()==helium::Rotation::ROLL?0.9:1.0),
		      pjointProp,pglobalProp,pjointGlobalProp,JointWidget::NUM),
  ViewChangeable(view),
  entry(pentry),
  motorW(WidgetProperty(pjointProp->getPos(),//pglobalProp.settings.sizes.smallJoint,pglobalProp.settings.sizes.smallJoint,
			(double)pglobalProp.settings.sizes.smallJoint,//*(double)(pjointProp->getType()==helium::Rotation::ROLL?0.9:1.0),
			(double)pglobalProp.settings.sizes.smallJoint),//*(double)(pjointProp->getType()==helium::Rotation::ROLL?0.:19.0),
	 pjointProp,pglobalProp,pjointGlobalProp),
  robotEW(WidgetProperty(pjointProp->getPos(),pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),pjointProp,pglobalProp,pjointGlobalProp,pentry),//CursorType::ROBOT,
  frameEW(WidgetProperty(pjointProp->getPos(),pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),pjointProp,pglobalProp,pjointGlobalProp,pentry),//CursorType::FRAME,
  textWidget(WidgetProperty(pjointProp->getPos(),pglobalProp.settings.sizes.entry.w*2,12/*,true*/),pjointProp,pglobalProp,pjointGlobalProp),
  calibTextWidget(WidgetProperty(pjointProp->getPos(),
				 pglobalProp.settings.sizes.entry.w*2,pglobalProp.settings.sizes.entry.h),pjointProp,pglobalProp,pjointGlobalProp),
  robotTextWidget(WidgetProperty(pjointProp->getPos(),
				 pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),pjointProp,pglobalProp,pjointGlobalProp),
  frameTextWidget(WidgetProperty(pjointProp->getPos(),
				 pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),pjointProp,pglobalProp,pjointGlobalProp)
{
  widgets[MOTORW] = &motorW;
  widgets[ROBOTEW] = &robotEW;
  widgets[FRAMEEW] = &frameEW;
  widgets[TEXTW] = &textWidget;
  widgets[CALIBTEXT] = &calibTextWidget;
  widgets[ROBOTTW] = &robotTextWidget;
  widgets[FRAMETW] = &frameTextWidget;

  setVisible(true);

  widgets[MOTORW]->setVisible(true);
  widgets[TEXTW]->setVisible(true);
 
  prop.name = "jointwidget " + helium::toString(jointProp->getId());
}

void JointWidget::onNewView(cfg::guide::GuideView::Type view) {
  switch(view) {
  case cfg::guide::GuideView::MOTION:
    widgets[ROBOTEW]->setVisible(true);
    widgets[FRAMEEW]->setVisible(true);
    break;
  case cfg::guide::GuideView::TOUCH:
    widgets[ROBOTTW]->setVisible(true);
    widgets[FRAMETW]->setVisible(true);
    break;
  case cfg::guide::GuideView::CALIB:
    widgets[CALIBTEXT]->setVisible(true);
    break;
  default:
    break;
  }
}

void JointWidget::onPrevView(cfg::guide::GuideView::Type view) {
  switch(view) {
  case cfg::guide::GuideView::MOTION:
    entry.entry.hide();
    widgets[ROBOTEW]->setVisible(false);
    widgets[FRAMEEW]->setVisible(false);
    break;
  case cfg::guide::GuideView::TOUCH:
    widgets[ROBOTTW]->setVisible(false);
    widgets[FRAMETW]->setVisible(false);
    break;
  case cfg::guide::GuideView::CALIB:
    widgets[CALIBTEXT]->setVisible(false);
    break;
  default:
    break;
  }
}

ColdetDisplayWidget::ColdetDisplayWidget(const WidgetProperty& pprop, WidgetGlobalProperty& pglobalProp,cfg::guide::Coldet &pcoldet,helium::TsSignal<std::string> *pshowSubWindowSignal): //WidgetProperty &pprop, WidgetGlobalProperty& pglobalProp,cfg::guide::Coldet &pcoldet,helium::TsSignal<std::string> *psetShowColdetWindow):
  GWidget(pprop, pglobalProp),
  idleActive(true),
  coldet(pcoldet),
  showSubWindowSignal(pshowSubWindowSignal),
  callColdetRefresh(this)
{
  coldet.refresh.connect(callColdetRefresh);
}

void ColdetDisplayWidget::draw() {
  ///draw box one by one
  drawBox(0,idleActive,coldet.idle.cur,"idle");
  drawBox(1,!idleActive,coldet.busy.cur,"busy");
}

void ColdetDisplayWidget::drawBox(int i, bool active, int cpu,string label) {
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x-prop.width()/2),floor(prop.anchor().y+(-1+i)*prop.height()/2));
  getCtx().getcr()->rectangle(0,0,prop.width(),prop.height()/2);
  if(active)
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.error[0],globalProp.settings.colors.error[1],globalProp.settings.colors.error[2]);
  else
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.disable[0],globalProp.settings.colors.disable[1],globalProp.settings.colors.disable[2]);
  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->restore();
  getCtx().getcr()->set_line_width(globalProp.settings.sizes.stroke);
  getCtx().getcr()->stroke();
  
  //insert label
  getCtx().getcr()->save();
  Cairo::TextExtents txext;
  Glib::ustring text = label;
  getCtx().getcr()->set_font_size(9*globalProp.scale.get()); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL );
  getCtx().getcr()->get_text_extents( text, txext );
  double height = prop.anchor().y+prop.height()/2*(i?1:0)-txext.height/2+globalProp.settings.sizes.stroke;
  getCtx().getcr()->move_to(prop.anchor().x-prop.width()/2+globalProp.settings.sizes.stroke,height);
  getCtx().getcr()->show_text(text);
  getCtx().getcr()->restore();  

  getCtx().getcr()->save();
  helium::convert(text,cpu);
  getCtx().getcr()->set_font_size(9*globalProp.scale.get()); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  getCtx().getcr()->get_text_extents( text, txext );
  getCtx().getcr()->move_to(prop.anchor().x+prop.width()/2-globalProp.settings.sizes.stroke*2-txext.width,height);
  getCtx().getcr()->show_text(text);
  getCtx().getcr()->restore();  

}

void ColdetDisplayWidget::setIdleActive(bool b) {
  idleActive = b;
}

/*
bool ColdetDisplayWidget::isResponsible(const Point2D &e) {
  bool fw = fabs(e.x - prop.anchor().x) < (prop.width());
  bool fh = fabs(e.y - prop.anchor().y) < (prop.height());
  return fw && fh;
}
*/

void ColdetDisplayWidget::onClicked(GdkEventButton * event){
  ///signal to open collision detection window when clicked
  (*showSubWindowSignal)("Collision Detection Window");
}


template<>
void CopyArrowWidget<cfg::guide::CursorType::FRAME>::adjustInverse(int &w) {
  w = w*-1;
  getCtx().getcr()->translate(prop.width(),0);
}


AutoReachBox::AutoReachBox(Point2D pos, WidgetGlobalProperty& pglobalProp,helium::GtkProxy<helium::Connection> &pconnection, helium::ValueHub<cfg::guide::GuideView::Type> &view):
  RetryReachPostureBox(pos,pglobalProp,pconnection,view,"auto reach")
{
}

RetryReachPostureBox::RetryReachPostureBox(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection> &pconnection, helium::ValueHub<cfg::guide::GuideView::Type> &view, std::string name):
  ValueSettingBox<cfg::guide::CursorType::ROBOT>(pos,pglobalProp,pconnection,view,name),
  onRobotCollide(this)
{
}

void RetryReachPostureBox::onNewView(cfg::guide::GuideView::Type view) {
  if(view == cfg::guide::GuideView::CALIB) 
    setVisible(false);
  //  else
  //setVisible(true);
}

//ValueSettingBox
ValueSettingBox<cfg::guide::CursorType::FRAME>::ValueSettingBox(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::ValueHub<cfg::guide::GuideView::Type> &view, std::string name):
  SignallingClickableWidget<void>(WidgetProperty(pos,pglobalProp.settings.sizes.arrowBox.w,pglobalProp.settings.sizes.arrowBox.h/2/*+pglobalProp.settings.sizes.stroke,true*/),pglobalProp),
  ViewChangeable(view)
{
  prop.name = name;
}


void ValueSettingBox<cfg::guide::CursorType::FRAME>::onNewView(cfg::guide::GuideView::Type view) {
  if(view == cfg::guide::GuideView::CALIB) 
    setVisible(false);
  else
    setVisible(true);
}

void ValueSettingBox<cfg::guide::CursorType::FRAME>::draw() {
  getCtx().getcr()->set_font_size(12*globalProp.scale.get()); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  
  //rectangle
  getCtx().getcr()->save();
  //getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);
  getCtx().getcr()->translate(floor(prop.anchor().x-prop.width()/2),floor(prop.anchor().y-prop.height()/2));
  getCtx().getcr()->rectangle(0,0,prop.width(),prop.height());

  double *rgb = getColor();
  if(getState().mouseState == GWidgetState::MOUSEDOWN)
    rgb = getMouseDownColor();
    
  getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);

  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->restore();
  getCtx().getcr()->stroke();
  
  
  //insert text
  getCtx().getcr()->save();
  Cairo::TextExtents txext;
  getCtx().getcr()->get_text_extents( prop.name, txext );

  //if the text is longer than widget, 
  if(prop.width() < txext.width) {
    //divide text by the space
    if(prop.name.find(' ')  != std::string::npos) {

      std::string s = prop.name.substr(0,prop.name.find(' '));
      getCtx().getcr()->get_text_extents(s, txext );
      getCtx().getcr()->translate(floor(prop.anchor().x),floor(prop.anchor().y-prop.height()/4-globalProp.settings.sizes.stroke/2));
      getCtx().getcr()->move_to(-txext.width/2,+txext.height/2);
      getCtx().getcr()->show_text(s);

      s = prop.name.substr(prop.name.find(' ')+1,prop.name.size());
      getCtx().getcr()->get_text_extents(s, txext );
      getCtx().getcr()->move_to(-txext.width/2,+txext.height/2+prop.height()/2);
      getCtx().getcr()->show_text(s);
      getCtx().getcr()->stroke();
      getCtx().getcr()->restore();
      

      return;
    }  

    //or make the font smaller
    else {
      getCtx().getcr()->set_font_size(12*globalProp.scale.get()); 
      getCtx().getcr()->get_text_extents( prop.name, txext );
    }
  }
  getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);
  getCtx().getcr()->move_to(-txext.width/2,+txext.height/2);
  getCtx().getcr()->show_text(prop.name);
  getCtx().getcr()->restore();
  
}

double* ValueSettingBox<cfg::guide::CursorType::FRAME>::getMouseDownColor() {
  return globalProp.settings.colors.cursor[cfg::guide::CursorType::FRAME].data;
}

double* ValueSettingBox<cfg::guide::CursorType::FRAME>::getColor() {
  return globalProp.settings.colors.disable.data;
}

ValueSettingBox<cfg::guide::CursorType::ROBOT>::ValueSettingBox(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection> &pconnection, helium::ValueHub<cfg::guide::GuideView::Type> &view, std::string name):
  ValueSettingBox<cfg::guide::CursorType::FRAME>(pos,pglobalProp,view,name),
  ConnectionDependent(pconnection)
{
}

void ValueSettingBox<cfg::guide::CursorType::ROBOT>::onConnected() {
  GWidget::setVisible(true);
}

double* ValueSettingBox<cfg::guide::CursorType::ROBOT>::getMouseDownColor() {
  return globalProp.settings.colors.cursor[cfg::guide::CursorType::ROBOT].data;
}


void ValueSettingBox<cfg::guide::CursorType::ROBOT>::onDisconnected() {
  this->setVisible(false);
}

double* RetryReachPostureBox::getColor() {
  if(this->getState().mouseState == GWidgetState::MOUSEOVER)
    return this->globalProp.settings.colors.coldetSelected.data;
  else
    return this->globalProp.settings.colors.coldet.data;
}

void AutoReachBox::onMouseOver(bool b) {
  if(b)
    setMouseState(GWidgetState::MOUSEOVER);
  else
    setMouseState(GWidgetState::NORMAL);
}

ValueSetting::ValueSetting(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection> &pconnection,cfg::guide::Joints &pjoints, coldet::ColdetManager *pcoldetm, helium::ValueHub<cfg::guide::GuideView::Type> &view):  
  GWidgets(WidgetProperty(pos,pglobalProp.settings.sizes.arrowBox.w,pglobalProp.settings.sizes.arrowBox.h),pglobalProp,ValueSetting::NUM),
  joints(pjoints),
  coldetm(pcoldetm),
  robotZeroBox(Point2D(pos.x,pos.y+prop.height()/2+pglobalProp.settings.sizes.arrowBox.h/4)
	       ,pglobalProp,pconnection,view),
  frameZeroBox(Point2D(pos.x+pglobalProp.settings.sizes.frame.w*3-prop.width(), robotZeroBox.getAnchor().y),
	       pglobalProp,view),
  enableAllBox(Point2D(frameZeroBox.getAnchor().x,frameZeroBox.getAnchor().y+pglobalProp.settings.sizes.arrowBox.h/2),pglobalProp,view,"enable"),
  autoReachBox(Point2D(robotZeroBox.getAnchor().x,frameZeroBox.getAnchor().y+pglobalProp.settings.sizes.arrowBox.h/2),pglobalProp,pconnection,view),
  retryReachBox(Point2D(robotZeroBox.getAnchor().x,autoReachBox.getAnchor().y+pglobalProp.settings.sizes.arrowBox.h/2),pglobalProp,pconnection,view),

  robotArrow(Point2D(pos.x+prop.width()/2+(pglobalProp.settings.sizes.frame.w*3-pglobalProp.settings.sizes.arrowBox.w*2)/2,pos.y-pglobalProp.settings.sizes.arrowBox.h/4),pglobalProp,pconnection,pjoints.syncState[CursorType::ROBOT],view),
  frameArrow(Point2D(robotArrow.getAnchor().x,robotArrow.getAnchor().y+pglobalProp.settings.sizes.arrowBox.h/2),pglobalProp,pconnection,pjoints.syncState[CursorType::FRAME],view),


  callRobotAllZero(this),
  callFrameAllZero(this),
  callEnableAll(this),

  callAutoReach(this),
  callRetryReach(this),

  callOnRobotCollision(this),
  callOnFrameCollision(this)
{

  widgets[RobotZeroBox] = &robotZeroBox;
  widgets[FrameZeroBox] = &frameZeroBox;
  widgets[RobotArrow] = &robotArrow;
  widgets[FrameArrow] = &frameArrow;
  widgets[EnableAll] = &enableAllBox;
  widgets[AutoBox] = &autoReachBox;
  widgets[RetryBox] = &retryReachBox;

  widgets[EnableAll]->setVisible(true);
  widgets[FrameZeroBox]->setVisible(true);

  if(coldetm) {
    //coldetm->robotCollide.connect(autoReachBox.onRobotCollide); /// \todo
    //coldetm->robotCollide.connect(retryReachBox.onRobotCollide);
    //coldetm->robotCollide.connect(callOnRobotCollision);
    coldetm->isOnCollidingFrame.connect(callOnFrameCollision);
  }
  robotZeroBox.onClickedSignal.connect(callRobotAllZero);
  frameZeroBox.onClickedSignal.connect(callFrameAllZero);
  enableAllBox.onClickedSignal.connect(callEnableAll);	
  autoReachBox.onClickedSignal.connect(callAutoReach);	
  retryReachBox.onClickedSignal.connect(callRetryReach);
}



void ValueSetting::onCollision() {
  if(/*coldetm->robotCollide.get() || */coldetm->isOnCollidingFrame.get()) { /// \todo
    robotArrow.setEnable(false);
    frameArrow.setEnable(false);
  }
  else {
    robotArrow.setEnable(true);
    frameArrow.setEnable(true);
  }
}
void ValueSetting::autoReach(){
  //coldetm->autoReach(); /// \todo
}
void ValueSetting::retryReach(){
  //coldetm->retryReach(); /// \todo
}


void ValueSetting::draw() {
  for(size_t i=0;i<widgets.size();i++) {
    widgets[i]->canvasDraw();
  }

  for(int i=0;i<2;i++) { //for robot and file only
    getCtx().getcr()->set_font_size(12*globalProp.scale.get()); 
    getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
    
    //rectangle
    getCtx().getcr()->save();
    getCtx().getcr()->translate(i==0?floor(prop.anchor().x-prop.width()/2):floor(frameZeroBox.getAnchor().x-prop.width()/2),floor(prop.anchor().y-prop.height()/2));
    getCtx().getcr()->rectangle(0,0,prop.width(),prop.height()/*-ceil(globalProp.settings.sizes.stroke/2)*/);
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.cursor[i][0],globalProp.settings.colors.cursor[i][1],globalProp.settings.colors.cursor[i][2]);
    getCtx().getcr()->fill_preserve();
    getCtx().getcr()->restore();
    getCtx().getcr()->stroke();
    
    //insert text
    getCtx().getcr()->save();
    Cairo::TextExtents txext;
    Glib::ustring text;
    text = helium::EnumStrings<cfg::guide::CursorType::Type>::strings[i];
    getCtx().getcr()->get_text_extents( text, txext );
    getCtx().getcr()->translate(i==0?floor(prop.anchor().x-prop.width()/2):floor(frameZeroBox.getAnchor().x-prop.width()/2),floor(prop.anchor().y-prop.height()/2));
    getCtx().getcr()->move_to(-globalProp.settings.sizes.stroke/2+prop.width()/2-txext.width/2,prop.height()/2+txext.height/2);
    getCtx().getcr()->show_text(text);
    getCtx().getcr()->restore();
  }
}

void ValueSetting::robotAllZero(){
  for(size_t i=0;i<joints.size();i++)
    joints[i].desiredTarget.set(0);
}
void ValueSetting::frameAllZero(){  
  for(size_t i=0;i<joints.size();i++) {
    joints[i].frameValue.set<GuideEvent::ALLZEROJOINTCHANGE>(std::make_pair(joints[i].frameValue.get().first,0));
  }
  joints.postureChange(GuideEvent::ALLZEROPOSTURECHANGE);
}
void ValueSetting::enableAll(){
  for(size_t i=0;i<joints.size();i++) {
    joints[i].frameValue.set<GuideEvent::ALLENABLEJOINTCHANGE>(std::make_pair
							       (helium::VALUEFRAME,
								joints[i].frameValue.get().second));
  }
  joints.postureChange(GuideEvent::ALLENABLEPOSTURECHANGE);
}


ColdetAutoFixWidget::ColdetAutoFixWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, coldet::ColdetManager &pcoldetm,cfg::guide::Coldet &pcoldet):
  SignallingClickableWidget<void>(WidgetProperty(pos,pglobalProp.settings.sizes.frame.w*3,pglobalProp.settings.sizes.frame.h/2),pglobalProp),
  coldetm(pcoldetm),coldet(pcoldet),
  callSetVisibleState(this),
  callSetWarningState(this),
  callCopyAutoFix(&coldetm),
  callSetColdetOn(&coldet.coldetState)
{
  prop.name = "Fix Colliding Frame";
  coldetm.isOnCollidingFrame.connect(callSetVisibleState);
  coldet.coldetState.connect(callSetWarningState);
  //onClickedSignal.connect(callSetColdetOn); //default state is coldet on (this will be disconnected)
}

void ColdetAutoFixWidget::setWarningState(bool coldetOn) {
  setWarning(!coldetOn);
  setVisible(!coldetOn);
  /*
  if(coldetOn) {
    cout << "coldet on " << endl;
    //onClickedSignal.connect(callCopyAutoFix);
  }
  else {
    cout << "coldet off " << endl;
    //onClickedSignal.disconnect(callCopyAutoFix);
    //onClickedSignal.connect(callSetColdetOn);
  }
  */
}

void ColdetAutoFixWidget::onClicked(GdkEventButton *event) {
  if(getState().warning)
    callSetColdetOn();
  else
    callCopyAutoFix();
}

void ColdetAutoFixWidget::onVisible() {
  onMouseOver(false);
}

void ColdetAutoFixWidget::setVisibleState(bool isColliding) {
  if(!getState().warning)
    setVisible(isColliding);
}

void ColdetAutoFixWidget::onMouseOver(bool b) {  
  if(b) {    
    if(getState().warning) {
      prop.name = "Turn on Coldet";
    }
    else {
      prop.name = "Fix Colliding Frame";
    }
    setMouseState(GWidgetState::MOUSEOVER);
  }
  else {
    if(getState().warning) {
      prop.name = "Coldet is off";
    }
    else {
      prop.name = "Fix View";
    }    
    setMouseState(GWidgetState::NORMAL);
  }
}

void ColdetAutoFixWidget::draw() {
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x-prop.width()/2),floor(prop.anchor().y-prop.height()/2));
  getCtx().getcr()->rectangle(0,0,prop.width(),prop.height());

  double *rgb = globalProp.settings.colors.coldet.data;
  if(getState().mouseState == GWidgetState::MOUSEOVER)
    rgb = globalProp.settings.colors.coldetSelected.data;
  else if(getState().mouseState == GWidgetState::MOUSEDOWN)
    rgb = globalProp.settings.colors.selected.data;
    

  getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);
  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->set_source_rgb(globalProp.settings.colors.selected[0],globalProp.settings.colors.selected[1],globalProp.settings.colors.selected[2]);
  getCtx().getcr()->stroke();
  getCtx().getcr()->restore();

  //insert text
  getCtx().getcr()->set_font_size(11*globalProp.scale.get()); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  getCtx().getcr()->save();
  Cairo::TextExtents txext;
  Glib::ustring text;
  text = prop.name;
  getCtx().getcr()->get_text_extents( text, txext );
  getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);
  getCtx().getcr()->move_to(-txext.width/2,txext.height/2);
  getCtx().getcr()->set_source_rgb(0,0,0);
  getCtx().getcr()->show_text(text);
  getCtx().getcr()->stroke();
  getCtx().getcr()->restore();
}

FrameSaveSetting::FrameSaveSetting(Point2D pos, WidgetGlobalProperty &pglobalProp, helium::ValueHub<cfg::guide::GuideView::Type> &view, helium::ValueHub<bool> &pediting):
  GWidgets(WidgetProperty(pos,pglobalProp.settings.sizes.frame.w*3+pglobalProp.settings.sizes.stroke,pglobalProp.settings.sizes.frame.h*1.5+pglobalProp.settings.sizes.stroke/*,true*/),pglobalProp,NUM),
  ViewChangeable(view),
  editing(pediting),
  discardFrameBox(pos,pglobalProp),
  selectFrameBox(pos,pglobalProp),
  thisFrameBox(pos,pglobalProp),
  callSetVisible(this),
  callSetFrameSaveTo(&frameSaveSignal,gwidget::FrameSaveTo::NUM)
{
  widgets[DISCARDFRAME] = &discardFrameBox;
  widgets[SELECTFRAME] = &selectFrameBox;
  widgets[THISFRAME] = &thisFrameBox;
  //widgets[NOTSAVED] = &notSavedWidget;


  widgets[DISCARDFRAME]->setVisible(true);
  widgets[SELECTFRAME]->setVisible(true);
  widgets[THISFRAME]->setVisible(true);
  //widgets[NOTSAVED]->setVisible(true);

  discardFrameBox.onClickedSignal.connect(callSetFrameSaveTo);
  selectFrameBox.onClickedSignal.connect(callSetFrameSaveTo);
  thisFrameBox.onClickedSignal.connect(callSetFrameSaveTo);

  editing.connect(callSetVisible);
}

void FrameSaveSetting::onNewView(cfg::guide::GuideView::Type view) {
  if((view == cfg::guide::GuideView::MOTION || view == cfg::guide::GuideView::TOUCH) && editing.get())
    setVisible(true);
  if(view == cfg::guide::GuideView::CALIB)
    setVisible(false);
}



ViewChangeable::ViewChangeable(helium::ValueHub<cfg::guide::GuideView::Type> &view):
  prevView(view.get()),
  callOnViewChange(this)
{
  view.connect(callOnViewChange);
}

void ViewChangeable::onViewChange(cfg::guide::GuideView::Type view) {
  onPrevView(prevView);
  onNewView(view);
  prevView = view;
}

NotSavedWidget::NotSavedWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp,helium::ValueHub<cfg::guide::GuideView::Type> &pview, helium::ValueHub<bool> &pediting, helium::ValueHub<bool> &pcalib):
  GWidget(pprop,pglobalProp),
  ViewChangeable(pview),
  view(pview),
  editing(pediting),calib(pcalib),
  callSetVisibleEditing(this),
  callSetVisibleCalibSaved(this)
{
  //cout << "not saved widget " << getAnchor() << endl;
  prop.name = "Not Saved";
  editing.connect(callSetVisibleEditing);
  calib.connect(callSetVisibleCalibSaved);
}

void NotSavedWidget::setVisibleState() {
  if(view == cfg::guide::GuideView::CALIB && !calib.get()) {
    prop.name = "Calib Not Saved";
    setVisible(true);
    //    cout << "setvisible state " << getState().visible << " "<< calib.get() << endl;
  }
  else if(view != cfg::guide::GuideView::CALIB && editing.get()) {
    prop.name = "Frame Not Saved";
    setVisible(true);
    //    cout << "setvisible state " << getState().visible << " "<< editing.get() << endl;
  }
  else {
    setVisible(false);
  }
}

void NotSavedWidget::onNewView(cfg::guide::GuideView::Type view) {
  setVisibleState();
}

void NotSavedWidget::draw() {
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x-prop.width()/2),floor(prop.anchor().y-prop.height()/2));
  getCtx().getcr()->rectangle(0,0,prop.width(),prop.height());
  getCtx().getcr()->set_source_rgb(globalProp.settings.colors.cursor[1][0],globalProp.settings.colors.cursor[1][1],globalProp.settings.colors.cursor[1][2]);
  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->restore();
  getCtx().getcr()->set_line_width(0);
  getCtx().getcr()->stroke();
  getCtx().getcr()->set_line_width(globalProp.settings.sizes.stroke);
    
  //insert text
  getCtx().getcr()->save();
  getCtx().getcr()->set_font_size(12*globalProp.scale.get()); 
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  Cairo::TextExtents txext;
  Glib::ustring text = prop.name;
  getCtx().getcr()->set_source_rgb(1,1,1);
  getCtx().getcr()->get_text_extents( text, txext );
  getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);
  getCtx().getcr()->move_to(-txext.width/2,+txext.height/2);
  getCtx().getcr()->show_text(text);
  getCtx().getcr()->restore();
}

ModelBoxWidget::ModelBoxWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp):
  GWidget(pprop,pglobalProp),
  callSwitchModelBox(this)
{
  //setVisible(false);
}

void ModelBoxWidget::switchModel(int id) {
  switch(id) {
  case cfg::guide::SwitchingModelArea::ROBOT :
    rgb = globalProp.settings.colors.cursor[cfg::guide::CursorType::ROBOT].data;
    setVisible(true); 
    break;
  case cfg::guide::SwitchingModelArea::FIX :
    rgb = globalProp.settings.colors.coldet.data;
    setVisible(true); 
    break;
  default: setVisible(false); break;
  }
}

void ModelBoxWidget::draw() {
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x-prop.width()/2),floor(prop.anchor().y-prop.height()/2));
  getCtx().getcr()->rectangle(0,0,prop.width(),prop.height());
  getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);
  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->restore();
  getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);
  getCtx().getcr()->stroke();
  getCtx().getcr()->set_source_rgb(0,0,0);
}

//** HardwareWidget

HardwareWidget::HardwareWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp, helium::GtkProxy<helium::Connection> &connection, helium::TsSignal<std::string> &psetShowWindow):
  GWidget(pprop,pglobalProp),
  ConnectionDependent(connection),
  setShowWindow(psetShowWindow),
  callOnStateChange(this)
{  
  //setVisible(false);
}

/*
bool HardwareWidget::isResponsible(const Point2D &e) {
  bool cw = fabs(e.x - prop.anchor().x ) < (prop.width()/2);
  bool ch = fabs(e.y - prop.anchor().y ) < (prop.height()/2);
  return cw && ch;
}
*/
void HardwareWidget::onClicked(GdkEventButton * event) {
  setShowWindow(getWindowName());
}

void HardwareWidget::onConnected() {
  setVisible(true);
}
void HardwareWidget::onDisconnected() {
  setVisible(false);
}

void HardwareWidget::draw() {

  getCtx().getcr()->save(); getCtx().getcr()->translate(floor(prop.anchor().x+prop.width()/2-prop.height()),floor(prop.anchor().y-prop.height()/2));
  getCtx().getcr()->rectangle(0,0,prop.height(),prop.height());

  double *rgb = globalProp.settings.colors.powerState[1].data;
  if(getState().warning)
    rgb = globalProp.settings.colors.powerState[0].data;
  getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);

  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->restore();
  getCtx().getcr()->stroke();
  
  //insert text
  getCtx().getcr()->save();
  getCtx().getcr()->set_font_size(11*globalProp.scale.get()); 
  Cairo::TextExtents txext;
  Glib::ustring text;
  getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL );

  text = prop.name + " " + (getState().warning?"Err":"Ok");

  getCtx().getcr()->set_source_rgb(0,0,0);
  getCtx().getcr()->get_text_extents( text, txext );
  getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);
  //getCtx().getcr()->move_to(prop.width()*5/14-globalProp.settings.sizes.stroke*2-txext.width,txext.height/2);
  getCtx().getcr()->move_to(prop.width()/2-prop.height()-globalProp.settings.sizes.stroke*2-txext.width,txext.height/2);
  getCtx().getcr()->show_text(text);
  getCtx().getcr()->restore();
}

SensorWidget::SensorWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp, helium::GtkProxy<helium::Connection> &connection, helium::TsSignal<std::string> &psetShowWindow, cfg::guide::Sensors &psensors):
  HardwareWidget(pprop,pglobalProp,connection,psetShowWindow),
  sensors(psensors)
{
  prop.name = "Sensors";
  sensors.state.connect(callOnStateChange);
}

void SensorWidget::onStateChange() {  
  std::stringstream ss;
  ss.str("");
  for(size_t i=0;i<sensors.state.get().size();i++) {
    if(sensors.state.get().data[i] != helium::WORKINGHWSTATE) {
      ss << i << " ";
    }
  }
  setWarning(ss.str().size()!=0);
}

std::string SensorWidget::getWindowName() {
  return "Sensor Calib Window";
}

void SensorWidget::onVisible(){
  onStateChange();
} 

void SensorWidget::onMouseOver(bool b) {
  
}


GyroWidget::GyroWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp, helium::GtkProxy<helium::Connection> &connection, helium::TsSignal<std::string> &psetShowWindow, cfg::guide::Gyro &pgyro):
  HardwareWidget(pprop,pglobalProp,connection,psetShowWindow),
  gyro(pgyro)
{
  prop.name = "Gyro";
  gyro.state.connect(callOnStateChange);
}

void GyroWidget::onStateChange() {  
  for(size_t i=0;i<gyro.state.get().size();i++) {
    if(gyro.state.get()[i] != helium::WORKINGHWSTATE) {
      setWarning(true);
      return;
    }
  }
  setWarning(false);
}

std::string GyroWidget::getWindowName() {
  return "Gyro Calib Window";
}

void GyroWidget::onVisible(){
  onStateChange();
} 

//** start of PowerSwitchCircle

PowerSwitchCircle::PowerSwitchCircle(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp):
  SignallingClickableWidget<void>(pprop,pglobalProp)
{
}

bool PowerSwitchCircle::isResponsible(const Point2D &e) {
  return infinityNormDistance(prop.anchor(),e) < (prop.height()/2*globalProp.scale.get());
}

void PowerSwitchCircle::draw() {
  //draw circle
  getCtx().getcr()->save();
  getCtx().getcr()->translate(prop.anchor().x,prop.anchor().y);
  getCtx().getcr()->arc(0,0, (prop.height()/2-globalProp.settings.sizes.stroke/2)/*globalProp.scale.get()*/, 0.0, 2.0 * M_PI); // full circle
  
  if(!getState().enable) 
    getCtx().getcr()->set_source_rgb(globalProp.settings.colors.disable[0],globalProp.settings.colors.disable[1],globalProp.settings.colors.disable[2]);
  else {
    double *rgb = getColor();
    if(getState().mouseState==GWidgetState::MOUSEDOWN)
      getCtx().getcr()->set_source_rgb(rgb[0]/2,rgb[1]/2,rgb[2]/2);
    else
      getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);
  }

  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->set_source_rgb(0,0,0);
  getCtx().getcr()->stroke();
  getCtx().getcr()->restore();
}

double* PowerSwitchCircle::getColor() {
  return globalProp.settings.colors.powerState[(getState().warning?0:1)].data;
}

//** end of PowerSwitchCircle

EnableSwitchWidget::EnableSwitchWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  PowerSwitchCircle(pprop,pglobalProp),
  JointElement(pjointProp,pjointGlobalProp),
  callOnValueChange(this)
{
}

double* EnableSwitchWidget::getColor() {
  if(getState().warning)
    return globalProp.settings.colors.disable.data;
  return globalProp.settings.colors.cursor[cfg::guide::CursorType::FRAME].data;
}


void EnableSwitchWidget::onValueChange(helium::FrameValue value) {
  setWarning(value.first == helium::INTERPOLATEFRAME);
}

void EnableSwitchWidget::onClicked(GdkEventButton*) {
  ///call signal to calculate appropriate frame value according to enable status
  jointProp->toggleEnable(jointProp->frameValue.get().first==helium::INTERPOLATEFRAME?
			  helium::VALUEFRAME:
			  helium::INTERPOLATEFRAME);
}

void EnableSwitchWidget::onVisible() {
  jointProp->frameValue.connect(callOnValueChange);
  onValueChange(jointProp->frameValue.get());
}

void EnableSwitchWidget::onInvisible() {
  jointProp->frameValue.disconnect(callOnValueChange);
}

void EnableSwitchWidget::setLocation(double scale) {


  //location
  Point2D v;
  prop.anchor() = prop.anchor.def;
  helium::mulScalar(prop.anchor(),globalProp.scale.get());
  jointProp->getV(v);
  helium::mulScalar(v,globalProp.settings.sizes.switchOffset*globalProp.scale.get());
  helium::sub(prop.anchor(),v);
  //pscR.setAnchor(prop.anchor().x ,prop.anchor().y);
  //pscG.setAnchor(prop.anchor().x ,prop.anchor().y);
}


template<>
void SignallingClickableWidget<void>::onClicked(GdkEventButton * event) {
  onClickedSignal();
}


PowerSwitchBox::PowerSwitchBox(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp):
  SignallingClickableWidget<void>(pprop,pglobalProp)
{
}

void PowerSwitchBox::onMouseOver(bool b) {
  setWarning(b);
}

void PowerSwitchBox::draw() {
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x-prop.width()/2),floor(prop.anchor().y-prop.height()/2));
  getCtx().getcr()->rectangle(0,0,prop.width(),prop.height());
  double *rgb = globalProp.settings.colors.disable.data;
  if(getState().warning)
    rgb = globalProp.settings.colors.selected.data;
  getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);
  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->restore();
  getCtx().getcr()->stroke();
}

PowerSwitchWidget::PowerSwitchWidget(const WidgetProperty &pprop, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection>& pconnection):
  GWidgets(pprop,pglobalProp,NUM),
  ConnectionDependent(pconnection),
  powerSwitchBox(WidgetProperty(pprop.anchor.def,pprop.width.def,pprop.height.def),pglobalProp),
  pscR(pprop,pglobalProp),
  pscG(pprop,pglobalProp),
  callSetPowerOn(this,true),
  callSetPowerOff(this,false),
  callTogglePower(this)
{
  widgets[Box] = &powerSwitchBox;
  widgets[RedCircle] = &pscR;
  widgets[GreenCircle] = &pscG;
  
  //set the circle colors
  pscR.setWarning(true);
  pscG.setWarning(false);

  //turn on widget
  widgets[Box]->setVisible(true);
  widgets[GreenCircle]->setVisible(true);

  //set disabled as initial state
  setEnable(false);
  pscR.setEnable(false);
  pscG.setEnable(false);

  prop.name = "powerSwitchWidget " + prop.name;

  powerSwitchBox.onClickedSignal.connect(callTogglePower);
  pscR.onClickedSignal.connect(callSetPowerOn);
  pscG.onClickedSignal.connect(callSetPowerOff);
}

void PowerSwitchWidget::togglePower() {
  setPower(pscR.getState().visible);
}

void PowerSwitchWidget::setLocation(double scale) {
  //location
  prop.anchor() = prop.anchor.def;
  helium::mulScalar(prop.anchor(),globalProp.scale.get());
  modifyLocation();
  pscR.setAnchor(prop.anchor().x - prop.height()/2,prop.anchor().y);
  pscG.setAnchor(prop.anchor().x + prop.height()/2 - globalProp.settings.sizes.stroke/2,prop.anchor().y);
  powerSwitchBox.setAnchor(prop.anchor().x,prop.anchor().y);
}


void PowerSwitchWidget::onConnected() {
  setEnable(true);
  pscR.setEnable(true);
  pscG.setEnable(true);
}
void PowerSwitchWidget::onDisconnected() {
  setEnable(false);
  pscR.setEnable(false);
  pscG.setEnable(false);
}


BigJointSwitchWidget::BigJointSwitchWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  PowerSwitchWidget(pprop,pglobalProp,pjointGlobalProp.connection),
  JointElement(pjointProp,pjointGlobalProp),
  callOnPowerChange(this)
{  
}
void BigJointSwitchWidget::modifyLocation() {
  Point2D v;
  jointProp->getV(v);
  helium::mulScalar(v,globalProp.settings.sizes.switchOffset*globalProp.scale.get());
  helium::sum(prop.anchor(),v);
}
void BigJointSwitchWidget::onVisible() {
  jointProp->jointPower.connect(callOnPowerChange);
  onPowerChange(jointProp->jointPower.get());
}
void BigJointSwitchWidget::onInvisible() {
  jointProp->jointPower.disconnect(callOnPowerChange);
}
void BigJointSwitchWidget::setPower(bool b) {
  jointProp->jointPower.assure(b);
}
void BigJointSwitchWidget::onPowerChange(bool b) {
  pscR.setVisible(!b);
  pscG.setVisible(b);
}



LimbWidget::LimbWidget(WidgetGlobalProperty& pglobalProp, cfg::guide::Limb &plimb, cfg::guide::Joints &pjoints, helium::GtkProxy<helium::Connection>& pconnection):
  PowerSwitchWidget(WidgetProperty(Point2D(plimb.pos.x,plimb.pos.y),pglobalProp.settings.sizes.powerSwitch.w,pglobalProp.settings.sizes.powerSwitch.h),pglobalProp,pconnection),
  limb(plimb),
  joints(pjoints),
  callOnPowerChange(this,limb.member.size())
{
  for(size_t i=0; i<limb.member.size();i++) {
    joints[limb.member[i]].jointPower.connect(callOnPowerChange[i]);
  }
  setVisible(true);
}


void LimbWidget::togglePower() {
  if(!(pscR.getState().visible && pscG.getState().visible))
  setPower(pscR.getState().visible);
}


void LimbWidget::setPower(bool b) {
  //convert the meaning of each circle's click when some of the child motors are on and some are off
  if(pscR.getState().visible && pscG.getState().visible)
    b = !b;

  for(size_t i=0; i<limb.member.size();i++) {
    joints[limb.member[i]].jointPower.assure(b);
  }  
}

void LimbWidget::onPowerChange() {
  int sum = 0;
  for(size_t i=0; i<limb.member.size();i++) {
    sum += joints[limb.member[i]].jointPower.get();
  }
  if(sum == 0) {
    pscR.setVisible(true);
    pscG.setVisible(false);
  }
  else if(sum == (int) limb.member.size()) {
    pscR.setVisible(false);
    pscG.setVisible(true);
  }
  else {
    pscR.setVisible(true);
    pscG.setVisible(true);
  }
}

MotionBigJointWidget::MotionBigJointWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::Joints &pjoints,helium::ValueHub<cfg::guide::GuideView::Type> &pview, LinkableEntry<double>&pentry, coldet::ColdetManager *coldetm):
  BigJointWidget<double,cfg::guide::GuideView::MOTION>(pos,pglobalProp,pjointGlobalProp,pjoints,pview,NUM,&pentry),
  robotCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib,joints.syncState[CursorType::ROBOT],coldetm),
  potCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),
  frameCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib,joints.syncState[CursorType::FRAME]),

  robotEW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp,pentry),//CursorType::ROBOT,
  frameEW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp,pentry),//CursorType::FRAME,
  potTW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp),

  enableSW(WidgetProperty(pos,pglobalProp.settings.sizes.powerSwitch.h,pglobalProp.settings.sizes.powerSwitch.h),NULL,pglobalProp,pjointGlobalProp)
  //callRefresh_FrameValue(this)
{
  //cout << "motionbig " << pos << endl;
  widgets[MOTORW] = &motorW;
  widgets[BARW] = &barW;
  widgets[ROBOTEW] = &robotEW;
  widgets[FRAMEEW] = &frameEW;
  widgets[POTTW] = &potTW;
  widgets[ROBOTCW] = &robotCW;
  widgets[POTCW] = &potCW;
  widgets[FRAMECW] = &frameCW;
  widgets[POWERSW] = &powerSW;
  widgets[ENABLESW] = &enableSW;
}

void MotionBigJointWidget::moveCursor(bool increase, bool page, bool overLimit) {
  if(robotCW.getClickState() == Clickable::State::CLICKED)
    robotCW.moveCursor(increase,page,overLimit);
  else if (frameCW.getClickState() == Clickable::State::CLICKED) 
    frameCW.moveCursor(increase,page,overLimit);
}

void MotionBigJointWidget::setJointPointer(cfg::guide::Joint *pjoint) {
  motorW.jointProp = pjoint;
  barW.jointProp = pjoint;
  robotEW.jointProp = pjoint;
  frameEW.jointProp = pjoint;
  potTW.jointProp = pjoint;
  robotCW.jointProp = pjoint;
  potCW.jointProp = pjoint;
  frameCW.jointProp = pjoint;
  powerSW.jointProp = pjoint;
  enableSW.jointProp = pjoint;
}

////***********
TouchBigJointWidget::TouchBigJointWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::Joints &pjoints,helium::ValueHub<cfg::guide::GuideView::Type> &view, coldet::ColdetManager *coldetm)://, JointValueValidate<double> &pjvv
  BigJointWidget<double,cfg::guide::GuideView::TOUCH>(pos,pglobalProp,pjointGlobalProp,pjoints,view,NUM),
  robotCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib,coldetm),
  potCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),
  frameCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),//,joints.syncState[CursorType::FRAME]),

  robotTW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp),
  frameTW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp),
  potTW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp),

  enableSW(WidgetProperty(pos,pglobalProp.settings.sizes.powerSwitch.h,pglobalProp.settings.sizes.powerSwitch.h),NULL,pglobalProp,pjointGlobalProp)
{
  //cout << "touchbig " << pos << endl;
  widgets[MOTORW] = &motorW;
  widgets[BARW] = &barW;
  widgets[ROBOTTW] = &robotTW;
  widgets[FRAMETW] = &frameTW;
  widgets[POTTW] = &potTW;
  widgets[ROBOTCW] = &robotCW;
  widgets[POTCW] = &potCW;
  widgets[FRAMECW] = &frameCW;
  widgets[POWERSW] = &powerSW;
  widgets[ENABLESW] = &enableSW;
}

void TouchBigJointWidget::setJointPointer(cfg::guide::Joint *pjoint) {
  motorW.jointProp = pjoint;
  barW.jointProp = pjoint;
  robotTW.jointProp = pjoint;
  frameTW.jointProp = pjoint;
  potTW.jointProp = pjoint;
  robotCW.jointProp = pjoint;
  potCW.jointProp = pjoint;
  frameCW.jointProp = pjoint;
  powerSW.jointProp = pjoint;
  enableSW.jointProp = pjoint;
}

////*********

CalibBigJointWidget::CalibBigJointWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::Joints &pjoints,helium::ValueHub<cfg::guide::GuideView::Type> &view,LinkableEntry<int>&pentry)://,JointValueValidate<int> &pjvv
  BigJointWidget<int,cfg::guide::GuideView::CALIB>(pos,pglobalProp,pjointGlobalProp,pjoints,view,NUM,&pentry),
  robotCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),
  potCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),
  minCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),
  zeroCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),
  maxCW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.h,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),

  robotEW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp,pentry),//,value),
  minEW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp,pentry),//,cfg::guide::CalibType::MIN,pentry,value),
  zeroEW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp,pentry),//cfg::guide::CalibType::ZERO,pentry,value),
  maxEW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp,pentry),//cfg::guide::CalibType::MAX,pentry,value),
  potTW(WidgetProperty(pos,pglobalProp.settings.sizes.entry.w,pglobalProp.settings.sizes.entry.h/*,true*/),NULL,pglobalProp,pjointGlobalProp)
{
  widgets[MOTORW] = &motorW;
  widgets[BARW] = &barW;
  widgets[ROBOTEW] = &robotEW;
  widgets[MINEW] = &minEW;
  widgets[ZEROEW] = &zeroEW;
  widgets[MAXEW] = &maxEW;
  widgets[POTTW] = &potTW;
  widgets[ROBOTCW] = &robotCW;
  widgets[POTCW] = &potCW;
  widgets[POWERSW] = &powerSW;
  widgets[MINCW] = &minCW;
  widgets[ZEROCW] = &zeroCW;
  widgets[MAXCW] = &maxCW;
}
void CalibBigJointWidget::setJointPointer(cfg::guide::Joint *pjoint) {
  motorW.jointProp = pjoint;
  barW.jointProp = pjoint;
  robotEW.jointProp = pjoint;
  minEW.jointProp = pjoint;
  zeroEW.jointProp = pjoint;
  maxEW.jointProp = pjoint;
  potTW.jointProp = pjoint;
  robotCW.jointProp = pjoint;
  potCW.jointProp = pjoint;
  powerSW.jointProp = pjoint;
  minCW.jointProp = pjoint;
  zeroCW.jointProp = pjoint;
  maxCW.jointProp = pjoint;
}
void CalibBigJointWidget::moveCursor(bool increase, bool page,bool overLimit) {
  if(robotCW.getClickState() == Clickable::State::CLICKED)
    robotCW.moveCursor(increase,page); 
  else if(minCW.getClickState() == Clickable::State::CLICKED)
    minCW.moveCursor(increase,page);
  else if(zeroCW.getClickState() == Clickable::State::CLICKED)
    zeroCW.moveCursor(increase,page);
  else if(maxCW.getClickState() == Clickable::State::CLICKED)
    maxCW.moveCursor(increase,page);
}

template<> int BarPositionWidget<int>::getMax() { return defaultCalib.calibMax;}
template<> int BarPositionWidget<int>::getMin() { return defaultCalib.calibMin;}		     
template<> int BarWidget<int>::getMaxLimit() { return jointProp->calib.absLim.maxv.get();}
template<> int BarWidget<int>::getMinLimit() { return jointProp->calib.absLim.minv.get();}

template<> double BarPositionWidget<double>::getMax() { return helium::getDegtoRad(defaultCalib.max);}
template<> double BarPositionWidget<double>::getMin() { return helium::getDegtoRad(defaultCalib.min);}
template<> double BarWidget<double>::getMaxLimit() { return jointProp->calib.relLim.maxv;}
template<> double BarWidget<double>::getMinLimit() { return jointProp->calib.relLim.minv;}

CursorTypeAdapter<double,EntryDestination::Robot>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CursorTypeAdapter<double,EntryDestination::RobotOnly>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib),
  callOnValueChange(this)
{
}

void CursorTypeAdapter<double,EntryDestination::Robot>::onVisible(){
  jointProp->jointPower.connect(callOnPowerChange);
  jointProp->desiredTarget.connect(callRefresh);
  jointProp->displayedTarget.connect(callOnValueChange);
  onPowerChange(jointProp->jointPower.get());
  onValueChange(jointProp->displayedTarget.get());
}
void CursorTypeAdapter<double,EntryDestination::Robot>::onInvisible(){
  jointProp->jointPower.disconnect(callOnPowerChange);
  jointProp->desiredTarget.disconnect(callRefresh);
  jointProp->displayedTarget.disconnect(callOnValueChange);
}
double CursorTypeAdapter<double,EntryDestination::Robot>::getValue(){
  //  cout << "cursor robot " << jointProp->desiredTarget.get() <<  " converted "<< helium::getRadtoDeg(getPostoVal(getValtoPos(jointProp->desiredTarget.get())))<< endl;
  return jointProp->desiredTarget.get();
}
void CursorTypeAdapter<double,EntryDestination::Robot>::onValueChange(double val){
  setWarning(val == helium::motor::Interface::COLLISIONPOS);
}


CursorTypeAdapter<int,EntryDestination::CalibRobot>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CursorTypeAdapter<int,EntryDestination::RobotOnly>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}
  
void CursorTypeAdapter<int,EntryDestination::CalibRobot>::onVisible(){
  jointProp->absTarget.connect(callRefresh);
  jointProp->jointPower.connect(callOnPowerChange);
  onPowerChange(jointProp->jointPower.get());
}

void CursorTypeAdapter<int,EntryDestination::CalibRobot>::onInvisible(){
  jointProp->absTarget.disconnect(callRefresh);
}

int CursorTypeAdapter<int,EntryDestination::CalibRobot>::getValue(){
 return jointProp->absTarget.get();
}





CursorTypeAdapter<double,EntryDestination::Frame>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  BasicCursor<double>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib),
  callOnEnableChange(this)
{
}
  
void CursorTypeAdapter<double,EntryDestination::Frame>::onEnableChange(helium::FrameValue value){
  setEnable(value.first != helium::INTERPOLATEFRAME);
}

void CursorTypeAdapter<double,EntryDestination::Frame>::onVisible(){
  jointProp->frameValue.connect(callOnEnableChange);
  onEnableChange(jointProp->frameValue.get());
}
void CursorTypeAdapter<double,EntryDestination::Frame>::onInvisible(){
  jointProp->frameValue.connect(callOnEnableChange);
}

double CursorTypeAdapter<double,EntryDestination::Frame>::getValue(){
  return jointProp->frameValue.get().second;
}
double* CursorTypeAdapter<double,EntryDestination::Frame>::getColor(){
  return globalProp.settings.colors.cursor[cfg::guide::CursorType::FRAME].data;
}

bool CursorTypeAdapter<double,EntryDestination::Frame>::setFramePosition(){
  return true;
}


CursorTypeAdapter<double,EntryDestination::Pot>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CursorTypeAdapter<double,EntryDestination::PotAll>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}

void CursorTypeAdapter<double,EntryDestination::Pot>::onVisible(){
  //jointProp->displayedTarget.connect(callRefresh);
  jointProp->displayedTarget.connect(callOnValueChange);
  jointProp->jointPower.connect(callOnPowerChange);
  onPowerChange(jointProp->jointPower.get());
  onValueChange(jointProp->displayedTarget.get());
}
void CursorTypeAdapter<double,EntryDestination::Pot>::onInvisible(){
  //jointProp->displayedTarget.disconnect(callRefresh);
  jointProp->displayedTarget.disconnect(callOnValueChange);
  jointProp->jointPower.disconnect(callOnPowerChange);
}
double CursorTypeAdapter<double,EntryDestination::Pot>::getValue(){
  static double prev = jointProp->displayedTarget.get();//default starting value
  if(jointProp->jointState.isConnected()) { //robot is connected
    double now = jointProp->displayedTarget.get();
    if(now != helium::motor::Interface::INVALIDPOS && now != helium::motor::Interface::FREEPOS && now != helium::motor::Interface::MOVINGPOS && now != helium::motor::Interface::COLLISIONPOS) { //not weird value
      prev = now;
    }
  }
  return prev;
}

bool CursorTypeAdapter<double,EntryDestination::Pot>::isWarning(double val) {
  return val==helium::motor::Interface::MOVINGPOS;
}


CursorTypeAdapter<int,EntryDestination::CalibPot>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CursorTypeAdapter<int,EntryDestination::PotAll>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}

bool CursorTypeAdapter<int,EntryDestination::CalibPot>::isWarning(int val) {
  return val == helium::AbsMotorSign::MOVINGPOS;
}

void CursorTypeAdapter<int,EntryDestination::CalibPot>::onVisible(){
  //jointProp->calibCurrentPosition.connect(callRefresh);
  jointProp->calibCurrentPosition.connect(callOnValueChange);
  jointProp->jointPower.connect(callOnPowerChange);
  onPowerChange(jointProp->jointPower.get());
  onValueChange(jointProp->calibCurrentPosition.get());
}
void CursorTypeAdapter<int,EntryDestination::CalibPot>::onInvisible(){
  //jointProp->calibCurrentPosition.disconnect(callRefresh);
  jointProp->calibCurrentPosition.disconnect(callOnValueChange);
  jointProp->jointPower.disconnect(callOnPowerChange);
}
int CursorTypeAdapter<int,EntryDestination::CalibPot>::getValue(){
 static int prev = jointProp->calibCurrentPosition.get(); //default starting value
  if(jointProp->jointState.isConnected()) { //robot is connected
    int now = jointProp->calibCurrentPosition.get();
    if(now != helium::AbsMotorSign::INVALIDPOS && now != helium::AbsMotorSign::FREEPOS && now != helium::AbsMotorSign::MOVINGPOS) { //not weird value
      prev = now;
    }
  }
  return prev;
}


CursorTypeAdapter<int,EntryDestination::CalibMin>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CalibCursorTypeAdapter<EntryDestination::CalibMin>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}

CursorTypeAdapter<int,EntryDestination::CalibZero>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CalibCursorTypeAdapter<EntryDestination::CalibZero>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}

CursorTypeAdapter<int,EntryDestination::CalibMax>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CalibCursorTypeAdapter<EntryDestination::CalibMax>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}


RobotCursorWidget::RobotCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib,helium::ValueHub<cfg::guide::SyncType::Type> &psyncState,coldet::ColdetManager *coldetm):
  SignallingCursor<EntryDestination::Robot,cfg::guide::CursorType::ROBOT>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib,psyncState)
{
}
void RobotCursorWidget::setValue(double val){
  //  cout << "cursor setvalue valid " << std::setprecision(10) << val << " indeg " << helium::getRadtoDeg(val) << " becomes ";
  validateValue(val);
  //cout <<  val << " "<< helium::getRadtoDeg(val) << endl;
  jointProp->desiredTarget.assure(val);
}
FrameCursorWidget::FrameCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib,helium::ValueHub<cfg::guide::SyncType::Type> &psyncState):
  SignallingCursor<EntryDestination::Frame,cfg::guide::CursorType::FRAME>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib,psyncState)
{
}
void FrameCursorWidget::setValue(double val){
  //cout << "framecursorwidget setvalue " << val << endl;
  validateValue(val);
  jointProp->frameValue.set<GuideEvent::DIRECTJOINTCHANGE>(std::make_pair(helium::VALUEFRAME,val));
}
CalibRobotCursorWidget::CalibRobotCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
StateCursor<int,EntryDestination::CalibRobot,true>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}
void CalibRobotCursorWidget::setValue(int val){
  jointProp->absTarget.assure(val);
}
int CalibRobotCursorWidget::getIncrement(){
  return 10; 
}

TouchViewRobotCursorWidget::TouchViewRobotCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib,coldet::ColdetManager *coldetm):
  StateCursor<double,EntryDestination::Robot,false>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}
