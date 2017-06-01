/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDESTATE
#define GUIDESTATE

#include "guideConfigTypes.h"
#include <app/guide2/connectionCell.h>
#include "point2D.h"
#include "guideEvents.h"
#include "motionInfo.h"

#include <helium/signal/tsSignal.h>
#include <helium/system/cellDescription.h>
#include <helium/system/valueHub.h>
#include <helium/system/tsValueHub.h>
#include <helium/system/protectedValueHub.h>

#include <helium/core/basicEnum.h>
//#include <helium/gtk/gtkProxy.h>

#include <helium/gtk/protectedGtkProxy.h>

#include <helium/robot/motorExceptionHandler.h>

#include <helium/robot/motorOffMode.h>
#include <helium/robot/motorLinearMode.h>

#include <helium/robot/sensorSpooler.h>
#include <helium/robot/hwStateMonitor.h>




#include <helium/robot/genericMotorSign.h>
#include <helium/robot/knownPosNormalizer.h>

#include <helium/util/moutPrinter.h>

#include <helium/robot/motorPropertyManager.h>    
#include <helium/robot/motorFramePlayMode.h>
#include <helium/robot/motorNotifiedLinearMode.h>
#include <helium/robot/gyroStructures.h>
#include <helium/system/cachePartialUpdate.h>

#include <memory>

namespace helium{
  template<typename Sign>
  class GtkArrayProxy:public CachePartialUpdate<GtkProxy<Sign> >{
  };
}

namespace cfg{


  namespace guide{
    

    struct Calib {

      struct AbsLimit{	
	helium::ValueHub<int> minv;
	helium::ValueHub<int> zero;
	helium::ValueHub<int> maxv;
      } absLim;
      
      helium::RelativeLimit relLim; 
      helium::ValueHub<int>& operator[](int i);//gets a CalibType,returns the corresponding ValueHub
    };


    class Joint{    
    
    public:
      Joint();         
      helium::GtkProxy<helium::motor::ExactPositionCellSignature> motorPosition;  
      helium::GtkProxy<helium::NotifiedLinearModeSignature<helium::AbsMotorSign,
							   helium::motor::RAWLEVEL> 
		       > motorRawPosition;

      helium::GtkProxy<helium::motor::MotorModeCellSignature> motorMode;
      
      
      typedef helium::MotorPropertyManager<helium::RelMotorSign>::JointPropertyCellSignature<helium::motProp::NORMFORMAT> NormPropertySign;
      typedef helium::MotorPropertyManager<helium::AbsMotorSign>::JointPropertyCellSignature<helium::motProp::RAWFORMAT>   RawPropertySign;


      helium::GtkProxy< NormPropertySign,
			 helium::DISABLERETRIEVEONREMOTE,
			 true//last value only
			 > motorNormProperties;
    
      helium::GtkProxy< RawPropertySign,
			 helium::DISABLERETRIEVEONREMOTE,
			 true//last value only
			 > motorRawProperties;


      helium::GtkProxy<helium::TargetCellBaseSignature<
			 helium::LinearModeSignature<helium::RelMotorSign,
						     helium::motor::IFACELEVEL>::JointTarget> 
		       > remoteLinearNormTarget;


    helium::GtkProxy<helium::TargetCellBaseSignature<
		       helium::LinearModeSignature<helium::AbsMotorSign,
						   helium::motor::RAWLEVEL>::JointTarget> 
		     > remoteLinearRawTarget;

      int mirrorId;

    private:
      int id;
      bool reverse;
      Point2D pos,u,v;
      std::string name;
      Orientation::Type descr;
      helium::Rotation::Type type;

      
      friend class helium::DefDescr<cfg::guide::Joint>;
    

    public:   
      Calib calib;
      
      helium::ProtectedValueHub<helium::FrameValue,
				GuideEvent::JointChangeReason,
				GuideEvent::INITIALIZEJOINT> frameValue; ///< the current value of the displayed frame

      helium::Array<helium::ValueHub<cfg::guide::SyncType::Type>,2> syncState; //FRANSISKA: initialize this
      helium::Array<cfg::guide::SyncType::Type,2> prevSyncState; ///<prevent double connection
      

      helium::ValueHub<double>desiredTarget; ///<connect cursor here
      helium::ValueHub<double> displayedTarget; ///<connect potentio bar here    
      //double displayedTargetBuffer; ///< for guideTeachWindow (defining which position to return)


      helium::ValueHub<int> absTarget;  ///<connect calib cursor here

      helium::ValueHub<int> calibCurrentPosition; ///<connect calib potentio bar here

