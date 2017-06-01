/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDECANVAS
#define GUIDECANVAS

#include <vector>


#include <app/guide2/angleEntry.h>

#include <app/guide2/widgetDrawer.h>

#include "guideState.h"
#include <helium/signal/callback.h>

#include <helium/compilerWorkaround/CWAinclassf.h>
#include "coldetManager.h"
#include "robotModels.h"



class GuideBgLoader {
  bool loaded;
public:
  Cairo::Format format;
  Cairo::RefPtr< Cairo::Context > context;
  Glib::RefPtr<Gdk::Pixbuf> img;
  Cairo::RefPtr<Cairo::ImageSurface> imgCairo;
  /*
   * @param w replace w with width of image if necessary
   * @param h replace h with width of image if necessary
   */
  GuideBgLoader(std::string pbg, cfg::guide::Size &winSize);
  //friend class GuideCanvas;
  bool isLoaded();
  void resize(const Glib::RefPtr<Gdk::Pixbuf> &scaleImage);
};

/**
 * checks gui elements loaded from xml
 * auto fixes position of:
 * - sensorWidget, gyroWidget
 */
class GuideGuiLoader {
public:
  GuideGuiLoader(GuideState &gs, int w, int h);
};
/**
 * \brief canvas for drawing widgets in Guide
 * there can only be one canvas in Guide
 */
class GuideCanvas : public Gtk::DrawingArea {
  Gtk::Window& win;
  friend class GuideWindow;
  friend class GuideJointLoader2;
public:
  //enum WidgetName {MBIGJOINT, TBIGJOINT, CBIGJOINT, SYNC, FRAME, SENSOR, GYRO, COLLDETBOX, JOINT};

  ///enum for widgets in GuideCanvas
//  struct WidgetNameEnum {
//    typedef enum {MBIGJOINT, TBIGJOINT, CBIGJOINT, SYNC, FRAME, SENSOR, GYRO, MODELBOX, COLDETDISP, COLDETAUTOFIX, JOINT} Type;
//    ///strings for the main widgets (not containing the name for each motor joint widgets)
//    static const char* strings[];
//  };
  ///strings for the complete widgets in GuideCanvas (including the names for joint widgets)
  std::vector<std::string> widgetNames;

 protected:
  GuideMotionManagement &gmm;
  coldet::ColdetManager *coldetm;
  GuideBgLoader bg;
  GuideGuiLoader guiLoad;

  AngleEntry *ae;
  HexEntry *he;
  LinkableEntry<int> lei;
  LinkableEntry<double> led;

  gwidget::DrawingContext *ctx;
  helium::ValueHub<double> &scale;

  gwidget::WidgetGlobalProperty globalProp;
  gwidget::JointGlobalProperty jointGlobalProp;

  //  void allocJointWidgetProp(gwidget::JointWidgetProperty* jwp, int i) {
  //new (jwp) gwidget::JointWidgetProperties(i,i>=(int)gmm.gs.joints.size()?NULL:&gmm.gs.joints[i]);
  //}
  void allocJointWidget(gwidget::JointWidget* jew,int i) {
    new (jew) gwidget::JointWidget(&gmm.gs.joints[i],globalProp,jointGlobalProp,led,gmm.gs.view);//gmm.gs.settings,&jointWidgetProperties[i],scale,led,&gmm.gs.conn);
  }
  void allocLimbWidget(gwidget::LimbWidget* lw,int i) {
    new (lw) gwidget::LimbWidget(globalProp,gmm.gs.limbs[i],gmm.gs.joints,gmm.gs.conn);
  }

  
//  helium::Array<gwidget::JointWidgetProperty,helium::array::RESIZABLE,
//		helium::array::InternalAllocator<helium::DynamicClassPlacerAllocator
//						 <gwidget::JointWidgetProperties, GuideCanvas> 
//						 > > jointWidgetProperties;

  helium::Array<gwidget::JointWidget,helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::DynamicClassPlacerAllocator
						 <gwidget::JointWidget, GuideCanvas> 
						 > > jointWidget;

  helium::Array<gwidget::LimbWidget,helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::DynamicClassPlacerAllocator
						 <gwidget::LimbWidget, GuideCanvas> 
						 > > limbWidget;

public:
  std::vector<gwidget::GWidget*> widgets; ///< array of gwidgets to be able to do loop on general widget functions
protected:
  //int dragCalibParam;
  //std::pair<double,int> dragMotionParam;

  gwidget::MotionBigJointWidget motionBigJointWidget;
  gwidget::TouchBigJointWidget touchBigJointWidget;
  gwidget::CalibBigJointWidget calibBigJointWidget;
  gwidget::SensorWidget sensorWidget;
  gwidget::GyroWidget gyroWidget;
  gwidget::ValueSetting valueSetting;  
  gwidget::FrameSaveSetting frameSaveSetting;
  gwidget::NotSavedWidget notSavedWidget;
  gwidget::ModelBoxWidget modelBoxWidget;
  std::auto_ptr<gwidget::ColdetDisplayWidget> coldetDisplayWidget;
  std::auto_ptr<gwidget::ColdetAutoFixWidget> coldetAutoFixWidget;

private:
  //passed values
  //int clickId,releaseId;
  double prevScale;
  Glib::RefPtr<Gdk::Pixbuf> scaleImage;
  //std::pair<helium::ValueHub<int>,helium::ValueHub<gwidget::JointElementType> > selectedJoint;
  //std::pair<int,gwidget::JointElementType> prevSelectedJoint;
  //helium::ValueHub<int> draggedJoint;
  //bool draggedCursor; // to check if user dragged any cursor (to save state for undo)

