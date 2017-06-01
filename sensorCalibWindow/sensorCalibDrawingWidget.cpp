/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include <helium/vector/operations.h>
#include "sensorCalibDrawingWidget.h"

#include <gtkmm.h>
#include <iostream>

using namespace std;

SensorCalibDrawingWidget::SensorCalibDrawingWidget(cfg::guide::Sensors &psensors,cfg::guide::SensorGui &psensorgui,cfg::guide::Colors &pcolors):
						   //,helium::ValueHub<int>& pas):
  sensors(psensors),sensorGui(psensorgui),colors(pcolors),
  active_sensor(-1),
  callAssureActiveSensor(&active_sensor)
  //,active_sensor(pas)
{
  //read pictures
  try {
    bgimage = Gdk::Pixbuf::create_from_file(sensorGui.bg.filename);
  }    
  catch(...) {
    Glib::exception_handlers_invoke();
    //exit(1);
  }
  if(bgimage) {
    width = bgimage->get_width();
    height = bgimage->get_height();
  }
  else {
    width = sensorGui.bg.size.w;
    height = sensorGui.bg.size.h;
  }
  //cout << "!!!!wh " << width << " " << height << endl; //DELETE
  set_size_request((double)width/2,(double)height/2);

}


bool SensorCalibDrawingWidget::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::Window> window = get_window();
  if( window ) {
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    if (event) {
      cr->rectangle( event->area.x, event->area.y, event->area.width, event->area.height );
      cr->clip();
    }

    int xw,yw; //current drawing area size
    xw = this->get_width();
    yw = this->get_height();
    pr = (float)xw/width;

    if(bgimage) {
      //make image scalable
      Glib::RefPtr<Gdk::Pixbuf> tmpimage = 
	bgimage->scale_simple(xw,yw,Gdk::INTERP_BILINEAR);
      tmpimage->render_to_drawable(this->get_window(),
				   this->get_style()->get_white_gc(), 0, 0, 0, 0, 
				   xw,yw,Gdk::RGB_DITHER_NONE, 0, 0);
    }
    else {
      //draw a white rectangle
      Gdk::Rectangle r;
      r = Gdk::Rectangle(0,0,xw,yw);
      window->begin_paint_rect(r);
      cr->save();
      cr->set_source_rgb(1,1,1); 
      cr->paint();
      cr->restore();
      window->end_paint();
    
    }
    cr->set_line_width(4*pr);
    double intensity=0;
    
    // now draw a circle
    for(size_t i=0;i<sensors.size();i++) {
      //replace double val = sensors[i].getRatio(sensors.rawValue.get().vals[i]);
      double val = sensors[i].getRatio(sensors[i].rawValue.get());
      int xc, yc;
      xc = (sensors[i].pos.x+3)*pr;
      yc = (sensors[i].pos.y)*pr;

      cr->save();

      //FIX!!
      /*      if(sdm->get_active_sensor_no()==i)
	cr->set_source_rgba(0,0.5,0,1);    // color partially translucent
      else
      cr->set_source_rgba(0,0,0,1);    // color partially translucent*/


      //calculate translucent (how much pressed)
      if (sensors.size()!=1){ 
	intensity=0.2+0.8*helium::limit(0.0,1.0,val);
      }


      cfg::guide::Color circleColor;

      //BASE
      //selected
      if(get_active_sensor_no()==(int)i) {
	circleColor = colors.sensorTheme[1];
	cr->set_line_width(5.0*pr);
	if(intensity<1)
	  intensity = 1;
      }
      else {
	cr->set_line_width(4*pr);
	if (sensors[i].rawValue.get()==0) { //(sensors[i].state.get() != helium::WORKINGHWSTATE){//not connected
	  circleColor = colors.disable;
	}	
	else if(sensors[i].limitCalibrated.first && sensors[i].limitCalibrated.second) {//calibrated or not (only if connected)
	  circleColor = colors.sensorTheme[3];
	}
	else { //only connected
	  if(val<0)
	    circleColor = colors.sensorTheme[0];
	  else
	    circleColor = colors.sensorTheme[4];
	}
      }

      if (sensors[i].rawValue.get()==0) intensity = 0.7;

      cr->set_source_rgba(circleColor[0],circleColor[1],circleColor[2],0.7*intensity);
      cr->arc(xc, yc,sensorGui.radius*pr, 0.0, 2.0 *M_PI); // full circle
      cr->fill_preserve();

      //LINE (grayish outline base)
      cr->set_source_rgba(0.2,0.2,0.2,0.5);
      cr->stroke();
	
      //PRESSED LINE (minus or positive)      
      if (sensors[i].rawValue.get()>0) {//(sensors[i].state.get() != helium::WORKINGHWSTATE)
	cr->set_source_rgba(circleColor[0],circleColor[1],circleColor[2],1);
	if (val<0){
	  cr->arc(xc, yc,sensorGui.radius*pr, 2.0 *val*M_PI-M_PI/2,-M_PI/2); // full circle
	}else if (val>1){
	  cr->arc(xc, yc,sensorGui.radius*pr, -M_PI/2,-M_PI/2+2*M_PI*(val-1)); // full circle
	}else{	
	  //cr->arc(xc, yc,sensorGui.radius*pr, -M_PI/2, -M_PI/2+2.0 *val*M_PI); // full circle	  
	  cr->arc(xc, yc,sensorGui.radius*pr, -M_PI/2, -M_PI/2+2.0 *val*M_PI); // full circle	  
	}
	cr->stroke();
      }

      //CROSS (faulty sensor)
      if(sensors.state.isConnected() && sensors.state.get().data[i] != helium::WORKINGHWSTATE) {
	cr->move_to(xc-sensorGui.radius*0.7*pr,yc-sensorGui.radius*0.7*pr);
	cr->line_to(xc+sensorGui.radius*0.7*pr,yc+sensorGui.radius*0.7*pr);
	cr->move_to(xc+sensorGui.radius*0.7*pr,yc-sensorGui.radius*0.7*pr);
	cr->line_to(xc-sensorGui.radius*0.7*pr,yc+sensorGui.radius*0.7*pr);

	if(sensors.monitorMode.isConnected() &&  sensors.monitorMode.get().data[i] == helium::NOHWMON)//different color when the error is ignored
	  cr->set_source_rgba(colors.disable[0],colors.disable[1],colors.disable[2],0.7);
	else 
	  cr->set_source_rgba(colors.powerState[0][0],colors.powerState[0][1],colors.powerState[0][2],0.7);

	cr->stroke();
	cr->restore();
      }

    }
  }
  return 1;
}

//force program to update the drawing area
void SensorCalibDrawingWidget::on_refresh() {
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win) {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

int SensorCalibDrawingWidget::get_active_sensor_no() {
  return active_sensor;
}

/*
void SensorCalibDrawingWidget::set_active_sensor_no(int i) {
  active_sensor.assure(i);
}
*/

int SensorCalibDrawingWidget::update_active_sensor_no(int x, int y,double scale) {
  x = (double)x/scale;
  y = (double)y/scale;
  //active_sensor.assure(-1);
  double min = sensorGui.radius;
  int min_sen = -1;
  // calculate the mouse click is closest to which sensor
  for(size_t i=0;i<sensors.size();i++) {

    double dist = sqrt((double)(sensors[i].pos.x-x)*(sensors[i].pos.x-x) + (double)(sensors[i].pos.y-y)*(sensors[i].pos.y-y) );    
    if (dist < min) {
      min_sen = i;
      min = dist;
    }
  }
  //active_sensor.assure(min_sen);
  return min_sen;
}