      helium::ValueHub<bool> jointPower; ///<guideLogic listens here
      helium::ValueHub<double> jointTemp;
      helium::GtkProxy<helium::HwStateSingleSignature,helium::DISABLERETRIEVEONREMOTE> jointState; 


      helium::GtkProxy<helium::HwMonitorModeSingleSignature> jointMonitor; 

      int getId() {return id;}
      Point2D getPos() { return pos; }
      void getV(Point2D &p) { p=v; }
      void getU(Point2D &p) { p=u; }
      Point2D &getV() { return v; }
      Point2D &getU() { return u; }
      bool isLeftDescription() { return descr; }
      bool isReverse() { return reverse; }
      helium::Rotation::Type getType() { return type; }

      helium::TsSignal<helium::FrameValueType> toggleEnable;

      std::string getJointName() { return name; }


      double getLimit(CalibType::Type c);


      
      bool isOverLimit(double b); //angle

      bool isOverLimit(int b); //abs

   

      bool validateAbsTarget();//returns whether absTaget is in its limits;
      

      void validateValue(double &v);//bring v in the relative limits

      /** \brief set the limit of each calib (used in cursor and entry)
       * \todo DELETE?
       */
    void getCalibLimit(CalibType::Type t,int &min, int &max);

    ///set calib value (no value validation)
    void setCalib(CalibType::Type t,int c);


    
      //todo: replace with config
    bool getPower() {return jointPower.get();}
    double getTemp() {return jointTemp.get();}
    void setPower(bool p) { jointPower.assure(p);} 
    void setTemp(double t) {jointTemp.assure(t);}
    
      

    };//class Joint

    
    struct DefaultCalib {
      double max,min;
      int calibMin,calibMax;       
    };



    class Joints:public helium::Array<Joint,helium::array::RESETTABLE> {
    public:
      DefaultCalib defaultCalib;
      std::string modelFilename; 
      helium::Array<helium::ValueHub<cfg::guide::SyncType::Type>,2> syncState;

      helium::TsSignal<GuideEvent::PostureChangeReason> postureChange;
      Joints();

      helium::GtkProxy<helium::TargetCellBaseSignature<
			 helium::LinearModeSignature<helium::RelMotorSign,
						     helium::motor::IFACELEVEL>::Target> 
		       > remoteLinearNormWholeTarget;
      helium::GtkProxy<helium::motor::MultipleMotorModeCellSignature> multipleMotorMode;
      void mirrorPosture();
      void getMirrorPosture(double *pose);
    private:
      friend class ::ConnectionCell;
      friend class helium::DefDescr<cfg::guide::Joints>;     
    };
    
    
    

   

    struct Colors{
      helium::Array<Color,3> cursor;
      helium::Array<Color,2> powerState;
      Color selected;
      Color disable;
      Color disablebg;
      Color error;
      Color entryDefault;
      Color entryInvalid;
      helium::Array<Color,3> reds;
      helium::Array<Color,3> frameGradations;
      helium::Array<Color,3> calibs;
      helium::Array<Color,2> bgs;
      Color moving;
      Color movingModel;
      Color defaultModelBG; //white
      Color coldet;
      Color coldetSelected;
      Color nanTemp;
      helium::Array<Color,5> sensorTheme;

    };
    
    struct Sizes{
      int smallJoint,bigJoint,bigJointTagHeight,warningBar;
      Size bigJointOffset;
      int stroke;
      Size bar,entry,powerSwitch,arrowBox,frame,windowOff,model,sensorWidget,gyroWidget,coldetWidget;
      Size coldetBox; ///< size of collision detection box on the canvas (on the lower right of robot model)
      int entryOffset,entrysOffset,entrySpace,switchOffset,calibOffset;      
      Size winSize;
    };
      
    struct Settings{
      std::string timestamp;
      Colors colors;
      Sizes sizes;
      //int jointSize;
    };
    
    struct Postures:public std::vector<helium::Posture> { 
    };

    struct ImageInfo {
      std::string filename;
      Size size;
    };

    struct SensorGui {
      ImageInfo bg;
      int radius;
    };

    struct Pedal {
      std::string dev;
      int checksum1;
      int checksum2;
      int rdownThreshold;
      int rupThreshold;
      int ldownThreshold;
      int lupThreshold;
      Pedal();
    };

    struct Preferences{
      std::string robot;
      std::string bgFilename;
      std::string plFilename;

      /*! logfile 
       * can have tilda
       * extension will be ignored (assumed to be the characters behind last found dot)
       * date will be added behind this name
       */
      std::string teachLogfile;
      std::string sensorLogfile;
      std::string motionLogfile;

