#ifndef POSTUREDRAWER
#define POSTUREDRAWER

#include <heliumBased/convex/drawConvex.h>
#include <heliumBased/convex/convexModel.h>
#include <helium/callback/callback.h>
#include <helium/graphics/pixelReader.h>

class ReadablePostureDrawer;
class PostureDrawer:public helium::Callback<void>,public helium::Callback<const std::pair<int,int>&>{  
  friend class ReadablePostureDrawer;

 protected:
  helium::convex::DrawConvex drawConvex;

  helium::convex::ConvexCollisionModel *ccm;
  virtual void readPosture() {}

 public:
  helium::ConnReference<void> drawConn; //connection token 
  helium::ConnReference<const std::pair<int,int>& > onResizeConn; //connection token

  PostureDrawer(helium::convex::ConvexCollisionModel *pccm):
    drawConvex(pccm),ccm(pccm),
    drawConn(this),onResizeConn(this)
  { 
    drawConvex.setZoom(0.6);
  }

  helium::convex::ConvexCollisionModel* getCCM() {
    return ccm;
  }
  
  void setBG(double* color){
    drawConvex.setBG(color);
  }
  void draw(){
    drawConvex.draw();
  }

  void operator()(){
    drawConvex.draw(); 
  }

  void operator()(const std::pair<int,int>& size){
    drawConvex.resize(size.first,size.second);
  }

  helium::Callback<void>* clone() const{
    return NULL;
  }
  
  void rotate(double* r){
    ccm->rotate(r);
  }

  bool onKeyPress(char c, int x, int y) {
    if(drawConvex.onKeyPushed(c,x,y) == helium::PrimitiveKeyboardInteraction::MANAGED) {
      draw();
      return true;
    }
    return false;
  }
};

#endif
