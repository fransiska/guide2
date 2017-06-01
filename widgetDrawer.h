/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef WIDGETDRAWER
#define WIDGETDRAWER


#include <iostream>
#include <sstream>
#include <vector>

//#include <app/guide2/widgetProperties.h>
#include <helium/core/nan.h>
#include <helium/signal/tsSignal.h>

#include <helium/vector/array.h>
#include <helium/system/valueHub.h>
#include <helium/robot/motorExceptionHandler.h>

#include <helium/compilerWorkaround/CWAinclassf.h>

#include "coldetManager.h"

#include "clickable.h"
#include "drawable.h"
#include "changeable.h"
#include "angleEntry.h"


///guide widgets
namespace gwidget {
  
  //enum JointElementType {MOTORW,BMOTORW,ROBOTEW,FRAMEEW,BROBOTEW,BFRAMEEW,ROBOTCW,FRAMECW,POWERSW,ENABLESW,ROBOTCCW,MINCW,ZEROCW,MAXCW,MINEW,ZEROEW,MAXEW,NOTCLICKABLE};

struct JointElementType {
  typedef enum {MOTORW,BMOTORW,ROBOTEW,FRAMEEW,BROBOTEW,BFRAMEEW,ROBOTCW,FRAMECW,POWERSW,ENABLESW,ROBOTCCW,MINCW,ZEROCW,MAXCW,MINEW,ZEROEW,MAXEW,NOTCLICKABLE} Type;
  static const char* strings[];
};

  //class WidgetProperty;
  //std::ostream& operator<<(std::ostream& o,const WidgetProperty& p);

  ///status of the widget
struct GWidgetState {
  bool visible;
  bool enable;
  bool warning;
  enum MouseState {NORMAL, MOUSEOVER, MOUSEDOWN};
  MouseState mouseState;
  GWidgetState();
};

  /** \brief created by GuideCanvas
   */
struct WidgetGlobalProperty {
  WidgetGlobalProperty(cfg::guide::Settings &psettings, helium::ValueHub<double> &pscale);
  cfg::guide::Settings &settings;
  helium::ValueHub<double> &scale;
  DragData dragData;
};

/** \brief positions and size on canvas, different for each element in same joint
 */
class WidgetProperty{ 
 public:
  changeable<Point2D> anchor;
  changeable<double> width;
  changeable<double> height;
  //bool cornerOrigin;
  std::string name;

  /** \brief properties for particular widget
   * @param height height for the whole widget (in xml it may be for single widget)
   * @param width width for the whole widget
   */
  WidgetProperty(const Point2D &panchor,int pwidth,int pheight/*, bool pcornerOrigin=false*/);
};
inline std::ostream& operator<<(std::ostream& o,const WidgetProperty& p);


/**
 * \brief the basic class for widget in Guide's canvas
 */
class GWidget:public Clickable,public DragDrop,public Drawable {
  //props
protected:
  WidgetProperty prop; ///< received WidgetProperty will be copied here
  WidgetGlobalProperty& globalProp;

public:
  GWidget(const WidgetProperty& pprop, WidgetGlobalProperty& pglobalProp);

  //set get position
  void setAnchor(int x, int y);
  Point2D &getAnchor();
  double getWidth();
  double getHeight();
  bool isCanvasResponsible(const Point2D &e);

protected:
  bool isResponsible(const Point2D &e); ///< default is for origin on the center (cornerOrigin = false)

  //states
private:
  GWidgetState state;

public:
  GWidgetState getState();

  void setVisible(bool v);
  void setEnable(bool e); ///< not clickable + gray
  void setWarning(bool w); //GWidgetState::Warning w);
  void setMouseState(GWidgetState::MouseState m);

protected:
  virtual void onVisible() {} ///< do refresh connections and set jew visibility here
  virtual void onInvisible() {} ///< do disconnections and set jew visibility here
public:
  // drawings
  virtual void refresh(); ///< invalidate rectangle
  virtual void setCustomInvalidate(int &rx, int &ry, int &rw, int &rh) {}
  void canvasDraw(); 
  void onCanvasResize(double d);

private:
  void invalidate(double* bgColor);
protected:
  virtual void draw() = 0;
  virtual void onResize(double d) {}
  virtual double* getRefreshColor(); ///< by default is white, overwrite to use different bg color
  virtual void setLocation(double d);
  virtual void setSize(double d);

public:
  //mouse
  bool onCanvasClick(GdkEventButton * event); ///< only one widget can have click event, return whether it's on click (to break from loop)
  void onCanvasRelease(GdkEventButton * event); ///< run on all widget to return to NORMAL state
  void onCanvasMotion(GdkEventMotion * event); ///< calculate isMouseOver

  //dragdrop
  void onCanvasDrag();
  void onCanvasDrop();

  //virtuals
  //we have conn in joint, no need to connect all. virtual void onConnection(helium::Connection::State s) {};   ///< on connections change, do respective actions
protected:
  virtual void onClick(GdkEventButton * event) {} ///< setMouseState here if needed
  virtual void onRelease(GdkEventButton * event) {} ///< setMouseState here if needed
  virtual void onClicked(GdkEventButton * event) {}
  virtual void onMotion(GdkEventMotion * event) {}
  virtual void onMouseOver(bool b) {}

  virtual void onDrag() {}
  virtual void onDrop() {}

  helium::StaticConnReference<bool,GWidget,bool,&GWidget::onMouseOver> onMouseOverConn;
  helium::StaticConnReference<double,GWidget,double,&GWidget::onCanvasResize> onResizeConn; 
};

/** \brief for widgets with widgets inside
 */
class GWidgets: public GWidget {
public:
  GWidgets(const WidgetProperty& pprop, WidgetGlobalProperty& pglobalProp, int widgetsNumber);  
  helium::Array<GWidget*> widgets;
protected:
  virtual void draw();
  bool isResponsible(const Point2D &p);
  void onClick(GdkEventButton * event);
  void onRelease(GdkEventButton * event);
  void onMotion(GdkEventMotion * event);
  void refresh();
};

struct JointGlobalProperty{
  JointGlobalProperty(helium::GtkProxy<helium::Connection>& pconnection);
  helium::ValueHub<int> bigJointId;
  helium::GtkProxy<helium::Connection>& connection;
};

class ConnectionDependent {
public:
  ConnectionDependent(helium::GtkProxy<helium::Connection>& pconnection);
protected:
  void onConnectionChange(helium::Connection connection);
  virtual void onConnected() {}
  virtual void onDisconnected() {}
  helium::StaticConnReference<helium::Connection,ConnectionDependent,helium::Connection,&ConnectionDependent::onConnectionChange> onConnectionChangeConn;
};

/** \brief widgets which change on change view
 */
class ViewChangeable {
public:
  ViewChangeable(helium::ValueHub<cfg::guide::GuideView::Type> &view);
private:
  cfg::guide::GuideView::Type prevView;
  void onViewChange(cfg::guide::GuideView::Type view);
  helium::StaticConnReference<cfg::guide::GuideView::Type,ViewChangeable,cfg::guide::GuideView::Type,&ViewChangeable::onViewChange> callOnViewChange;  
protected:
  virtual void onNewView(cfg::guide::GuideView::Type view) {}
  virtual void onPrevView(cfg::guide::GuideView::Type view) {}
};

/** \brief joint info
 */
class JointElement {
public:
  JointElement(cfg::guide::Joint* pjointProp, JointGlobalProperty& pjointGlobalProp);
  cfg::guide::Joint* jointProp;
  JointGlobalProperty& jointGlobalProp;
};

/** \brief widget with joint info
 */
class JointElementWidget: public GWidget, public JointElement, public ConnectionDependent {
public:
  JointElementWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp); 
}; 