      std::string motorCalibFilename;
      Pedal pedal;
      bool hasTouchSensor;
      bool hasModel;
      bool hasGyro;
      bool hasTemperature;
      bool hasColdet; ///< collision detection enabled or not, default value is FALSE
      //sensor
      SensorGui sensorGui;
    };

    struct Limb{
      std::string name;
      Point2D pos;
      helium::Array<int> member;
    }; 

    //20110616 added to resemble joints struct
    struct Limbs:public helium::Array<Limb>{
    };

    //for sensors
    struct Sensor{
      helium::GtkProxy<helium::SensorCellSignature<int>,
		       helium::DISABLERETRIEVEONREMOTE >rawValue;
      //helium::GtkProxy<helium::HwStateSingleSignature> state;


      // sensorId = array id on hardwareConfig
      // 19 7 is the last possible sensor (row 160, index 159)
      // TODO DEBUG save vals here??
      Point2D pos;
      int id; //90
      int board;
      int channel;
      std::pair<int,int> limit; //min and max value
      std::pair<bool,bool> limitCalibrated; //min and max value calibrated flag
      std::string name;
    private:
      Sensor(const Sensor& ):rawValue(-1){}
    public:
      Sensor():rawValue(-1){}
      int getRawSensorID() {
	return board*8 + channel;
      }
      int getBoard() { return board; }
      int getChannel() { return channel; }
      int getMin() { return limit.first; }
      int getMax() { return limit.second; }
      std::string getName() { return name; }
      double getRatio() {
	return getRatio(rawValue.get());
      }
      double getRatio(int rawValue) {
	if (limit.second==limit.first) return 0.5;
	return (double)(rawValue-limit.first)/(limit.second-limit.first);
      }
      double getLimitedRatio(int value) {
	return helium::limit(0.0,1.0,getRatio(value));
      }
      void setMin(int value){
	limit.first = value;
	limitCalibrated.first = true;
      }
      void setMax(int value){
	limit.second = value;
	limitCalibrated.second = true;
      }
    }; 


    struct Sensors:public helium::Array<Sensor,helium::array::RESETTABLE>{
      Sensors();
      //void startMassiveOperation();
      //void endMassiveOperation();
    private:            friend class helium::DefDescr<Sensors>;
      friend class ::ConnectionCell;
      //helium::GtkProxy<helium::SpoolerControlSignature> spoolctrl;
      Sensors(const Sensors& s);
      //helium::ConnReference<int> scPrinterConn;
    public:
      helium::GtkArrayProxy<helium::HwStateGroupSignature> state;
      helium::GtkArrayProxy<helium::HwMonitorModeGroupSignature> monitorMode;
      //std::string getFaultySensorsString();
      //helium::GtkProxy<helium::SensorGroupCellSignature<int> > rawValue; //DEBUG replace by single 

      //std::string bg; //TODO DELETE
      //int radius;
      //Size winSize;
    };

    struct Gyro {
      helium::GtkProxy<helium::SensorGroupCellSignature<int>,
		       helium::DISABLERETRIEVEONREMOTE  > data;
      helium::GtkArrayProxy<helium::HwStateGroupSignature> state;
      helium::GtkArrayProxy<helium::HwMonitorModeGroupSignature> monitorMode;
  
      helium::GyroCalib calib;
      Gyro():data(helium::Void()){
	helium::SensorValList<int> gyroVal(5);	
	helium::zero(gyroVal.vals);
	data.setCache(gyroVal);
      }
      void getMirrorGyro(double*data) {
	data[0] = (data[0]-calib.xoffset)*-1+calib.xoffset;
      }
    };

    /**
     * \date 20130708
     * \brief collision detection parameters struct
     * 
     * 1. robotname.coldet.xml
     * <guide><coldet><idle default="" current=""> ...
     * 2. robotname.gui.xml
     * remember to turn on preferences.properties.coldet
     */
    struct Coldet {
      DefPairValue<int,1> idle; ///< used CPU number in idle time, first value = default, second value = current (always saved)
      DefPairValue<int,1> busy; ///< idem for busy , default value is set as 1 (if no data found in xml)
      helium::Signal<void> refresh; ///< signal to refresh coldet 
      helium::ValueHub<bool> coldetState; ///< coldet on or off
    };
      
    struct Teach {
      struct Power {
	helium::Array<int> motor;
	helium::Array<helium::Array<int>,2> sensor;
      };
      std::vector<Power> powers;
      struct Touch {
	helium::Array<int> ignored;
	double threshold;
      };
      helium::Array<double> minVelocity;
      helium::Array<double> maxVelocity;
      Touch touch;
    };
  }//guide
};//cfg
  

