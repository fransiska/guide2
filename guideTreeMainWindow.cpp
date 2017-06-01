#include "connectionCell.h"
#include <helium/system/dynamicClient.h>

//#include <helium/gtk/gtkErrorHandler.h>
#include <helium/gtk/gtkExceptionHandling.h>
#include <helium/system/valueProxy.h>

#include <helium/robot/knownPosNormalizer.h>

#include <sstream>
using std::cout;
using std::endl;
using std::stringstream;

#include "guideDebugWindow.h"

using std::cout;
using std::endl;


int main(int argc, char ** argv) {
  if (argc<3){
    std::cerr<<"usage "<<argv[0]<<" ip port "<<std::endl;
    return -1;
  }
  if(!Glib::thread_supported()){
    Glib::thread_init();
  }
  gdk_threads_enter();
  Gtk::Main kit(argc, argv);
  helium::GtkExceptionHandling eh;
  helium::Thread::setGlobalExceptionHandler(&eh);
  helium::DynamicClient client;
  client.connect(argv[1],helium::convert<int>(argv[2]));
  GuideDebugWindow g(client.getMem(),&eh);  
  Gtk::Main::run(g); 
  gdk_threads_leave();
}