/** \brief for widgets with widgets inside with joint info
 */
class JointElementWidgets:public GWidgets, public JointElement, public ConnectionDependent {
public:
  JointElementWidgets(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, int widgetsNumber); 
};

/**
 * \brief implements basic entry functions: responsible area, click becomes active, release becomes normal
 */
class BasicEntry:public JointElementWidget{
public:
  BasicEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
  Point2D getOffset();
private:
  void onClick(GdkEventButton *event);
  void onRelease(GdkEventButton *event);
  void setSize(double d) {}
protected:
  bool overLimit;/// whether the current value is over limit (to print text in red)
  virtual void onVisible()=0;
  virtual void onInvisible()=0;

  virtual double* getColor()=0;
  virtual void modifyOffset(Point2D &u) {}
  virtual std::string getValueString()=0;
};


/**  
 * \brief enum for EntryTypeAdapter
 */
namespace EntryDestination {
  enum Type{RobotAll,RobotOnly,Robot,Frame,PotAll,Pot,CalibRobot,CalibPot,Calib,CalibZero,CalibMin,CalibMax};
};

template <EntryDestination::Type ed>
class EntryTypeAdapter:public BasicEntry {
public:
  typedef int DataType; ///< types of the entry (double / int)
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
protected:
  void onValueChange(double d);  ///< conn func
  void onPowerChange(bool b);  ///< conn func
  helium::StaticConnReference<double,EntryTypeAdapter,double,&EntryTypeAdapter::onValueChange> callOnValueChange; ///< conn
  helium::StaticConnReference<bool,EntryTypeAdapter,bool,&EntryTypeAdapter::onPowerChange> callOnPowerChange; ///< conn

  void onVisible() {} ///< do connections and signal the connected function to update the old value
  void onInvisible() {} ///< do disconnections
  void onConnected() {} ///< upon connected to robot
  void onDisconnected() {} ///< upon disconnected to robot

  double getValue(); ///< return in 2 digit decimal rounded, ready for view in entry
  std::string getValueString(); ///< return the text version of getValue();
  double* getColor(); ///< the color of the widget
  void modifyOffset(Point2D &u) {} ///< offset of the entry if needed, default is on the pot position (right side)
};



/**
 * \brief connection, location, value for RobotEntry and RobotText and CalibRobot and Pot and CalibPot
 */
template<>
class EntryTypeAdapter<EntryDestination::RobotAll>:public BasicEntry{
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
protected:  
  void onPowerChange(bool power);
  helium::StaticConnReference<bool,EntryTypeAdapter,bool,&EntryTypeAdapter::onPowerChange> callOnPowerChange;

  void onConnected();
  void onDisconnected();
};

/**
 * \brief connection, location, value for RobotEntry and RobotText and CalibRobot
 */
template<>
class EntryTypeAdapter<EntryDestination::RobotOnly>:public EntryTypeAdapter<EntryDestination::RobotAll> {
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
    
protected:
  double* getColor();
  void modifyOffset(Point2D &u);
};

/**
 * \brief connection, location, value for RobotEntry and RobotText
 */
template<>
class EntryTypeAdapter<EntryDestination::Robot>:public EntryTypeAdapter<EntryDestination::RobotOnly> {
public:
  typedef double DataType;
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
    
protected:
  void onValueChange(double d);
  helium::StaticConnReference<double,EntryTypeAdapter,double,&EntryTypeAdapter::onValueChange> callOnValueChange;
  void onDisplayedValueChange(double d);
  helium::StaticConnReference<double,EntryTypeAdapter,double,&EntryTypeAdapter::onDisplayedValueChange> callOnDisplayedValueChange; ///< connected to displayedTarget

  void onVisible();
  void onInvisible();

  double getValue();
  std::string getValueString();
};


/**
 * \brief for FrameEntry and FrameText
 */
template<>
class EntryTypeAdapter<EntryDestination::Frame>:public BasicEntry{
public:
  typedef double DataType;

  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
    
protected:
  void onValueChange(helium::FrameValue d);
  void onEnableChange(helium::FrameValue b);
  helium::StaticConnReference<helium::FrameValue,EntryTypeAdapter,helium::FrameValue,&EntryTypeAdapter::onValueChange> callOnValueChange;
  helium::StaticConnReference<helium::FrameValue,EntryTypeAdapter,helium::FrameValue,&EntryTypeAdapter::onEnableChange> callOnEnableChange;

  void onVisible();
  void onInvisible();

  double getValue();
  std::string getValueString();
  double* getColor();
  void modifyOffset(Point2D &u);
};


/**
 * \brief for Potentio and CalibPot
 */
template<>
class EntryTypeAdapter<EntryDestination::PotAll>:public EntryTypeAdapter<EntryDestination::RobotAll> {
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);

protected:    
  double* getColor();
};


/**
 * \brief for Potentio
 */
template<>
class EntryTypeAdapter<EntryDestination::Pot>:public EntryTypeAdapter<EntryDestination::PotAll> {
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
  typedef double DataType;

protected:    
  void onValueChange(double d);
  helium::StaticConnReference<double,EntryTypeAdapter,double,&EntryTypeAdapter::onValueChange> callOnValueChange;

  void onVisible();
  void onInvisible();

  std::string getValueString();
  double getValue();
};

/**
 * \brief for Calib Pot
 */
template<>
class EntryTypeAdapter<EntryDestination::CalibPot>:public EntryTypeAdapter<EntryDestination::PotAll> {
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
  typedef int DataType;

protected:    
  //void onValueChange(int d);
  //helium::StaticConnReference<int,EntryTypeAdapter,int,&EntryTypeAdapter::onValueChange> callOnValueChange;
  helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callOnValueChange;

  void onVisible();
  void onInvisible();

  // used in specification below
  int getValue();
  std::string getValueString();
};


/**
 * \brief for Calib Robot
 */
template<>
class EntryTypeAdapter<EntryDestination::CalibRobot>:public EntryTypeAdapter<EntryDestination::RobotOnly> {
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
  typedef int DataType;

protected:    
  helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callOnValueChange;

  void onVisible();
  void onInvisible();

  // used in specification below
  int getValue();
  std::string getValueString();
};



/**
 * \brief for Calib Max Min Zero
 */

template<EntryDestination::Type ed>
class CalibEntryTypeAdapter: public BasicEntry {
public:
  typedef int DataType;
  CalibEntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
protected:
  helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callOnValueChange;

