//readable version of posturePreview.h
#ifndef POSTURECAPTUREWINDOW
#define POSTURECAPTUREWINDOW

#include "readablePostureDrawer.h"

class PostureCaptureWindow: public Gtk::Window {
  helium::convex::ConvexCollisionModel ccm;
  helium::ReadablePostureDrawer rpd;
  GlArea glarea;
  Gtk::VBox& vbox;
  helium::Array<unsigned char> uc;
public:
  PostureCaptureWindow(int w, int h, const helium::convex::ConvexCollisionModel* srcCCM):
    ccm(*srcCCM,NULL),
    rpd(&ccm),
    vbox(*glarea.getWidget())
  {
    set_title("Capturing Postures");
    rpd.resize(w,h);
    glarea.onDraw.connect(rpd.drawConn);
    double bg[3];     bg[0]=0;    bg[1]=0;     bg[2]=0;
    rpd.setBG(bg);
    vbox.set_size_request(w,h);
    add(vbox);
    show_all_children();
  }
  void rotate(double* d){
    rpd.rotate(d);
  }
  const unsigned char* getPostureSnap() {
    //uc.resize(rpd.getWidth()*4*rpd.getHeight());
    //uc = rpd.getPostureSnap();
    //helium::zero(uc);
    //for(int i=0;i<uc.size();i+=4)
    //uc[i+3] = 255;
    //std::cout << "getposturesnap "<< uc[0] << std::endl;
    //return uc.data;
    std::cout << "postureCapture Window " << rpd.getWidth() << " " <<rpd.getHeight() << std::endl;
    return rpd.getPostureSnap();
  }
  const unsigned char* getPostureSnap(double* d,int n) {
    return rpd.getPostureSnap(d,n);
  }
  int getWidth() { return rpd.getWidth(); }
  int getHeight() { return rpd.getHeight(); }
  void setWidth(int pw) { rpd.setWidth(pw); }
  void setHeight(int ph) { rpd.setHeight(ph); }
  bool isAlpha() { return rpd.isAlpha(); }
};

#endif
