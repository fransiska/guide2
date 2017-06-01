/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include <iostream>
#include <math.h>
#include "guideCanvas.h"
#include <helium/util/mathUtil.h>
#include "inputDialog.h"

using namespace std;
using namespace gwidget;
using namespace cfg::guide;



GuideBgLoader::GuideBgLoader(string pbg,  cfg::guide::Size &winSize):
  loaded(false),
  format(Cairo::FORMAT_RGB24)
{
  if(pbg.empty())
    cerr << "error: no background file specified" << endl;
  try {
    img = Gdk::Pixbuf::create_from_file(pbg);

    // Detect transparent colors for loaded image
    if (img->get_has_alpha())
      format = Cairo::FORMAT_ARGB32;
    
    // Create a new ImageSurface
    imgCairo = Cairo::ImageSurface::create  (format, img->get_width(), img->get_height());    

    // Create the new Context for the ImageSurface
    context = Cairo::Context::create (imgCairo);
	
    // Draw the image on the new Context
    Gdk::Cairo::set_source_pixbuf (context, img, 0.0, 0.0);
    context->paint();

    loaded = true;

    winSize.w = img->get_width();
    winSize.h = img->get_height();

  }
  catch(Glib::FileError e) {
    cerr << "error: background file " << pbg <<" not found" << endl;
  }
  catch(Gdk::PixbufError e) {
    cerr << "error: background file " << pbg <<" not found" << endl;
  }
}

bool GuideBgLoader::isLoaded() {
  return loaded;
}

void GuideBgLoader::resize(const Glib::RefPtr<Gdk::Pixbuf> &scaleImage) {    
  imgCairo.clear();
  context.clear();

  imgCairo = Cairo::ImageSurface::create(format, scaleImage->get_width(), scaleImage->get_height());    
  context = Cairo::Context::create (imgCairo);
  
  // Draw the image on the new Context
  Gdk::Cairo::set_source_pixbuf (context, scaleImage, 0.0, 0.0);
  context->paint();
}

GuideGuiLoader::GuideGuiLoader(GuideState &gs, int w, int h){

  //position on the right hand side of the last limb switch
  int defX = gs.limbs[gs.limbs.size()-1].pos.x+gs.settings.sizes.frame.w*2-gs.settings.sizes.powerSwitch.w/2- gs.settings.sizes.stroke/2;
  int defY = gs.limbs[gs.limbs.size()-1].pos.y;  

  if(gs.settings.sizes.sensorWidget.w == -1 || gs.settings.sizes.sensorWidget.h == -1) 
    gs.settings.sizes.sensorWidget.set(defX,defY);
  
  
  if(gs.settings.sizes.gyroWidget.w == -1 || gs.settings.sizes.gyroWidget.h == -1) {
    if(gs.pref.hasTouchSensor) //set on top of touch sensor widget
      gs.settings.sizes.gyroWidget.set(gs.settings.sizes.sensorWidget.w,
				       gs.settings.sizes.sensorWidget.h - gs.settings.sizes.powerSwitch.h - gs.settings.sizes.stroke*2);
    else
      gs.settings.sizes.gyroWidget.set(defX,defY);
  }
  
  if(gs.settings.sizes.bigJointOffset.w == -1)
    gs.settings.sizes.bigJointOffset.w = w-gs.settings.sizes.bigJoint/2;
  if(gs.settings.sizes.bigJointOffset.h == -1) { 
    gs.settings.sizes.bigJointOffset.h = h-gs.settings.sizes.bigJoint/2-gs.settings.sizes.warningBar;
  }

  //cout << "loading " << gs.settings.sizes.bigJointOffset.w  << " " << gs.settings.sizes.bigJointOffset.h << endl;//<< gs.settings.sizes.bigJointOffsetY << " " << gs.settings.sizes.warningBarOffset << endl;
  
}


//const char* GuideCanvas::WidgetNameEnum::strings[] = {"mbigjoint","tbigjoint","cbigjoint","sync","frame","sensor","gyro","modelbox","coldetdisp","coldetautofix","joint0"};


