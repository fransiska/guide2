/*!
 * \brief   guide
 * \author  Fabio Dalla Libera
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "yarpInterface.h"
#include <helium/util/makeReturnValue.h>
using namespace helium;

ValueHub<double>* getDisplayedPos(GuideState& gs,int i){
  return &gs.joints[i].displayedTarget;
}


std::vector<int> getAxesNum(YarpMotor& ym){
  std::vector<int> s;
  ym.getAxesNum(s);
  return s;
}

YarpInterface::YarpComponents::YarpComponents(helium::YarpClient& yc,const helium::YarpPaths& yp,GuideState& pgs):
  gs(pgs),
  callRotate(&ym,gs.joints.size()),
  callAssureDisplayedPos(gs,gs.joints.size()),
  ym(yc,yp),  
  enc(yc,yp,makeReturnValue(&YarpMotor::getAxesNum,ym)),
  play(ym,gs),
  hwState(gs)
    
{
  for (size_t j=0;j<gs.joints.size();++j){
    gs.joints[j].desiredTarget.connect(callRotate[j]);
    enc.connect(j,callAssureDisplayedPos[j]);
  }
  hwState.globalUpdate(WORKINGHWSTATE);
}



YarpInterface::YarpComponents::~YarpComponents(){
  for (size_t j=0;j<gs.joints.size();++j){
    gs.joints[j].desiredTarget.disconnect(callRotate[j]);
    enc.disconnect(callAssureDisplayedPos[j]);
  }
  hwState.globalUpdate(UNKNOWNHWSTATE);
}

YarpInterface::YarpInterface(const std::string& s,GuideState& pgs):
  yp(s),  
  comp(NULL),
  gs(pgs),
  conn(*this)
{  

}


void YarpInterface::clearComp(){
  if (comp){
    delete comp;
    comp=NULL;
  }
}

YarpInterface::~YarpInterface(){
  clearComp();
}


void YarpInterface::ConnectionRequests::notify(const helium::Connection& c){
  val=c;
  sig(c);
}


std::string YarpInterface::ConnectionRequests::getName() const{
  return "Yarp connection request";
}

void YarpInterface::connect(const helium::ConnInfo& c){
  try{
    //conn.notify(Connection(c.host,c.port,Connection::CONNECTING));
    yc.connect(c);
    clearComp();
    comp=new YarpComponents(yc,yp,gs); 
    conn.notify(Connection(c.host,c.port,Connection::CONNECTED));
  }catch(exc::Exception& e){
    conn.notify(Connection("",-1,Connection::DISCONNECTED));
    throw;
  }

   
}

void YarpInterface::disconnect(){
  clearComp();
  conn.notify(Connection("",-1,Connection::DISCONNECTING));
  yc.disconnect();
  conn.notify(Connection("",-1,Connection::DISCONNECTED));
}




YarpInterface::ConnectionRequests::ConnectionRequests(YarpInterface& piface):
  iface(piface){
  
}



void YarpInterface::ConnectionRequests::getData(helium::Connection &c){
  c=val;
}



void YarpInterface::ConnectionRequests::setData(const Connection &c){   
  switch (c.state){
  case Connection::CONNECTING:
    if (val.state==Connection::CONNECTED){
      iface.disconnect();
    }
    val=c;
    iface.connect(ConnInfo(c.host,c.port));
    break;
  case Connection::DISCONNECTING:
    iface.disconnect();
    break;
  default:
    throw helium::exc::InvalidOperation("Setting connection to state "+helium::toString(c.state));
  }

}//setData 


void YarpInterface::ConnectionRequests::connect(helium::Cell::CellConnection& ref){
  sig.connect(ref);
}


void YarpInterface::ConnectionRequests::disconnect(helium::Cell::CellConnection& ref){
  sig.disconnect(ref);
}


YarpInterface::HwStateUpdater::HwStateUpdater(GuideState& pgs):
  gs(pgs){
  for (size_t j=0;j<gs.joints.size();++j){
    gs.joints[j].jointState.setRemote(this);
  }
  //globalUpdate(WORKINGHWSTATE);  
}


YarpInterface::HwStateUpdater::~HwStateUpdater(){
  globalUpdate(UNKNOWNHWSTATE);  
  for (size_t j=0;j<gs.joints.size();++j){
    gs.joints[j].jointState.unsetRemote();
  }
}

void YarpInterface::HwStateUpdater::globalUpdate(helium::HwState state){
  for (size_t j=0;j<gs.joints.size();++j){
    gs.joints[j].jointState.notify(state);
  }
}


std::string YarpInterface::HwStateUpdater::getName() const{
  return "hwStateUpdater";
}

void YarpInterface::HwStateUpdater::connect(helium::Cell::CellConnection& c){
  HwState h=WORKINGHWSTATE;
  (*c.cb)(&h);
}


void YarpInterface::HwStateUpdater::disconnect(helium::Cell::CellConnection& c){
  //fake connection updates immediately, no need to actually connect and disconnect  
}

void YarpInterface::HwStateUpdater:: getData(helium::HwState& h){
  h=WORKINGHWSTATE;
}


YarpInterface::FramePlayCell::FramePlayCell(helium::YarpMotor& ym,GuideState& pgs):
  helium::ConnReference<const helium::Motion&>(this),
  helium::ConnReference<std::pair<helium::framePlay::PlayState,Time> >(this),
  gs(pgs),
  ymp(ym,&(makeArrayReturnValue<std::vector<double> >(ym.getAxesNum(),M_PI))[0]),
  cache(helium::framePlay::STOPPED,0)
{
  gs.motionInfo.framePlayState.setRemote(this);
  gs.motionInfo.target.connect(*this);
}


void YarpInterface::FramePlayCell::operator()(std::pair<helium::framePlay::PlayState,helium::Time> u){
  cache=u;
  sig(u);
}


YarpInterface::FramePlayCell::~FramePlayCell(){
  gs.motionInfo.target.disconnect(*this);
  gs.motionInfo.framePlayState.unsetRemote();
}

void YarpInterface::FramePlayCell::operator()(const helium::Motion& m){
  ymp.play(m,gs.motionInfo.mp);
}


std::string YarpInterface::FramePlayCell::getName() const{
  return "framePlayCell";
}

void YarpInterface::FramePlayCell::getData(std::pair<helium::framePlay::PlayState,helium::Time> & c){
  c=cache;
}


void YarpInterface::FramePlayCell::setData(const helium::framePlay::RequestStop& c){
  ymp.stop();
}

void YarpInterface::FramePlayCell::connect(helium::Cell::CellConnection& ref){
  sig.connect(ref);
}

void YarpInterface::FramePlayCell::disconnect(helium::Cell::CellConnection& ref){
  sig.disconnect(ref);
}


void  DefDescr<YarpInterface >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
  members.push_back(scalarMapping(obj.conn,helium::IDPath("connection",".")));
}



