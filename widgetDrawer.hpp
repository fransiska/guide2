/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

template<EntryDestination::Type ed,bool isBigJoint>
AngleLimitEntry<ed,isBigJoint>::AngleLimitEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<double>& pentry):
  StateEntry<ed,true,isBigJoint>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pentry) 
{
}

//** RobotEntryWidget
template<bool isBigJoint>
RobotEntryWidget<isBigJoint>::RobotEntryWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<double>& pentry):
  AngleLimitEntry<EntryDestination::Robot,isBigJoint>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pentry) 
{
}

template<bool isBigJoint>
void RobotEntryWidget<isBigJoint>::setValue(double v) {
  helium::degtoRad(v);
  this->jointProp->desiredTarget.assure(v);
}

template<bool isBigJoint>
void RobotEntryWidget<isBigJoint>::connect(helium::ConnReference<double> & c){
  this->jointProp->desiredTarget.connect(c);
}

template<bool isBigJoint>
void RobotEntryWidget<isBigJoint>::disconnect(helium::ConnReference<double> & c){
  this->jointProp->desiredTarget.disconnect(c);
}



template<EntryDestination::Type ed,bool isBigJoint>
void AngleLimitEntry<ed,isBigJoint>::onClicked(GdkEventButton * event){
  if(event->button == 1)
    this->onLeftClicked();
  else if(event->button == 3)
    this->onRightClicked();

  if(this->getState().warning) return;

  this->entry.setDefaultRange();

  double a,b;
  a = this->jointProp->getLimit(cfg::guide::CalibType::MIN);
  b = this->jointProp->getLimit(cfg::guide::CalibType::MAX);
  //a = ceil(a*10000)/10000;
  //b = floor(b*10000)/10000;
  helium::radtoDeg(a);
  helium::radtoDeg(b);
  a = ceil(a*100.0)/100.0;
  b = floor(b*100.0)/100.0;
  if(this->getValue() >= a && this->getValue() <= b) {
    this->entry.setRange(a,b); 
  }
  else {
    this->entry.setDefaultRange(); 
  }



  this->entry.setLinked(this);
}


template<EntryDestination::Type ed,bool isBigJoint>
DragData::Type AngleLimitEntry<ed,isBigJoint>::getDragType(){
  return DragData::ANGLE;
}

template<EntryDestination::Type ed,bool isBigJoint>
void AngleLimitEntry<ed,isBigJoint>::onDrag(){
  this->globalProp.dragData.angleValue = std::make_pair(this->getValue(),this->jointProp->isReverse());
}

template<EntryDestination::Type ed,bool isBigJoint>
void AngleLimitEntry<ed,isBigJoint>::onDrop(){
  double v = this->globalProp.dragData.angleValue.first;
  std::cout << "v " << v;  
  v = ((this->globalProp.dragData.angleValue.second != this->jointProp->isReverse()?-1:1)*v);

  std::cout << " v " << v;

  //limit the value
  if(v < helium::getRadtoDeg(this->jointProp->calib.relLim.minv))
    v = helium::getRadtoDeg(this->jointProp->calib.relLim.minv);
  else if(v > helium::getRadtoDeg(this->jointProp->calib.relLim.maxv))
    v = helium::getRadtoDeg(this->jointProp->calib.relLim.maxv);

  std::cout << " v " << v;

    setValue(v);
    //setValue((this->globalProp.dragData.angleValue.second != this->jointProp->isReverse()?-1:1)*v);

}
//** end of RobotEntryWidget

//** FrameEntryWidget

template<bool isBigJoint>
FrameEntryWidget<isBigJoint>::FrameEntryWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<double>& pentry):
  AngleLimitEntry<EntryDestination::Frame,isBigJoint>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pentry),
  callValueChanged( &valueChanged)
{
}

template<bool isBigJoint>
void FrameEntryWidget<isBigJoint>::setValue(double v) {
  //std::cout << "frameentrywidget setvalue " << v << std::endl;
  helium::degtoRad(v);
  this->jointProp->frameValue.template set<GuideEvent::DIRECTJOINTCHANGE>(std::make_pair(helium::VALUEFRAME,v));
}

template<bool isBigJoint>
void FrameEntryWidget<isBigJoint>::connect(helium::ConnReference<double> & c){
  this->jointProp->frameValue.connect(callValueChanged);
  valueChanged.connect(c);
  //jointProp->frameValue.connect(c);
}

template<bool isBigJoint>
void FrameEntryWidget<isBigJoint>::disconnect(helium::ConnReference<double> & c){
  valueChanged.disconnect(c); 
  this->jointProp->frameValue.disconnect(callValueChanged);
}

//** end of RobotEntryWidget

template<bool isBigJoint>
CalibRobotEntryWidget<isBigJoint>::CalibRobotEntryWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<int>& pentry):
  StateEntry<EntryDestination::CalibRobot,true,isBigJoint>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pentry) 
{
}
template<bool isBigJoint>
DragData::Type CalibRobotEntryWidget<isBigJoint>::getDragType(){
  return DragData::HEX;
}
template<bool isBigJoint>
void CalibRobotEntryWidget<isBigJoint>::onClicked(GdkEventButton * event){
 this->entry.setLinked(this);
}
template<bool isBigJoint>
void CalibRobotEntryWidget<isBigJoint>::onDrag(){
  this->globalProp.dragData.hexValue = this->getValue();
}
template<bool isBigJoint>
void CalibRobotEntryWidget<isBigJoint>::onDrop(){
  //int min,max;
  //this->entry.getDefaultRange(min,max);
  //if(d.hexValue < min || d.hexValue > max) return; //limit to range of entry
  setValue(this->globalProp.dragData.hexValue);
}
template<bool isBigJoint>
void CalibRobotEntryWidget<isBigJoint>::connect(helium::ConnReference<int> & c){ this->jointProp->absTarget.connect(c); }
template<bool isBigJoint>
void CalibRobotEntryWidget<isBigJoint>::disconnect(helium::ConnReference<int> & c){ this->jointProp->absTarget.disconnect(c); }
template<bool isBigJoint>
void CalibRobotEntryWidget<isBigJoint>::setValue(int v) { this->jointProp->absTarget.assure(v); }



