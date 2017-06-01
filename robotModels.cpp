/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include <helium/graphics/glHeader.h>
#include <helium/graphics/glUtil.h>
#include <time.h>
#include "robotModels.h"


using namespace std;

RobotModels::RobotModels(GuideState& pgs):
			 //,helium::ValueHub<bool> &rc):
  gs(pgs),
  modelDimensions(gs.joints.modelFilename.c_str()),

  robotModel(&modelDimensions,gs.joints),
  frameModel(robotModel,NULL,gs.joints),
  timelineModel(robotModel,NULL),
  captureModel(robotModel,NULL),
  fixModel(robotModel,NULL),
  coldetModel(robotModel,NULL),

  mainCanvasArea(frameModel,robotModel,fixModel),
  timelineArea(timelineModel),
  gyroArea(gs.pref.hasGyro?new ModelArea(robotModel):NULL),
  frameWindow(frameModel),
  robotWindow(robotModel),
  fixWindow(fixModel),
  captureWindow(captureModel),
  callEvaluateRobotBGColor(this,gs.joints.size())
  //callEvaluateBGColorDirect(this),
  //callOnCollideChange(this,gs.joints.size())
{
  for(size_t i=0; i<gs.joints.size();i++){
    gs.joints[i].displayedTarget.connect(callEvaluateRobotBGColor[i]);
  }

  timelineArea.setSize(gs.settings.sizes.model.w,gs.settings.sizes.model.h);
  mainCanvasArea.setSize(gs.settings.sizes.model.w,gs.settings.sizes.model.h);

  //mainCanvasArea.evalSetRobotMovingBG.connect(callEvaluateBGColorDirect);
  //rc.connect(mainCanvasArea.callSetModeltoRobot);
  double black[3] = {0,0,0};
  if(gyroArea.get())    
    gyroArea->setBG(black);
}

void RobotModels::setCaptureWindowTransient(Gtk::Window &window) {
  captureWindow.set_transient_for(window);
}

/// \todo find a better way to cache instead of finding everytime?
void RobotModels::evaluateRobotBGColor() {

  double color[3] = {1,1,1};

  /// loop through all joints for every joint change to evaluate the background color
  bool sendWhite = true;
  for(size_t i=0; i<gs.joints.size();i++){
    if(gs.joints[i].displayedTarget.get() == helium::motor::Interface::INVALIDPOS ||
       gs.joints[i].displayedTarget.get() == helium::motor::Interface::FREEPOS ||
       gs.joints[i].displayedTarget.get() == helium::motor::Interface::MOVINGPOS) {
      sendWhite = false;
      break;
    }
    if(gs.joints[i].displayedTarget.get() == helium::motor::Interface::COLLISIONPOS) {
      for(int i=0;i<3;i++)
	color[i] = gs.settings.colors.reds[0].data[i];
      break;
    }
      
  }
  if(!sendWhite) {
    for(int i=0;i<3;i++)
      color[i] = gs.settings.colors.movingModel.data[i];
  }

  //set color for both mainCanvasArea and modelWindow
  mainCanvasArea.signalSetBG(make_pair<int,double*>((int)cfg::guide::SwitchingModelArea::ROBOT,color));
  robotWindow.setBG(color);
}


void RobotModels::getPostureSnap(RobotModels::PostureSnap& p, const helium::Posture& pose, int w, int h,double* color) {
  mainCanvasArea.getBox().hide();
  p.resize(w*h*3);
  captureWindow.getPostureSnap(p.data,pose.data,w,h,color);
  mainCanvasArea.getBox().show();
}


/////////////////// SCCM

RobotModels::RobotSCCM::RobotSCCM(const helium::convex::ConvexModel *cm, cfg::guide::Joints &joints): 
  RobotModels::SignallingConvexCollisionModel(cm),
  callRotateRobot(this,joints.size())
{
  ///connect displayedTarget to rotate \n
  for(size_t i=0; i<joints.size();i++){
    joints[i].displayedTarget.connect(callRotateRobot[i]);
  }
}
void RobotModels::RobotSCCM::crotate(double value, int id) {
  if(value != helium::motor::Interface::INVALIDPOS && value != helium::motor::Interface::FREEPOS && value != helium::motor::Interface::MOVINGPOS) {
    rotate(id,value);    
  }
}

RobotModels::FrameSCCM::FrameSCCM(const ConvexCollisionModel& ccm, double* d, cfg::guide::Joints &joints):
  RobotModels::SignallingConvexCollisionModel(ccm,d),
  callRotateFrame(this,joints.size())
{
  ///connect frameValue to rotate (through updateFrame function) \n
  for(size_t i=0; i<joints.size();i++){
    joints[i].frameValue.connect(callRotateFrame[i]);
  }
}

void RobotModels::FrameSCCM::crotate(helium::FrameValue value, int id) {
  rotate(id,value.second);
}

RobotModels::SignallingConvexCollisionModel::SignallingConvexCollisionModel(const helium::convex::ConvexModel *cm):
  helium::convex::ConvexCollisionModel(cm)
{
}

