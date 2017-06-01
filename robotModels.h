/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef ROBOTMODELS
#define ROBOTMODELS
#include <memory>
#include "glarea.h"
#include <helium/convex/convexModel.h>
#include <helium/convex/drawConvex.h>
#include <helium/graphics/glUtil.h>
#include <app/guide2/guideState.h>

class RobotModels{
public:

  typedef Glib::RefPtr<Gdk::Pixbuf> PixbufPtr;
  typedef helium::Array<unsigned char> PostureSnap;
  typedef helium::ValueHub<int> ValueHubInt;

  /** \brief class as a wrapper for helium::convex::ConvexCollisionModel 
   *  to be able to signal when rotate function is called (for refreshing widget)
   */
  class SignallingConvexCollisionModel : public helium::convex::ConvexCollisionModel {

  public:
    SignallingConvexCollisionModel(const helium::convex::ConvexModel *cm); ///< default constructor
    SignallingConvexCollisionModel(const ConvexCollisionModel& ccm, double* d); ///<copy constructor

    void rotate(const double *v);    ///< override
    void rotate(const helium::Posture &p);    ///< override
    void rotate(int j, double d);     ///< override
    helium::TsSignal<void> signalRotated; ///< connect to this signal to be notified of ccm rotation
  };

  /**
   * \brief handles connection to model rotate for rotation per specified joint values, subclass of SignallingConvexCollisionModel
   */
  class RobotSCCM : public SignallingConvexCollisionModel {
    void crotate(double value, int id);    ///< call the correct rotate function here

    typedef helium::ParamStaticConnReference<double,RobotModels::RobotSCCM,int,&RobotModels::RobotSCCM::crotate> CallRotateRobot;     ///< set the connection to pass on id as int as well as the updated value
    typedef helium::array::InternalAllocator<helium::ParamIdxAllocator<CallRotateRobot,RobotModels::RobotSCCM*> > CallRotateRobotAlloc;     ///< set the connection to pass on id as int as well as the updated value

    helium::Array<CallRotateRobot,
		  helium::array::RESIZABLE,
		  CallRotateRobotAlloc> callRotateRobot;///< connection #robotModel with cfg::guide::Joint::displayedTarget, passing id as well as value
  public:
    /**
     * @param cm convexmodel
     * @param joints robot joints (guideState)
     */
    RobotSCCM(const helium::convex::ConvexModel *cm, cfg::guide::Joints &joints); ///< default constructor
  };

  /**
   * \brief handles connection to model rotate for rotation per specified joint values, subclass of SignallingConvexCollisionModel
   */
  class FrameSCCM : public SignallingConvexCollisionModel {
    void crotate(helium::FrameValue value, int id);    ///< call the correct rotate function here
    typedef helium::ParamStaticConnReference<helium::FrameValue,RobotModels::FrameSCCM,int,&RobotModels::FrameSCCM::crotate> CallRotateFrame;     ///< set the connection to pass on id as int as well as the updated value
    typedef helium::array::InternalAllocator<helium::ParamIdxAllocator<CallRotateFrame,RobotModels::FrameSCCM*> > CallRotateFrameAlloc;     ///< set the connection to pass on id as int as well as the updated value

    /// connection #frameModel with cfg::guide::Joint::frameValue
    helium::Array<CallRotateFrame,
		  helium::array::RESIZABLE,
		  CallRotateFrameAlloc> callRotateFrame;
  public:
    FrameSCCM(const ConvexCollisionModel& ccm, double* d, cfg::guide::Joints &joints); 
  };

public:  
  /** \brief wrapper of GlArea, handles key press, rotate model on setPosture signal,  redraw on model rotate signal
   */
  class ModelAreaBase:public GlArea{
    RobotModels::SignallingConvexCollisionModel& model;
    int w,h;
    double scale; ///< scale passed is copied here, 1 by default 
    Gtk::EventBox vbox; ///< box to put the glArea in    
  protected:
    helium::convex::DrawConvex* drawConvex; ///< drawConvex is sized 800x600 by default
  public:
    ModelAreaBase(RobotModels::SignallingConvexCollisionModel& c,helium::convex::DrawConvex* drawConvex);
    virtual ~ModelAreaBase();///< disconnect signals
  public:
    bool onGdkKeyPress(GdkEventKey* event); ///< calls the key press
    bool onKeyPress(char c, int x, int y);
    void face(helium::convex::Faces);
    void setViewMatrix(const double*);
    double* getViewMatrix();
    void setWorldMatrix(const double*);
    void setBG(const double* b); ///< set the background color of drawconvex
    void setSize(int pw, int ph); ///<point the default size to a variable of the parent
    void setScale(double pscale); ///< to set the scale of the vbox
    void update(); ///< to set the area to be redrawn
    void draw(); ///< drawconvex draw
    void rotate(const double *d); ///< model.rotate
    Gtk::EventBox& getBox(); ///<get the vbox to be added to a Gtk::Window
    bool onFocus(Gtk::DirectionType direction);
    bool onMouseOver(GdkEventCrossing* event);
    helium::TsSignal<const double *> setPosture; ///< gives signal to trigger rotation on model (used to completely change robot posture)
    helium::TsSignal<void> keyPressed;
    int getWidth();
    int getHeight();
  private:
    void resizeDrawConvex(const std::pair<int,int>& s); ///< interfacing helium::GLPrimitiveCanvas::resize(int,int) from connection
    helium::StaticConnReference<void,helium::GlPrimitiveCanvas,void,&helium::GlPrimitiveCanvas::draw> drawConnection; ///< glArea onDraw connection to drawConvex
    helium::StaticConnReference<const std::pair<int,int>&,RobotModels::ModelAreaBase,const std::pair<int,int>&,&RobotModels::ModelAreaBase::resizeDrawConvex> resizeConnection; ///< glArea onResize connection to drawConvex
    helium::InternalConnReference<const double*> callRotate;  ///< connection setPosture() to model rotate
  protected:
    helium::InternalConnReference<void> callUpdate; ///< connection model's rotated signal to queue_area
    helium::StaticConnReference<const double*,RobotModels::ModelAreaBase,const double*,&RobotModels::ModelAreaBase::setBG> callSetBG;  ///< connection model ccm setBG signal to ModelArea::setBG()
  };

