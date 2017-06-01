#ifndef GUIDEMODELWINDOW
#define GUIDEMODELWINDOW

#include <gtkmm.h>
#include <heliumBased/convex/drawConvex.h>
#include "postureDrawer.h"
#include "gtkglareaBox.h"

class GuideModelWindow: public Gtk::Window {
  Gtk::VBox m_vbox;
  Gtk::VBox *gtkglareaBox;
  PostureDrawer &pd;
  GlArea robotWindow;
  helium::ConnReference<void> windowConn; 
 public:
  GuideModelWindow(PostureDrawer &ppd);
  bool onKeyPress(GdkEventKey* event);
};

#endif
