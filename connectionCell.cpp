/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \author  Fabio Dalla Libera
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "connectionCell.h"
#include "guideState.h"

using namespace helium;

namespace helium{
std::ostream& operator<<(std::ostream& o,const helium::Connection& c){
  return o<<c.host<<" "<<c.port<<" "<<helium::en::eio<<c.state;
}
  std::istream& operator>>(std::istream& i,helium::Connection& c){
    return i>>c.host>>c.port>>helium::en::eio>>c.state;
  }
}

inline std::ostream& operator <<(std::ostream& o,const helium::Connection::State &state) {
  std::string s="";
  switch(state) {
  case helium::Connection::DISCONNECTED: s = "Disconnected"; break;
  case helium::Connection::CONNECTED: s = "Connected"; break;
  case helium::Connection::CONNECTING: s = "Connecting"; break;
  case helium::Connection::DISCONNECTING: s = "Disconnecting"; break;
  default : break;
  }
  return o << s;
}

Connection::Connection(const std::string& phost,int pport,State pstate):host(phost),port(pport),state(pstate){
  }

Connection::Connection():port(-1),state(DISCONNECTED){	  
}


ConnectionCell::AsyncClient::AsyncClient(helium::CellClient& pcellClient):
      cellClient(pcellClient),
      oper("AsyncClient::oper"){
    }

void ConnectionCell::AsyncClient::run(){
  helium::MutexLock lock(oper);
  try{
    cellClient.connect(host,port);
  }catch(helium::exc::NetException& ){
    onError();
    throw;
  }       
}

void ConnectionCell::AsyncClient::connect(const std::string& phost,int pport){
    helium::MutexLock lock(oper);
      host=phost;
      port=pport;
      start();
    }


void ConnectionCell::AsyncClient::disconnect(){
  helium::MutexLock lock(oper);
  cellClient.disconnect();
}

helium::CellClient&  ConnectionCell::AsyncClient::getSyncClient(){
  return cellClient;
}



  void ConnectionCell::onConnectionError(){
    val.state=Connection::DISCONNECTED;
    onChange(val);
  }




void ConnectionCell::onConnected(){
  val.state=Connection::CONNECTED;
  onChange(val);
  try{
    MutexLock lock(rciMutex);
    rci=new helium::RemoteCellImporter(mem,client.getSyncClient(),path);    
    
    //rci->connectProxy(&gs.joints.spoolctrl);
    //rci->connectProxy(&gs.sensors.spoolctrl);

    //gs.joints.startMassiveOperation();
    //gs.sensors.startMassiveOperation();
    
    rci->connectProxies(); //the others except the spoolctrl

    //gs.sensors.endMassiveOperation();
    //gs.joints.endMassiveOperation();


    std::vector<int> disc;
    for (size_t j=0;j<gs.joints.size();++j){
      if (gs.joints[j].jointState.isConnected()){
	  gs.joints[j].jointState.retrieveAndSignal();
      }else{
	disc.push_back(j);
      }
    }
    if (disc.size()){
      throw exc::InvalidOperation(stringBegin()<<"Remote end does not have motors "<<disc<<stringEnd());
      
    }
  }catch(exc::Exception& e){
    //std::cout<<"memory is"<<mem<<std::endl;
    Thread::getGlobalExceptionHandler()->handle(e);
  }
  //std::cout<<mem<<std::endl;

}

void ConnectionCell::onDisconnected(){
  val.state=Connection::DISCONNECTED;
  rciMutex.lock();
  delete rci;
  rci=NULL;
  rciMutex.unlock();
  onChange(val);
  //std::cout<<"onDisConnectedSignal"<<std::endl;
}



  ConnectionCell::ConnectionCell(helium::Memory &pmem,GuideState& pgs,const IDPath& ppath):
    mem(pmem),path(ppath),
    client(cellClient),
    rciMutex("rciMutex"),
    rci(NULL),
    connRef(this,FuncPlaceHolder<ConnectionCell,void,&ConnectionCell::onConnected>()),
    discRef(this,FuncPlaceHolder<ConnectionCell,void,&ConnectionCell::onDisconnected>()),
    errRef(this,FuncPlaceHolder<ConnectionCell,void,&ConnectionCell::onConnectionError>()),
    gs(pgs)

    //,connRef(this),discRef(this),errRef(this)



  {
    client.getSyncClient().onConnect.connect(connRef);
    client.getSyncClient().onDisconnect.connect(discRef);
    client.onError.connect(errRef);

}


ConnectionCell::~ConnectionCell(){
  client.getSyncClient().onConnect.disconnect(connRef);
  client.getSyncClient().onDisconnect.disconnect(discRef);
  if (rci!=NULL){
    delete rci;
  }
}

void ConnectionCell::getData(Connection &c){
  c=val;
}


void ConnectionCell::setData(const Connection &c){   
    switch (c.state){
    case Connection::CONNECTING:
      if (val.state==Connection::CONNECTED){
	client.disconnect();
      }
      val=c;
      client.connect(c.host,c.port);
      break;
    case Connection::DISCONNECTING:
      client.disconnect();
      break;
    default:
      throw helium::exc::InvalidOperation("Setting connection to state "+helium::toString(c.state));
    }
  }//set 


void ConnectionCell::connect(CellConnection& ref){
    onChange.connect(ref);
  }


void ConnectionCell::disconnect(CellConnection& ref){
    onChange.disconnect(ref);
  }


namespace helium{
  const char* EnumStrings<Connection::State>::strings[]={"disconnected","connected","connecting","disconnecting"};
}