struct BehaviorSettings{

  double targetReachingSpeed; //in radians/s
};


class GuideState{
public:

  typedef helium::GtkProxy<helium::Connection> ConnectionGtkProxy;

  ConnectionGtkProxy conn;
  helium::ValueHub<cfg::guide::GuideView::Type> view;
  cfg::guide::Settings settings;
  cfg::guide::Limbs limbs;
  cfg::guide::Joints joints;
  cfg::guide::Sensors sensors;
  cfg::guide::Gyro gyro;
  cfg::guide::Preferences pref;
  cfg::guide::Postures postures;
  cfg::guide::Coldet coldet; ///< collision detection parameters
  cfg::guide::Teach teach;
  BehaviorSettings behaviorSettings;
  
  helium::GtkProxy<int> errorn;
  
  MotionInfo motionInfo;

  bool verbose;

  void getCurrentRobotPosture(helium::Posture &p);

  void getCurrentPot(helium::Posture &p); ///< displayedTarget
  void getActualCurrentPot(helium::Posture &p); ///< actually sends command to robot, must be used sparingly

  void getCurrentPosture(helium::Array<helium::FrameValue> &v) { ///< frameValue itself
    for(size_t i=0;i<joints.size();i++)
      v[i] = joints[i].frameValue.get();
  }
  helium::Posture getCurrentPosture() { ///<with NAN included
    helium::Posture tempPost(joints.size());
    getCurrentPosture(tempPost);
    return tempPost;
  }
  void getCurrentPosture(helium::Posture &p) { ///< with NAN
    for(int i=0;i<(int)joints.size();i++)
      p[i] = joints[i].frameValue.get().first == helium::VALUEFRAME?joints[i].frameValue.get().second:NAN;
  }
  void getCurrentSensorRaw(helium::Array<int> &s) {
    for(size_t i=0;i<sensors.size();i++)
      s[i] = sensors[i].rawValue.get();
  }
  void getCurrentSensorRatio(helium::Array<double> &s) {
    for(size_t i=0;i<sensors.size();i++)
      s[i] = sensors[i].getRatio();
  }
  void getSensorMax(helium::Array<int> &s) {
    for(size_t i=0;i<sensors.size();i++)
      s[i] = sensors[i].getMax();
  }
  void getSensorMin(helium::Array<int> &s) {
    for(size_t i=0;i<sensors.size();i++)
      s[i] = sensors[i].getMin();
  }
  /*
  void setCurrentPosture(helium::Posture &p) {
    for(int i=0;i<(int)joints.size();i++)
      joints[i].frameValue = p[i];
  }
  */
  bool isRobotMoving();
  bool isRobotMoving(double* posture);
  void renameLogfiles(std::string &s) {
    size_t lastdot = s.rfind(".");
    size_t lastslash = s.find_last_of("/\\");

    std::string rs = s;

    //clean dot
    if(lastdot != std::string::npos) {
      if(lastslash == std::string::npos || (lastslash != std::string::npos && lastdot > lastslash+1)) {
	rs = rs.substr(0,lastdot);
      }
    }
    //replace tilda
    if(s[0] == '~') {
      rs = getenv("HOME") + rs.substr(1,rs.length());
    }

    rs.append(settings.timestamp);
    s = rs;
  }

  void powerOffRobot() {
    std::cout << "power off robot" << std::endl;
    for(size_t i=0;i<joints.size();i++) {
      joints[i].jointPower = false;
    }
  }

  GuideState();

};//GuideState



#endif

/*namespace helium{
      template<typename Sign>
      class GtkArrayProxy:public GtkProxy<Sign>,public Callback<typename GtkProxy<Sign>::V::RUT>,public ConnReference<typename GtkProxy<Sign>::V::RUT>{
	
      public:
	GtkArrayProxy():GtkProxy<Sign>(helium::Void()),ConnReference<typename GtkProxy<Sign>::V::RUT>(this){
	  this->connect(*this);
	}
	
	void init(size_t n, const typename Sign::GT::value_type& initVal){
	  this->wCache.resize(n);	
	  setAll(this->wCache,initVal);
	}
	
	void operator()(typename GtkProxy<Sign>::V::RUT p){
	  this->wCache[p.first]=p.second;
	}


	void onRemoteAvailable(){
	  this->get(this->wCache);
	  //std::cout<<"size "<<this->wCache.size()<<std::endl;
	  for (size_t i=0;i<this->wCache.size();i++){	    
	    typename Sign::UT ut(i,this->wCache[i]);
	    this->signalData(ut);
	  }

	}
	
      };
    }
*/