 public:
  GuideCanvas(Gtk::Window& pwin, AngleEntry* pae, HexEntry* phe,/*WidgetParser *pwp, */helium::ValueHub<double> &pscale, GuideMotionManagement &pgmm, coldet::ColdetManager *pcoldetm,RobotModels *rm);
  //virtual ~GuideCanvas();

  //different view related
  void setMotionView(bool b);
  void refresh();
  void drawBackground();
  void drawBackground(Cairo::RefPtr<Cairo::Context> &cr,GdkEventExpose* event);
  //void drawCollDetBox(bool b);
  //on events
  virtual bool on_expose_event(GdkEventExpose* event);
  bool onClick(GdkEventButton * event);
  bool onRelease(GdkEventButton * event);
  bool onMotion(GdkEventMotion * event);
  void onClicked(GdkEventButton * event);
  bool onKeyPressArrow(GdkEventKey* event);
  //void onResize();
  void onCopyValueChange();
  void onClickOther(GdkEventButton * event);
  //void onConnection(helium::Connection c);

  bool onDrag(GdkEventButton * event);
  bool onDrop(GdkEventButton * event);
  DragData dragData;

  //void refreshJoint(int i);

  int getWidth0() {return /*bg.img?bg.img->get_width():*/gmm.gs.settings.sizes.winSize.w; }
  int getHeight0() {return /*bg.img?bg.img->get_height():*/gmm.gs.settings.sizes.winSize.h; }

  //update thru boost
  void test(double* x);
  void testp() {std::cout << "testp" << std::endl;} //todo: delete

protected:
  //helium::StaticConnReference<int,GuideCanvas,int,&GuideCanvas::refreshJoint> callRefreshJoint;
  //helium::StaticConnReference<double,GuideCanvas,void,&GuideCanvas::onResize> callOnResize;
  //helium::InternalConnReference<int> callRefreshJoint;
  //helium::StaticConnReference<helium::Connection,GuideCanvas,helium::Connection,&GuideCanvas::onConnection> callOnConnection;
  helium::StaticConnReference<GdkEventButton*,GuideCanvas,GdkEventButton*,&GuideCanvas::onClickOther> callOnClickOther; ///< hide  entries
  //helium::InternalConnReference<double> callOnResize;

  helium::TsSignal<GdkEventMotion*> onMotionSignal;
public:
  helium::TsSignal<std::string> showSubWindowSignal;
protected:
  //helium::TsSignal<std::string> setShowSensorWindow;
  //helium::TsSignal<std::string> setShowGyroWindow;
  //helium::TsSignal<std::string> setShowColdetWindow; ///< passed to widget to be called on clicked event, connected in guideWindow

  //sigc::connection callDragMotionView;
  //sigc::connection callDragCalibView;
  //sigc::connection callDropMotionView;
  //sigc::connection callDropCalibView;

private:
  void onFrameSaveSignal(gwidget::FrameSaveTo::Type to);
  helium::StaticConnReference<gwidget::FrameSaveTo::Type,GuideCanvas,gwidget::FrameSaveTo::Type,&GuideCanvas::onFrameSaveSignal> callOnFrameSaveSignal;


  //helium::StaticConnReference<std::string,GuideMotionManagement,std::string,&GuideMotionManagement::execute> callExecuteEnableSwitch; //DEBUG to be replaced with GuideHistory
  //helium::StaticConnReference<std::string,GuideMotionManagement,std::string,&GuideMotionManagement::execute> callExecuteEnableEntry;

  //void setJoint(gwidget::BigJointState::Type t, int a);
protected:
  class SetJointCaller; 
  friend class SetJointCaller;
  std::auto_ptr<SetJointCaller> callSetJoints;//vs2010 workaround
  friend struct GuideCanvasCallSetJoint;
 

};

/*
struct GuideCanvasCallSetJoint{
 typedef helium::ParamStaticConnReference<gwidget::BigJointState::Type,GuideCanvas,int,
					   &GuideCanvas::setJoint> Type; 

  typedef helium::Array<GuideCanvasCallSetJoint::Type,
	      helium::array::RESIZABLE,
	      helium::array::InternalAllocator<helium::ParamIdxAllocator<GuideCanvasCallSetJoint::Type,GuideCanvas*> > 
			> CallerArray;


};

class GuideCanvas::SetJointCaller:public GuideCanvasCallSetJoint::CallerArray{
public:
  SetJointCaller(GuideCanvas* gc,size_t n):
    GuideCanvasCallSetJoint::CallerArray(gc,n){}

};
*/

  






#endif
