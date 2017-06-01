/*!
 * \brief   guide
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#include "guideLogic.h"
#include <helium/robot/framePolicy.h>


static const helium::motProp::ReadJointPropertyRequest potentioRequest(helium::motor::POTENTIOPOSITION,-1,false);



void GuideLogic::JointLogic::assureDisplayedPos(DataSrc desiredSrc){
  //std::cout << "assuredisp "<< desiredSrc << " of "<<joint.getId()<<std::endl;

  if (desiredSrc==currNormDataSrc) return;  
  //first connect, then disconnect, so we don't need to wait the disconnect time
  switch(desiredSrc){
  case DESIREDTARGETSRC:
    break; 
  case POTENTIOSRC:{
    //std::cout<<"motor norm properties connect"<<std::endl;
    joint.motorNormProperties.connect(callDisplayedPosAssure,potentioRequest);
    //std::cout<<"motor norm properties connect2"<<std::endl;
    joint.motorNormProperties.connect(callDesiredPosAssure,potentioRequest);
  }
    break;    
  }
  
  switch(currNormDataSrc){
  case DESIREDTARGETSRC:
    break; 
  case POTENTIOSRC:
    //std::cout<<"motor norm properties DISconnect"<<std::endl;
    joint.motorNormProperties.disconnect(callDisplayedPosAssure);
    joint.motorNormProperties.disconnect(callDesiredPosAssure);
    break;
  };  
  currNormDataSrc=desiredSrc;
}


void GuideLogic::JointLogic::assureRawDisplayedPos(DataSrc desiredSrc){
  //std::cout << "assurerawdisp" << std::endl;

  if (desiredSrc==currRawDataSrc) return;  
  //first connect, then disconnect, so we don't need to wait the disconnect time
  switch(desiredSrc){
  case DESIREDTARGETSRC:
    break; 
  case POTENTIOSRC:{
    joint.motorRawProperties.connect(callRawDisplayedPosAssure,potentioRequest);
    joint.motorRawProperties.connect(callRawDesiredPosAssure,potentioRequest);
  }
    break;    
  }
  
  switch(currRawDataSrc){
  case DESIREDTARGETSRC:
    break; 
  case POTENTIOSRC:
    joint.motorRawProperties.disconnect(callRawDisplayedPosAssure);
    joint.motorRawProperties.disconnect(callRawDesiredPosAssure);
    break;
  };  
  currRawDataSrc=desiredSrc;
}




void GuideLogic::JointLogic::assureModeIfPowered(const helium::motor::ModeId& mode){
  //std::cout << "assuremodeifpowered" << std::endl;
  if (joint.motorMode.get()!=helium::motor::OFFMODEID){  
    joint.motorMode.assure(mode);
  }    
}

void GuideLogic::JointLogic::onMotorPositionNotified(helium::motor::Interface::Position p){
  //  std::cout << "----------- onmotorpositionnotified " << std::setprecision(10) << p << std::endl;

  joint.displayedTarget=p; 
  if (p!=helium::motor::Interface::MOVINGPOS&&p!=helium::motor::Interface::FREEPOS){
    joint.desiredTarget.assureExcept(p,callOnPositionSetRequest);
  }
 
}


void GuideLogic::JointLogic::onRawMotorPositionNotified(RawPos p){
  //std::cout << "onrawnotif" << p << std::endl;
  joint.calibCurrentPosition=p; 
  if (p!=helium::AbsMotorSign::MOVINGPOS&&p!=helium::AbsMotorSign::FREEPOS){
    joint.absTarget.assureExcept(p,callOnRawPositionSetRequest);
  }
}

void GuideLogic::JointLogic::onPowerSetRequest(bool b){
  //std::cout << "onpoweerset" << std::endl;
  joint.motorMode.assure(b?
			 (owner.gs.view.get()==cfg::guide::GuideView::CALIB?
			  helium::MotorLinearMode<helium::RelMotorSign,helium::motor::RAWLEVEL>::modeID()
			  :    
			  helium::MotorLinearMode<helium::RelMotorSign,helium::motor::IFACELEVEL>::modeID()
			  ):
			 helium::motor::OFFMODEID
			 );  
}

void GuideLogic::JointLogic::onPositionSetRequest(double d){    
  //std::cout<<" state "<<owner.gs.motionInfo.framePlayState.get().first<<std::endl;

  if (owner.gs.motionInfo.framePlayState.get().first!=helium::framePlay::STOPPED){
    return;
  }
  //std::cout<<" position set request "<<d<<std::endl;
  //helium::printTrace();
  assureModeIfPowered(helium::MotorLinearMode<helium::RelMotorSign,helium::motor::IFACELEVEL>::modeID());
  targetPos.pos=d;  
  joint.remoteLinearNormTarget=targetPos;    
  
}

void GuideLogic::JointLogic::onRawPositionSetRequest(int p){    
  //std::cout<<"setting raw position to"<<p<<std::endl;  //helium::printTrace();
  assureModeIfPowered(helium::MotorLinearMode<helium::RelMotorSign,helium::motor::RAWLEVEL>::modeID());
  rawTargetPos.pos=p; 
  joint.remoteLinearRawTarget=rawTargetPos;    
}


void GuideLogic::JointLogic::fixConnections(cfg::guide::GuideView::Type view,const helium::motor::ModeId& mode){
  switch(view){
  case cfg::guide::GuideView::MOTION:    
  case cfg::guide::GuideView::TOUCH:    
    assureDisplayedPos(mode==helium::motor::OFFMODEID?
		       POTENTIOSRC:DESIREDTARGETSRC
		       );      		      		       
    assureRawDisplayedPos(DESIREDTARGETSRC);
    break;
  case cfg::guide::GuideView::CALIB:
    assureRawDisplayedPos(mode==helium::motor::OFFMODEID?
		       POTENTIOSRC:DESIREDTARGETSRC
		       );      		      		       
    assureDisplayedPos(DESIREDTARGETSRC);
    break;
  case cfg::guide::GuideView::INVALIDVIEW:
    assureRawDisplayedPos(DESIREDTARGETSRC);
    assureDisplayedPos(DESIREDTARGETSRC);
  default:
    break;
  }
}


void GuideLogic::JointLogic::onViewNotified(cfg::guide::GuideView::Type view){
  //std::cout<<"the view changed to "<<view<<std::endl;
  switch(view){
  case cfg::guide::GuideView::MOTION:    
  case cfg::guide::GuideView::TOUCH:   
    //std::cout<<"joint.motorPosition.get() rets "<<joint.motorPosition.get()<<std::endl;
    
    joint.desiredTarget.setExcept(joint.motorPosition.get(),callOnPositionSetRequest) ;
    assureModeIfPowered(helium::MotorLinearMode<helium::RelMotorSign,helium::motor::IFACELEVEL>::modeID());

    break;
  case cfg::guide::GuideView::CALIB:
    joint.absTarget.setExcept(joint.motorRawPosition.get(),callOnRawPositionSetRequest) ;
    assureModeIfPowered(helium::MotorLinearMode<helium::RelMotorSign,helium::motor::RAWLEVEL>::modeID());
    break;
  case cfg::guide::GuideView::INVALIDVIEW:
    break;
  default:
    break;
  }
  fixConnections(view,joint.motorMode.get());
}

void GuideLogic::JointLogic::onMotorModeNotified(const helium::motor::ModeId& mode){
  //std::cout << "motormodechange " << mode<< std::endl;
  fixConnections(owner.gs.view.get(),mode);
  joint.jointPower.assureExcept(mode!=helium::motor::OFFMODEID,callOnPowerSetRequest);//update the interface
}


void GuideLogic::JointLogic::onConnectionNotified(helium::Connection c){
  switch(c.state){
  case helium::Connection::CONNECTED:
    onViewNotified(owner.gs.view.get());
    break;
  case helium::Connection::DISCONNECTED:
  case helium::Connection::CONNECTING:
  case helium::Connection::DISCONNECTING:
    break;
  }
}

#include <helium/debug/tracePrinter.h>

GuideLogic::JointLogic::JointLogic(GuideJointParam j)://cfg::guide::Joint& pjoint, helium::JointPosConverter& jpc):
  owner(*j.third),
  calibCallback(std::make_pair(&(*j.first).calib,j.second)),

  joint(*j.first),//joint(pjoint),
  currNormDataSrc(DESIREDTARGETSRC),
  currRawDataSrc(DESIREDTARGETSRC),
  
  callTempAssure(&joint.jointTemp),


  callOnMotorPositionNotified(this),
  callOnRawMotorPositionNotified(this),
  callOnPowerSetRequest(this),
  callOnPositionSetRequest(this),						      
  callOnRawPositionSetRequest(this),

  callOnMotorModeNotified(this),
  callOnViewNotified(this),
  callOnConnectionNotified(this),

  callDesiredPosAssure(&joint.desiredTarget,callOnPositionSetRequest),
  callDisplayedPosAssure(&joint.displayedTarget),
  
  callRawDesiredPosAssure(&joint.absTarget,callOnRawPositionSetRequest),
  callRawDisplayedPosAssure(&joint.calibCurrentPosition)


{
  joint.motorNormProperties.connect(callTempAssure,helium::motProp::ReadJointPropertyRequest(helium::motor::TEMP,-1,false));//temperature read
  joint.motorMode.connect(callOnMotorModeNotified);
  joint.motorPosition.connect(callOnMotorPositionNotified);
  joint.motorRawPosition.connect(callOnRawMotorPositionNotified);
  joint.jointPower.connect(callOnPowerSetRequest);      
  joint.desiredTarget.connect(callOnPositionSetRequest);

  //helium::TracePrinter<double>* onDT=new helium::TracePrinter<double>();
  //joint.desiredTarget.connect(*onDT);

  joint.absTarget.connect(callOnRawPositionSetRequest);
  targetPos.v=j.third->gs.behaviorSettings.targetReachingSpeed;
  //M_PI/2/3000;
  rawTargetPos.v=j.third->posNormalizer.getAbsVelocity(j.third->gs.behaviorSettings.targetReachingSpeed) ;
  //((double)(0x0E00-0x200))/2/3000;

  //      joint.calib.absLim.minv.connect(updateMin)

  // connecting abslim(int) to update rellim(angle)
  joint.calib.absLim.minv.connect(calibCallback.onMin);
  joint.calib.absLim.maxv.connect(calibCallback.onMax);
  joint.calib.absLim.zero.connect(calibCallback.onZero);
    
  //initializing rellim
  joint.calib.absLim.zero.signalCache();
  joint.calib.absLim.minv.signalCache();
  joint.calib.absLim.maxv.signalCache();

  joint.motorMode.connect(callOnMotorModeNotified);
  owner.gs.view.connect(callOnViewNotified);
  owner.gs.conn.connect(callOnConnectionNotified);
}

GuideLogic::JointLogic::~JointLogic(){
  joint.motorNormProperties.disconnect(callTempAssure);
  joint.motorMode.disconnect(callOnMotorModeNotified);
  joint.jointPower.disconnect(callOnPowerSetRequest);      
  joint.desiredTarget.disconnect(callOnPositionSetRequest);
}






void GuideLogic::onPlayRequest(const helium::Motion &m){
  //std::cout << "called play" << std::endl;
  
  helium::FramePolicy fp(m);  
  fp.getPosture(smoothPlayPosture,mp.startTime);
  

  helium::HwIdList toMove;

  for (size_t j=0;j<joints.size();j++){
    if (gs.joints[j].motorMode.get()!=helium::motor::OFFMODEID){  
      toMove.ids.push_back(j);
    }    
  }
  helium::StdReply ret;
  gs.joints.multipleMotorMode.set(ret,make_pair(toMove,helium::MotorLinearMode<helium::RelMotorSign,helium::motor::IFACELEVEL>::modeID()));				  
  for (size_t j=0;j<joints.size();j++){
    smoothPlayTarget.pv[j].first=smoothPlayPosture[j];
  }
				  
  helium::TargetEndReason ter;
  gs.joints.remoteLinearNormWholeTarget.set(ter,smoothPlayTarget);

  gs.joints.multipleMotorMode.set(ret,make_pair(toMove,helium::MotorFramePlayMode<helium::RelMotorSign>::modeID()));

  setMotionData(framePlayMotion,m,mp);
  gs.motionInfo.framePlayTarget=framePlayMotion;
}








GuideLogic::GuideLogic(GuideState& pgs, helium::JointPosConverter& jpc):
  gs(pgs),
  //jointLogic(pjoints,pjoints.size()),
  joints(gs.joints),
  smoothPlayPosture(joints.size()),
  smoothPlayTarget(joints.size()),
  mp(gs.motionInfo.mp),
  posNormalizer(jpc),
  jointLogic(GuideJointParam(gs.joints.data,&jpc,this),joints.size()),
  callOnPlayRequest(this)
{
  //std::cout << "guidelogic" << std::endl;
  gs.motionInfo.target.connect(callOnPlayRequest);
  //resize motion
  framePlayMotion.frames.resize(joints.size());
  gs.motionInfo.motionEdit<GuideEvent::INITIALIZEMOTION,void,size_t,&helium::Motion::setJointSize>(joints.size());
  gs.motionInfo.motionEdit<GuideEvent::INITIALIZEMOTION,void,const std::string&,&helium::Motion::setRobotName>(gs.pref.robot);
  //gs.motionInfo.prevFrame.resize(joints.size());

  for (size_t j=0;j<joints.size();j++){
    smoothPlayTarget.pv[j].second=gs.behaviorSettings.targetReachingSpeed;    
  }
 

  //m.setJointSize(joints.size()); 
  //std::cout << "joint.size logic" << joints.size() << std::endl;
  //std::cout << "guidelogic " << gs.motionInfo.getMotion()[0] << std::endl;
}
 /*

  if (p==helium::motor::Interface::FREEPOS){
    assureDisplayedPos(POTENTIOSRC);
  }else if(p==helium::motor::Interface::MOVINGPOS){
    assureDisplayedPos(DESIREDTARGETSRC);
  }else if (p==helium::motor::Interface::INVALIDPOS){
    joint.displayedTarget.assure(NAN);
  }else{
    assureDisplayedPos(MOTORPOSITIONSRC);
    joint.displayedTarget.assure(p);
    }*/