  /** \brief model area with specific drawconvex
   */
  class ModelArea:public ModelAreaBase{
  protected:
    helium::convex::DrawConvex drawConvex;
  public:
    ModelArea(RobotModels::SignallingConvexCollisionModel& c);
    double* getViewMatrix(){return drawConvex.getViewMatrix();}
  };


  /** \brief model area with 2 different ccm model, can switch to view between those 2
   *
   */
  class SwitchingModelArea : public ModelArea {
    helium::Array<RobotModels::SignallingConvexCollisionModel*> model;
    helium::Array<double[3]> bgcolor;
  public:
    ValueHubInt modelid; ///< to set/get which model is being displayed
    helium::TsSignal<std::pair<int,double*> > signalSetBG;  ///<signal to set BG to certain model
    //helium::TsSignal<const double*> signalSetRobotMovingBG; ///< signal to tell this ModelArea to be refreshed because of changing BG color due to robot moving
    //helium::TsSignal<void> evalSetRobotMovingBG; ///< signal to ask RobotModels to reevaluate the color even when the model is not changing

  protected:
    void setBG(std::pair<int,double*> c); ///< from signal signalSetBG, indicating which model to set BG color to
    void setModel(int id); ///< do connections and set bgColor
    helium::StaticConnReference<int,RobotModels::SwitchingModelArea,int,&RobotModels::SwitchingModelArea::setModel> callSetModel;  ///< connection change of model id to setModel()
    helium::StaticConnReference<std::pair<int,double*>,RobotModels::SwitchingModelArea,std::pair<int,double*>,&RobotModels::SwitchingModelArea::setBG> callSetBG;  ///< connection change of model id to setModel()
  public:
    SwitchingModelArea(RobotModels::SignallingConvexCollisionModel& pca,RobotModels::SignallingConvexCollisionModel& pcb,RobotModels::SignallingConvexCollisionModel& pcc);
  };

  /** \brief model area for main canvas, includes related signals from widgets
   */
  class MainCanvasArea : public SwitchingModelArea {
  private:
    void setModeltoRobot(bool b); ///< set model to robot
    void setModelHover(bool b); ///< when mouse over on AutoReachBox, temporarily show fix model
  public:
    helium::StaticConnReference<bool,RobotModels::MainCanvasArea,bool,&RobotModels::MainCanvasArea::setModeltoRobot> callSetModeltoRobot;  ///< signal to reevaluate BG color + show robot model
    helium::StaticConnReference<bool,RobotModels::MainCanvasArea,bool,&RobotModels::MainCanvasArea::setModelHover> callSetModelHoverReach;  ///< connection from gwidget::AutoReachBox
    helium::StaticConnReference<bool,RobotModels::MainCanvasArea,bool,&RobotModels::MainCanvasArea::setModelHover> callSetModelHoverFix;  ///< connection from gwidget::ColdetAutoFixBox
    helium::StaticConnReference<const helium::Posture&,RobotModels::SignallingConvexCollisionModel,const helium::Posture&,&RobotModels::SignallingConvexCollisionModel::rotate> callSetAutoFixPosture;  ///< connection from gwidget::ColdetAutoFixBox
    MainCanvasArea(RobotModels::SignallingConvexCollisionModel& pca,RobotModels::SignallingConvexCollisionModel& pcb,RobotModels::SignallingConvexCollisionModel& pcc);
  };

public:

