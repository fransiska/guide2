/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef _DRAWABLE_
#define _DRAWABLE_

#include <gtkmm.h>

namespace gwidget{

/// holder for drawing context, there can be only 1 instance of this
class DrawingContext{ 
  //singleton
 private:
  //static DrawingContext *_drawingContext;
  int width0;
  int height0;
  friend class DrawingContextHolder;
  DrawingContext(){}
 public:
  static DrawingContext *get();


  Cairo::RefPtr<Cairo::Context> cr;
  Glib::RefPtr<Pango::Layout> pl;
  Glib::RefPtr<Gdk::Window> w;
  Gtk::DrawingArea *da;

  void setCtx(Cairo::RefPtr<Cairo::Context> &pcr, Glib::RefPtr<Pango::Layout> &ppl,Glib::RefPtr<Gdk::Window> &pw){
    //pcr->scale(100,100);
    cr = pcr;
    pl = ppl;
    w = pw;
  }
  
  void setCtx(Glib::RefPtr<Gdk::Window> window){
    w=window;
    cr = window->create_cairo_context();
    pl = Pango::Layout::create (cr); //text
  }

  void setDa(Gtk::DrawingArea *pda) {
    da = pda;
  }

  Cairo::RefPtr<Cairo::Context> &getcr() { return cr; }
  Glib::RefPtr<Pango::Layout> &getpl() { return pl; }
  Glib::RefPtr<Gdk::Window> &getwin() { return w; }
  Gtk::DrawingArea *getda() { return da; }
  int getWidth() { return da->get_width(); }
  int getHeight() { return da->get_height(); }
  void setWidth0(int w) {width0 = w;}
  void setHeight0(int h) {height0 = h;}
  int getWidth0() { return width0; }
  int getHeight0() { return height0; }
};



inline std::ostream & operator<<(std::ostream &o,const DrawingContext& c){
  o<<c.cr<<","<<c.pl;
  return o;
}

  /// the superclass for GWidget, all refer to the same DrawingContext
class Drawable{
 protected:
  DrawingContext *ctx;
 public:
 Drawable():ctx(DrawingContext::get()) {}
  DrawingContext &getCtx() { return *ctx; }
  virtual void draw()=0;
};
}
#endif
