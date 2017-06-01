/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


////////////////////////////// ModelWindow

template<typename MA>
RobotModels::ModelWindow<MA>::ModelWindow(RobotModels::SignallingConvexCollisionModel& c):
  w(200),h(150),
  frame_gl("",Gtk::ALIGN_CENTER,Gtk::ALIGN_CENTER,1.0,true),
  modelArea(c)
{
  modelArea.setSize(w,h);
  frame_gl.add(modelArea.getBox());
  add(frame_gl);
  show_all_children();
  signal_key_press_event().connect(sigc::mem_fun(&modelArea,&RobotModels::ModelArea::onGdkKeyPress));
}

template<typename MA>
void RobotModels::ModelWindow<MA>::setBG(const double* b) {
  modelArea.setBG(b);
}

template<typename MA>
void RobotModels::ModelWindow<MA>::setViewMatrix(const double* m) {
  modelArea.setViewMatrix(m);
}

template<typename MA>
double* RobotModels::ModelWindow<MA>::getViewMatrix() {
  return modelArea.getViewMatrix();
}

template<typename MA>
void RobotModels::ModelWindow<MA>::face(helium::convex::Faces f) {
  modelArea.face(f);
}

////////////////////////////// CaptureModelWindow
template<typename MA>
RobotModels::CaptureModelWindow<MA>::CaptureModelWindow(RobotModels::SignallingConvexCollisionModel& c):
  RobotModels::ModelWindow<MA>(c)
{
  Gtk::Window::resize(800,600);
  Gtk::Window::set_resizable(false);
  Gtk::Window::set_title("Capturing Postures");
  Gtk::Window::set_keep_above(true);
  Gtk::Window::set_decorated(false);
  Gtk::Window::set_skip_taskbar_hint();
  Gtk::Window::set_position(Gtk::WIN_POS_CENTER_ALWAYS);
  Gtk::Window::show_all_children();
}

template<typename MA>
void RobotModels::CaptureModelWindow<MA>::getPostureSnap(unsigned char* buffer,const double* d, const double* g, int w, int h, double* color, helium::convex::Faces* view) {    
  std::cout << "robortmodels cmw getsnap with gyro" << std::endl;
  getPostureSnap(buffer,d,w,h,color,view);
}

template<typename MA>
void RobotModels::CaptureModelWindow<MA>::getPostureSnap(unsigned char* buffer,const double* d, int w, int h, double* color, helium::convex::Faces* view) {    
  this->modelArea.setSize(w,h);
  this->modelArea.setPosture(d);  
  if(view)
    this->modelArea.face(*view);
  snap(buffer,w,h,color);
}

template<typename MA>
void RobotModels::CaptureModelWindow<MA>::snap(unsigned char* buffer,int w, int h, double* color) {    
  Gtk::Window::show();
  if(color)
    this->modelArea.setBG(color);
  else {
    double white[3] = {1,1,1};
    this->modelArea.setBG(white);
  }
  this->modelArea.getBox().queue_draw(); ///draw the eventbox
  Gdk::Window::process_all_updates();
  helium::getPixels(buffer,w,h); ///get last drawn gl pixels
  helium::flipUD(buffer,w,h);    ///flip to correct orientation
  Gtk::Window::hide(); ///hide window

}