RobotModels::SignallingConvexCollisionModel::SignallingConvexCollisionModel(const ConvexCollisionModel& ccm, double* d):
  helium::convex::ConvexCollisionModel(ccm,d)
{
}


void RobotModels::SignallingConvexCollisionModel::rotate(const helium::Posture &p) {
  rotate(p.data);
}

void RobotModels::SignallingConvexCollisionModel::rotate(const double *v) {
  helium::convex::ConvexCollisionModel::rotate(v);
  signalRotated();
}

void RobotModels::SignallingConvexCollisionModel::rotate(int j,double v) {
  helium::convex::ConvexCollisionModel::rotate(j,v);
  signalRotated();
}
////////////////////////////// ModelArea

RobotModels::ModelArea::ModelArea(RobotModels::SignallingConvexCollisionModel& c):
  RobotModels::ModelAreaBase(c,&drawConvex),
  drawConvex(&c)
{
  drawConvex.setZoom(0.6); /// set the model to be zoomed in to fill on the modelarea
}

RobotModels::ModelAreaBase::ModelAreaBase(RobotModels::SignallingConvexCollisionModel& c, helium::convex::DrawConvex *pdrawConvex):
  model(c),
  w(800),h(600),
  scale(1),
  drawConvex(pdrawConvex),
  drawConnection(drawConvex),
  resizeConnection(this),
  callRotate(&c,helium::FuncPlaceHolder<RobotModels::SignallingConvexCollisionModel,const double*,&RobotModels::SignallingConvexCollisionModel::rotate>()),
  callUpdate(this,helium::FuncPlaceHolder<RobotModels::ModelAreaBase,void,&RobotModels::ModelAreaBase::update>()), ///< set the connection to the correct function
  callSetBG(this)
{
  vbox.add(*getWidget());
  vbox.show_all_children();
  setSize(w,h);

  vbox.add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK |  Gdk::POINTER_MOTION_MASK | Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK);
  vbox.signal_key_press_event().connect(sigc::mem_fun(this,&RobotModels::ModelAreaBase::onGdkKeyPress));/// connect signal for key press on glArea onto function
  vbox.set_flags(Gtk::CAN_FOCUS); /// needs to be able to focus in order to get key events
  vbox.signal_enter_notify_event().connect(sigc::mem_fun(this,&RobotModels::ModelAreaBase::onMouseOver));

  setPosture.connect(callRotate); /// connect rotate function with setPosture

  this->onDraw.connect(drawConnection); /// connect drawConvex draw function
  this->onResize.connect(resizeConnection); /// connect resizing drawConvex
  model.signalRotated.connect(callUpdate); /// connect ccm rotated signal to trigger widget redrawn
}

RobotModels::ModelAreaBase::~ModelAreaBase() {
  model.signalRotated.disconnect(callUpdate);
}

Gtk::EventBox& RobotModels::ModelAreaBase::getBox() {
  return vbox;
}

int RobotModels::ModelAreaBase::getWidth() {
  return w;
}
int RobotModels::ModelAreaBase::getHeight() {
  return h;
}

bool RobotModels::ModelAreaBase::onMouseOver(GdkEventCrossing* event) {
  vbox.grab_focus();
  return false;
}

bool RobotModels::ModelAreaBase::onGdkKeyPress(GdkEventKey* event) {
  char c = *event->string;
  return onKeyPress(c,0,0);
}

bool RobotModels::ModelAreaBase::onKeyPress(char c, int x, int y){
  if(drawConvex->onKeyPushed(c,x,y) == helium::PrimitiveKeyboardInteraction::MANAGED) {
    drawConvex->draw();
    update();
    keyPressed();
    return true;
  }
  return false;
}

void RobotModels::ModelAreaBase::setViewMatrix(const double* d){
  drawConvex->setViewMatrix(d);
  update();
}
double* RobotModels::ModelAreaBase::getViewMatrix(){
  return drawConvex->getViewMatrix();
}
void RobotModels::ModelAreaBase::face(helium::convex::Faces d){
  drawConvex->face(d);
}
void RobotModels::ModelAreaBase::setWorldMatrix(const double* d){
  drawConvex->setWorldMatrix(d);
}
void RobotModels::ModelAreaBase::setBG(const double* b){
  drawConvex->setBG(b);
  update();
}


void RobotModels::ModelAreaBase::resizeDrawConvex(const std::pair<int,int>& s) {
  drawConvex->resize(s.first, s.second);
}

void RobotModels::ModelAreaBase::setSize(int pw,int ph) {
  w = pw;
  h = ph;
  vbox.set_size_request(w*scale,h*scale);
}
void RobotModels::ModelAreaBase::setScale(double pscale){
  scale = pscale;
  setSize(w,h);
} 
void RobotModels::ModelAreaBase::update() {
  vbox.queue_draw();
}

void RobotModels::ModelAreaBase::draw() {
  drawConvex->draw();
}
void RobotModels::ModelAreaBase::rotate(const double *d) {
  model.rotate(d);
}


