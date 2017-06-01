#ifndef WIDGETPROPERTIES
#define WIDGETPROPERTIES



#include <gtkmm.h>
#include <helium/callback/tsSignal.h>

#include "clickable.h"
#include "drawable.h"
#include "changeable.h"
#include <app/guide2/widgetDrawer.h>

namespace gwidget {


class EntryWidget;
class CalibTextWidget;
class CopyArrowWidget;
class ResetBox;
class CursorWidget;
class PowerSwitchWidget;

class WidgetProperties;
std::ostream& operator<<(std::ostream& o,const WidgetProperties& p);

class WidgetProperties{ //different for each element in same joint, created in constructor, hence passed by const
 public:
  cfg::guide::Settings &settings;
  changeable<Point2D> anchor;
  changeable<double> width;
  changeable<double> height;
  helium::ValueHub<double> &scale;

  WidgetProperties(cfg::guide::Settings &settings, const Point2D &panchor,int pwidth,int pheight, helium::ValueHub<double> &pscale);
};

inline std::ostream& operator<<(std::ostream& o,const WidgetProperties& p){
  o<<"WP:"<<std::endl;
  o<<p.width<<std::endl;
  o<<p.height<<std::endl;
  o<<"----"<<std::endl;
  return o;
}

class JointWidgetProperties { //same for all element in same joint
  int id; //0~22
 public:
  cfg::guide::Joint *ji; //0~21, BigJoint has copy of 0
  std::pair<helium::ValueHub<int>,helium::ValueHub<JointElementType> > *selectedJoint;
  JointWidgetProperties(int pid, cfg::guide::Joint *pji,std::pair<helium::ValueHub<int>,helium::ValueHub<JointElementType> >*sj);
  int getId() { return id; }
};

class GWidget:public Clickable,public gwidget::Drawable {
 protected:
  WidgetProperties prop;
  bool visibility; //todo: needed for calib view?
 public:
  GWidget(const WidgetProperties &pprop);
  void setAnchor(int x, int y) {prop.anchor().x = x; prop.anchor().y = y; }
  Point2D &getAnchor() { return prop.anchor(); }
  virtual void refresh();
  void refreshWidget(double *c=NULL);
  virtual void onResize();
  virtual void setLocation();
  virtual void setSize();
  bool isVisible();
  void setVisible(bool v);
  double getWidth();
  
  //inherited
  void draw();
  bool isResponsible(const Point2D &e);

  virtual EntryWidget* getEntry(cfg::guide::CursorType::Type t) {return NULL;}
  virtual CopyArrowWidget* getArrow(cfg::guide::CursorType::Type t) {return NULL;}
  virtual ResetBox* getBox(cfg::guide::CursorType::Type t) {return NULL;}
  virtual CalibTextWidget* getCalibText() {return NULL;}
  virtual CursorWidget* getCursor(cfg::guide::CursorType::Type t) {return NULL;}
  virtual PowerSwitchWidget* getPowerSwitch() {return NULL;}
protected:
  virtual void cdraw() = 0;
  virtual void crefresh();
  virtual bool cisResponsible(const Point2D &e) = 0;
  friend class JointWidget;
  friend class BigJointWidget;
  friend class ValueSetting;
};


}//namespace

#endif
