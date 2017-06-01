/*!
 * \brief   guide
 * \author  Fabio Dalla Libera
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef HE_YARPINTERFACE_
#define HE_YARPINTERFACE_


#include <helium/yarp/yarpMotor.h>
#include <helium/yarp/yarpEncoder.h>
//#include <helium/robot/hwStateMonitor.h>
#include <helium/yarp/yarpMotionPlayer.h>
#include <helium/util/copyAsNew.h>
#include <helium/gtk/gtkSignal.h>


#include "connectionCell.h"
#include "guideState.h"


helium::ValueHub<double>* getDisplayedPos(GuideState& gs,int j);

class YarpInterface{

  class HwStateUpdater:public helium::TypedCell<helium::HwStateSingleSignature >{
    GuideState& gs;
    std::string getName() const;
    void connect(helium::Cell::CellConnection&);
    void disconnect(helium::Cell::CellConnection&);
    void getData(helium::HwState&);
  public:
    void globalUpdate(helium::HwState);    
    HwStateUpdater(GuideState&);
    ~HwStateUpdater();
  };


  class FramePlayCell:
    public helium::TypedCell<helium::framePlay::FramePlayControlSignature>,
    public helium::Callback<const helium::Motion&>,
    public helium::ConnReference<const helium::Motion&>,
    public helium::Callback<std::pair<helium::framePlay::PlayState,helium::Time> >,
    public helium::ConnReference<std::pair<helium::framePlay::PlayState,helium::Time> >
  {
    GuideState& gs;
    helium::YarpMotionPlayer ymp;
    std::pair<helium::framePlay::PlayState,helium::Time> cache;

    typedef std::pair<helium::framePlay::PlayState,helium::Time> PlayUpdate;
    helium::CellSignal<std::pair<helium::framePlay::PlayState,helium::Time>,true> sig;

    std::string getName() const;    
    void getData(std::pair<helium::framePlay::PlayState,helium::Time> & c);
    void setData(const helium::framePlay::RequestStop& c);

    void connect(helium::Cell::CellConnection& ref); 
    void disconnect(helium::Cell::CellConnection& ref);

    void operator()(const helium::Motion&);
    void operator()(std::pair<helium::framePlay::PlayState,helium::Time>);
  public:    
    FramePlayCell(helium::YarpMotor& ym,GuideState& gs);
    ~FramePlayCell();
  };


  
  helium::YarpPaths yp;
  helium::YarpClient yc;

  class YarpComponents{

    GuideState& gs;

      typedef helium::ParamFirstStaticConnReference<double,helium::YarpMotor,int,&helium::YarpMotor::rotate> CallRotate;


  helium::Array<CallRotate,helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::ParamIdxAllocator<CallRotate,helium::YarpMotor*> >
		> callRotate;
  
  

  typedef helium::StaticConnReference<double,
				      helium::ValueHub<double>,
				      const double&,
				      &helium::ValueHub<double>::assure
				      > CallAssure;  

  typedef   helium::ParametricFuncAllocator<CallAssure,GuideState&,helium::ValueHub<double>*,getDisplayedPos>  DisplayedPosAllocator;

  helium::Array<CallAssure,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator< DisplayedPosAllocator>
		> callAssureDisplayedPos;

    typedef helium::YarpEncoder<helium::CopyAsNew<helium::GtkSignal<double,double,true,true> > > Encoder;

    

   		  


  public:
    helium::YarpMotor ym;
    Encoder enc;
    FramePlayCell play;
    HwStateUpdater hwState;
    
    
    YarpComponents(helium::YarpClient& yc,const helium::YarpPaths& yp,GuideState& pgs);
    ~YarpComponents();
  };


  YarpComponents* comp;//initialized only under connection, automatically retrieves the number of axes
  void clearComp();


  GuideState& gs;

  class ConnectionRequests: public helium::TypedCell<helium::Connection>{
    YarpInterface& iface;
    helium::Connection val;
    helium::CellSignal<helium::Connection,true> sig;
  public:

    ConnectionRequests(YarpInterface&);    
    std::string getName() const;
    void getData(helium::Connection &c);
    void setData(const helium::Connection &c);
    void setData(helium::StdReply& r,const helium::Connection &c){
      setData(c);
    }
    void connect(helium::Cell::CellConnection& ref); //the cell event 
    void disconnect(helium::Cell::CellConnection& ref);
    void notify(const helium::Connection& c);
  };

  ConnectionRequests conn;
  friend class  helium::DefDescr<YarpInterface >;




  
public:

  YarpInterface(const std::string&,GuideState& gs);
  ~YarpInterface();
  void connect(const helium::ConnInfo&);
  void disconnect();

  
 
};

namespace helium {
  template<>
  class DefDescr<YarpInterface >:public Description<YarpInterface>  {
  public:
    DefDescr(YarpInterface& t):Description<YarpInterface>(t){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);    
  };

}

#endif