GuideCanvas::GuideCanvas(Gtk::Window& pwin, AngleEntry* pae, HexEntry* phe, helium::ValueHub<double> &pscale, GuideMotionManagement &pgmm, coldet::ColdetManager *pcoldetm, RobotModels *rm):
  win(pwin),
  gmm(pgmm),
  coldetm(pcoldetm),
  bg(gmm.gs.pref.bgFilename,gmm.gs.settings.sizes.winSize),
  guiLoad(gmm.gs, getWidth0(), getHeight0()),
  ae(pae),he(phe),
  lei(*he),led(*ae),
  ctx(DrawingContext::get()),
  scale(pscale),
  
  globalProp(gmm.gs.settings,scale),
  jointGlobalProp(gmm.gs.conn),

  jointWidget(make_pair(this,&GuideCanvas::allocJointWidget),gmm.gs.joints.size()),
  limbWidget(make_pair(this,&GuideCanvas::allocLimbWidget),gmm.gs.limbs.size()),  
					 //widgets(gmm.gs.joints.size()+gmm.gs.limbs.size()+WidgetNameEnum::JOINT),
  motionBigJointWidget(Point2D(gmm.gs.settings.sizes.bigJointOffset.w,gmm.gs.settings.sizes.bigJointOffset.h),globalProp,jointGlobalProp,gmm.gs.joints,gmm.gs.view,led,coldetm),
  touchBigJointWidget(motionBigJointWidget.getAnchor(),
		      globalProp,jointGlobalProp,gmm.gs.joints,gmm.gs.view,coldetm),
  calibBigJointWidget(motionBigJointWidget.getAnchor(),
		      globalProp,jointGlobalProp,gmm.gs.joints,gmm.gs.view,lei),
  sensorWidget (WidgetProperty(Point2D(gmm.gs.settings.sizes.sensorWidget.w,gmm.gs.settings.sizes.sensorWidget.h),
			       gmm.gs.settings.sizes.frame.w*2,
			       gmm.gs.settings.sizes.powerSwitch.h),//h
		globalProp,gmm.gs.conn,
		showSubWindowSignal,gmm.gs.sensors),
  gyroWidget (WidgetProperty(Point2D(gmm.gs.settings.sizes.gyroWidget.w,gmm.gs.settings.sizes.gyroWidget.h),
			     gmm.gs.settings.sizes.frame.w*2,
			     gmm.gs.settings.sizes.powerSwitch.h),
	      globalProp,gmm.gs.conn,//h
	      showSubWindowSignal,gmm.gs.gyro),
  valueSetting(Point2D(gmm.gs.limbs[gmm.gs.limbs.size()-1].pos.x-(double)gmm.gs.settings.sizes.powerSwitch.w/2+gmm.gs.settings.sizes.arrowBox.w/2,
		       gmm.gs.limbs[gmm.gs.limbs.size()-1].pos.y+(double)gmm.gs.settings.sizes.powerSwitch.h/2+gmm.gs.settings.sizes.entry.h/2+gmm.gs.settings.sizes.arrowBox.h/2),
	       globalProp,gmm.gs.conn,gmm.gs.joints,coldetm,gmm.gs.view),
  frameSaveSetting(Point2D(gmm.gs.limbs[gmm.gs.limbs.size()-1].pos.x-(double)gmm.gs.settings.sizes.powerSwitch.w/2+gmm.gs.settings.sizes.frame.w/2,
			   valueSetting.getAnchor().y+gmm.gs.settings.sizes.arrowBox.h*3),
		   globalProp,gmm.gs.view,gmm.gs.motionInfo.editing.status),
  notSavedWidget(WidgetProperty(Point2D(globalProp.settings.sizes.winSize.w/2,globalProp.settings.sizes.winSize.h-globalProp.settings.sizes.warningBar/2),
				globalProp.settings.sizes.winSize.w,globalProp.settings.sizes.warningBar/*,true*/),globalProp,
		 gmm.gs.view,
		 gmm.gs.motionInfo.editing.status,
		 gmm.saved.calib),
  modelBoxWidget(WidgetProperty(Point2D(gmm.gs.settings.sizes.model.w/2+gmm.gs.settings.sizes.stroke,gmm.gs.settings.sizes.model.h/2+gmm.gs.settings.sizes.stroke),gmm.gs.settings.sizes.model.w+gmm.gs.settings.sizes.stroke*2,gmm.gs.settings.sizes.model.h+gmm.gs.settings.sizes.stroke*2/*,true*/),globalProp),
  prevScale(scale.get()),
  callOnClickOther(this),
 callOnFrameSaveSignal(this)
{
  if(gmm.gs.pref.hasColdet) {
    coldetAutoFixWidget.reset(new ColdetAutoFixWidget(Point2D(gmm.gs.limbs[gmm.gs.limbs.size()-1].pos.x-(double)gmm.gs.settings.sizes.powerSwitch.w/2+gmm.gs.settings.sizes.frame.w*3/2,frameSaveSetting.getAnchor().y+gmm.gs.settings.sizes.frame.h/2+gmm.gs.settings.sizes.frame.h/4),globalProp,*coldetm,gmm.gs.coldet));
    coldetDisplayWidget.reset(new ColdetDisplayWidget(WidgetProperty(Point2D(gmm.gs.settings.sizes.model.w+gmm.gs.settings.sizes.coldetBox.w/2+gmm.gs.settings.sizes.stroke*3,gmm.gs.settings.sizes.coldetBox.h/2),
								     gmm.gs.settings.sizes.coldetBox.w,gmm.gs.settings.sizes.coldetBox.h),
						      globalProp,gmm.gs.coldet,&showSubWindowSignal));
  }

  set_size_request(getWidth0(),getHeight0());

  widgets.push_back(&motionBigJointWidget);
  widgetNames.push_back("mbigjoint");
  widgets.push_back(&touchBigJointWidget);
  widgetNames.push_back("tbigjoint");
  widgets.push_back(&calibBigJointWidget); 
  widgetNames.push_back("cbigjoint");

  widgets.push_back(&valueSetting);    //frame sync/reset setting
  widgetNames.push_back("valuesetting");
  valueSetting.setVisible(true);
  widgets.push_back(&frameSaveSetting); //for onEvent check loop   //frame setting
  widgetNames.push_back("frame");
  widgets.push_back(&notSavedWidget);
  widgetNames.push_back("notsaved");


  if(gmm.gs.pref.hasModel) {  
    widgets.push_back(&modelBoxWidget);   //collision detection enable indicator
    widgetNames.push_back("modelbox");
  }
  if(gmm.gs.pref.hasTouchSensor) {
    widgets.push_back(&sensorWidget);   //sensor indicator
    widgetNames.push_back("sensor");
  }
  if(gmm.gs.pref.hasGyro) {
    widgets.push_back(&gyroWidget);   //sensor indicator
    widgetNames.push_back("gyro");
  }
  if(gmm.gs.pref.hasColdet) {
    widgets.push_back(coldetDisplayWidget.get()); //collision detection display widget
    widgets.push_back(coldetAutoFixWidget.get()); //button to copy auto fix
    widgetNames.push_back("coldetdisp");
    widgetNames.push_back("coldetautofix");
    coldetDisplayWidget.get()->setVisible(true);
  }


  //joint widget
  for(size_t i=0; i<gmm.gs.joints.size();i++) {
    //widget collections = for onEvent check loop
    widgets.push_back(&jointWidget[i]);
    //widget name
    stringstream ss;
    ss << "joint" << i;
    widgetNames.push_back(ss.str());
  }

  //limb widget
  for(size_t i=0;i<gmm.gs.limbs.size();i++) {
    widgets.push_back(&limbWidget[i]); //for onEvent check loop
    stringstream ss;
    ss << "limb" << i;
    widgetNames.push_back(ss.str());    
  }

  /// events: press, release, motion
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK);
  signal_button_press_event().connect(gmm.eh.wrap(*this,&GuideCanvas::onClick));
  signal_button_release_event().connect(gmm.eh.wrap(*this,&GuideCanvas::onRelease));
  signal_motion_notify_event().connect(gmm.eh.wrap(*this,&GuideCanvas::onMotion));

  he->signal_value_changed().connect(gmm.eh.wrap(lei,&LinkableEntry<int>::onGtkChange));
  ae->signal_value_changed().connect(gmm.eh.wrap(led,&LinkableEntry<double>::onGtkChange));

  he->signal_hide().connect(gmm.eh.wrap(lei,&LinkableEntry<int>::unLink));
  ae->signal_hide().connect(gmm.eh.wrap(led,&LinkableEntry<double>::unLink));

  frameSaveSetting.frameSaveSignal.connect(callOnFrameSaveSignal);

  ctx->setDa(this);
  ctx->setWidth0(getWidth0());
  ctx->setHeight0(getHeight0());

  if(gmm.gs.pref.hasModel && rm) {
    /// \todo widgets <-> robotModels.mainCanvasArea.modelid connections
    rm->mainCanvasArea.modelid.connect(modelBoxWidget.callSwitchModelBox);
    valueSetting.widgets[ValueSetting::AutoBox]->isMouseOver.connect(rm->mainCanvasArea.callSetModelHoverFix);

    if(gmm.gs.pref.hasColdet) {
      coldetAutoFixWidget.get()->isMouseOver.connect(rm->mainCanvasArea.callSetModelHoverReach);
      
      //set signal for coldetManager to rotate fix model for viewing
      coldetm->rotateModelToAutoFixPostureSignal.connect(rm->mainCanvasArea.callSetAutoFixPosture);
    }
  }

  set_flags(Gtk::CAN_FOCUS);   //Gtk::Widget::set_can_focus(true);
}