RobotModels::MainCanvasArea::MainCanvasArea(RobotModels::SignallingConvexCollisionModel& pca,RobotModels::SignallingConvexCollisionModel& pcb,RobotModels::SignallingConvexCollisionModel& pcc):
  RobotModels::SwitchingModelArea(pca,pcb,pcc),
  callSetModeltoRobot(this),
  callSetModelHoverReach(this),
  callSetModelHoverFix(this),
  callSetAutoFixPosture(&pcc) 
{
}

  

RobotModels::SwitchingModelArea::SwitchingModelArea(RobotModels::SignallingConvexCollisionModel& pca,RobotModels::SignallingConvexCollisionModel& pcb,RobotModels::SignallingConvexCollisionModel& pcc):
  ModelArea(pca),
  model(cfg::guide::SwitchingModelArea::NUM),
  bgcolor(cfg::guide::SwitchingModelArea::NUM),
  callSetModel(this),
  callSetBG(this)
{
  model[cfg::guide::SwitchingModelArea::FRAME] = &pca;
  model[cfg::guide::SwitchingModelArea::ROBOT] = &pcb;
  model[cfg::guide::SwitchingModelArea::FIX] = &pcc;

  modelid.connect(callSetModel);
  signalSetBG.connect(callSetBG);

  for(int i=0;i<3;i++) {
    bgcolor[cfg::guide::SwitchingModelArea::FRAME][i] = 1;
    bgcolor[cfg::guide::SwitchingModelArea::ROBOT][i] = 1;
    bgcolor[cfg::guide::SwitchingModelArea::FIX][i] = 1;
  }

  modelid = (int)cfg::guide::SwitchingModelArea::FRAME;
}

void RobotModels::SwitchingModelArea::setBG(std::pair<int,double*> c) {
  for(int i=0;i<3;i++) {
    bgcolor[c.first][i] = c.second[i];
  }
  if(modelid == c.first) RobotModels::ModelArea::setBG(c.second);
  update();
}

void RobotModels::MainCanvasArea::setModeltoRobot(bool b) {
  if(b) {
    modelid = (int)cfg::guide::SwitchingModelArea::ROBOT;
  }
}

void RobotModels::MainCanvasArea::setModelHover(bool b) {  
  static int prev;
  if(b) {
    prev = modelid.get();
    modelid = cfg::guide::SwitchingModelArea::FIX;
  }
  else
    modelid = prev;
}

void RobotModels::SwitchingModelArea::setModel(int id) {
  static int prev = -1;

  //disconnections
  switch(prev) {
  case (int)cfg::guide::SwitchingModelArea::FRAME: 
    model[prev]->signalRotated.disconnect(callUpdate);
    break;
  case (int)cfg::guide::SwitchingModelArea::ROBOT: 
    model[prev]->signalRotated.disconnect(callUpdate);
    //signalSetRobotMovingBG.disconnect(callSetBG);
    break;
  case (int)cfg::guide::SwitchingModelArea::FIX: 
    model[prev]->signalRotated.disconnect(callUpdate);
    break;
  default: break;
  }

  prev = id;
  //double white[3] = {1,1,1};
  RobotModels::ModelArea::setBG(bgcolor[id]);

  //connections
  switch(id) {
  case (int)cfg::guide::SwitchingModelArea::FRAME: 
    //setBG(white);
    model[id]->signalRotated.connect(callUpdate);
    break;
  case (int)cfg::guide::SwitchingModelArea::ROBOT: 
    model[id]->signalRotated.connect(callUpdate);
    //signalSetRobotMovingBG.connect(callSetBG);
    //evalSetRobotMovingBG();
    break;
  case (int)cfg::guide::SwitchingModelArea::FIX: 
    //setBG(white);
    model[id]->signalRotated.connect(callUpdate);
    break;
  default:  modelid = 0; return;   // limit the model id
  }

  drawConvex.fastReplaceModel(model[id]);
  update();


}

/*
void RobotModels::SwitchingModelArea::switchModel() {
  modelA = !modelA;
  drawConvex.fastReplaceModel(modelA?&ca:&cb);

  ///disconnect connection to modelA, connect to modelB
  if(modelA) {
    cb.signalRotated.disconnect(callUpdate); 
    ca.signalRotated.connect(callUpdate); 
    if(signalSetRobotMovingBG)
      signalSetRobotMovingBG->disconnect(callSetBG);
    /// set background to white
    double white[3] = {1,1,1};
    setBG(white);
  }
  ///disconnect connection to modelB, connect to modelA
  else {
    ca.signalRotated.disconnect(callUpdate); 
    cb.signalRotated.connect(callUpdate); 
    if(signalSetRobotMovingBG)
      signalSetRobotMovingBG->connect(callSetBG);
    /// tell RobotModels to re-evaluate the BG color of robot
    if(evalSetRobotMovingBG)
      (*evalSetRobotMovingBG)();
  }
  update();
}
*/

void operator ++(RobotModels::ValueHubInt &a, int) {
  a = a.get()+1;
}
