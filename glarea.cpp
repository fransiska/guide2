/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#include "glarea.h"
#include <stdio.h>
#include <helium/graphics/glHeader.h>
#include <GL/glu.h>
#include <math.h>

#include <gdkmm/pixbuf.h>
#include <gtk/gtk.h>

gint glAreaInit(GtkWidget *widget)
{
  //gdk_window_ensure_native(widget->window);
  /* OpenGL functions can be called only if make_current returns true */
  if (gtk_gl_area_make_current(GTK_GL_AREA(widget)))
    {

      int w = widget->allocation.width;
      int h = widget->allocation.height;

      /*TODO DEBUG mesa DELETE?
      glViewport(0,0, widget->allocation.width, widget->allocation.height);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0,100, 100,0, -1,1);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      */
      
      //20110609
      //from initGL
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
      glClearDepth(1.0f);
      glEnable(GL_DEPTH_TEST);		        // Enables Depth Testing
      glDepthRange(0.0f,1.0f);				// Enables Clearing Of The Depth Buffer
      glDepthFunc(GL_LESS);			        // The Type Of Depth Test To Do
      glDepthMask(true);
      glDisable (GL_BLEND);
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      


      glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();				// Reset The Projection Matrix
      gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f); // Calculate The Aspect Ratio Of The Window
      glMatrixMode(GL_MODELVIEW);

    }

  return TRUE;
}

/* When widget is exposed it's contents are redrawn. */
gint GlArea::glAreaDraw(GtkWidget *widget, GdkEventExpose *event)
{
  // Draw only last expose. 
  if (event->count > 0)
    return TRUE;

  //OpenGL functions can be called only if make_current returns true 

  if (gtk_gl_area_make_current(GTK_GL_AREA(widget)))
    {

      //std::cout << "ondraw gl" << std::endl;

      onDraw();     
      //Swap backbuffer to front
      gtk_gl_area_swapbuffers(GTK_GL_AREA(widget));
    }
  return TRUE;
}

gint drawFunc(GtkWidget *widget, GdkEventExpose *event,GlArea* a){
  return a->glAreaDraw(widget,event);
}


/* When glarea widget size changes, viewport size is set to match the new size */
gint glAreaReshape(GtkWidget *widget, GdkEventConfigure *event,GlArea* a){
  /* OpenGL functions can be called only if make_current returns true */
  if (gtk_gl_area_make_current(GTK_GL_AREA(widget)))
    {
      glViewport(0,0, widget->allocation.width, widget->allocation.height);
      a->onResize(std::pair<int,int>(widget->allocation.width, widget->allocation.height));
    }
  return TRUE;
}

/*
Gtk::VBox&  GlArea::getWidget(){
  return *thisWidget;
}
*/

Gtk::Widget* GlArea::getWidget() {
  return glWidget;
}

GlArea::GlArea()
//:thisWidget(NULL)
{
  GtkWidget *glArea; //*glBox, 
  //glBox = gtk_vbox_new(TRUE,0);

  //------- copied example c file ------- 
  //gchar *info_str;
  /* Attribute list for gtkglArea widget. Specifies a
     list of Boolean attributes and enum/integer
     attribute/value pairs. The last attribute must be
     GDK_GL_NONE. See glXChooseVisual manpage for further
     explanation.
  */
  int attrlist[] = {
    GDK_GL_RGBA,
    GDK_GL_RED_SIZE,1,
    GDK_GL_GREEN_SIZE,1,
    GDK_GL_BLUE_SIZE,1,
    GDK_GL_DOUBLEBUFFER,
    GDK_GL_DEPTH_SIZE,16,
    GDK_GL_NONE
  };

  /* Check if OpenGL is supported. */
  if (gdk_gl_query() == FALSE) {
    g_print("OpenGL not supported\n");
    return; //NULL;
  }  

  /* Quit form main if got delete event */
  // causes main window to be destroyed when this window is closed
  //gtk_signal_connect(GTK_OBJECT(glWindow), "delete_event", GTK_SIGNAL_FUNC(gtk_main_quit), NULL); 
  /* You should always delete gtk_gl_area widgets before exit or else
     GLX contexts are left undeleted, this may cause problems (=core dump)
     in some systems.
     Destroy method of objects is not automatically called on exit.
     You need to manually enable this feature. Do gtk_quit_add_destroy()
     for all your top level windows unless you are certain that they get
     destroy signal by other means.
  */
  //gtk_quit_add_destroy(1, GTK_OBJECT(glWindow)); 
  //todo: ---------------------- NOT SURE IF THIS IS NEEDED ----------------------

  /* Create new OpenGL widget. */
  glArea = GTK_WIDGET(gtk_gl_area_new(attrlist));
  /* Events for widget must be set before X Window is created */
  gtk_widget_set_events(GTK_WIDGET(glArea),
			GDK_EXPOSURE_MASK|
			GDK_BUTTON_PRESS_MASK|			
			GDK_KEY_PRESS_MASK  
			);
  GTK_WIDGET_SET_FLAGS (glArea, GTK_CAN_FOCUS); 
  gtk_widget_grab_focus(GTK_WIDGET(glArea));

  /* do this in the keypress handler and return TRUE from handler */
  //gtk_signal_emit_stop_by_name(GTK_OBJECT(glarea), "key_press_event");

  /// https://github.com/GNOME/gtkglarea/blob/master/docs/HOWTO.txt 
  /// gtk_can_focus is needed so that the area can be focused to receive a keyboard event

  /* Connect signal handlers */
  /* Redraw image when exposed. */
  gtk_signal_connect(GTK_OBJECT(glArea), "expose_event",
		     //GTK_SIGNAL_FUNC(glAreaDraw), NULL);
		     G_CALLBACK(drawFunc),this);
		     
  /* When window is resized viewport needs to be resized also. */
  gtk_signal_connect(GTK_OBJECT(glArea), "configure_event",
		     //GTK_SIGNAL_FUNC(glAreaReshape), NULL);
		     G_CALLBACK(glAreaReshape),this);
  /* Do initialization when widget has been realized. */
  gtk_signal_connect(GTK_OBJECT(glArea), "realize",
		     GTK_SIGNAL_FUNC(glAreaInit), NULL);
  /*
  gtk_signal_connect (GTK_OBJECT(glWindow), "delete-event",
  		      GTK_SIGNAL_FUNC(glAreaHide), NULL);
  */


  /* set minimum size */
  gtk_widget_set_usize(GTK_WIDGET(glArea), 100,100);

  /* put glarea into window and show it all */
  //gtk_container_add(GTK_CONTAINER(glBox),GTK_WIDGET(glArea));
  gtk_widget_show(GTK_WIDGET(glArea));
  //gtk_widget_show(GTK_WIDGET(glBox));

  //------- end of copy ------- 

  //thisWidget=static_cast<Gtk::VBox*>(Glib::wrap(glBox));
  glWidget = static_cast<Gtk::Widget*>(Glib::wrap(glArea));
}

/*
gint glAreaHide(GtkWidget *widget, GdkEvent *event) {
  //preventing deleting the window upon clicking "x" button
  gtk_widget_hide(widget);
  return FALSE; //need to launch the hide signal to untick the guide menu
}
*/