  void onVisible(); 
  void onInvisible(); 

  int getValue();
  std::string getValueString(); 
  double* getColor(); 
  void modifyOffset(Point2D &u); 
};

/**
 * \brief for Calib
 */


template<>
class EntryTypeAdapter<EntryDestination::CalibMin>:public CalibEntryTypeAdapter<EntryDestination::CalibMin>{
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
};
template<>
class EntryTypeAdapter<EntryDestination::CalibZero>:public CalibEntryTypeAdapter<EntryDestination::CalibZero>{
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
};
template<>
class EntryTypeAdapter<EntryDestination::CalibMax>:public CalibEntryTypeAdapter<EntryDestination::CalibMax>{
public:
  EntryTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
};



/**
 * \class EntryTypeAdapter
 * \brief last template for entrywidget
 * \tparam ed different types of values, robot, frame, calib, pot
 * \tparam isEditable is the entry editable by clicking it
 * \tparam isBigJoint is the entry located in bigJoint
 */
  template<EntryDestination::Type ed,bool isEditable, bool isBigJoint>
class StateEntry;



/**
 * \brief defines all basic GWidget function of entry
 * \tparam ed different types of values, robot, frame, calib, pot
 * \tparam isEditable is the entry editable by clicking it
 */
template<EntryDestination::Type ed>
class StateEntry<ed,false,false>:public EntryTypeAdapter<ed>{
public:
  StateEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
protected:
  virtual double* getBgColor();
  virtual void setLocation(double scale);
  virtual void onLeftClicked();
  virtual void onRightClicked() {}
private:
  void draw();
};

template<EntryDestination::Type ed>
class StateEntry<ed,false,true>:public StateEntry<ed,false,false> {
public:
  StateEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
protected:
  void setLocation(double scale);
  virtual void onLeftClicked() {}
};

  //  template<EntryDestination::Type ed> void StateEntry<ed,false,false>::setLocation();
  //template<EntryDestination::Type ed> void StateEntry<ed,false,true>::setLocation();

/**
 * \brief defines entry which links with Gtk::Entry
 * \tparam ed different types of values, robot, frame, calib, pot
 */
template<EntryDestination::Type ed, bool isBigJoint>
class StateEntry<ed,true,isBigJoint>:public StateEntry<ed,false,isBigJoint>,
				     public EntryWidgetLink<typename EntryTypeAdapter<ed>::DataType> {
protected:
  LinkableEntry<typename EntryTypeAdapter<ed>::DataType>& entry;
public:
  StateEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<typename EntryTypeAdapter<ed>::DataType>& pentry);
  double* getBgColor();

  //EntryWidgetLink
  virtual void setRange(typename EntryTypeAdapter<ed>::DataType &min,typename EntryTypeAdapter<ed>::DataType &max) {}
  void setVisibleEntry(bool b);
  void getLocation(Point2D& loc);  
  typename EntryTypeAdapter<ed>::DataType getEntryValue();

protected:
  virtual void onClicked(GdkEventButton * event) = 0;
};

template<EntryDestination::Type ed,bool isBigJoint>
class AngleLimitEntry : public StateEntry<ed,true,isBigJoint> {
public:
  AngleLimitEntry(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<double>& pentry);
  virtual void setValue(double v) {}
protected:  
  DragData::Type getDragType();
  void onDrag();
  void onDrop();
  void onClicked(GdkEventButton * event);
};  



template<bool isBigJoint>
class RobotEntryWidget:public AngleLimitEntry<EntryDestination::Robot,isBigJoint> {
public:
  RobotEntryWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<double>& pentry);

  //EntryWidgetLink
  void setValue(double v);
  void connect(helium::ConnReference<double>& );
  void disconnect(helium::ConnReference<double> & );  

};

typedef StateEntry<EntryDestination::Robot,false,false> RobotValueTextWidget;
typedef StateEntry<EntryDestination::Robot,false,true> BigRobotValueTextWidget;
typedef StateEntry<EntryDestination::Frame,false,false> FrameValueTextWidget;
typedef StateEntry<EntryDestination::Frame,false,true> BigFrameValueTextWidget;
  //typedef StateEntry<EntryDestination::Pot,false,false> PotValueTextWidget;
typedef StateEntry<EntryDestination::Pot,false,true> BigPotValueTextWidget;
  //typedef StateEntry<EntryDestination::CalibPot,false,false> CalibPotValueTextWidget;
typedef StateEntry<EntryDestination::CalibPot,false,true> BigCalibPotValueTextWidget;

template<bool isBigJoint>
class FrameEntryWidget:public AngleLimitEntry<EntryDestination::Frame,isBigJoint> {
  helium::Signal<double> valueChanged;
  helium::StaticConnReference<helium::FrameValue, helium::Signal<double>, double,&helium::Signal<double>::operator() > callValueChanged;
public:
  FrameEntryWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<double>& pentry);
protected:
  //EntryWidgetLink
  void setValue(double v);
  void connect(helium::ConnReference<double>& );
  void disconnect(helium::ConnReference<double> & );
  void onRightClicked() {}
};
template<> void FrameEntryWidget<false>::onRightClicked();

template<bool isBigJoint>
class CalibRobotEntryWidget:public StateEntry<EntryDestination::CalibRobot,true,isBigJoint> {
public:
  CalibRobotEntryWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<int>& pentry);

  //EntryWidgetLink
  void setValue(int v);
  void connect(helium::ConnReference<int>& );
  void disconnect(helium::ConnReference<int> & );  

protected:
  DragData::Type getDragType();
  void onDrag();
  void onDrop();
  void onClicked(GdkEventButton * event);
};

  template<EntryDestination::Type ed,bool isBigJoint>
  class CalibEntryWidget:public StateEntry<ed,true,isBigJoint> {
public:
  CalibEntryWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,LinkableEntry<int>& pentry);

  //EntryWidgetLink
  void setValue(int v);
  void setRange(int &min,int &max);
  void connect(helium::ConnReference<int>& );
  void disconnect(helium::ConnReference<int> & ); 

protected:
  DragData::Type getDragType();
  void onDrag();
  void onDrop();
  void onClicked(GdkEventButton * event);
};

class MotorWidget:public JointElementWidget {
public:
  MotorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
private:

  void draw();
  void drawDiamond();
  void drawCircle();
  void drawCross();

  bool isResponsible(const Point2D &e);

  void onConnected();
  void onDisconnected();

protected:
  virtual void drawAdditional() {}
  virtual void onClicked(GdkEventButton * event)=0;

  helium::StaticConnReference<bool,GWidget,void,&GWidget::refresh> callRefreshonPower;
  helium::StaticConnReference<helium::FrameValue,GWidget,void,&GWidget::refresh> callRefreshonFrameValue;
  helium::StaticConnReference<double,GWidget,void,&GWidget::refresh> callRefreshonTemp;
  helium::StaticConnReference<helium::HwState,GWidget,void,&GWidget::refresh> callRefreshonState;
  helium::StaticConnReference<helium::MonitorMode,GWidget,void,&GWidget::refresh> callRefreshonMonitor;
};

