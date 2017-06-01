#include "guideModelWindow.h"

GuideModelWindow::GuideModelWindow(PostureDrawer &ppd):
  pd(ppd),
  windowConn(&pd)
{
  gtkglareaBox = robotWindow.getWidget();
  robotWindow.onDraw.connect(windowConn); //connection

  //gtkglareaBox = static_cast<Gtk::VBox*>(Glib::wrap(glAreaBox()));
  m_vbox.pack_start(*gtkglareaBox);
  add(m_vbox);

  Gdk::Geometry size;
  size.base_width  = 400;
  size.base_height = 300;
  size.min_width  = 200;
  size.min_height = 150;
  size.min_aspect = 4.0f/3.0f;
  size.max_aspect = 4.0f/3.0f;
  set_geometry_hints(*this, size, Gdk::HINT_MIN_SIZE | Gdk::HINT_BASE_SIZE | Gdk::HINT_ASPECT);
  resize(size.base_width, size.base_height);

  show_all_children();
}

bool GuideModelWindow::onKeyPress(GdkEventKey* event) {
  int x,y;
  this->get_pointer(x,y);
  char c = *event->string;
  if(pd.onKeyPress(c,x,y)) {
    queue_draw();
    show_all_children();
  }

  return true;
}
