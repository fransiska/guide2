#include "connectionCellWin.h"

namespace helium{


  void ConnectionCell::onConnectionError(){
    val.state=Connection::DISCONNECTED;
    onChange(&val);
    std::cout<<"onErrorSignal"<<std::endl;
  }



  void ConnectionCell::onConnected(){
  val.state=Connection::CONNECTED;
  std::cout<<"************************RCI**********"<<std::endl;
  rci=new helium::RemoteCellImporter(mem,client.getSyncClient(),path);    
  std::cout<<"========================CONNECTED========================="<<std::endl;
  //std::cout<<mem<<std::endl;
  onChange(&val);
  std::cout<<"onConnectedSignalWin"<<std::endl;
  }

void ConnectionCell::onDisconnected(){
  val.state=Connection::DISCONNECTED;
  delete rci;
  rci=NULL;
  onChange(&val);
  std::cout<<"onDisConnectedSignal"<<std::endl;
}



  ConnectionCell::ConnectionCell(helium::Memory &pmem,const IDPath& ppath):
    mem(pmem),path(ppath),
    client(cellClient),

    rci(NULL),

    connRef(this,FuncPlaceHolder<ConnectionCell,void,&ConnectionCell::onConnected>())

    /*discRef(this),errRef(this)*/

  {

    /*
    client.getSyncClient().onConnect.connect(connRef);
    client.getSyncClient().onDisconnect.connect(discRef);
    client.onError.connect(errRef);
    */
}


ConnectionCell::~ConnectionCell(){
  /*
  client.getSyncClient().onConnect.disconnect(connRef);
  client.getSyncClient().onDisconnect.disconnect(discRef);
  */
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


  void ConnectionCell::connect(Cell::CellConnection& ref){
    onChange.connect(ref);
  }


  void ConnectionCell::disconnect(Cell::CellConnection& ref){
    onChange.disconnect(ref);
  }



  const char* EnumStrings<Connection::State>::strings[]={"disconnected","connected","connecting","disconnecting"};



}//helium