class BigMotorWidget:public MotorWidget {
public:
  BigMotorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
  void onClicked(GdkEventButton * event);
protected:
  void onVisible();
  void onInvisible();
  void setSize(double scale);
  void drawAdditional();
  void setCustomInvalidate(int &rx, int &ry, int &rw, int &rh);
};

class SmallMotorWidget:public MotorWidget {
public:
  SmallMotorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);

protected:
  double* getRefreshColor();
  void drawAdditional();

  void onVisible();
  void onInvisible();

  void onClicked(GdkEventButton * event);
  void onBigJointChange(int id);

  helium::StaticConnReference<int,SmallMotorWidget,int,&SmallMotorWidget::onBigJointChange> callOnBigJointChange;
  helium::StaticConnReference<helium::FrameValue,GWidget,void,&GWidget::refresh> callRefreshonFrameValue;
};




/** a widget displaying text in jointwidget
    motor id, motor name, and temperature
 */
class TextWidget:public JointElementWidget {
 public:
  TextWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
  void draw();
  bool isResponsible(const Point2D &e) { return false; }

  //void onMotion(GdkEventMotion * event) {}
  void setLocation(double scale);
  void setSize() {}


  helium::StaticConnReference<double,GWidget,void,&GWidget::refresh> callRefresh;

};

class CalibTextWidget: public JointElementWidget {
public:
  CalibTextWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
  void draw();

  void onClicked(GdkEventButton * event);

  //void onMotion(GdkEventMotion * event) {}
  void setLocation(double scale);
  void setSize() {}

  void onVisible();
  void onInvisible();

  helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callRefreshonMin;
  helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callRefreshonZero;
  helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callRefreshonMax;

};






class JointWidget:public JointElementWidgets, public ViewChangeable{
public:
  LinkableEntry<double>& entry;
  SmallMotorWidget motorW;
  RobotEntryWidget<false> robotEW;
  FrameEntryWidget<false> frameEW;
  TextWidget textWidget;
  CalibTextWidget calibTextWidget;
  RobotValueTextWidget robotTextWidget;
  FrameValueTextWidget frameTextWidget;

private:
  enum WidgetsName {MOTORW,ROBOTEW,FRAMEEW,TEXTW,CALIBTEXT,ROBOTTW,FRAMETW,NUM}; 
  void onNewView(cfg::guide::GuideView::Type view);
  void onPrevView(cfg::guide::GuideView::Type view);
public:  
  JointWidget(cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, LinkableEntry<double>&pentry, helium::ValueHub<cfg::guide::GuideView::Type> &view);
};



/**
 * Display the number of CPU used, placed near the robot model on GuideCanvas
 */
class ColdetDisplayWidget: public GWidget {
  bool idleActive;
  cfg::guide::Coldet &coldet;
  helium::TsSignal<std::string> *showSubWindowSignal;
public:
  ColdetDisplayWidget(const WidgetProperty& pprop, WidgetGlobalProperty& pglobalProp,cfg::guide::Coldet &pcoldet,helium::TsSignal<std::string> *pshowSubWindowSignal);
  void draw();
  //bool isResponsible(const Point2D &e);
  //void onMotion(GdkEventMotion * event) {}
  void onClicked(GdkEventButton * event);
  /**
   * draw box one by one
   * @param i the id (determining the position)
   * @param active the boolean to determine the color
   * @param cpu the number of cpu used to be written on the display
   * @param label the text to be displayed before cpu (idle/busy)
   */
  void drawBox(int i, bool active, int cpu, std::string label); 
  /**
   * set the active process (idle/busy)
   * @param b set idle to become active
   */
  void setIdleActive(bool b);
private:
  helium::StaticConnReference<void,gwidget::GWidget,void,&gwidget::GWidget::refresh> callColdetRefresh;
};


template<typename T>
class SignallingClickableWidget : public GWidget {
public:
  SignallingClickableWidget(const WidgetProperty &pprop, WidgetGlobalProperty& pglobalProp);
  helium::Signal<T> onClickedSignal;
protected:
  void onClick(GdkEventButton * event);
  void onRelease(GdkEventButton * event);
  virtual void onClicked(GdkEventButton * event) {}
};

  template<> void SignallingClickableWidget<void>::onClicked(GdkEventButton * event);


template<cfg::guide::CursorType::Type type>
class ValueSettingBox;

template<>
class ValueSettingBox<cfg::guide::CursorType::FRAME>:public SignallingClickableWidget<void>, public ViewChangeable {
public:
  ValueSettingBox(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::ValueHub<cfg::guide::GuideView::Type> &view, std::string name="all zero");
protected:
  void draw();
  virtual double* getMouseDownColor(); ///< frame or robot side color
  virtual double* getColor();

  virtual void onNewView(cfg::guide::GuideView::Type view);
};

template<>
class ValueSettingBox<cfg::guide::CursorType::ROBOT> : public ValueSettingBox<cfg::guide::CursorType::FRAME>,public ConnectionDependent {
public:
  ValueSettingBox(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection> &pconnection, helium::ValueHub<cfg::guide::GuideView::Type> &view, std::string name="all zero");
protected:
  virtual void onConnected();
  void onDisconnected();
  double* getMouseDownColor(); ///< frame or robot side color
  virtual void onNewView(cfg::guide::GuideView::Type view) {}
};

/** \brief the box on the left hand side (robot side) of ValueSettingBox, retry shift the posture of the robot
 */
class RetryReachPostureBox: public ValueSettingBox<cfg::guide::CursorType::ROBOT> {
public:
  RetryReachPostureBox(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection> &pconnection, helium::ValueHub<cfg::guide::GuideView::Type> &view, std::string name="retry");
  helium::StaticConnReference<bool,gwidget::GWidget,bool,&gwidget::GWidget::setVisible> onRobotCollide;
protected:
  double* getColor();
  void onConnected() {}
  void onNewView(cfg::guide::GuideView::Type view);
};

/** \brief the box on the left hand side (robot side) of ValueSettingBox, auto shift the posture of the robot
 */
class AutoReachBox: public RetryReachPostureBox {  
public:
  AutoReachBox(Point2D pos, WidgetGlobalProperty& pglobalProp,helium::GtkProxy<helium::Connection> &pconnection, helium::ValueHub<cfg::guide::GuideView::Type> &view);
protected:
  void onMouseOver(bool b);
};

template<cfg::guide::CursorType::Type type>
class CopyArrowWidget:public GWidget, public ConnectionDependent, public ViewChangeable{
  helium::ValueHub<cfg::guide::SyncType::Type> &syncState;
  helium::GtkProxy<helium::Connection> &connection;
  helium::ValueHub<cfg::guide::GuideView::Type> &view;
public:
  CopyArrowWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection> &pconnection, helium::ValueHub<cfg::guide::SyncType::Type> &psyncState, helium::ValueHub<cfg::guide::GuideView::Type> &pview);
protected:
  void draw();
  void onClick(GdkEventButton * event);
  void onRelease(GdkEventButton * event);
  void onClicked(GdkEventButton * event);
  double* getColor();
  void adjustInverse(int &w) {}
  void onConnected();
  void onDisconnected();