//** StateEntry false
template<EntryDestination::Type ed>
StateEntry<ed,false,false>::StateEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  EntryTypeAdapter<ed>(pprop,pjointProp,pglobalProp,pjointGlobalProp)
{
  this->prop.name = "entry " + helium::toString(ed);
}

template<EntryDestination::Type ed>
double* StateEntry<ed,false,false>::getBgColor()  {
  return this->globalProp.settings.colors.disablebg.data;
}


template<EntryDestination::Type ed>
void StateEntry<ed,false,false>::draw()  {

  
  //setLocation();

  //rectangle
  this->getCtx().getcr()->save();
  this->getCtx().getcr()->rectangle(floor(this->prop.anchor().x/*+this->globalProp.settings.sizes.stroke*/-this->prop.width()/2),
				    floor(this->prop.anchor().y/*+this->globalProp.settings.sizes.stroke*/-this->prop.height()/2), 
				    this->prop.width()-this->globalProp.settings.sizes.stroke, this->prop.height()-this->globalProp.settings.sizes.stroke);

  //bg color
  if(this->getState().mouseState == GWidgetState::MOUSEDOWN)
    this->getCtx().getcr()->set_source_rgb(this->globalProp.settings.colors.selected[0],this->globalProp.settings.colors.selected[1],this->globalProp.settings.colors.selected[2]);
  else {
    double* bg = getBgColor();
    this->getCtx().getcr()->set_source_rgb(bg[0],bg[1],bg[2]);
  }
  this->getCtx().getcr()->fill_preserve();

  //box border stroke color
  //if(this->getState().enable) {
  double* rgb = this->getColor();
  this->getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);
  //}

  this->getCtx().getcr()->stroke();
  this->getCtx().getcr()->restore();

  //insert text
  this->getCtx().getcr()->save();
  this->getCtx().getcr()->translate(this->prop.anchor().x-this->prop.width()/2,this->prop.anchor().y-this->prop.height()/2);
  this->getCtx().getcr()->set_font_size(12); 
  this->getCtx().getcr()->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
  Cairo::TextExtents txext;
  Glib::ustring text = this->getValueString();

  rgb = NULL;
  //text color
  if(!this->getState().enable || this->getState().warning)
    rgb = this->globalProp.settings.colors.disable.data;
  else if (this->overLimit)
    rgb = this->globalProp.settings.colors.error.data;
  if(rgb)
    this->getCtx().getcr()->set_source_rgb(rgb[0],rgb[1],rgb[2]);

  this->getCtx().getcr()->get_text_extents( text, txext );
  this->getCtx().getcr()->move_to(0,this->prop.height()/2+txext.height/2);
  this->getCtx().getcr()->show_text(text);
  this->getCtx().getcr()->set_source_rgb(0, 0, 0);
  this->getCtx().getcr()->stroke();
  this->getCtx().getcr()->restore();

}

template<EntryDestination::Type ed>
void StateEntry<ed,false,false>::onLeftClicked()  {
  this->jointGlobalProp.bigJointId = this->jointProp->getId();
}

template<EntryDestination::Type ed>
void StateEntry<ed,false,false>::setLocation(double scale)  {
  this->prop.anchor() = this->prop.anchor.def;
  helium::mulScalar(this->prop.anchor(),this->globalProp.scale.get());

  //small joint
  if(this->jointProp->isLeftDescription()) 
    this->prop.anchor().x -= (this->globalProp.settings.sizes.entrysOffset*this->globalProp.scale.get()+
			      this->globalProp.settings.sizes.entry.w/2*3-this->globalProp.settings.sizes.stroke+this->globalProp.settings.sizes.entrySpace);
  else
    this->prop.anchor().x += (this->globalProp.settings.sizes.entrysOffset)*this->globalProp.scale.get()+this->globalProp.settings.sizes.entry.w/2;
  
  if(ed == EntryDestination::Frame || ed == EntryDestination::CalibMin || ed == EntryDestination::CalibMax || ed == EntryDestination::CalibZero)
      this->prop.anchor().x += this->prop.width()+this->globalProp.settings.sizes.entrySpace;
  this->prop.anchor().y += this->prop.height()/2;
}

///for bigjoint
template<EntryDestination::Type ed>
StateEntry<ed,false,true>::StateEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp):
  StateEntry<ed,false,false>(pprop,pjointProp,pglobalProp,pjointGlobalProp)
{
}
template<EntryDestination::Type ed>
void StateEntry<ed,false,true>::setLocation(double scale)  {
  this->prop.anchor() = this->prop.anchor.def;
  helium::mulScalar(this->prop.anchor(),this->globalProp.scale.get());
  //big joint
  Point2D u,v;
  this->jointProp->getU(u);
  this->jointProp->getV(v);
  helium::mulScalar(v,(this->globalProp.settings.sizes.entryOffset)*this->globalProp.scale.get());
  //if(C == CursorType::FRAME)
  if(ed == EntryDestination::Frame || ed == EntryDestination::CalibMin || ed == EntryDestination::CalibMax || ed == EntryDestination::CalibZero)
    helium::mulScalar(v,-1);
  helium::sum(this->prop.anchor(),v);
  helium::sum(this->prop.anchor(),this->getOffset());
}


//** end of StateEntry false