void GuideCanvas::onClickOther(GdkEventButton * event) {
  ae->hide();
  he->hide();
}

bool GuideCanvas::onClick(GdkEventButton * event){
  grab_focus(); // make the focus on guideCanvas
  for(size_t i=0; i<widgets.size(); i++) {
    if(widgets[i]->onCanvasClick(event))
      return true;
  }

  return false;
}

bool GuideCanvas::onRelease(GdkEventButton * event){
  ae->hide();
  he->hide(); //hide gtk entries

  for(size_t i=0; i<widgets.size(); i++) {
    widgets[i]->onCanvasRelease(event);
  }
  return false;
}

bool GuideCanvas::onKeyPressArrow(GdkEventKey* event) {
  //cout << "key event is by " << event->window << " " << this->get_window() << endl;
  bool increase = true;
  bool page = false;

  //check for up/down/left/right arrow key press 
  switch (event->keyval) {
  case GDK_Page_Up:
    page = true;    
  case GDK_Up:      //cout << "up";
  case GDK_Left:    //cout << "left";
    increase = false;
    break;
  case GDK_Page_Down:
    page = true;
  case GDK_Right:   //cout << "right";
  case GDK_Down:    //cout << "down";
    break;   
    //case GDK_Shift_Mask:
    //break;
  default:          //cout << "unhandled key";
    return false;
  }
  //cout << endl;
  //cout << "guidecanvas key shift " << (event->state&GDK_SHIFT_MASK) << endl;
  if(motionBigJointWidget.getState().visible) {
    //cout << "moving motionbig by key" << endl;
    motionBigJointWidget.moveCursor(increase,page,event->state&GDK_SHIFT_MASK);
  }
  else if(calibBigJointWidget.getState().visible) {
    //cout << "moving calibbig by key" << endl;
    calibBigJointWidget.moveCursor(increase,page,event->state&GDK_SHIFT_MASK);
  }

  return true;




}

