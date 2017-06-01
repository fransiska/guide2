/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \author  Fabio Dalla Libera
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef CONNECTIONCELL
#define CONNECTIONCELL

#include <helium/system/cellClient.h>
#include <helium/system/remoteCellImporter.h>
#include <helium/system/cellSignal.h>
#include <helium/signal/staticConnReference.h>

class GuideState;
namespace helium{
struct Connection{
  typedef enum {DISCONNECTED,CONNECTED,CONNECTING,DISCONNECTING} State; 
  
  
  friend std::ostream& operator<<(std::ostream& o,const Connection& c);
  friend std::istream& operator>>(std::istream& i,Connection& c);
  std::string host;
  int port;	
  //helium::Enum<ConnState> state;
  State state;
  
  Connection(const std::string& phost,int pport,State pstate);
  Connection();


  static Connection connect(const std::string& host,const int port){
    return Connection(host,port,CONNECTING);
  }
  
  static Connection disconnect(){
    return Connection("",-1,DISCONNECTING);
  }
  
  
};

  inline std::ostream& operator <<(std::ostream& o,const helium::Connection::State &state);


}
namespace helium{
  template<>  struct EnumStrings<Connection::State>{
    static const int NUM=4;
    static const char* strings[NUM];

  };

   std::ostream& operator<<(std::ostream& o,const helium::Connection& c);  
  std::istream& operator>>(std::istream& i,helium::Connection& c);

  template<>
  class DefaultAccessOptions<Connection>{
  public:
    static const int value= ORDERRECURSIVEPROCESSVALUE;
  };

}



  




  class ConnectionCell:public helium::TypedCell<helium::Connection>{

    //connection runs as a new thread
  class AsyncClient:public helium::Thread{
    helium::CellClient& cellClient;    
    helium::Mutex oper;
    std::string host;
    int port;   
  public:
    AsyncClient(helium::CellClient& pcellClient);    
    helium::Signal<void> onError;
    void run();        
    void connect(const std::string& phost,int pport); 
    void disconnect();    
    helium::CellClient& getSyncClient();
  };




  helium::Memory& mem; //for the importer
  helium::IDPath path; 

  helium::CellClient cellClient;
  AsyncClient client;
  helium::Connection val;
  
  helium::CellSignal<helium::Connection,true> onChange;

  helium::Mutex rciMutex;
  helium::RemoteCellImporter* rci;

  std::string getName() const{
    return "connection cell";
  }


  void onConnected();
  void onDisconnected();
  void onConnectionError();

  helium::InternalConnReference<void> connRef;
  helium::InternalConnReference<void> discRef;
  helium::InternalConnReference<void> errRef;
  
  GuideState& gs;

public:
  
  ConnectionCell(helium::Memory &pmem,GuideState& gs,const helium::IDPath& path=helium::IDPath());   
  ~ConnectionCell();
  void getData(helium::Connection &c);
  void setData(const helium::Connection &c);
  void setData(helium::StdReply& r,const helium::Connection &c){
    setData(c);
  }
  void connect(helium::Cell::CellConnection& ref); //the cell event 
  void disconnect(helium::Cell::CellConnection& ref);



};




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