private:
  void onSyncChange(cfg::guide::SyncType::Type sync);
  helium::StaticConnReference<cfg::guide::SyncType::Type,CopyArrowWidget,cfg::guide::SyncType::Type,&CopyArrowWidget::onSyncChange> callOnSyncChange;

  void onNewView(cfg::guide::GuideView::Type view);
};

  template<> void CopyArrowWidget<cfg::guide::CursorType::FRAME>::adjustInverse(int &w);

  /** \brief A widget to set sync or to set all values of joints at once
   */
class ValueSetting:public GWidgets  {
  cfg::guide::Joints &joints;
  coldet::ColdetManager *coldetm;

  ValueSettingBox<cfg::guide::CursorType::ROBOT> robotZeroBox;
  ValueSettingBox<cfg::guide::CursorType::FRAME> frameZeroBox;
  ValueSettingBox<cfg::guide::CursorType::FRAME> enableAllBox;
  AutoReachBox autoReachBox;
  RetryReachPostureBox retryReachBox;

  CopyArrowWidget<cfg::guide::CursorType::ROBOT> robotArrow;
  CopyArrowWidget<cfg::guide::CursorType::FRAME> frameArrow;

public:
  ValueSetting(Point2D pos, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection> &pconnection, cfg::guide::Joints &pjoints, coldet::ColdetManager *pcoldetm, helium::ValueHub<cfg::guide::GuideView::Type> &view);
  enum WidgetsName { RobotZeroBox, FrameZeroBox, RobotArrow, FrameArrow, EnableAll, AutoBox, RetryBox, NUM };
protected:
  void draw();

  void robotAllZero();
  void frameAllZero();
  void enableAll();
  void autoReach();
  void retryReach();

  helium::StaticConnReference<void,ValueSetting,void,&ValueSetting::robotAllZero> callRobotAllZero;
  helium::StaticConnReference<void,ValueSetting,void,&ValueSetting::frameAllZero> callFrameAllZero;
  helium::StaticConnReference<void,ValueSetting,void,&ValueSetting::enableAll> callEnableAll;	
  helium::StaticConnReference<void,ValueSetting,void,&ValueSetting::autoReach> callAutoReach;
  helium::StaticConnReference<void,ValueSetting,void,&ValueSetting::retryReach> callRetryReach;

  void onCollision();
  helium::StaticConnReference<bool,ValueSetting,void,&ValueSetting::onCollision> callOnRobotCollision;
  helium::StaticConnReference<bool,ValueSetting,void,&ValueSetting::onCollision> callOnFrameCollision;

};

/* \brief SettingBox with options for Coldet, also have different shape
 */
class ColdetAutoFixWidget: public SignallingClickableWidget<void>  {
  coldet::ColdetManager &coldetm;
  cfg::guide::Coldet &coldet;
public:
  ColdetAutoFixWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, coldet::ColdetManager &coldetm, cfg::guide::Coldet &coldet);
protected:
  void draw();
  void onVisible();
  void onMouseOver(bool b);
  void onClicked(GdkEventButton * event);
  void setWarningState(bool coldetOn); /// warning state = coldet is off
  void setVisibleState(bool isColliding);
  helium::StaticConnReference<bool,ColdetAutoFixWidget,bool,&ColdetAutoFixWidget::setVisibleState> callSetVisibleState;
  helium::StaticConnReference<bool,ColdetAutoFixWidget,bool,&ColdetAutoFixWidget::setWarningState> callSetWarningState;
  helium::CallbackType<void,coldet::ColdetManager,void,&coldet::ColdetManager::copyAutoFix>::value callCopyAutoFix;
  helium::scb::ReplaceTConstClassCallback<void,helium::ValueHub<bool>,const bool&,&helium::ValueHub<bool>::set,bool,true> callSetColdetOn;
};

/* \brief SettingBox with options for frame
 */
template<FrameSaveTo::Type type>
class FrameBox:public SignallingClickableWidget<FrameSaveTo::Type> {
public:
  FrameBox(Point2D pos, WidgetGlobalProperty& pglobalProp);
private:
  void draw();
  //bool isResponsible(const Point2D &e);
protected:
  void onClicked(GdkEventButton * event);
};

/** \brief the red bar below the canvas to indicate frame not saved or calib not saved
 */
class NotSavedWidget: public GWidget,public ViewChangeable { 
  helium::ValueHub<cfg::guide::GuideView::Type> &view;
  helium::ValueHub<bool> &editing;
  helium::ValueHub<bool> &calib;
public:
  NotSavedWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp,helium::ValueHub<cfg::guide::GuideView::Type> &pview, helium::ValueHub<bool> &pediting, helium::ValueHub<bool> &pcalib);
protected:
  void draw();
  bool isResponsible(const Point2D &e) { return false; }
  void setVisibleState();
  void onNewView(cfg::guide::GuideView::Type view);
public:
  helium::StaticConnReference<bool,NotSavedWidget,void,&NotSavedWidget::setVisibleState> callSetVisibleEditing;  
  helium::StaticConnReference<bool,NotSavedWidget,void,&NotSavedWidget::setVisibleState> callSetVisibleCalibSaved;  
};


/* \brief widgets to be shown when the row of 3 boxes to set where the frame should be saved, also show coldet when appropriate     
 */
class FrameSaveSetting: public GWidgets, public ViewChangeable {
  helium::ValueHub<bool>& editing;
  FrameBox<FrameSaveTo::DISCARDFRAME> discardFrameBox;
  FrameBox<FrameSaveTo::SELECTFRAME> selectFrameBox;
  FrameBox<FrameSaveTo::THISFRAME> thisFrameBox;
  //NotSavedWidget notSavedWidget;
public:
  FrameSaveSetting(Point2D pos, WidgetGlobalProperty &pglobalProp, helium::ValueHub<cfg::guide::GuideView::Type> &view, helium::ValueHub<bool> &pediting);
  enum WidgetsName { DISCARDFRAME, SELECTFRAME, THISFRAME, NUM };
  helium::Signal<FrameSaveTo::Type> frameSaveSignal;
protected:
  void onNewView(cfg::guide::GuideView::Type view);
private:
  //bool isResponsible(const Point2D &e);
  helium::StaticConnReference<bool,GWidget,bool,&GWidget::setVisible> callSetVisible;  
  typedef helium::StaticConnReference<FrameSaveTo::Type,helium::Signal<FrameSaveTo::Type>,FrameSaveTo::Type,&helium::Signal<FrameSaveTo::Type>::operator()> CallSetFrameSaveTo;
  helium::Array<CallSetFrameSaveTo,helium::array::RESIZABLE,helium::array::InternalAllocator<helium::ParamAllocator<CallSetFrameSaveTo,helium::Signal<FrameSaveTo::Type>*> > > callSetFrameSaveTo;
};

/*! the border around robot model to indicate robot view (bordered) or frame view (no border)
 */
class ModelBoxWidget: public GWidget { 
  double *rgb;
public:
  ModelBoxWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp);