//** StateEntry true
template<EntryDestination::Type ed, bool isBigJoint>
StateEntry<ed,true,isBigJoint>::StateEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<typename EntryTypeAdapter<ed>::DataType>& pentry):
  StateEntry<ed,false,isBigJoint>(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  entry(pentry)
{
  this->prop.name = this->prop.name + " true";
}

template<EntryDestination::Type ed, bool isBigJoint>
double* StateEntry<ed,true,isBigJoint>::getBgColor()  {
  return this->globalProp.settings.colors.bgs[0].data;
}
template<EntryDestination::Type ed, bool isBigJoint>
void StateEntry<ed,true,isBigJoint>::setVisibleEntry(bool b) {
  this->setVisible(b);
}
template<EntryDestination::Type ed, bool isBigJoint>
void StateEntry<ed,true,isBigJoint>::getLocation(Point2D& loc)  {
  loc.x = this->prop.anchor().x-this->prop.width()/2-this->globalProp.settings.sizes.stroke;
  loc.y = this->prop.anchor().y-this->prop.height()/2-this->globalProp.settings.sizes.stroke;
}
template<EntryDestination::Type ed,bool isBigJoint>
typename EntryTypeAdapter<ed>::DataType StateEntry<ed,true,isBigJoint>::getEntryValue() {
  return this->getValue();
}
//** end of StateEntry true


//** Entry Dest Calib
template<EntryDestination::Type ed>
CalibEntryTypeAdapter<ed>::CalibEntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp): 
  BasicEntry(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  callOnValueChange(this)
{
}



template<EntryDestination::Type ed>
std::string CalibEntryTypeAdapter<ed>::getValueString() {
  int now = getValue();
  
  if(now == helium::AbsMotorSign::INVALIDPOS)
    return "inv";
  
  else if(now == helium::AbsMotorSign::FREEPOS)
    return "free";
  
  else if(now == helium::AbsMotorSign::MOVINGPOS)
    return "mov";
  
  else { //normal
    std::stringstream ss;
    ss << std::hex << now;
    return ss.str();
  }
  return "err";
}

template<EntryDestination::Type ed>
void CalibEntryTypeAdapter<ed>::onVisible() {
  jointProp->calib[ed-EntryDestination::Calib-1].connect(callOnValueChange);
  refresh();
}

template<EntryDestination::Type ed>
void CalibEntryTypeAdapter<ed>::onInvisible() {
  jointProp->calib[ed-EntryDestination::Calib-1].disconnect(callOnValueChange);
}

template<EntryDestination::Type ed>
int CalibEntryTypeAdapter<ed>::getValue() {
  return jointProp->calib[ed-EntryDestination::Calib-1].get();
}

template<EntryDestination::Type ed>
double* CalibEntryTypeAdapter<ed>::getColor() {
  return globalProp.settings.colors.calibs[ed-EntryDestination::Calib-1].data;
}

template<EntryDestination::Type ed>
void CalibEntryTypeAdapter<ed>::modifyOffset(Point2D &u) {
  jointProp->getU(u);
  u.x *=  (-1+(int)(ed-EntryDestination::Calib-1)) * (prop.width() + globalProp.settings.sizes.stroke);
  u.y *=  (-1+(int)(ed-EntryDestination::Calib-1)) * (prop.height() + globalProp.settings.sizes.stroke);
}

//** end of Entry Dest Calib

//** CalibEntryWidget
template<EntryDestination::Type ed, bool isBigJoint>
CalibEntryWidget<ed,isBigJoint>::CalibEntryWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<int>& pentry):
  StateEntry<ed,true,isBigJoint>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pentry)
{
}
template<EntryDestination::Type ed, bool isBigJoint>
void CalibEntryWidget<ed,isBigJoint>::onClicked(GdkEventButton * event) {
  this->entry.setLinked(this);
  switch((cfg::guide::CalibType::Type)(ed-EntryDestination::Calib-1)) {
  case cfg::guide::CalibType::ZERO: 
    this->entry.setRange(this->jointProp->calib.absLim.minv.get(),this->jointProp->calib.absLim.maxv.get());break;
  case cfg::guide::CalibType::MIN: 
    this->entry.setMaxRange(this->jointProp->calib.absLim.zero.get());break;
  case cfg::guide::CalibType::MAX: 
    this->entry.setMinRange(this->jointProp->calib.absLim.zero.get());break;
    
  default: break;
  }
  
}
template<EntryDestination::Type ed, bool isBigJoint>
void CalibEntryWidget<ed,isBigJoint>::setRange(int &min, int &max) {
  this->jointProp->getCalibLimit((cfg::guide::CalibType::Type)(ed-EntryDestination::Calib-1),min,max);
}

template<EntryDestination::Type ed, bool isBigJoint>
void CalibEntryWidget<ed,isBigJoint>::setValue(int v) {
  this->jointProp->setCalib((cfg::guide::CalibType::Type)(ed-EntryDestination::Calib-1),v);
}


template<EntryDestination::Type ed, bool isBigJoint>
void CalibEntryWidget<ed,isBigJoint>::connect(helium::ConnReference<int>& c){
  this->jointProp->calib[(int)(ed-EntryDestination::Calib-1)].connect(c);
}

template<EntryDestination::Type ed, bool isBigJoint>
void CalibEntryWidget<ed,isBigJoint>::disconnect(helium::ConnReference<int>& c){
  this->jointProp->calib[(int)(ed-EntryDestination::Calib-1)].disconnect(c);
}
template<EntryDestination::Type ed, bool isBigJoint>
DragData::Type CalibEntryWidget<ed,isBigJoint>::getDragType(){
  return DragData::HEX;
}
template<EntryDestination::Type ed, bool isBigJoint>
void CalibEntryWidget<ed,isBigJoint>::onDrag() {
  this->globalProp.dragData.hexValue = this->getValue();
}
template<EntryDestination::Type ed, bool isBigJoint>
void CalibEntryWidget<ed,isBigJoint>::onDrop() {
  //int min,max;
  //entry.getDefaultRange(min,max);
  //if(globalProp.dragData.hexValue < min || globalProp.dragData.hexValue > max) return; //limit to range of entry
  setValue(this->globalProp.dragData.hexValue);
}
//** end of CalibEntryWidget

template<cfg::guide::CursorType::Type type>
CopyArrowWidget<type>::CopyArrowWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection> &pconnection, helium::ValueHub<cfg::guide::SyncType::Type> &psyncState, helium::ValueHub<cfg::guide::GuideView::Type> &pview):
  GWidget(WidgetProperty(pos,
			 pglobalProp.settings.sizes.frame.w*3-pglobalProp.settings.sizes.arrowBox.w*2+pglobalProp.settings.sizes.stroke,
			 pglobalProp.settings.sizes.arrowBox.h/2)
	  ,pglobalProp),
  ConnectionDependent(pconnection),
  ViewChangeable(pview),
  syncState(psyncState),
  connection(pconnection),
  view(pview),
  callOnSyncChange(this)
{
  syncState.connect(callOnSyncChange);
}

