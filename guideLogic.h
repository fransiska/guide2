/*!
 * \brief   guide
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDELOGIC
#define GUIDELOGIC



#include <helium/system/cellDescription.h>
#include "guideState.h"
//#include <helium/robot/knownPosNormalizer.h>

//added to extract joint pair to jointlogic
class GuideLogic;

typedef helium::triple<cfg::guide::Joint*,helium::JointPosConverter*,GuideLogic*>  GuideJointParam;

inline GuideJointParam extractJoint(helium::triple<cfg::guide::Joint*, helium::JointPosConverter*,GuideLogic*> p,int j){
  return GuideJointParam(p.first+j,p.second,p.third);
}

class GuideLogic{
  GuideState& gs;
  
public:
  class JointLogic{
    
    GuideLogic& owner;
    typedef helium::AbsMotorSign::Pos RawPos;
    typedef helium::motor::Interface::Position NormPos;
    
    enum DataSrc{POTENTIOSRC,DESIREDTARGETSRC};
    

    struct CalibCallback{   
      class OnMin:public helium::Callback<RawPos>,public helium::ConnReference<RawPos>{
	cfg::guide::Calib& owner;
	helium::JointPosConverter& pc;
      public:
	void operator()(RawPos newmin){
	  owner.relLim.minv=pc.getRelPos(newmin,owner.absLim.zero);
	}
	OnMin(cfg::guide::Calib& powner,helium::JointPosConverter& ppc):
	  helium::ConnReference<RawPos>(this),
	  owner(powner),pc(ppc){}
      } onMin;
      
      class OnMax:public helium::Callback<RawPos>,public helium::ConnReference<RawPos>{
	cfg::guide::Calib& owner;
	helium::JointPosConverter& pc;
      public:
	void operator()(RawPos newmax){
	  owner.relLim.maxv=pc.getRelPos(newmax,owner.absLim.zero);
	}
	OnMax( cfg::guide::Calib& powner,helium::JointPosConverter& ppc):
	  helium::ConnReference<RawPos>(this),
	  owner(powner),pc(ppc){}
      } onMax;
      
      
      class OnZero:public helium::Callback<RawPos>,public helium::ConnReference<RawPos>{
	cfg::guide::Calib& owner;
	helium::JointPosConverter& pc;
      public:
	void operator()(RawPos newzero){
	  owner.relLim.minv=pc.getRelPos(owner.absLim.minv,newzero);
	  owner.relLim.maxv=pc.getRelPos(owner.absLim.maxv,newzero);
	}
	OnZero( cfg::guide::Calib& powner,helium::JointPosConverter& ppc):
	  helium::ConnReference<RawPos>(this),
	  owner(powner),pc(ppc){}
      } onZero;
    
      CalibCallback(const std::pair<cfg::guide::Calib*,helium::JointPosConverter*>& owner):
	onMin(*owner.first,*owner.second),
	onMax(*owner.first,*owner.second),
	onZero(*owner.first,*owner.second){      
      }
      
      ~CalibCallback(){
      }
    
    }calibCallback;//CalibCallback
    //move from guidedata

    //friend class helium::DefDescr<GuideLogic::JointLogic>;
    friend class GuideLogic;
    cfg::guide::Joint& joint;


    static const int DataSrcTypes=3;
    DataSrc currNormDataSrc,currRawDataSrc;
    helium::mlin::JointEPVTarget<helium::RelMotorSign,helium::motor::IFACELEVEL> targetPos;
    helium::mlin::JointEPVTarget<helium::AbsMotorSign,helium::motor::RAWLEVEL> rawTargetPos;
    

    class CallTempAssure:public helium::Callback<double>,
      public helium::ConnReference<double>{
      helium::ValueHub<double>& t;
    public:
      void operator()(double d){
	t.assure(round(d));
      }
      CallTempAssure(helium::ValueHub<double>* pt):helium::ConnReference<double>(this),t(*pt){
      }
      
    };

    
    //helium::StaticConnReference<double, helium::ValueHub<double>,
    //const double&,&helium::ValueHub<double>::assure > 

    CallTempAssure callTempAssure;

    


    void assureDisplayedPos(DataSrc desiredSrc);
    void assureRawDisplayedPos(DataSrc desiredSrc);
    
    void assureModeIfPowered(const helium::motor::ModeId& mode);

    void onMotorModeNotified(const helium::motor::ModeId& mode); 
    void onMotorPositionNotified(helium::motor::Interface::Position p);
    void onRawMotorPositionNotified(RawPos p);

    void onPowerSetRequest(bool b);
    void onPositionSetRequest(double d);
    void onRawPositionSetRequest(RawPos d);
    
    void fixConnections(cfg::guide::GuideView::Type view,const helium::motor::ModeId& mode);

    void onViewNotified(cfg::guide::GuideView::Type v);
    
    void onConnectionNotified(helium::Connection c);
    

    
  
    
    
    
    helium::StaticConnReference<NormPos, JointLogic,
				NormPos, &JointLogic::onMotorPositionNotified > callOnMotorPositionNotified;
    
    helium::StaticConnReference< RawPos, JointLogic,
				 RawPos, &JointLogic::onRawMotorPositionNotified > callOnRawMotorPositionNotified;

    
    helium::StaticConnReference< bool, JointLogic,
				 bool, &JointLogic::onPowerSetRequest > callOnPowerSetRequest;
    
    helium::StaticConnReference< NormPos, JointLogic,
				 NormPos, &JointLogic::onPositionSetRequest > callOnPositionSetRequest;
    
    helium::StaticConnReference< RawPos, JointLogic,
				 RawPos, &JointLogic::onRawPositionSetRequest > callOnRawPositionSetRequest;

    
 


      helium::StaticConnReference< const helium::motor::ModeId& , JointLogic,
				 const helium::motor::ModeId&, &JointLogic::onMotorModeNotified >
    
      callOnMotorModeNotified;

    helium::StaticConnReference< cfg::guide::GuideView::Type, JointLogic,
				 cfg::guide::GuideView::Type, &JointLogic::onViewNotified > callOnViewNotified;
   


    helium::StaticConnReference< helium::Connection, JointLogic,
				 helium::Connection, &JointLogic::onConnectionNotified > callOnConnectionNotified;
    
    template<typename T>
    class CallPosAssureExcept:public helium::Callback<T>,public helium::ConnReference<T>{
      helium::ValueHub<T>& vh;
      helium::ConnReference<T>& ex;
    public:
      CallPosAssureExcept(helium::ValueHub<T>* pvh,helium::ConnReference<T>& pex):
	helium::ConnReference<T>(this),vh(*pvh),ex(pex){
      }
      
      void operator()(T t){
	//std::cout<<"was notified"<<t<<std::endl;
	//helium::printTrace();
	vh.assureExcept(t,ex);
      }

      };
    
    typedef helium::StaticConnReference<NormPos,helium::ValueHub<NormPos>,
					const NormPos&,
					&helium::ValueHub<NormPos>::assure > 
    CallNormPosAssure;
  
    typedef CallPosAssureExcept<NormPos> CallNormPosAssureExcept;
  
    
  typedef helium::StaticConnReference<RawPos,helium::ValueHub<RawPos>,
				      const RawPos&,&helium::ValueHub<RawPos>::assure > 
  CallRawPosAssure;


    typedef CallPosAssureExcept<RawPos> CallRawPosAssureExcept;
  
  
    //typedef CallPosAssureExcept<RawPos> CallRawPosAssureExcept;
    
    /*helium::Array<CallDisplayedPosAssure,
		DataSrcTypes,
		helium::array::InternalAllocator<
		  helium::ParamAllocator<CallDisplayedPosAssure,helium::ValueHub<double>* > 
		  >
		  > callDisplayedPosAssure;*/
    
    
    CallNormPosAssureExcept callDesiredPosAssure;
    CallNormPosAssure callDisplayedPosAssure;
    CallRawPosAssureExcept callRawDesiredPosAssure;
    CallRawPosAssure callRawDisplayedPosAssure;
  public:
    //JointLogic(cfg::guide::Joint& joint, helium::JointPosConverter& jpc);
    JointLogic(GuideJointParam j);
    ~JointLogic();
    
    
    
  };//class JointLogic
  
private:
  cfg::guide::Joints& joints;
  helium::Posture smoothPlayPosture;
  helium::mlin::EPVTarget<helium::RelMotorSign,helium::motor::IFACELEVEL> smoothPlayTarget;
  helium::MotionParameter &mp;
    /*
  helium::Array<JointLogic,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<helium::VectAllocator<JointLogic,cfg::guide::Joints > >
		> jointLogic;
    */
  helium::JointPosConverter& posNormalizer;
  
    helium::Array<JointLogic,
		helium::array::RESIZABLE,
		helium::array::InternalAllocator<
		  helium::ParametricFuncAllocator<JointLogic, 
						  GuideJointParam,
						  GuideJointParam,
						  extractJoint>
		  > 
		> jointLogic;




  

//friend class helium::DefDescr<GuideLogic>;
  
  helium::framePlay::Target framePlayMotion;

 
  
  void onPlayRequest(const helium::Motion &m);
  
  helium::StaticConnReference< const helium::Motion &, GuideLogic,
			       const helium::Motion &, &GuideLogic::onPlayRequest > callOnPlayRequest;
  
public:
  GuideLogic(GuideState& gs, helium::JointPosConverter& jpc);
  
  
};

#endif
