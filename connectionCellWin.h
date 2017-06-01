#ifndef HE_CONNECTIONCELL
#define HE_CONNECTIONCELL

#include <helium/system/cellClient.h>
#include <helium/system/remoteCellImporter.h>

namespace helium{
struct Connection{
  typedef enum {DISCONNECTED,CONNECTED,CONNECTING,DISCONNECTING,NUM} State; 
  
  
  friend std::ostream& operator<<(std::ostream& o,const Connection& c);
  friend std::istream& operator>>(std::istream& i,Connection& c);
  std::string host;
  int port;	
  //helium::Enum<ConnState> state;
  State state;
  
  Connection(const std::string& phost,int pport,State pstate):host(phost),port(pport),state(pstate){
  }
  
  
  Connection():port(-1),state(DISCONNECTED){	  
  }
  
  static Connection connect(const std::string& host,const int port){
    return Connection(host,port,CONNECTING);
  }
  
  static Connection disconnect(){
	return Connection("",-1,DISCONNECTING);
  }
  
  
};


  template<>  struct EnumStrings<Connection::State>{
    static const int NUM=4;
    static const char* strings[NUM];

  };



  inline std::ostream& operator<<(std::ostream& o,const Connection& c){
    return o<<c.host<<" "<<c.port<<" "<<helium::en::io<<c.state;
  }
  
  inline std::istream& operator>>(std::istream& i,Connection& c){
    return i>>c.host>>c.port>>helium::en::io>>c.state;
  }
  



  struct Suck{
    
    void f(){}
  };



 
  class ConnectionCell
//,public helium::ProxiableCell
{

 


  class AsyncClient:public Thread{
    CellClient& cellClient;
    
    Mutex oper;
    std::string host;
    int port;
    


  public:
    AsyncClient(CellClient& pcellClient):
      cellClient(pcellClient),
      oper("AsyncClient::oper"){
    }
    
    Signal<void> onError;

    void run(){
      MutexLock lock(oper);
      try{
	cellClient.connect(host,port);
      }catch(exc::NetException& ){
	onError();
	throw;
      }       
    }

        
    void connect(const std::string& phost,int pport){
      MutexLock lock(oper);
      host=phost;
      port=pport;
      std::cout<<"executing connect"<<std::endl;
      start();
    }

    void disconnect(){
      MutexLock lock(oper);
      std::cout<<"executing disconnect"<<std::endl;
      cellClient.disconnect();
    }

     CellClient& getSyncClient(){
      return cellClient;
    }

  };


  

  helium::Memory& mem; //for the importer
  IDPath path; 

  CellClient cellClient;
  AsyncClient client;
  Connection val;
  
  helium:: TsSignal<Cell::CellSignalData> onChange;
  helium::RemoteCellImporter* rci;

  std::string getName() const{
    return "connection cell";
  }

  void onConnected();
  void onDisconnected();
  void onConnectionError();

  


  InternalConnReference<void> connRef;
  //DELETE? fabio DEBUG WIN StaticConnReference<void,ConnectionCell,void,&ConnectionCell::onDisconnected> discRef;
  //DELETE? fabio DEBUG WIN StaticConnReference<void,ConnectionCell,void,&ConnectionCell::onConnectionError> errRef;

public:
  
  ConnectionCell(helium::Memory &pmem,const IDPath& path=IDPath());   
  ~ConnectionCell();
  void getData(Connection &c);
  void setData(const Connection &c);
  void setData(StdReply& r,const Connection &c){
    setData(c);
  }
  void connect(Cell::CellConnection& ref); //the cell event 
  void disconnect(Cell::CellConnection& ref);



};

}


 //fb win 20110630


  //fb win 20110630
  //typedef void (ConnectionCell::*CcFp)();
  //template<CcFp CALL>
  //class OnEvent:public helium::Callback<void>,public helium::ConnReference<void>{


  //fb win 20110630 
  //OnEvent<&ConnectionCell::onConnected> onConnect;
  //OnEvent<&ConnectionCell::onDisconnected> onDisconnect;

  /*
  template<void (ConnectionCell::*m)()>
  class ConnCall:public helium::ConnReference<void>,public helium::Callback<void>{
    ConnectionCell* cc;
    void operator()(){
      (cc->*m)();
    }
  public:
    ConnCall(ConnectionCell* pcc):helium::ConnReference<void>(this),cc(pcc){
    }
  };


  ConnCall<&ConnectionCell::onConnected> connRef;
  ConnCall<&ConnectionCell::onDisconnected> discRef;

  */

  //--20110704 window fix
#endif