template<cfg::guide::CursorType::Type type>
double* CopyArrowWidget<type>::getColor() {
  return globalProp.settings.colors.cursor[type].data;
}

template<cfg::guide::CursorType::Type type>
void CopyArrowWidget<type>::onNewView(cfg::guide::GuideView::Type view) {
  if(view != cfg::guide::GuideView::CALIB && connection.get().state == helium::Connection::CONNECTED)
    setVisible(true);
  else
    setVisible(false);
}

template<cfg::guide::CursorType::Type type>
void CopyArrowWidget<type>::draw() {
  //std::cout << "drawing copy arrow widget " << std::endl;
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x-prop.width()/2),floor(prop.anchor().y-prop.height()/2));

  int width = prop.width()/2;
  adjustInverse(width);

  getCtx().getcr()->move_to(0,round(prop.height()/4));
  getCtx().getcr()->line_to(width,round(prop.height()/4));
  getCtx().getcr()->line_to(width,0);
  getCtx().getcr()->line_to(width*2,round(prop.height()/2));
  getCtx().getcr()->line_to(width,round(prop.height()));
  getCtx().getcr()->line_to(width,round(prop.height()/4*3));
  getCtx().getcr()->line_to(0,round(prop.height()/4*3));
  getCtx().getcr()->close_path();

  double *rgb = globalProp.settings.colors.disable.data;
  if(getState().mouseState == GWidgetState::MOUSEDOWN)
    rgb = getColor();
  else if(!getState().enable)
    rgb = globalProp.settings.colors.selected.data;
  getCtx().getcr()->set_source_rgb(rgb[0], rgb[1], rgb[2]);

  getCtx().getcr()->fill_preserve();
  getCtx().getcr()->restore();
  getCtx().getcr()->set_line_join(Cairo::LINE_JOIN_BEVEL);
  getCtx().getcr()->stroke();
}

template<cfg::guide::CursorType::Type type>
void CopyArrowWidget<type>::onClick(GdkEventButton * event) {
  setMouseState(GWidgetState::MOUSEDOWN);
}

template<cfg::guide::CursorType::Type type>
void CopyArrowWidget<type>::onRelease(GdkEventButton * event) {
  if(syncState != cfg::guide::SyncType::ALLCONNECT)
    setMouseState(GWidgetState::NORMAL);
}
template<cfg::guide::CursorType::Type type>
void CopyArrowWidget<type>::onSyncChange(cfg::guide::SyncType::Type sync) {
  if(sync == cfg::guide::SyncType::ALLCONNECT)
    setMouseState(GWidgetState::MOUSEDOWN);
  else
    setMouseState(GWidgetState::NORMAL);
}
template<cfg::guide::CursorType::Type type>
void CopyArrowWidget<type>::onClicked(GdkEventButton * event) {
  //std::cout << "copyarrowwidget onclicked" << std::endl;
  std::stringstream ss;
  if(event->button == 1 && syncState != cfg::guide::SyncType::ALLCONNECT) {
    syncState.assure(cfg::guide::SyncType::ALLCOPY); 
  }
  else {
    if(event->button == 3) {
      if(syncState != cfg::guide::SyncType::ALLCONNECT) {
	syncState.assure(cfg::guide::SyncType::ALLCONNECT);
      }
      else {
	syncState.assure(cfg::guide::SyncType::DISCONNECT);
      }
    }
  }
}

template<cfg::guide::CursorType::Type type>
void CopyArrowWidget<type>::onConnected() {
  if(view != cfg::guide::GuideView::CALIB) {
    setVisible(true);
  }
}

template<cfg::guide::CursorType::Type type>
void CopyArrowWidget<type>::onDisconnected() {
  setVisible(false);
}

template<typename T>
SignallingClickableWidget<T>::SignallingClickableWidget(const WidgetProperty &pprop, WidgetGlobalProperty& pglobalProp):
  GWidget(pprop,pglobalProp)
{
}

template<typename T>
void SignallingClickableWidget<T>::onClick(GdkEventButton * event) {
  setMouseState(GWidgetState::MOUSEDOWN);
}
template<typename T>
void SignallingClickableWidget<T>::onRelease(GdkEventButton * event) {
  setMouseState(GWidgetState::NORMAL);
}

template<FrameSaveTo::Type type>
FrameBox<type>::FrameBox(Point2D pos, WidgetGlobalProperty& pglobalProp):
  SignallingClickableWidget<FrameSaveTo::Type>(WidgetProperty(Point2D(pos.x+((int)type)*((pglobalProp.settings.sizes.frame.w)),pos.y),
							      pglobalProp.settings.sizes.frame.w+pglobalProp.settings.sizes.stroke,pglobalProp.settings.sizes.frame.h+pglobalProp.settings.sizes.stroke/*,true*/),
					       pglobalProp)
{
  prop.name = gwidget::FrameSaveTo::strings[type];
}

//template<FrameSaveTo::Type type>
//bool FrameBox<type>::isResponsible(const Point2D &e) {
//  bool cw = fabs(e.x - prop.anchor().x ) < (prop.width()/2);
//  bool ch = fabs(e.y - prop.anchor().y ) < (prop.height()/2);
//  return cw && ch;
//}


template<FrameSaveTo::Type type>
void FrameBox<type>::onClicked(GdkEventButton * event) {
  onClickedSignal(type);
}

