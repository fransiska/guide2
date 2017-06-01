#ifndef POSTUREPREVIEW
#define POSTUREPREVIEW

#include <app/guide2/gtkglareaBox.h>
#include <app/guide2/postureDrawer.h>

class PosturePreview{
  helium::convex::ConvexCollisionModel frameCCM;
  PostureDrawer framePD;
  GlArea robotView;
  Gtk::VBox& imageBox;

public:
  PosturePreview(int w, int h, const helium::convex::ConvexCollisionModel* srcCCM):
    frameCCM(*srcCCM,NULL),
    framePD(&frameCCM),
    imageBox(*robotView.getWidget())
  {
    robotView.onDraw.connect(framePD.drawConn);
    imageBox.set_size_request(w,h);
    double bg[3];     bg[0]=0;    bg[1]=0;     bg[2]=0;
    framePD.setBG(bg);
  }
  
  Gtk::VBox& getWidget(){
    return imageBox;
  }

  void rotate(double* d){
    framePD.rotate(d);
  }
};

#endif
