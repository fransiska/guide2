#ifndef GTKGLAREABOX
#define GTKGLAREABOX

#include <math.h>
#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>
#include <GL/gl.h>
#include <gdkmm/pixbuf.h>
#include <helium/signal/tsSignal.h>
#include <gtkmm.h>



class GlArea{
  friend gint drawFunc(GtkWidget *widget, GdkEventExpose *event,GlArea* a);
  gint glAreaDraw(GtkWidget *widget, GdkEventExpose *event);
  Gtk::VBox *thisWidget;
 
 public: 
  GlArea();
  ~GlArea(){
    //std::cout<<"GLAREA"<<this<<"DELETED"<<std::endl;
    //helium::printTrace();
  }
  Gtk::VBox*  getWidget();
  helium::TsSignal<void> onDraw;
  helium::TsSignal<const std::pair<int,int>&> onResize;
};
#endif