template<FrameSaveTo::Type type>
void FrameBox<type>::draw() {
  getCtx().getcr()->save();
  getCtx().getcr()->translate(floor(prop.anchor().x-prop.width()/2+globalProp.settings.sizes.stroke/2),floor(prop.anchor().y-prop.height()/2));
  getCtx().getcr()->rectangle(0,0,prop.width()-globalProp.settings.sizes.stroke,prop.height()-globalProp.settings.sizes.stroke);

  double *rgb = globalProp.settings.colors.frameGradations[type].data;
  if(getState().mouseState == GWidgetState::MOUSEDOWN)
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
  getCtx().getcr()->translate(floor(prop.anchor().x+globalProp.settings.sizes.stroke/2),floor(prop.anchor().y));
  //getCtx().getcr()->translate(prop.anchor().x+prop.width()/2,prop.anchor().y+prop.height()/2);
  getCtx().getcr()->move_to(-txext.width/2-globalProp.settings.sizes.stroke/2,-txext.height/2);
  getCtx().getcr()->set_source_rgb(1,1,1);
  getCtx().getcr()->show_text(text);

  text = "Frame";
  getCtx().getcr()->get_text_extents( text, txext );
  getCtx().getcr()->move_to(-txext.width/2-globalProp.settings.sizes.stroke/2,txext.height*3/2);
  getCtx().getcr()->show_text(text);
  getCtx().getcr()->stroke();
  getCtx().getcr()->restore();
}


template <typename T, cfg::guide::GuideView::Type type>
BigJointWidget<T,type>::BigJointWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::Joints &pjoints,helium::ValueHub<cfg::guide::GuideView::Type> &pview,int widgetsNumber,LinkableEntry<T>*pentry):
  JointElementWidgets(WidgetProperty(pos,pglobalProp.settings.sizes.bigJoint,pglobalProp.settings.sizes.bigJoint+pglobalProp.settings.sizes.bigJointTagHeight),jointProp,pglobalProp,pjointGlobalProp,widgetsNumber),//-pglobalProp.settings.sizes.entry.h/2
  ViewChangeable(pview),
  entry(pentry),
  joints(pjoints),
  motorW(WidgetProperty(pos,pglobalProp.settings.sizes.bigJoint,pglobalProp.settings.sizes.bigJoint),jointProp,pglobalProp,pjointGlobalProp),
  powerSW(WidgetProperty(pos,pglobalProp.settings.sizes.powerSwitch.w,pglobalProp.settings.sizes.powerSwitch.h),jointProp,pglobalProp,pjointGlobalProp),
  barW(WidgetProperty(pos,pglobalProp.settings.sizes.bar.w,pglobalProp.settings.sizes.bar.h),NULL,pglobalProp,pjointGlobalProp,pjoints.defaultCalib,pjoints),
  curId(-1),
  callSetJoint(this)
{
  //setVisible(false);
}

template <typename T, cfg::guide::GuideView::Type type>
void BigJointWidget<T,type>::onNewView(cfg::guide::GuideView::Type view) {
  if(view == type) {
    jointGlobalProp.bigJointId.connect(callSetJoint);
    if(jointGlobalProp.bigJointId.get() >= 0)
      setJoint(jointGlobalProp.bigJointId.get());
  }
}

template <typename T, cfg::guide::GuideView::Type type>
void BigJointWidget<T,type>::onPrevView(cfg::guide::GuideView::Type view) {
  if(view == type) {
    if(entry)
      entry->entry.hide();
    jointGlobalProp.bigJointId.disconnect(callSetJoint);
    setJoint(-1);
  }  
}

template <typename T, cfg::guide::GuideView::Type type>
void BigJointWidget<T,type>::setJoint(int i) {
  //disconnect from current joint 
  if(curId != -1) {
    setVisible(false);
  }
  // set jwp to null
  if(i == -1) {
    jointProp = NULL;
    setJointPointer(jointProp);
  }
  // set jwp to other joint, connect to other joint
  else {
    jointProp = &joints[i];     
    setJointPointer(jointProp); 
    setVisible(true);
  }
  curId = i;
}

template <typename T, cfg::guide::GuideView::Type type>
void BigJointWidget<T,type>::onVisible() {
  for(int i=widgets.size()-1;i>=0;i--) {
    widgets[i]->setVisible(true);
  }
}

template <typename T, cfg::guide::GuideView::Type type>
void BigJointWidget<T,type>::onInvisible() {
  for(int i=widgets.size()-1;i>=0;i--)
    widgets[i]->setVisible(false);
}


//** end of BigJointWidget


template<typename T>
BarPositionWidget<T>::BarPositionWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  JointElementWidget(pprop,pjointProp,pglobalProp,pjointGlobalProp),
  defaultCalib(pdefaultCalib)
{
}

/** \brief get position in x,y given a value
 * \param val value
 */
template<typename T>
Point2D BarPositionWidget<T>::getValtoPos(T val) {
  /// \todo DELETE?
  //limit to viewable view


  if(isnan(val))
    val = 0;
  if(val > getMax())
    val = getMax();
  else if(val < getMin())
    val = getMin();

  Point2D a = jointProp->getU();
  helium::mulScalar(a,(-0.5+(double)(val-getMin())/(double)(getMax()-getMin()))*globalProp.settings.sizes.bar.w*globalProp.scale.get());

  prop.anchor() = prop.anchor.def;
  helium::mulScalar(prop.anchor(),globalProp.scale.get());

  helium::sum(a,prop.anchor());
  return a;
}

template<typename T>
T BarPositionWidget<T>::getPostoVal(Point2D p) {
  //limit to viewable val

  Point2D anc = prop.anchor.def;
  helium::mulScalar(anc,globalProp.scale.get());
  helium::sub(p,anc);
  Point2D u = jointProp->getU();
  double val = u.x * p.x + u.y * p.y;
  val = ( val / (globalProp.settings.sizes.bar.w*globalProp.scale.get()) + 0.5 ) * (getMax()-getMin()) + getMin();

  if(val > getMax())
    val = getMax();
  else if(val < getMin())
    val = getMin();

  return val;
}

//BarWidget
template<typename T>
BarWidget<T>::BarWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,cfg::guide::DefaultCalib &pdefaultCalib, cfg::guide::Joints& pjoints):
  BarPositionWidget<T>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pjoints.defaultCalib),
  joints(pjoints)
{
}