protected:
  void draw();
  bool isResponsible(const Point2D &e) { return false; }
  void switchModel(int id);
public:
  helium::StaticConnReference<int,ModelBoxWidget,int,&ModelBoxWidget::switchModel> callSwitchModelBox;
};


/*! showing hardware status
 */
class HardwareWidget: public GWidget, public ConnectionDependent {
  helium::TsSignal<std::string> &setShowWindow; ///signal to window to be opened
public:
  HardwareWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp, helium::GtkProxy<helium::Connection> &connection, helium::TsSignal<std::string> &psetShowWindow);
private:
  void draw();
  //bool isResponsible(const Point2D &e);
  void onClicked(GdkEventButton * event);
  void onConnected();
  void onDisconnected();

protected:
  virtual void onStateChange() = 0;
  //virtual std::string getString(bool b) = 0; ///< return string for false (faulty) and for true (OK)
  virtual std::string getWindowName() = 0; ///< window name to be called
  virtual void onVisible() = 0; ///< connections
  helium::StaticConnReference<const std::pair<int,helium::HwState>&,HardwareWidget,void,&HardwareWidget::onStateChange> callOnStateChange;
};


/*! showing sensor status
 */
class SensorWidget: public HardwareWidget {
  cfg::guide::Sensors &sensors;
public:
  SensorWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp, helium::GtkProxy<helium::Connection> &connection, helium::TsSignal<std::string> &psetShowWindow, cfg::guide::Sensors &psensors);
  void onStateChange();
  std::string getWindowName(); ///< window name to be called
private:
  void onVisible(); ///< connections
  void onMouseOver(bool b); ///< \TODO delete
};

/*! showing gyro status
 */
class GyroWidget: public HardwareWidget {
  cfg::guide::Gyro &gyro;
public:
  GyroWidget(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp, helium::GtkProxy<helium::Connection> &connection, helium::TsSignal<std::string> &psetShowWindow, cfg::guide::Gyro &pgyro);
  void onStateChange();
  std::string getWindowName(); ///< window name to be called
private:
  void onVisible(); ///< connections
};

/** \brief clickable circle
 */
class PowerSwitchCircle:public SignallingClickableWidget<void> {
public:
  PowerSwitchCircle(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp);
private:
  void draw();
  bool isResponsible(const Point2D &e);
protected:
  virtual void setLocation(double scale) {}
  virtual double* getColor();
};

/** \brief enable widget
 */
class EnableSwitchWidget:public PowerSwitchCircle, public JointElement {
public:
  EnableSwitchWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);
private:
  void onClicked(GdkEventButton * event);

  void setLocation(double scale);
  double* getColor();

  void onVisible();
  void onInvisible();

  void onValueChange(helium::FrameValue value);
  helium::StaticConnReference<helium::FrameValue,EnableSwitchWidget,helium::FrameValue,&EnableSwitchWidget::onValueChange> callOnValueChange;
};

class PowerSwitchBox: public SignallingClickableWidget<void> {
public:
  PowerSwitchBox(const WidgetProperty &pprop, WidgetGlobalProperty &pglobalProp);
private:
  void draw();
  void setLocation(double scale) {}  
  void onMouseOver(bool b); ///< \todo TO DELETE after trying whether the click area is ok
};

class PowerSwitchWidget:public GWidgets, public ConnectionDependent{
protected:
  PowerSwitchBox powerSwitchBox;
  PowerSwitchCircle pscR;
  PowerSwitchCircle pscG;
public:
  PowerSwitchWidget(const WidgetProperty &pprop, WidgetGlobalProperty& pglobalProp, helium::GtkProxy<helium::Connection>& pconnection);
  enum WidgetsName{Box,RedCircle,GreenCircle,NUM};
protected:
  //enable and disable
  void onConnected();
  void onDisconnected();
  void setLocation(double scale); ///< set location for circles based on this widget's anchor

  virtual void modifyLocation() {};  
  virtual void setPower(bool b) = 0;
  virtual void togglePower();

  helium::ParamStaticConnReference<void,PowerSwitchWidget,bool,&PowerSwitchWidget::setPower> callSetPowerOn;
  helium::ParamStaticConnReference<void,PowerSwitchWidget,bool,&PowerSwitchWidget::setPower> callSetPowerOff;
  helium::StaticConnReference<void,PowerSwitchWidget,void,&PowerSwitchWidget::togglePower> callTogglePower;
};

class BigJointSwitchWidget:public PowerSwitchWidget, public JointElement{
public:
  BigJointSwitchWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp);

private:
  void modifyLocation();
  void onVisible();
  void onInvisible();
  void setPower(bool b);

  void onPowerChange(bool b);
  helium::StaticConnReference<bool,BigJointSwitchWidget,bool,&CWAinclassf(BigJointSwitchWidget,onPowerChange)> callOnPowerChange;

};

class LimbWidget:public PowerSwitchWidget {
public:
  LimbWidget(WidgetGlobalProperty& pglobalProp, cfg::guide::Limb &plimb, cfg::guide::Joints &pjoints, helium::GtkProxy<helium::Connection>& pconnection);

private:
  cfg::guide::Limb &limb;
  cfg::guide::Joints &joints;

  void setPower(bool b);
  void togglePower();
  void onPowerChange();
  helium::Array<helium::StaticConnReference<bool,LimbWidget,void,&LimbWidget::onPowerChange>,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<
		  helium::ParamAllocator<
		    helium::StaticConnReference<bool,LimbWidget,void,&LimbWidget::onPowerChange>, LimbWidget*
		    > 
		  > 
		>callOnPowerChange;
};




/** \brief the parent class for bar widgets and cursor widgets
 * \tparam T int or double
 */
template<typename T>
class BarPositionWidget : public JointElementWidget {
public:
  BarPositionWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
protected:
  cfg::guide::DefaultCalib &defaultCalib;
  Point2D getValtoPos(T val);
  T getPostoVal(Point2D p);
  T getMax(); ///< the maximum viewable value 
  T getMin();  
};

  template<> double BarPositionWidget<double>::getMax(); ///< in degree
  template<> double BarPositionWidget<double>::getMin(); ///< in degree
  template<> int BarPositionWidget<int>::getMax();
  template<> int BarPositionWidget<int>::getMin();


template<typename T>
class BarWidget : public BarPositionWidget<T> {
  cfg::guide::Joints& joints;
public:
  BarWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib,cfg::guide::Joints& pjoints);
protected:
  void draw();
  bool isResponsible(const Point2D &e) { return false; } ///<unclickable
  T getMinLimit(); ///< return the joint limit in T type
  T getMaxLimit(); ///< return the joint limit in T type
};
											     
template<> double BarWidget<double>::getMaxLimit();
template<> double BarWidget<double>::getMinLimit();
template<> int BarWidget<int>::getMaxLimit(); 
template<> int BarWidget<int>::getMinLimit(); 

/**
 * \brief implements basic entry functions: responsible area, click becomes active, release becomes normal
 */
