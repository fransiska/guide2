/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef _GUIDECONFIGTYPES_
#define _GUIDECONFIGTYPES_

#include <helium/core/enum.h>

namespace cfg{
  namespace guide{

    namespace Orientation{
       enum Type {RIGHT,LEFT} ; 
    };

    namespace CursorType{
      enum Type{ROBOT,FRAME,POT,NUM};
    };
    
    namespace GuideView {
      enum Type{MOTION,TOUCH,CALIB,NUM,INVALIDVIEW=-1};
    };

    
    namespace CalibType {
      enum Type {MIN,ZERO,MAX};
    };

    namespace SwitchingModelArea {
      enum ModelType {FRAME,ROBOT,FIX,NUM}; ///< \todo need to make this configurable for different switchingmodelarea subclasses
    }; 
  
    struct SyncType { 
      typedef enum { ALLCONNECT, CONNECT, ALLCOPY, COPY, DISCONNECT, NUM} Type; //DISABLE, 
      //static const char* strings[];
    };
    
    struct TicksType {
      typedef enum { KEYFRAMETICK, COLDETTICK, COLDETFRAMETICK } Type;
    };


    




  //fb win 20110630
  struct Color{
    double data[3];
    enum{R,G,B};
    double& operator[](int i){return data[i];}
    const double& operator[](int i) const{return data[i];}
  };
  
  inline std::ostream& operator<<(std::ostream& o,const Color& c){
    return o<<c.data[0]*255<<" "<<c.data[1]*255<<" "<<c.data[2]*255;
  }
  
  
  inline std::istream& operator>>(std::istream& i,Color& c){
      i>>c.data[0]>>c.data[1]>>c.data[2];
      for(int j=0;j<3;j++)
	c.data[j] /= 255;
      return i;
  }
  
  struct Size{
    int w;
    int h;	
    void set(int pw,int ph) { w =pw; h=ph; }
    Size():w(-1),h(-1) {}
  };


}//ns guide
  
}//ns cfg




namespace helium{
  template<>  struct EnumStrings<cfg::guide::Orientation::Type>{
    static const int NUM=2;
    static const char* strings[NUM];       
  };

  template<>  struct EnumStrings<cfg::guide::CursorType::Type>{
    static const int NUM=3;
    static const char* strings[NUM];       
  };
}//ns helium






#endif