template<typename T>
void BarWidget<T>::draw() {
  Point2D p,p1,p2;
  /*
  this->getCtx().getcr()->save();
  this->getCtx().getcr()->translate(this->prop.anchor.def.x,this->prop.anchor.def.y);
  this->getCtx().getcr()->arc(0, 0, 20, 0.0, 2.0 * M_PI);
  this->getCtx().getcr()->fill_preserve();
  this->getCtx().getcr()->stroke();
  this->getCtx().getcr()->restore();

  return;
  */


  //disable bar

  this->getCtx().getcr()->set_line_width(this->prop.height());

  p1 = BarPositionWidget<T>::getValtoPos(this->getMinLimit()); //barPos.getMinBarPos();//barPos.getBarPos(barPos.getAbsMin());
  p2 = BarPositionWidget<T>::getValtoPos(this->getMaxLimit()); //barPos.getMaxBarPos();//barPos.getBarPos(barPos.getAbsMax());

  //helium::sum(p1,this->prop.anchor());
  //helium::sum(p2,this->prop.anchor());

  this->getCtx().getcr()->save();
  this->getCtx().getcr()->set_source_rgb(0.5, 0.5, 0.5);
  this->getCtx().getcr()->move_to(p1[0],p1[1]);
  this->getCtx().getcr()->line_to(p2[0],p2[1]);
  this->getCtx().getcr()->stroke();
  this->getCtx().getcr()->restore(); 
  
  //bar limit
  Point2D pm[2];
  pm[0] = BarPositionWidget<T>::getValtoPos(this->getMin()); // barPos.getMinBarPos();
  pm[1] = BarPositionWidget<T>::getValtoPos(this->getMax()); //barPos.getMaxBarPos();

  T po[2];

  po[0] = getMinLimit(); // joint's minimum calib value
  po[1] = getMaxLimit(); //(cfg::guide::CalibType::MAX);

  for(int i=0; i<2; i++) {
    this->getCtx().getcr()->save();
    this->getCtx().getcr()->set_source_rgb(this->globalProp.settings.colors.error[0], this->globalProp.settings.colors.error[1], this->globalProp.settings.colors.error[2]);
    p2 = BarPositionWidget<T>::getValtoPos(po[i]);//,true
    //helium::sum(pm[i],this->prop.anchor());
    //helium::sum(p2,this->prop.anchor());
    this->getCtx().getcr()->move_to(pm[i][0],pm[i][1]);
    this->getCtx().getcr()->line_to(p2[0],p2[1]);
    this->getCtx().getcr()->stroke();
    this->getCtx().getcr()->restore(); 
  }
  this->getCtx().getcr()->set_line_width(this->globalProp.settings.sizes.stroke);

  //bar border
  p1 = BarPositionWidget<T>::getValtoPos(this->getMin()); //barPos.getMinBarPos();
  p2 = BarPositionWidget<T>::getValtoPos(this->getMax()); //barPos.getMaxBarPos();

  //helium::sum(p1,this->prop.anchor());
  //helium::sum(p2,this->prop.anchor());

  this->jointProp->getV(p); //upper left
  helium::mulScalar(p,-this->prop.height()/2);
  helium::sum(p1,p);

  this->jointProp->getV(p); //bottom right
  helium::mulScalar(p,this->prop.height()/2);
  helium::sum(p2,p);

  this->getCtx().getcr()->save();
  this->getCtx().getcr()->move_to(p1[0],p1[1]);
  this->getCtx().getcr()->line_to(p2[0],p1[1]);
  this->getCtx().getcr()->line_to(p2[0],p2[1]);
  this->getCtx().getcr()->line_to(p1[0],p2[1]);
  this->getCtx().getcr()->close_path();
  this->getCtx().getcr()->restore();  // back to opaque black
  this->getCtx().getcr()->stroke();
}


template<typename T>
BasicCursor<T>::BasicCursor(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp,  JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  BarPositionWidget<T>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}

template<typename T>
void BasicCursor<T>::setCustomInvalidate(int &rx, int &ry, int &rw, int &rh) {
  rx = (this->prop.anchor.def.x - (abs(this->p2.y)!=0?this->globalProp.settings.sizes.bar.w/2+this->globalProp.settings.sizes.bar.h:this->globalProp.settings.sizes.bar.h*4/3)) * this->globalProp.scale.get();
  ry = (this->prop.anchor.def.y - (abs(this->p2.x)!=0?this->globalProp.settings.sizes.bar.w/2+this->globalProp.settings.sizes.bar.h:this->globalProp.settings.sizes.bar.h*4/3)) * this->globalProp.scale.get();
  rw = (abs(this->p2.y)!=0? this->globalProp.settings.sizes.bar.w+this->globalProp.settings.sizes.bar.h*2 : this->globalProp.settings.sizes.bar.h*8/3) * this->globalProp.scale.get();
  rh = (abs(this->p2.x)!=0? this->globalProp.settings.sizes.bar.w+this->globalProp.settings.sizes.bar.h*2 : this->globalProp.settings.sizes.bar.h*8/3) * this->globalProp.scale.get();
}

template<typename T>
void BasicCursor<T>::draw(){
  this->prop.anchor() = BarPositionWidget<T>::getValtoPos(getValue());
  Point2D pa = this->prop.anchor();
  pa.x = round(pa.x); 
  pa.y = round(pa.y); 
  this->jointProp->getU(p1);
  this->jointProp->getV(p2);
  helium::mulScalar(p1,this->prop.width());
  helium::mulScalar(p2,this->prop.height());

  this->getCtx().getcr()->save();
  this->getCtx().getcr()->translate(this->prop.anchor().x,this->prop.anchor().y);

  drawCursor();


  if(!this->getState().enable)
    this->getCtx().getcr()->set_source_rgb(this->globalProp.settings.colors.disable[0], 
					   this->globalProp.settings.colors.disable[1], 
					   this->globalProp.settings.colors.disable[2]);
  else {
    double *rgb = this->getColor();
    if(this->getState().mouseState == GWidgetState::MOUSEDOWN) 
      this->getCtx().getcr()->set_source_rgb(rgb[0]/2, rgb[1]/2, rgb[2]/2);
    else
      this->getCtx().getcr()->set_source_rgb(rgb[0], rgb[1], rgb[2]);
  }

  this->getCtx().getcr()->fill_preserve();
  this->getCtx().getcr()->restore();
  this->getCtx().getcr()->stroke();

}
template<typename T>
bool BasicCursor<T>::setFramePosition(){
  return false;
}
template<typename T>
void BasicCursor<T>::drawCursor(){
  if(setFramePosition())
    helium::mulScalar(this->p2,-1);

  this->getCtx().getcr()->move_to(0,0);
  this->getCtx().getcr()->line_to(this->p1.x/2+this->p2.x/2, this->p1.y/2+this->p2.y/2);
  this->getCtx().getcr()->line_to(this->p1.x/2+this->p2.x, this->p1.y/2+this->p2.y);
  this->getCtx().getcr()->line_to(-this->p1.x/2+this->p2.x, -this->p1.y/2+this->p2.y);
  this->getCtx().getcr()->line_to(-this->p1.x/2+this->p2.x/2, -this->p1.y/2+this->p2.y/2);
  this->getCtx().getcr()->close_path();
}

