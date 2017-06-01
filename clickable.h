/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef _CLICKABLE_
#define _CLICKABLE_

#include <gtkmm.h>
#include "point2D.h"
#include <helium/system/valueHub.h>

class Clickable{  
protected:
  struct State {
    typedef enum {NONE,CLICK,RELEASE,CLICKED} Type;
    static const char* strings[];
  };
  State::Type clickState;
public:
  helium::ValueHub<bool> isMouseOver;
  Clickable():
    clickState(State::NONE) 
  {
  }
  State::Type getClickState() {
    return clickState;
  }
protected:
  virtual bool isResponsible(const Point2D &e)=0;
  virtual void onClick(GdkEventButton * event) = 0;
  virtual void onRelease(GdkEventButton * event) = 0;
  virtual void onClicked(GdkEventButton * event) = 0;
  virtual void onMotion(GdkEventMotion * event)=0;
};

class DragData {
public:
  enum Type {NONE,HEX,ANGLE} type;
  int hexValue;
  std::pair<double,bool> angleValue; ///< bool indicates the motor is reversed or not
};

class DragDrop{
public:
  virtual DragData::Type getDragType() { return DragData::NONE; }
  virtual void onDrag()=0;
  virtual void onDrop()=0;

  //DragData::DragDataType dragDataType;
  //void setDragDataType(DragData::DragDataType t) { dragDataType = t; }
  //DragDrop(DragData::DragDataType t=DragData::NONE):dragDataType(t) {}
};

/*
template <typename T>
class Draggable{
public:
  virtual bool onDrag(const Point2D &e, T &value)=0;
  virtual bool onDrop(const Point2D &e, T value)=0;
  virtual int getId()=0;
};
*/

#endif
