/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef ANGLEENTRY
#define ANGLEENTRY
#include <gtkmm.h>
#include <iostream>
#include <helium/signal/signal.h>
#include <helium/system/valueHub.h>
#include <helium/gtk/gtkProxy.h>
#include <helium/util/mathUtil.h>
#include "point2D.h"
#include <helium/signal/staticConnReference.h>
#include <helium/core/selectByType.h>

/**
 * class for Gtk::SpinButton using hex numbers
 * by overwriting on_input and on_output function
 * default range is 0,100
 * input containing invalid characters will be ignored
 * /param min small increment step
 * /param max big increment step
 */
class HexSpinButton : public Gtk::SpinButton {
 public:
  HexSpinButton(int min,int max);
protected:
  virtual int on_input(double* new_value);
  virtual bool on_output();
};





template<typename T>
class MoveableEntry: public helium::correspondingType<HexSpinButton, Gtk::SpinButton,int,double,T>::value {
protected:
  T defaultMin,defaultMax;
  Gtk::Fixed *fixed;
public:
  MoveableEntry(Gtk::Fixed *pfixed,T dmin,T dmax);
  void setActive(T v);
  virtual void on_activate();

  void move(int x, int y);
  void setDefaultRange(); ///<
  //  helium::TsSignal<std::string> setSaveState;  ///<to save state
};

class AngleEntry :  public MoveableEntry<double> {//,Gtk::SpinButton {
public:
  AngleEntry(Gtk::Fixed *pfixed,double dmin,double dmax);
  //void setActive(double v);
};

class HexEntry : public MoveableEntry<int>{ // ,HexSpinButton {
public:
  HexEntry(Gtk::Fixed *pfixed,int dmin,int dmax);
  //void setActive(double v);
};



///to subclass entryWidget, define these virtual functions in the subclass
template<typename T>
class EntryWidgetLink{
public:
  virtual T getEntryValue()=0;
  virtual void setValue(T v)=0;
  virtual void setRange(T &min,T &max)=0;
  virtual void getLocation(Point2D& loc)=0;  
  virtual void setVisibleEntry(bool b)=0;

  virtual void connect(helium::ConnReference<T>& )=0;
  virtual void disconnect(helium::ConnReference<T> & )=0;
};


///a wrapper for AngleEntry, range is in radian / int
template<typename T>
class LinkableEntry{
  EntryWidgetLink<T>* el;
public:
  MoveableEntry<T> &entry;
  LinkableEntry(MoveableEntry<T> &pentry);
  void setLinked(EntryWidgetLink<T>* pel);
  void unLink();
  void onGtkChange();
  void onExternalChange(T v);

  void setRange();
  void setMinRange(T min);
  void setMaxRange(T max);
  void setRange(T min, T max);
  void getValue();
  void setDefaultRange(); /// set default range minvalue and maxvalue, which is defined every time a joint is selected (eg. red limit for joint)
  void getDefaultRange(T &min, T &max); /// return the limitation of joint (for conDrop in entryWidget)
private:
  helium::StaticConnReference<T,LinkableEntry<T>,T,&LinkableEntry<T>::onExternalChange> conn;
  bool exChange;
};

#include "angleEntry.hpp"

#endif