template <typename T>
CursorTypeAdapter<T,EntryDestination::RobotAll>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp,  JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  BasicCursor<T>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib),
  callOnPowerChange(this),callRefresh(this)
{
}

template <typename T>
void CursorTypeAdapter<T,EntryDestination::RobotAll>::onPowerChange(bool power){
  this->setEnable(this->jointGlobalProp.connection.get().state == helium::Connection::CONNECTED && power);
}
template <typename T>
void CursorTypeAdapter<T,EntryDestination::RobotAll>::onConnected(){
  if(this->getState().visible)
    this->setEnable(this->jointProp->jointPower.get());
}
template <typename T>
void CursorTypeAdapter<T,EntryDestination::RobotAll>::onDisconnected(){
  this->setEnable(false);
}

template <typename T>
CursorTypeAdapter<T,EntryDestination::RobotOnly>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp,  JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CursorTypeAdapter<T,EntryDestination::RobotAll>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}

template <typename T>
double* CursorTypeAdapter<T,EntryDestination::RobotOnly>::getColor(){
  if(this->getState().warning)
    return this->globalProp.settings.colors.coldet.data;
  else
    return this->globalProp.settings.colors.cursor[cfg::guide::CursorType::ROBOT].data;
}


template<typename T>
CursorTypeAdapter<T,EntryDestination::PotAll>::CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp,  JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CursorTypeAdapter<T,EntryDestination::RobotAll>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib), callOnValueChange(this)
{
}


template<typename T>
void CursorTypeAdapter<T,EntryDestination::PotAll>::onValueChange(T val) {
  GWidget::setWarning(isWarning(val));
}

template<typename T>
void CursorTypeAdapter<T,EntryDestination::PotAll>::drawCursor(){
  this->getCtx().getcr()->move_to(0,0);
  if(this->getState().warning) {
    //std::cout << "draw cursor warning " << std::endl;
    //draw +
    this->getCtx().getcr()->move_to(-this->p1.x/5+this->p2.x/5, -this->p1.y/5+this->p2.y/5);
    this->getCtx().getcr()->line_to(-this->p1.x/5+this->p2.x*1.2, -this->p1.y/5+this->p2.y*1.2); //1 up
    this->getCtx().getcr()->line_to(this->p1.x/5+this->p2.x*1.2, this->p1.y/5+this->p2.y*1.2); //2 right
    this->getCtx().getcr()->line_to(this->p1.x/5+this->p2.x/5, this->p1.y/5+this->p2.y/5); //3 down
    this->getCtx().getcr()->line_to(this->p1.x+this->p2.x/5, this->p1.y+this->p2.y/5);//4 right
    this->getCtx().getcr()->line_to(this->p1.x-this->p2.x/5, this->p1.y-this->p2.y/5);//5 down
    this->getCtx().getcr()->line_to(this->p1.x/5-this->p2.x/5, this->p1.y/5-this->p2.y/5);//6 left
    this->getCtx().getcr()->line_to(this->p1.x/5-this->p2.x*1.2, this->p1.y/5-this->p2.y*1.2);//7 down
    this->getCtx().getcr()->line_to(-this->p1.x/5-this->p2.x*1.2, -this->p1.y/5-this->p2.y*1.2);//8 left
    this->getCtx().getcr()->line_to(-this->p1.x/5-this->p2.x/5, -this->p1.y/5-this->p2.y/5);//9 up
    this->getCtx().getcr()->line_to(-this->p1.x-this->p2.x/5, -this->p1.y-this->p2.y/5);//10 left
    this->getCtx().getcr()->line_to(-this->p1.x+this->p2.x/5, -this->p1.y+this->p2.y/5);//11 up

   }
   else {
    //draw normal rectangle
    this->getCtx().getcr()->move_to(-this->p1.x/5, -this->p1.y/5);
    this->getCtx().getcr()->line_to(-this->p1.x/5+this->p2.x*1.2, -this->p1.y/5+this->p2.y*1.2);
    this->getCtx().getcr()->line_to(this->p1.x/5+this->p2.x*1.2, this->p1.y/5+this->p2.y*1.2);
    this->getCtx().getcr()->line_to(this->p1.x/5-this->p2.x*1.2, this->p1.y/5-this->p2.y*1.2);
    this->getCtx().getcr()->line_to(-this->p1.x/5-this->p2.x*1.2, -this->p1.y/5-this->p2.y*1.2);
  }
  this->getCtx().getcr()->close_path();
}
template<typename T>
double* CursorTypeAdapter<T,EntryDestination::PotAll>::getColor(){
 return this->globalProp.settings.colors.cursor[cfg::guide::CursorType::POT].data;
}



template<EntryDestination::Type ed>
CalibCursorTypeAdapter<ed>::CalibCursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp,  JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  BasicCursor<int>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib),
  callRefresh(this)
{
}

