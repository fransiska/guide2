/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "guideDescription.h"
#include "connectionCell.h"

namespace helium{

  void DefDescr<Point2D>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.x),"x"));
    members.push_back(scalarMapping(d(obj.y),"y"));
  }

  void DefDescr<cfg::guide::Calib>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(hexMapping(obj.absLim.minv,"min"));
    members.push_back(hexMapping(obj.absLim.zero,"zero"));
    members.push_back(hexMapping(obj.absLim.maxv,"max"));
  }

  void DefDescr<cfg::guide::Joint>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.motorPosition),IDPath("position.exact",".")));
    members.push_back(scalarMapping(d(obj.motorMode),IDPath("mode",".")));
    members.push_back(scalarMapping(d(obj.motorNormProperties),IDPath("properties.norm",".")));
    members.push_back(scalarMapping(d(obj.motorRawProperties),IDPath("properties.raw",".")));
    members.push_back(scalarMapping(d(obj.remoteLinearNormTarget),IDPath("target.linear.norm",".")));
    members.push_back(scalarMapping(d(obj.remoteLinearRawTarget),IDPath("target.linear.raw",".")));
    members.push_back(scalarMapping(d(obj.motorRawPosition),IDPath("target.linear.raw.current",".")));
     
    members.push_back(scalarMapping(d(obj.descr),"description"));
    members.push_back(scalarMapping(d(obj.type),"type"));
    members.push_back(scalarMapping(d(obj.reverse),"reverse"));

    //members.push_back(scalarMapping(d(obj.relLimit.minv),IDPath("relLimit.min",".")));
    //members.push_back(scalarMapping(d(obj.relLimit.maxv),IDPath("relLimit.max",".")));
    //members.push_back(scalarMapping(d(obj.desiredTarget.remoteTarget),IDPath("target.norm",".")));
    //members.push_back(scalarMapping(d(obj.desiredTarget.remoteTarget),IDPath("plan.norm",".")));
  

    members.push_back(scalarMapping(d(obj.name),"name"));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.mirrorId,"mirror"));
    members.push_back(scalarMapping(d(obj.pos),""));

    members.push_back(scalarMapping(d(obj.jointState),"state"));
    members.push_back(scalarMapping(d(obj.jointMonitor),"monitorMode"));//added 20120529

    members.push_back(scalarMapping(d(obj.calib),"calib")); 
    members.push_back(scalarMapping(d(obj.id),"id"));
  }

  void DefDescr<cfg::guide::Joint>::onInitializationFinished(helium::Memory& mem){
    if(obj.type == helium::Rotation::PITCH) {
      obj.u[0] = 0;//slider direction
      obj.u[1] = 1;//facing down
      obj.v[0] = -1;//slider normal
      obj.v[1] = 0;//facing left    
    }
    else {
      obj.u[0] = 1;//slider direction
      obj.u[1] = 0;//facing right
      obj.v[0] = 0;//slider normal
      obj.v[1] = -1;//facing up    
    }
    
    if(obj.reverse) {
      helium::mulScalar(obj.u,-1);//reverse slider normal direction
    }
  }

  void DefDescr<MotionInfo>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){ //DEBUG motionInfo
    members.push_back(scalarMapping(d(obj.framePlayTarget),"target"));
    members.push_back(scalarMapping(d(obj.framePlayState),"state"));
    
  }

  void DefDescr<cfg::guide::Joints>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.defaultCalib.min),IDPath("min",".")));
    members.push_back(scalarMapping(d(obj.defaultCalib.max),IDPath("max",".")));
    members.push_back(hexMapping(obj.defaultCalib.calibMin,IDPath("calibmin",".")));
    members.push_back(hexMapping(obj.defaultCalib.calibMax,IDPath("calibmax",".")));
    members.push_back(scalarMapping(obj.remoteLinearNormWholeTarget,IDPath("target.linear.norm",".")));
    members.push_back(scalarMapping(obj.multipleMotorMode,"mode"));

    //members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.modelFilename)),IDPath("model",".")));
    //members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.modelFilename)),IDPath("model",".")));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.modelFilename,"model"));
	  
    //members.push_back(scalarMapping(d(obj.monitorMode),"monitorMode")); //monitormode added by FB 20120529
    //members.push_back(scalarMapping(d(obj.motion),IDPath("frames")));     
    //members.push_back(scalarMapping(obj.spoolctrl,IDPath("spooler.minpriority",".")));    
  }

  void DefDescr<cfg::guide::Joints>::onInitializationFinished(helium::Memory& mem){
    
  }

  /*  
  void DefDescr<cfg::guide::Size>::import(Memory& mem,const IDPath &path) {
    obj.w = -1;
    obj.h = -1;
    Description<cfg::guide::Size>::import(mem,path);
  }
  */

  void DefDescr<cfg::guide::Size>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(fscalarMapping<SILENTFAIL>(obj.w,"width"));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.h,"height"));
  }

  void DefDescr<cfg::guide::Colors>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(vectMapping(vd(obj.cursor),IDPath("cursors.cursor","."),"rgb",IDPath("cursors.num",".")));
    members.push_back(vectMapping(vd(obj.powerState),IDPath("switches.switch","."),"rgb",IDPath("switches.num",".")));
    members.push_back(scalarMapping(d(obj.disable),IDPath("disable.rgb",".")));
    members.push_back(scalarMapping(d(obj.disablebg),IDPath("disablebg.rgb",".")));
    members.push_back(scalarMapping(d(obj.moving),IDPath("moving.rgb",".")));
    members.push_back(scalarMapping(d(obj.coldet),IDPath("coldet.rgb",".")));
    members.push_back(scalarMapping(d(obj.coldetSelected),IDPath("coldetselected.rgb",".")));
    members.push_back(scalarMapping(d(obj.movingModel),IDPath("movingmodel.rgb",".")));
    members.push_back(scalarMapping(d(obj.defaultModelBG),IDPath("defaultmodelbg.rgb",".")));
    members.push_back(scalarMapping(d(obj.error),IDPath("error.rgb",".")));

    members.push_back(scalarMapping(d(obj.entryDefault),IDPath("entry.default.rgb",".")));
    members.push_back(scalarMapping(d(obj.entryInvalid),IDPath("entry.invalid.rgb",".")));

    members.push_back(scalarMapping(d(obj.selected),IDPath("selected.rgb",".")));
    members.push_back(vectMapping(vd(obj.reds),IDPath("reds.red","."),"rgb",IDPath("reds.num",".")));
    members.push_back(vectMapping(vd(obj.frameGradations),IDPath("framegradations.framegradation","."),"rgb",IDPath("framegradations.num",".")));
    members.push_back(vectMapping(vd(obj.calibs),IDPath("calibs.calib","."),"rgb",IDPath("calibs.num",".")));
    members.push_back(vectMapping(vd(obj.bgs),IDPath("bgs.bg","."),"rgb",IDPath("bgs.num",".")));
    members.push_back(scalarMapping(d(obj.nanTemp),IDPath("nantemp.rgb",".")));
    members.push_back(vectMapping(vd(obj.sensorTheme),IDPath("sensorthemes.sensortheme","."),"rgb",IDPath("sensortheme.num",".")));
  }

  void DefDescr<cfg::guide::Sizes>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.smallJoint),IDPath("joint.small",".")));
    members.push_back(scalarMapping(d(obj.bigJoint),IDPath("joint.big",".")));
    members.push_back(scalarMapping(d(obj.bigJointTagHeight),IDPath("joint.bigtagh",".")));
    members.push_back(scalarMapping(d(obj.stroke),IDPath("stroke.size",".")));
    members.push_back(scalarMapping(d(obj.bar),"bar"));
    members.push_back(scalarMapping(d(obj.warningBar),IDPath("warningbar.size",".")));
    members.push_back(scalarMapping(d(obj.entry),"entry"));
    members.push_back(scalarMapping(d(obj.model),"model"));
    members.push_back(scalarMapping(d(obj.calibOffset),IDPath("calib.offset",".")));
    members.push_back(scalarMapping(d(obj.entryOffset),IDPath("entry.offset",".")));
    members.push_back(scalarMapping(d(obj.entrysOffset),IDPath("entry.soffset",".")));
    members.push_back(scalarMapping(d(obj.entrySpace),IDPath("entry.spaceH",".")));
    members.push_back(scalarMapping(d(obj.powerSwitch),"switch"));
    //members.push_back(scalarMapping(d(obj.arrow),"arrow"));
    members.push_back(scalarMapping(d(obj.arrowBox),"arrowbox"));
    members.push_back(scalarMapping(d(obj.frame),"frame"));
    members.push_back(scalarMapping(d(obj.windowOff),"windowoff"));
    members.push_back(scalarMapping(d(obj.coldetBox),"coldetbox"));
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.bigJointOffset)),"bigjointoffset"));

    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.sensorWidget)),"sensorwidget"));
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.gyroWidget)),"gyrowidget"));

    members.push_back(scalarMapping(d(obj.switchOffset),IDPath("switch.offset",".")));


    //members.push_back(fscalarMapping<SILENTFAIL>(obj.name,"name"));
    //members.push_back(fscalarMapping<SILENTFAIL>(obj.warningBarOffset,IDPath("warningbar.offset",".")));
  }

  void DefDescr<cfg::guide::Settings>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.colors)),"colors"));
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.sizes)),"sizes"));
  }    


  void DefDescr<cfg::guide::ImageInfo>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.filename),"filename"));
    members.push_back(scalarMapping(d(obj.size),""));
  }   
  void DefDescr<cfg::guide::SensorGui>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.bg),""));
    members.push_back(scalarMapping(d(obj.radius),"radius"));
  }   

  void DefDescr<cfg::guide::Pedal>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(fscalarMapping<SILENTFAIL>(obj.checksum1,"checksum1"));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.checksum2,"checksum2"));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.rdownThreshold,"rdown"));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.rupThreshold,"rup"));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.ldownThreshold,"ldown"));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.lupThreshold,"lup"));
  }

  void DefDescr<cfg::guide::Preferences>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.robot),IDPath("robot.type",".")));
    members.push_back(scalarMapping(d(obj.bgFilename),IDPath("background.filename",".")));
    //members.push_back(scalarMapping(d(obj.gyroFilename),IDPath("gyrocalib.filename",".")));

    members.push_back(scalarMapping(d(obj.hasTouchSensor),IDPath("properties.touchsensor",".")));
    members.push_back(scalarMapping(d(obj.hasGyro),IDPath("properties.gyro",".")));
    members.push_back(scalarMapping(d(obj.hasModel),IDPath("properties.model",".")));
    members.push_back(scalarMapping(d(obj.hasColdet),IDPath("properties.coldet",".")));
    members.push_back(scalarMapping(d(obj.hasTemperature),IDPath("properties.temperature",".")));

    //members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.pedal)),"pedal"));
    //members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.tlFilename)),IDPath("teachlist.filename",".")));
    //members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.sensorGui)),"sensor"));

    //members.push_back(scalarMapping(d(obj.logFilename),IDPath("log.filename",".")));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.sensorLogfile,IDPath("log.sensor",".")));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.motionLogfile,IDPath("log.motion",".")));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.teachLogfile,IDPath("log.teach",".")));

    members.push_back(fscalarMapping<SILENTFAIL>(d(obj.sensorGui),"sensor"));
    members.push_back(fscalarMapping<SILENTFAIL>(d(obj.pedal),"pedal"));

    //sensors
    members.push_back(scalarMapping(obj.motorCalibFilename,GenericIDPath(IDPath("config.configfile.calib.filename","."),GenericIDPath::ROOT)));
  }

  void DefDescr<cfg::guide::Limb>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.name),"name"));
    members.push_back(scalarMapping(d(obj.pos),""));
    members.push_back(scalarMapping(d(obj.member),"member"));
  }    

  void DefDescr<cfg::guide::Limbs>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(psvectMapping(vd(obj),"limbs","limb",IDPath("robot.structure",".")));
  }

  void DefDescr<cfg::guide::Sensor>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(obj.rawValue,IDPath("value",".")));//added 20120316
    //members.push_back(scalarMapping(obj.state,IDPath("state",".")));//added 20120316
    //members.push_back(scalarMapping(obj.monitorMode,IDPath("monitorMode",".")));//added 20120316

    members.push_back(scalarMapping(d(obj.pos),""));
    members.push_back(scalarMapping(d(obj.id),"id"));
    members.push_back(scalarMapping(d(obj.board),"board"));
    members.push_back(scalarMapping(d(obj.channel),"channel"));
    members.push_back(scalarMapping(d(obj.limit.first),"min"));
    members.push_back(scalarMapping(d(obj.limit.second),"max"));
    members.push_back(scalarMapping(d(obj.name),"name"));
  }    

  void DefDescr<cfg::guide::Sensors>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){

    //members.push_back(scalarMapping(obj.spoolctrl,IDPath("spooler.minpriority",".")));
    members.push_back(scalarMapping(d(obj.state),IDPath("sensor.group.state",".")));
    members.push_back(scalarMapping(d(obj.monitorMode),IDPath("sensor.group.monitorMode",".")));
    //members.push_back(scalarMapping(obj.rawValue,IDPath("sensor.group.value",".")));//DEBUG replacing 
    //members.push_back(scalarMapping(obj.state,IDPath("sensor.group.state",".")));//DEBUG replacing 
  }


  /*void DefDescr<cfg::guide::Sensors>::onInitializationFinished(Memory& mem){
    Array<HwState> stateValue(obj.size());
    setAll(stateValue,UNKNOWNHWSTATE);
    obj.state.setCache(stateValue);
    std::cout<<"Initial state"<<std::endl;
    print(obj.state);
    obj.state.connect(debug);
    }*/



  void DefDescr<cfg::guide::Sensors>::onInitializationFinished(Memory& mem){
    obj.state.init(obj.size(),helium::UNKNOWNHWSTATE);
    obj.monitorMode.init(obj.size(),helium::UNKNOWNHWMON);
  }


  void DefDescr<cfg::guide::Gyro>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.data),IDPath("group.value","."))); 
    members.push_back(scalarMapping(fd<SILENTFAIL>(obj.calib),""));

    members.push_back(scalarMapping(d(obj.state),IDPath("group.state",".")));
    members.push_back(scalarMapping(d(obj.monitorMode),IDPath("group.monitorMode",".")));


  }

  void DefDescr<cfg::guide::Gyro>::onInitializationFinished(Memory& mem){
    obj.state.init(5,helium::UNKNOWNHWSTATE);
    obj.monitorMode.init(5,helium::UNKNOWNHWMON);
  }


  /*struct Debug:public  Callback<const std::pair<int,HwState>& >,
	       ConnReference<const std::pair<int,HwState>& > {
    
    void operator()(const std::pair<int,HwState>& p){
      std::cout<<"notified state"<<p.first<<" "<<p.second;
    }
    Debug():ConnReference<const std::pair<int,HwState>& >(this){
    }

  };
  Debug debug;
  */




  /*
  void DefDescr<helium::Posture>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(fscalarMapping<SILENTFAIL>(obj.name,"name"));
    members.push_back(scalarMapping(static_cast<Array<double>&>(obj),"angle"));
  } 
*/  

  void DefDescr<cfg::guide::Postures>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(psvectMapping(vd(obj),"postures","posture","guide"));
  }

  void DefDescr<GuideState>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){

    members.push_back(scalarMapping(d(obj.settings),IDPath("guide.settings",".")));
    members.push_back(scalarMapping(d(obj.conn),IDPath("guide.connection",".")));
    members.push_back(scalarMapping(d(obj.pref),IDPath("guide.preferences",".")));    
    members.push_back(scalarMapping(d(obj.settings.sizes.winSize),IDPath("guide.preferences.background",".")));
    //std::cout << (obj.pref.hasColdet?"1":"0") << "hascoldet before" << std::endl;
    members.push_back(psvectMapping(vd(obj.postures),"postures","posture","guide")); 
    //members.push_back(scalarMapping(d(obj.sensors.rawValue),IDPath("robot.touch.raw",".")));    

    members.push_back(psvectMapping(vd(obj.joints),"motors","motor","robot")); //import the array elements first, so we get the size
    members.push_back(psvectMapping(vd(obj.limbs),"limbs","limb",IDPath("robot.structure",".")));
    members.push_back(scalarMapping(d(obj.joints),IDPath("robot.motors","."))); //import the scalars
    members.push_back(scalarMapping(d(obj.motionInfo),IDPath("robot.motors.framePlayMode",".")));

    members.push_back(psvectMapping(vd(obj.sensors),"sensors","sensor","robot")); 
    members.push_back(scalarMapping(d(obj.sensors),IDPath("robot.sensors","."))); 
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.gyro)),IDPath("robot.gyro","."))); 
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.coldet)),IDPath("guide.coldet",".")));
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.teach)),IDPath("guide.teach",".")));
    members.push_back(scalarMapping(obj.behaviorSettings.targetReachingSpeed,IDPath("guide.logic.targetReachingSpeed","."))); 
    members.push_back(fscalarMapping<SILENTFAIL>(host,"ip"));
    members.push_back(fscalarMapping<SILENTFAIL>(port,"port"));
    members.push_back(fscalarMapping<SILENTFAIL>(obj.pref.pedal.dev,"pedal"));
  }



  void DefDescr<GuideState>::onInitializationFinished(Memory& mem){
    obj.conn.setCache(Connection(host,port,helium::Connection::DISCONNECTED));
    if (obj.pref.hasGyro){
      if (isnan(obj.gyro.calib.xoffset)) throw exc::MissingPath(IDPath("robot.gyro.xoffset"));
      if (isnan(obj.gyro.calib.yoffset)) throw exc::MissingPath(IDPath("robot.gyro.yoffset"));
      if (isnan(obj.gyro.calib.zoffset)) throw exc::MissingPath(IDPath("robot.gyro.zoffset"));
      if (isnan(obj.gyro.calib.xratio)) throw exc::MissingPath(IDPath("robot.gyro.xratio"));
      if (isnan(obj.gyro.calib.yratio)) throw exc::MissingPath(IDPath("robot.gyro.yratio"));
    }
    if(obj.teach.minVelocity.size() != obj.joints.size()) {
      std::cerr << "teach.minVelocity setting does not have correct length" << std::endl;
      obj.teach.minVelocity.resize(obj.joints.size());
      helium::setAll(obj.teach.minVelocity,0);
    }
    if(obj.teach.maxVelocity.size() != obj.joints.size()) {
      std::cerr << "teach.maxVelocity setting does not have correct length" << std::endl;
      obj.teach.maxVelocity.resize(obj.joints.size());
      helium::setAll(obj.teach.maxVelocity,1E10);
    }
  }

  
  /*  void DefDescr<GuideLogic::JointLogic>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){    
    //std::cout<<"motion.mode is being mapped"<<std::endl;
    }*/
  
  void DefDescr<cfg::guide::Coldet>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){    
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.idle)),IDPath("idle",".")));
    members.push_back(scalarMapping(fd<SILENTFAIL>(d(obj.busy)),IDPath("busy",".")));
  }
  
  /*void DefDescr<GuideLogic>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){    
    members.push_back(scalarMapping(d(obj.targetMotion),IDPath("robot.motors.target.linear",".") ));
    members.push_back(vectMapping(vd(obj.jointLogic),IDPath("robot.motors.motor","."),"",IDPath("robot.motors.num",".")));

    }*/

  void DefDescr<cfg::guide::Teach::Power>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){    
    members.push_back(scalarMapping(d(obj.motor),IDPath("motor.value",".")));
    members.push_back(psvectMapping(vd(obj.sensor),"sensors","sensor"));
  }
  void DefDescr<cfg::guide::Teach::Touch>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){    
    members.push_back(scalarMapping(d(obj.threshold),IDPath("threshold",".")));
    members.push_back(scalarMapping(d(obj.ignored),IDPath("ignored",".")));
  }
  void DefDescr<cfg::guide::Teach>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){    
    members.push_back(psvectMapping(vd(obj.powers),"powers","power"));
    members.push_back(scalarMapping(d(obj.touch),IDPath("touch",".")));
    members.push_back(scalarMapping(d(obj.minVelocity),IDPath("minVelocity",".")));
    members.push_back(scalarMapping(d(obj.maxVelocity),IDPath("maxVelocity",".")));
  }

};//ns helium
