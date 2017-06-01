#include "gyroConnectionLine.h"
#include "gyroCalib.h"
#include <iostream>

GyroConnectionLine::GyroConnectionLine(GyroCalib* pgyroCalib, const std::string& host) :
  g(pgyroCalib)
{
  g->registerConnectionLine(this);
  ipe.set_text(host);
  ipcb.signal_clicked().connect(sigc::mem_fun(*this,&GyroConnectionLine::requestConnection) );  
  pack_start(ipcb,Gtk::PACK_SHRINK);
  pack_start(ipe,Gtk::PACK_SHRINK);
  stateDispatcher.connect( sigc::mem_fun(*this , &GyroConnectionLine::setStateCB ));   
}

void GyroConnectionLine::setStateCB() {
  changedExternally=true;
  ipcb.set_active(state);
  changedExternally=false;
}

void GyroConnectionLine::setState(bool pstate){
  state=pstate;
  stateDispatcher();
}

void GyroConnectionLine::requestConnection(){
  if (changedExternally) return;
  if (ipcb.get_active()){    
    g->connect(ipe.get_text().c_str(),5001);
  }else{
    g->disconnect();
  }
}
