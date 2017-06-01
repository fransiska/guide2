#include "readablePostureDrawer.h"


helium::ReadablePostureDrawer::ReadablePostureDrawer(helium::convex::ConvexCollisionModel *pccm):
  PostureDrawer(pccm),buff(drawConvex),alpha(false){
}

const  unsigned char* helium::ReadablePostureDrawer::getPostureSnap() {      
  draw();
  std::cout << "getposture snap "<< drawConvex.getWidth() << " " << drawConvex.getHeight() << std::endl;
  std::cout <<"buff "<< buff.getWidth() << " " << buff.getHeight()<< std::endl;
  getPixels(buff,drawConvex.getWidth(),drawConvex.getHeight());
  flipUD(buff,drawConvex.getWidth(),drawConvex.getHeight());
  return buff;
}

const unsigned char* helium::ReadablePostureDrawer::getPostureSnap(double* d,int n) {  
  double x,y,z;
  helium::Array<double> curPos(n);
  curPos = ccm->getAngles();
  drawConvex.getKeyViewMatrix(x,y,z);
  drawConvex.setDefViewMatrix();
  rotate(d);
  draw();
  getPixels(buff,drawConvex.getWidth(),drawConvex.getHeight());
  flipUD(buff,drawConvex.getWidth(),drawConvex.getHeight());
  drawConvex.setKeyViewMatrix(x,y,z);
  rotate(curPos.data);
  draw();
  return buff;
}
