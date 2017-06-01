#ifndef FLOATINGMODELWINDOW
#define FLOATINGMODELWINDOW

#include "guideConfig.h"
#include "guideModelWindow.h"
#include "postureDrawer.h"
#include "gtkglareaBox.h"

class FloatingModelWindow{
  //GuideData &gd;
  cfg::guide::Config&  config;
  helium::Array<double> d;//temp storage
  helium::convex::ConvexCollisionModel& colConvexModel;
  PostureDrawer poseDrawer;
  GuideModelWindow guideModelWindow;
  GlArea robotView;
  Gtk::VBox& modelBox;
  
public:
 
  Gtk::VBox& getWidget(){
    return modelBox;
  }

  Gtk::Window& getModelWindow(){
    return guideModelWindow;
  }

  PostureDrawer& getPosDrawer(){
    return poseDrawer;
  }

  
  void updateModel();
  
  bool onKeyPressModel(GdkEventKey* event);
  void resize(double scale);
  void setSizeModelBox(int w, int h);

  typedef helium::StaticConnReference<double,FloatingModelWindow,void,&FloatingModelWindow::updateModel> CallUpdateModel;
  typedef helium::array::InternalAllocator<helium::ParamAllocator<CallUpdateModel,FloatingModelWindow*> > CallUpdateModelAlloc;
  helium::Array<CallUpdateModel,
		helium::array::RESIZABLE,
		CallUpdateModelAlloc> callUpdateModels;


  FloatingModelWindow(cfg::guide::Config& pconfig, helium::convex::ConvexCollisionModel& pcolConvexModel):
    config(pconfig),
    d(config.joints.size()),
    colConvexModel(pcolConvexModel),
    poseDrawer(&colConvexModel),//pass
    guideModelWindow(poseDrawer), //pass
    modelBox(*robotView.getWidget()),
    callUpdateModels(this,config.joints.size())
  {    
    robotView.onDraw.connect(poseDrawer.drawConn);
    robotView.onResize.connect(poseDrawer.onResizeConn);
    modelBox.set_size_request(config.settings.sizes.model.w-config.settings.sizes.stroke*2,config.settings.sizes.model.h-config.settings.sizes.stroke*2);
  }

};

class FloatingNewModelWindow{
  helium::convex::ConvexCollisionModel colConvexModel;
  FloatingModelWindow floatingModelWindow;
public:
  FloatingNewModelWindow(cfg::guide::Config& pconfig, helium::convex::ConvexCollisionModel& pcolConvexModel):
    colConvexModel(pcolConvexModel,NULL),
    floatingModelWindow(pconfig,colConvexModel) {
  }
  FloatingModelWindow* getFloatingModelWindow() {
    return &floatingModelWindow;
  }
};

#endif