template<EntryDestination::Type ed>
void CalibCursorTypeAdapter<ed>::onVisible(){
  jointProp->calib[ed-EntryDestination::Calib-1].connect(callRefresh);
  refresh();
}
template<EntryDestination::Type ed>
void CalibCursorTypeAdapter<ed>::onInvisible(){
  jointProp->calib[ed-EntryDestination::Calib-1].disconnect(callRefresh);
}
template<EntryDestination::Type ed>
int CalibCursorTypeAdapter<ed>::getValue(){
  return jointProp->calib[ed-EntryDestination::Calib-1].get();
}
template<EntryDestination::Type ed>
double* CalibCursorTypeAdapter<ed>::getColor(){
  return globalProp.settings.colors.calibs[ed-EntryDestination::Calib-1].data;
}
template<EntryDestination::Type ed>
bool CalibCursorTypeAdapter<ed>::setFramePosition() {
  return true;
}

template<typename T,EntryDestination::Type ed>
StateCursor<T,ed,false>::StateCursor(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp,  JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CursorTypeAdapter<T,ed>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}
template<typename T,EntryDestination::Type ed>
bool StateCursor<T,ed,false>::isResponsible(const Point2D &e) {
  return false;
}

template<typename T,EntryDestination::Type ed>
StateCursor<T,ed,true>::StateCursor(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp,  JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  CursorTypeAdapter<T,ed>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}
template<typename T,EntryDestination::Type ed>
void StateCursor<T,ed,true>::moveCursor(bool increase, bool page, bool overLimit){
 //increase = down or right
  if (!this->getState().enable) return;
  setNormalClick();
  if(overLimit) this->setShiftClick();
  setValue(this->getPostoVal(this->getValtoPos(this->getValue() + (increase?1:-1) * (this->jointProp->isReverse()?-1:1) * getIncrement())));
}
template<typename T,EntryDestination::Type ed>
bool StateCursor<T,ed,true>::isResponsible(const Point2D &e){
  bool cw = fabs(e.x - this->prop.anchor().x - this->p2.x/2) < (this->prop.width()/2);
  bool ch = fabs(e.y - this->prop.anchor().y - this->p2.y/2) < (this->prop.height()/2);
  return (cw && ch);
}

template<typename T,EntryDestination::Type ed>
void StateCursor<T,ed,true>::onClick(GdkEventButton *event){
  if(event->state & Gdk::SHIFT_MASK)
    setShiftClick();
  else
    setNormalClick();
  this->setMouseState(GWidgetState::MOUSEDOWN);
  if(event->button == 3) {
    onRightClick();
  }
  else if(event->button == 2) {
    onWheelClick();
  }

}
template<typename T,EntryDestination::Type ed>
void StateCursor<T,ed,true>::onRelease(GdkEventButton *event){
  this->setMouseState(GWidgetState::NORMAL);
  onReleaseClick();
}
template<typename T,EntryDestination::Type ed>
void StateCursor<T,ed,true>::onMotion(GdkEventMotion * event){
  if(this->getState().mouseState != GWidgetState::MOUSEDOWN) return;
  //std::cout << "onmotion " << std::setprecision(10) << this->getPostoVal(Point2D((event->x),(event->y))) << " indeg: " << helium::getRadtoDeg(this->getPostoVal(Point2D((event->x),(event->y)))) << std::endl;
  setValue(this->getPostoVal(Point2D((event->x),(event->y))));
}

template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
SignallingCursor<ed,ct>::SignallingCursor(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp,  JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib,helium::ValueHub<cfg::guide::SyncType::Type> &psyncState):
  StateCursor<double,ed,true>(pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib),
  //syncState(psyncState),
  //prevSyncState(pprevSyncState),
  overLimitAllowed(false)
{
}

template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
void SignallingCursor<ed,ct>::validateValue(double &val) {
  //limiting
  if(!overLimitAllowed) {  
    if(val>this->jointProp->calib.relLim.maxv)
      val = this->jointProp->calib.relLim.maxv;
    else if(val<this->jointProp->calib.relLim.minv)
      val = this->jointProp->calib.relLim.minv;
  }
  else {
    if(val > this->defaultCalib.max)
      val = this->defaultCalib.max;
    else if(val < this->defaultCalib.min)
      val = this->defaultCalib.min;
  }
}
template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
void SignallingCursor<ed,ct>::onReleaseClick(){
  if(this->jointProp->prevSyncState[ct] !=  cfg::guide::SyncType::NUM)
    this->jointProp->syncState[ct].assure(this->jointProp->prevSyncState[ct]);
  overLimitAllowed = false;
}
/// @brief set overlimit to be allowed if shift is clicked
template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
void SignallingCursor<ed,ct>::setShiftClick(){
  overLimitAllowed = true;
}
/// @brief set overlimit to be allowed if the value itself is already overlimit
template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
void SignallingCursor<ed,ct>::setNormalClick(){
 if(this->getValue() > this->jointProp->calib.relLim.maxv || this->getValue() < this->jointProp->calib.relLim.minv)
    overLimitAllowed = true;
 else
   overLimitAllowed = false;
}
template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
void SignallingCursor<ed,ct>::onRightClick(){
  if(this->jointProp->syncState[ct] != cfg::guide::SyncType::ALLCONNECT) {
    this->jointProp->syncState[ct].assure(cfg::guide::SyncType::CONNECT);
  }
}
template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
void SignallingCursor<ed,ct>::onWheelClick(){
  //std::cout << "onwheel click " << std::endl;
  this->jointProp->syncState[ct].assure(cfg::guide::SyncType::COPY);
}
template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
double SignallingCursor<ed,ct>::getIncrement(){
  return helium::getDegtoRad(1.0); 
}

template<EntryDestination::Type ed>
CalibCursorWidget<ed>::CalibCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib):
  StateCursor<int,ed,true> (pprop,pjointProp,pglobalProp,pjointGlobalProp,pdefaultCalib)
{
}
template<EntryDestination::Type ed>
void CalibCursorWidget<ed>::setValue(int val){
  int min,max;
  this->jointProp->getCalibLimit((cfg::guide::CalibType::Type)(ed-EntryDestination::Calib-1),min,max);
  if(val<min)
    val = min;
  else if(val>max)
    val = max;
  this->jointProp->setCalib((cfg::guide::CalibType::Type)(ed-EntryDestination::Calib-1),val);
}
template<EntryDestination::Type ed>
int CalibCursorWidget<ed>::getIncrement(){
  return 10;
}
