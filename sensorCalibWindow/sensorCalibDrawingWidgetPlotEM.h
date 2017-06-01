///for data analysis
#ifndef SENSORCALIBDRAWINGWIDGETPLOT
#define SENSORCALIBDRAWINGWIDGETPLOT

#include <app/guide2/userInfo.h>

class SensorValues:public helium::Array<double> {
};

class SensorYTouch { 
public:
  std::vector<SensorValues> v;
  SensorValues exid; ///mapping the exoeriment id into real robot id
  SensorValues misid; ///missing id
  int user;
};


class SensorCalibDrawingWidgetPlotEM : public SensorCalibDrawingWidget {
protected:
  bool on_expose_event(GdkEventExpose* event) {

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




      //draw sensor circle and number

	double r = sensorGui.radius*pr/1.5;       


	//for missing id
       	for(size_t mi=0;mi<sy.misid.size();mi++) {
	  //set location
	  int xc, yc;
	  int i = sy.misid[mi];
	  xc = (sensors[i].pos.x+3)*pr;
	  yc = (sensors[i].pos.y)*pr;
	  cr->save();
	  cr->set_source_rgba(0.2,0.2,0.2,1);

	  //OUTER LINE
	  //selected
	  if(get_active_sensor_no()==(int)i)
	    cr->set_line_width(5.0*pr);
	  else
	    cr->set_line_width(1*pr);
	  cr->arc(xc, yc,r, 0.0, 2.0 *M_PI); // full circle

	  //fill color
	  cr->set_source_rgba(0.1,0.1,0.1,0.8);
	  cr->fill_preserve();	  

	  //stroke color
	  cr->set_source_rgba(0.2,0.2,0.2,0.9);
	  cr->stroke();


	  //write id
	  cr->set_font_size(13*pr); 
	  cr->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
	  cr->set_line_width(1*pr);
	  Cairo::TextExtents txext;
	  Glib::ustring text;
	  helium::convert(text,i);
	  cr->get_text_extents( text, txext );
	  cr->translate(xc,yc);
	  cr->move_to(-txext.width/2,+txext.height/2);
	  cr->show_text(text);
	  cr->restore();
	  cr->stroke();

	}
	std::cout << sy.user << std::endl;
	std::cout << sy.v[sy.user] << std::endl;
	//for valid id
       	for(size_t ei=0;ei<sy.exid.size();ei++) {
	  //set location
	  int xc, yc;
	  int i = sy.exid[ei];
	  //std::cout << ei<<" " << sy.v[sy.user][i] << " " << i <<std::endl;
	  xc = (sensors[i].pos.x+3)*pr;
	  yc = (sensors[i].pos.y)*pr;
	  cr->save();
	  cr->set_source_rgba(0.2,0.2,0.2,1);

	  //OUTER LINE
	  //selected
	  if(get_active_sensor_no()==(int)i)
	    cr->set_line_width(5.0*pr);
	  else
	    cr->set_line_width(1*pr);
	  cr->arc(xc, yc,r, 0.0, 2.0 *M_PI); // full circle

	  //fill color
	  cr->set_source_rgba(0.5,0.1,0.1,sy.v[sy.user][ei]);
	  cr->fill_preserve();	  

	  //stroke color
	  cr->set_source_rgba(0.2,0.2,0.2,0.9);
	  cr->stroke();


	  //write id
	  cr->set_font_size(13*pr); 
	  cr->select_font_face( "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD );
	  cr->set_line_width(1*pr);
	  Cairo::TextExtents txext;
	  Glib::ustring text;
	  helium::convert(text,i);
	  cr->get_text_extents( text, txext );
	  cr->translate(xc,yc);
	  cr->move_to(-txext.width/2,+txext.height/2);
	  cr->show_text(text);
	  cr->restore();
	  cr->stroke();

	}
	  

    

	  /*


      //per user

      cr->set_line_width(4*pr);
      
      // now draw a circle
      for(size_t i=0;i<sensors.size();i++) {
	double intensity=0.9;

	if(isnan(plotValue[i])) {
	  continue;
	}
	
	int xc, yc;
	xc = (sensors[i].pos.x+3)*pr;
	yc = (sensors[i].pos.y)*pr;
	
	cr->save();
	
	cfg::guide::Color circleColor;
	
	//BASE
	//selected
	if(get_active_sensor_no()==(int)i) {
	  cr->set_line_width(5.0*pr);
	  if(intensity<1)
	    intensity = 1;
	  std::cout  << i << " " << plotValue[i] << std::endl;
	}
	else {
	  //cr->set_line_width(4*pr);
	  cr->set_line_width(1*pr);
	}

	circleColor[0] = circleColor[1] = circleColor[2] = 1;
	if(plotValue[i] < 0)
	  circleColor = colors.sensorTheme[4];
	else if(plotValue[i] > 0)
	  circleColor = colors.sensorTheme[0];
	//else
	//circleColor = colors.disable;//colors.sensorTheme[1];
	
	cr->set_source_rgba(circleColor[0],circleColor[1],circleColor[2],intensity);
	cr->arc(xc, yc,sensorGui.radius*pr/2, 0.0, 2.0 *M_PI); // full circle
	cr->fill_preserve();
	
	//LINE (grayish outline base)
	cr->set_source_rgba(0.2,0.2,0.2,(get_active_sensor_no()==(int)i)?0.9:0.5);
	cr->stroke();
      }
      




	  */












      /*

      else if(plotPie != NULL) {
      //pie for all

	cr->set_line_width(4*pr);

	double r = sensorGui.radius*pr;
	
	// now draw a circle
	for(size_t i=0;i<sensors.size();i++) {
	//for(size_t i=0;i<1;i++) {

	  //if the first user (done all) is nan, means that sensor is nan
	  if(isnan(plotPie[i][0])) {
	    continue;
	  }
	  
	  //calculate plus minus sum for plotting
	  double plus=0,minus=0,zero=0,num=0;
	  for(int u=0;u<experiment::USR;u++) {
	    if(!isnan(plotPie[i][u])){
	      num++;
	      if(plotPie[i][u] < 0) 
		minus++;
	      else if(plotPie[i][u] > 0)
		plus++;
	      else
		zero++;
	    }	      
	  }



	  int xc, yc;
	  xc = (sensors[i].pos.x+3)*pr;
	  yc = (sensors[i].pos.y)*pr;
	  
	  cr->save();
	  
	  cfg::guide::Color circleColor;
	  

	  //draw pie
	  cr->set_line_width(0);

	  //ZERO draw circle
	  cr->set_source_rgba(1,1,1,0.9); //white
	  cr->arc(xc, yc,r, 0.0, 2.0 * M_PI); // full circle
	  cr->fill_preserve();
	  cr->set_source_rgba(0.2,0.2,0.2,0);
	  cr->stroke();

	  double angle1 = -M_PI/2.0;
	  double angle2 = angle1 + minus/num * 2.0 * M_PI;

	  //MINUS / ACTIVE
	  circleColor = colors.sensorTheme[4];
	  cr->set_source_rgba(circleColor[0],circleColor[1],circleColor[2],0.9);

	  cr->move_to(xc,yc);
	  cr->arc(xc, yc,r, angle1, angle2);
	  cr->line_to(xc,yc);

	  cr->fill_preserve();
	  cr->set_source_rgba(0.2,0.2,0.2,0);
	  cr->stroke();

	  angle1 = angle2;
	  angle2 = angle1 + plus/num * 2.0 * M_PI;

	  //PLUS / PASSIVE
	  circleColor = colors.sensorTheme[0];
	  cr->set_source_rgba(circleColor[0],circleColor[1],circleColor[2],0.9);

	  cr->move_to(xc,yc);
	  cr->arc(xc, yc,r, angle1, angle2);
	  cr->line_to(xc,yc);

	  cr->fill_preserve();
	  cr->set_source_rgba(0.2,0.2,0.2,0);
	  cr->stroke();


	  //OUTER LINE
	  //selected
	  if(get_active_sensor_no()==(int)i) {
	    cr->set_line_width(5.0*pr);
	    //std::cout  << i << " " << plus << " " << minus << " " << zero  << std::endl;
	    //<< " tot=" << num <<" = +" << plus << " -" << minus << " _" << zero<< std::endl;
	    std::cout << user << " " << i << " " <<plotPie[i][user] << std::endl;
	  }
	  else {
	    cr->set_line_width(1*pr);
	  }
	  cr->arc(xc, yc,r, 0.0, 2.0 *M_PI); // full circle
	  cr->set_source_rgba(0.2,0.2,0.2,1);
	  cr->stroke();
	}
      }//pie plot




      //plot number
      else {
       

	

	}
      }

      */
    }
    
    return true;
  }
public:
  int user;
  SensorYTouch &sy;
  SensorCalibDrawingWidgetPlotEM(cfg::guide::Sensors &sensors,cfg::guide::SensorGui &sensorGui,cfg::guide::Colors &pcolors,SensorYTouch &psy): 
    SensorCalibDrawingWidget(sensors,sensorGui,pcolors),
    sy(psy)
  {
    user = sy.user;
    //std::cout << "sensor " << sy.exid.size() << std::endl;
    //std::cout << "sensor " << sy.v[1].size() << std::endl;
  }

};
#endif