template<typename T>
class BasicCursor:public BarPositionWidget<T> {
public:
  BasicCursor(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
private:
  void setLocation(double scale) {}
  void draw();
protected:
  Point2D p1,p2;
  void setCustomInvalidate(int &rx, int &ry, int &rw, int &rh);
  virtual bool setFramePosition();  ///< by default is on robot side
  virtual void drawCursor(); ///< by default is drawing an arrow

  virtual void onVisible()=0;///< to be implemented by last class (directly connect value to statconn)
  virtual void onInvisible()=0;
  virtual double* getColor()=0;
  virtual T getValue() = 0;


};

template <typename T, EntryDestination::Type ed>
class CursorTypeAdapter;

template <typename T>
class CursorTypeAdapter<T,EntryDestination::RobotAll>:public BasicCursor<T> {
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
private:
  void onConnected(); ///< upon connected to robot
  void onDisconnected(); ///< upon disconnected to robot

protected:
  void onPowerChange(bool b);  ///< conn func
  helium::StaticConnReference<bool,CursorTypeAdapter,bool,&CursorTypeAdapter::onPowerChange> callOnPowerChange; ///< conn
  helium::StaticConnReference<T,GWidget,void,&GWidget::refresh> callRefresh;
};

/** @brief for calib robot, robot and robot-nonclick
 */
template<typename T>
class CursorTypeAdapter<T,EntryDestination::RobotOnly>:public CursorTypeAdapter<T,EntryDestination::RobotAll> {
  double* getColor();
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
};

/** @brief for robot only
 */
template<>
class CursorTypeAdapter<double,EntryDestination::Robot>:public CursorTypeAdapter<double,EntryDestination::RobotOnly> {
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
    
protected:
  void onVisible();
  void onInvisible();
  double getValue();
  void onValueChange(double val);
  helium::StaticConnReference<double,CursorTypeAdapter,double,&CursorTypeAdapter::onValueChange> callOnValueChange; ///< connected to displayedTarget
};

/**
 * \brief for calibRobot
 */
template<>
class CursorTypeAdapter<int,EntryDestination::CalibRobot>:public CursorTypeAdapter<int,EntryDestination::RobotOnly> {
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);

protected:    
  void onVisible();
  void onInvisible();

  int getValue();
};

/**
 * \brief for FrameCursors
 */
template<>
class CursorTypeAdapter<double,EntryDestination::Frame>:public BasicCursor<double>{
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
    
private:
  void onEnableChange(helium::FrameValue b);
  helium::StaticConnReference<helium::FrameValue,CursorTypeAdapter,helium::FrameValue,&CursorTypeAdapter::onEnableChange> callOnEnableChange;

  void onVisible();
  void onInvisible();
  double* getColor();
  bool setFramePosition();

protected:
  double getValue();
};

/**
 * \brief for Potentio and CalibPot
 */
template<typename T>
class CursorTypeAdapter<T,EntryDestination::PotAll>:public CursorTypeAdapter<T,EntryDestination::RobotAll> {
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
private:
  void drawCursor();
  double* getColor();

protected:
  virtual bool isWarning(T val) = 0;

  void onValueChange(T val);
  helium::StaticConnReference<T,CursorTypeAdapter,T,&CursorTypeAdapter::onValueChange> callOnValueChange; ///< connected to displayedTarget

};

/**
 * \brief for Potentio
 */
template<>
class CursorTypeAdapter<double,EntryDestination::Pot>:public CursorTypeAdapter<double,EntryDestination::PotAll> {
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);

protected:    
  void onVisible();
  void onInvisible();

  double getValue();

  bool isWarning(double val);
};





/**
 * \brief for CalibPotentio
 */
template<>
class CursorTypeAdapter<int,EntryDestination::CalibPot>:public CursorTypeAdapter<int,EntryDestination::PotAll> {
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);

protected:    
  bool isWarning(int val);
  void onVisible();
  void onInvisible();

  int getValue();
};

/**
 * \brief for Calib Max Min Zero
 */

template<EntryDestination::Type ed>
class CalibCursorTypeAdapter: public BasicCursor<int> {
public:
  CalibCursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
protected:
  helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callRefresh;

  void onVisible(); 
  void onInvisible(); 

  void setValue(int v);
  int getValue();
  double* getColor(); 
  bool setFramePosition();
};


/**
 * \brief for Calib
 */


template<>
class CursorTypeAdapter<int,EntryDestination::CalibMin>:public CalibCursorTypeAdapter<EntryDestination::CalibMin>{
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
};
template<>
class CursorTypeAdapter<int,EntryDestination::CalibZero>:public CalibCursorTypeAdapter<EntryDestination::CalibZero>{
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
};
template<>
class CursorTypeAdapter<int,EntryDestination::CalibMax>:public CalibCursorTypeAdapter<EntryDestination::CalibMax>{
public:
  CursorTypeAdapter(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
};

/** \brief class for Cursor, similar structure as entry
 * \tparam T type for pos calculation
 * \tparam ed destination of entry
 * \tparam isDraggable click to drag or not
 */
template<typename T, EntryDestination::Type ed,bool isDraggable>
class StateCursor;

/**
 * \brief defines all basic GWidget function of cursor
 * \tparam ed different types of values, robot, frame, calib, pot
 * \tparam isEditable is the entry editable by clicking it
 */
template<typename T,EntryDestination::Type ed>
class StateCursor<T,ed,false>:public CursorTypeAdapter<T,ed>{
public:
  StateCursor(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
private:
  bool isResponsible(const Point2D &e);
};


template<typename T,EntryDestination::Type ed>
class StateCursor<T,ed,true>:public CursorTypeAdapter<T,ed>{
public:
  StateCursor(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
  void moveCursor(bool increase, bool page, bool overLimit=true);
private:
  bool isResponsible(const Point2D &e);
  void onClick(GdkEventButton *event);
  void onRelease(GdkEventButton *event);
  void onMotion(GdkEventMotion * event);
protected:
  virtual void onReleaseClick(){} ///< set state when cursor is right clicked etc.
  virtual void setShiftClick() {}
  virtual void setNormalClick() {}
  virtual void onRightClick() {}
  virtual void onWheelClick() {}
  virtual void setValue(T val) = 0;
  virtual T getIncrement() = 0;
};