bool GuideCanvas::onMotion(GdkEventMotion * event){
  for(size_t i=0; i<widgets.size(); i++) {
    widgets[i]->onCanvasMotion(event);
  }
  return true;
}

bool GuideCanvas::on_expose_event(GdkEventExpose* event) {
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window) {
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    Glib::RefPtr<Pango::Layout> pl = Pango::Layout::create (cr); //text

    cr->rectangle( event->area.x, event->area.y, event->area.width, event->area.height );
    cr->clip();

    ctx->setCtx(cr,pl,window);
    if(bg.isLoaded()) 
      drawBackground(cr,event);
    else {
      cr->save();
      cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
      cr->set_source_rgb(1,1,1);
      cr->fill_preserve();
      cr->restore();
      cr->stroke();
    }
    for(size_t i=0; i<widgets.size();i++)
      widgets[i]->canvasDraw();
  }
  return true;
}

void GuideCanvas::drawBackground(Cairo::RefPtr<Cairo::Context> &cr,GdkEventExpose* event) {  
  if(prevScale != scale.get()) {
    scaleImage = bg.img->scale_simple(get_width(),get_height(),Gdk::INTERP_BILINEAR);
    bg.resize(scaleImage);
  }

  // Store context
  cr->save();
  
  // Draw the source image on the widget context
  cr->set_source (bg.imgCairo, 0.0, 0.0);
  cr->rectangle (0.0, 0.0, get_width(), get_height());
  cr->clip();
  cr->paint();
  
  // Restore context
  cr->restore();
  prevScale = scale.get();
}

void GuideCanvas::drawBackground() {
  if(prevScale != scale.get() || scaleImage) {
    scaleImage = bg.img->scale_simple(get_width(),get_height(),Gdk::INTERP_BILINEAR);
  }
  scaleImage->render_to_drawable(this->get_window(),
				 this->get_style()->get_white_gc(), 0, 0, 0, 0, 	
				 get_width(),get_height(),Gdk::RGB_DITHER_NONE, 0, 0);
  prevScale = scale.get();
}

void GuideCanvas::refresh() {
  Glib::RefPtr<Gdk::Window> window = get_window();
  if (window) {
    Gdk::Rectangle r(0, 0, get_width(),get_height());
    window->invalidate_rect(r, false);
  }
}

void GuideCanvas::onFrameSaveSignal(gwidget::FrameSaveTo::Type to) {
  string frame = "";
  helium::Time t = -1; //ok is pressed

  if(to == gwidget::FrameSaveTo::SELECTFRAME) {
    InputDialog inputDialog("Input a frame number",frame,&gmm.eh);  //dialog window
    inputDialog.set_transient_for(win);
    if(inputDialog.run()) {
      helium::convert(t,frame," Invalid frame time");
      //valid time
      if(t < 0) {
	cerr << "Error: Frame time can't be negative" << endl;
	return;
      }
    }
  }
  gmm.onSaveFrame(to,t);
}
