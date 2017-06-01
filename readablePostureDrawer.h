#ifndef HE_READABLEPOSTUREDRAWER
#define HE_READABLEPOSTUREDRAWER

#include "gtkglareaBox.h"
//#include "postureDrawer.h"
//#include <helium/graphics/autoresizingBuffer.h>
//#include <helium/graphics/glUtil.h>
namespace helium{

  class ReadablePostureDrawer:public PostureDrawer{
    AutoresizingBuffer buff;
    int w; int h;
    helium::Array<unsigned char> buf;
    bool alpha;
  public:
    ReadablePostureDrawer(helium::convex::ConvexCollisionModel *pccm);
    const unsigned char* getPostureSnap();
    const unsigned char* getPostureSnap(double* d,int n);

    bool isAlpha() { return alpha; }

    int getWidth() { return w; }//buff.getWidth(); }
    int getHeight() { return h; }//buff.getHeight(); }
    void setWidth(int pw) { 
      resize(pw,h);
    }
    void setHeight(int ph) { 
      resize(w,ph);
    }
    void resize(int pw, int ph) {
      int bit = alpha?4:3;
      if((pw*bit)%8 != 0)
	pw -= (pw*bit)%8;
      std::cout << pw << std::endl;
      drawConvex.resize(pw,ph);
      w = pw;
      h = ph;
      //drawingCanvas.resize(pw,ph);
    }

  };//ReadablePostureDrawer

}//helium

#endif