  template<EntryDestination::Type ed,cfg::guide::CursorType::Type ct>
class SignallingCursor:public StateCursor<double,ed,true> {
  //helium::ValueHub<cfg::guide::SyncType::Type> &syncState;
  //cfg::guide::SyncType::Type &prevSyncState;
public:
  SignallingCursor(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib,helium::ValueHub<cfg::guide::SyncType::Type> &psyncState);
protected:
  bool overLimitAllowed;
  virtual void onReleaseClick(); ///< set state when cursor is right clicked etc.
  virtual void setShiftClick();
  virtual void setNormalClick();
  virtual void onRightClick();
  virtual void onWheelClick();
  double getIncrement();
  void validateValue(double &val);
};


class TouchViewRobotCursorWidget: public StateCursor<double,EntryDestination::Robot,false> {
public:
  TouchViewRobotCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,cfg::guide::DefaultCalib &pdefaultCalib, coldet::ColdetManager *coldetm);
};

typedef StateCursor<double,EntryDestination::Frame,false> TouchViewFrameCursorWidget;
typedef StateCursor<double,EntryDestination::Pot,false> PotentioCursorWidget;
typedef StateCursor<int,EntryDestination::CalibPot,false> CalibPotentioCursorWidget;

class RobotCursorWidget : public SignallingCursor<EntryDestination::Robot,cfg::guide::CursorType::ROBOT>{
public:
  RobotCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,cfg::guide::DefaultCalib &pdefaultCalib, helium::ValueHub<cfg::guide::SyncType::Type> &psyncState,coldet::ColdetManager *coldetm);
private:
  void setValue(double val);
};
class FrameCursorWidget : public SignallingCursor<EntryDestination::Frame,cfg::guide::CursorType::FRAME> {
public:
  FrameCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,cfg::guide::DefaultCalib &pdefaultCalib, helium::ValueHub<cfg::guide::SyncType::Type> &psyncState);
private:
  void setValue(double val);
};
class CalibRobotCursorWidget : public StateCursor<int,EntryDestination::CalibRobot,true> {
public:
  CalibRobotCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp,cfg::guide::DefaultCalib &pdefaultCalib);
private:
  void setValue(int val);
  int getIncrement();
};

template<EntryDestination::Type ed>
class CalibCursorWidget : public StateCursor<int,ed,true> {
public:
  CalibCursorWidget(const WidgetProperty& pprop, cfg::guide::Joint* pjointProp, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::DefaultCalib &pdefaultCalib);
private:
  void setValue(int val);
  int getIncrement();
};





template <typename T, cfg::guide::GuideView::Type type>
class BigJointWidget:public JointElementWidgets, public ViewChangeable {
public:
  BigJointWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::Joints &pjoints,helium::ValueHub<cfg::guide::GuideView::Type> &pview,int widgetsNumber,LinkableEntry<T>*pentry=NULL);

protected:
  LinkableEntry<T>* entry;
  cfg::guide::Joints &joints;
  BigMotorWidget motorW;
  BigJointSwitchWidget powerSW; 
  BarWidget<T> barW;
  virtual void moveCursor(bool increase=true, bool page=true, bool overLimit=true) = 0;
  virtual void setJointPointer(cfg::guide::Joint *pjoint) = 0; ///< reset the joint for each JointElementWidget
private:
  int curId;
  void setJoint(int i);
  void onVisible(); ///< loop to call children's setVisible
  void onInvisible();
  void onNewView(cfg::guide::GuideView::Type view);  ///< automatically hide/show
  void onPrevView(cfg::guide::GuideView::Type view);
protected:
  helium::StaticConnReference<int,BigJointWidget,int,&BigJointWidget::setJoint> callSetJoint;
};



class MotionBigJointWidget:public BigJointWidget<double,cfg::guide::GuideView::MOTION> {
  //JointValueValidate<double> &jvv;
  //BarPosition<double> barPos;

  RobotCursorWidget robotCW;
  PotentioCursorWidget potCW;
  FrameCursorWidget frameCW;

  RobotEntryWidget<true> robotEW;
  FrameEntryWidget<true> frameEW;
  BigPotValueTextWidget potTW;

  EnableSwitchWidget enableSW;


public:
  enum WidgetsName {MOTORW,ROBOTEW,FRAMEEW,POTTW,BARW,POTCW,ROBOTCW,FRAMECW,POWERSW,ENABLESW,NUM}; ///insert enum here to draw widget
  MotionBigJointWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::Joints &pjoints,helium::ValueHub<cfg::guide::GuideView::Type> &view, LinkableEntry<double>&pentry, coldet::ColdetManager *coldetm); //, JointValueValidate<double> &pjvv

  void moveCursor(bool increase, bool page, bool overLimit);
protected:
  void setJointPointer(cfg::guide::Joint *pjoint);

  //helium::StaticConnReference<helium::FrameValue,GWidget,void,&GWidget::refresh> callRefresh_FrameValue;
};

class TouchBigJointWidget:public BigJointWidget<double,cfg::guide::GuideView::TOUCH> {
  //JointValueValidate<double> &jvv;
  //BarPosition<double> barPos;

  TouchViewRobotCursorWidget robotCW;
  PotentioCursorWidget potCW;
  TouchViewFrameCursorWidget frameCW;

  BigRobotValueTextWidget robotTW;
  BigFrameValueTextWidget frameTW;
  BigPotValueTextWidget potTW;

  EnableSwitchWidget enableSW;

public:
  enum WidgetsName {MOTORW,ROBOTTW,FRAMETW,POTTW,BARW,POTCW,ROBOTCW,FRAMECW,POWERSW,ENABLESW,NUM}; 
  TouchBigJointWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::Joints &pjoints,helium::ValueHub<cfg::guide::GuideView::Type> &view, coldet::ColdetManager *coldetm);//, JointValueValidate<double> &pjvv

protected:
  void moveCursor(bool increase, bool page, bool overLimit) {}
  void setJointPointer(cfg::guide::Joint *pjoint);
  //helium::StaticConnReference<helium::FrameValue,GWidget,void,&GWidget::refresh> callRefresh_FrameValue;
};

class CalibBigJointWidget:public BigJointWidget<int,cfg::guide::GuideView::CALIB> {
  //JointValueValidate<int> &jvv;
  //BarPosition<int> barPos;

  CalibRobotCursorWidget robotCW;
  CalibPotentioCursorWidget potCW;
  CalibCursorWidget<EntryDestination::CalibMin> minCW;
  CalibCursorWidget<EntryDestination::CalibZero> zeroCW;
  CalibCursorWidget<EntryDestination::CalibMax> maxCW;
  
  CalibRobotEntryWidget<true> robotEW;
  CalibEntryWidget<EntryDestination::CalibMin,true> minEW;
  CalibEntryWidget<EntryDestination::CalibZero,true> zeroEW;
  CalibEntryWidget<EntryDestination::CalibMax,true> maxEW;
  BigCalibPotValueTextWidget potTW;

public:
  enum WidgetsName {MOTORW,ROBOTEW,MINEW,ZEROEW,MAXEW,POTTW,BARW,POTCW,ROBOTCW,MINCW,ZEROCW,MAXCW,POWERSW,NUM}; 
  CalibBigJointWidget(Point2D pos, WidgetGlobalProperty& pglobalProp, JointGlobalProperty& pjointGlobalProp, cfg::guide::Joints &pjoints,helium::ValueHub<cfg::guide::GuideView::Type> &view,LinkableEntry<int>&pentry);//,JointValueValidate<int> &pjvv


  void moveCursor(bool increase, bool page, bool overLimit=true);
protected:
  void setJointPointer(cfg::guide::Joint *pjoint);
  //helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callRefresh_Calib0Value;
  //helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callRefresh_Calib1Value;
  //helium::StaticConnReference<int,GWidget,void,&GWidget::refresh> callRefresh_Calib2Value;
};


 





 














#include "widgetDrawer.hpp"

}; //namespace gwidget
#endif
