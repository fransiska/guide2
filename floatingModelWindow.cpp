#include "floatingModelWindow.h"

void FloatingModelWindow::updateModel(){
  for(size_t i=0;i<config.joints.size();i++) {
    d[i] = config.joints[i].getValue(cfg::guide::CursorType::FRAME);
  }    
  poseDrawer.rotate(d.data);
  if(guideModelWindow.is_visible()){
    guideModelWindow.queue_draw();
  }
  modelBox.show();
    modelBox.queue_draw();
}


bool FloatingModelWindow::onKeyPressModel(GdkEventKey* event){
  char c = *event->string;
  if (poseDrawer.onKeyPress(c,0,0)){
    updateModel();
  }
  return false;
}
void FloatingModelWindow::setSizeModelBox(int w, int h) {
  modelBox.set_size_request(w,h);
}

void FloatingModelWindow::resize(double scale){
  modelBox.set_size_request(config.settings.sizes.model.w*scale-config.settings.sizes.stroke*2,config.settings.sizes.model.h*scale-config.settings.sizes.stroke*2);
}
