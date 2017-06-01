/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef GTKGLAREABOX
#define GTKGLAREABOX

#include <gtkmm.h>
#include <gtkgl/gtkglarea.h>
#include <helium/signal/tsSignal.h>


/**
 * this class gives out a \b Gtk::Widget GlArea::glWidget (called glArea in the constructor) which can be get by calling GlArea::getWidget() \n
 * \b Usage: \n
 * GlArea::glWidget can then be added into a VBox by vbox.add(*getWidget()) function, it is equipped with several signals, which can be connected from outside \n
 * such as getWidget()->signal_key_press_event().connect(sigc::mem_fun(...
 */
class GlArea{
  friend gint drawFunc(GtkWidget *widget, GdkEventExpose *event,GlArea* a);
  gint glAreaDraw(GtkWidget *widget, GdkEventExpose *event);
  //Gtk::VBox&  getWidget();
protected:
  //Gtk::VBox *thisWidget;
  Gtk::Widget *glWidget; ///< has signals for button and key press, this is where the gl is drawn
 public: 
  Gtk::Widget* getWidget(); ///< returns glWidget
  GlArea();
  ~GlArea(){
    //std::cout<<"GLAREA"<<this<<"DELETED"<<std::endl;
    //helium::printTrace();
  }
  helium::TsSignal<void> onDraw;
  helium::TsSignal<const std::pair<int,int>&> onResize;
};
#endif