  /**
   * \brief A small window showing the model of the robot
   */
  template<typename MA>
  class ModelWindow:public Gtk::Window{    
    int w,h;
    Gtk::AspectFrame frame_gl;    
  protected:
    MA modelArea;
  public:
    ModelWindow(RobotModels::SignallingConvexCollisionModel& c);
    void setBG(const double* b); 
    void setViewMatrix(const double* m);
    double* getViewMatrix();
    void face(helium::convex::Faces f);
  };

  /**
   * \brief specialized window for capturing postures, called from RobotModels
   */
  template<typename MA>
  class CaptureModelWindow :public ModelWindow<MA>{
    //RobotModels::SignallingConvexCollisionModel& m;
  public:
    CaptureModelWindow(RobotModels::SignallingConvexCollisionModel& c);
    /**
     * gets a capture in pointers
     * @param buffer the returned posture
     * @param d angle of joints to capture
     * @param w width
     * @param h height
     * @param color background color of the capture, default is white
     */
    void getPostureSnap(unsigned char* buffer,const double* d, int w, int h, double* color=NULL, helium::convex::Faces* view=NULL);
    /** for those with gyro, by default is ignoring gyro, replaced in guide2Related/modelWithFloor */
    virtual void getPostureSnap(unsigned char* buffer,const double* d, const double* g, int w, int h, double* color=NULL, helium::convex::Faces* view=NULL);
    void snap(unsigned char* buffer,int w, int h, double* color=NULL);    
  };


  void evaluateRobotBGColor(); ///< evaluate BG color to produce setBG signal or not for ROBOT model

  /**
   * interfacing RobotModels::CaptureModelWindow::getPostureSnap() 
   * @param buffer the returned posture
   * @param pose posture to capture
   * @param w width
   * @param h height
   * @param color background color of the capture, default is white
   */
  void getPostureSnap(RobotModels::PostureSnap& p, const helium::Posture& pos, int w, int h,double* color=NULL); ///< get the image of the robot model with the required size
  
  void getGdkPixmap(int w, int h);

  RobotModels(GuideState& gs); //,helium::ValueHub<bool> &rc);
private:
  GuideState &gs;

  //helium::TsSignal<const double *> signalSetRobotMovingBG;
  //helium::TsSignal<void> evalSetRobotMovingBG;

  helium::convex::ConvexModel modelDimensions; ///< to be used to create other ccms

  RobotModels::RobotSCCM robotModel; ///< model with rotate signal, setPosture  cfg::guide::Joint::displayedTarget
  RobotModels::FrameSCCM frameModel; ///< model with rotate signal, rotate with the cfg::guide::Joint::frameValue
  
  RobotModels::SignallingConvexCollisionModel timelineModel; ///< model with signal on rotate
  RobotModels::SignallingConvexCollisionModel captureModel; ///< model with signal on rotate

  RobotModels::SignallingConvexCollisionModel fixModel; ///< model to view the auto solution of shifting robot posture and colliding keyframe

public:
  RobotModels::SignallingConvexCollisionModel coldetModel; ///< model for coldet calculation

  MainCanvasArea mainCanvasArea; ///< for the small window on GUI canvas to show robot/frame posture
  ModelArea timelineArea; ///< for the ticks in GUI to show the posture at keyframes
  std::auto_ptr<ModelArea> gyroArea; ///< for gyroWindow showing robot orientation and posture
  
  ModelWindow<ModelArea> frameWindow; ///< separate Gtk::Window just to show frame posture
  ModelWindow<ModelArea> robotWindow;///< separate Gtk::Window just to show robot posture
  ModelWindow<ModelArea> fixWindow;///< separate Gtk::Window just to show fix posture

  void setCaptureWindowTransient(Gtk::Window &window);
private:
  CaptureModelWindow<ModelArea> captureWindow;///< separate Gtk::Window used to do posture image capturings

  //connections for deciding the color of robot color
  typedef helium::StaticConnReference<double,RobotModels,void,&RobotModels::evaluateRobotBGColor> CallEvaluateRobotBGColor;
  typedef helium::array::InternalAllocator<helium::ParamAllocator<CallEvaluateRobotBGColor,RobotModels*> > CallEvaluateRobotBGColorAlloc;     
  helium::Array<CallEvaluateRobotBGColor,
		helium::array::RESIZABLE,
		CallEvaluateRobotBGColorAlloc> callEvaluateRobotBGColor;///< connection to calculation of BG color
  //helium::StaticConnReference<void,RobotModels,void,&RobotModels::evaluateBGColor> callEvaluateBGColorDirect;  ///< signal to reevaluate BG color
};

void operator ++(RobotModels::ValueHubInt &a, int);

#include "robotModels.hpp"

#endif
