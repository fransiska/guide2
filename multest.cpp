#include <helium/core/enum.h>
#include <iostream>
using namespace std;

//compile
//g++ multest.cpp -o multest -I ../../
enum A {ao,ab};

namespace helium {
template<>  struct EnumStrings<A>{
 static const int NUM=2;
 static const char* strings[NUM];
};
const char* EnumStrings<A>::strings[]={"pippo","pluto"};
namespace ns{
en::Enio io;
}
}

int main() {

  enum Testing {tone,ttwo};
  A x = ao;
  //helium::EnumIO<Testing> a;

  cout << tone << " " << ttwo << endl;
  cout << "e " << helium::ns::io << x << endl;
  return 0;
}
/*

#include <iostream>
#include <sstream>
using namespace std;



template<typename E>  struct EnumStrings{
  static const int NUM=0;
};


template<typename T,bool isEnum=(EnumStrings<T>::NUM>0?true:false)>
class EnumIO{
};


template<typename E>
class EnumIO<E,false>{
public:
    E& e;
  static std::ostream& out(std::ostream&o,const E& e){
    return o<<e;
  }
  
  static std::istream& in(std::istream& is,E& e){
    int x;
    is>>x;
    e=(E)x;
  }  
  EnumIO(E& pe):e(pe){}
  
};

template<typename E>
class EnumIO<E,true>{
public:
  E& e;
  static std::ostream& out(std::ostream&o,const E& e){
    return o<<EnumStrings<E>::strings[e];
  }
  
  static std::istream& in(std::istream& is,E& e){
    std::string s;
      is>>s;
      for (int i=0;i<EnumStrings<E>::NUM;i++){
	if (EnumStrings<E>::strings[i]==s){
	  e=(E)i;
	  return is;
	}
      }
      //throw exc::FormatConversion(s,e);
  }  
  EnumIO(E& pe):e(pe){}
};






namespace en{
template<typename T>
EnumIO<T> enumio(T& t){
  return EnumIO<T>(t);
}


struct Enio{
  struct ostrm{
    std::ostream& o;
    explicit ostrm(std::ostream& po):o(po){}
  };
  struct istrm{
    std::istream& i;
    explicit istrm(std::istream& pi):i(pi){}
  };
};

Enio io;
  Enio::ostrm operator<<(std::ostream& o,const Enio& e){
    return Enio::ostrm(o);
  }
  

template<typename E>
std::ostream& operator<<(const Enio::ostrm& s,const E& e){
  return EnumIO<E>::out((ostream&)s.o,e);  
}

  Enio::istrm operator>>(std::istream& i,Enio& e){
    return Enio::istrm(i);
  }


template<typename E>
std::istream& operator>>(const Enio::istrm& s,E& e){
  return EnumIO<E>::in((istream&)s.i,e);  
}


}


enum A{A1,A2};


template<>  struct EnumStrings<A>{
  static const int NUM=2;
  static const char* strings[NUM];
};

const char* EnumStrings<A>::strings[]={"pippo","pluto"};


int main(){
  A x=A2;
  int a=-1;
  int z=10;

  std::cout<<a<<" "<<en::io<<x<<" "<<z;
  std::cin>>a>>en::io>>x>>z;
  std::cout<<a<<" "<<en::io<<x<<" "<<z;
  //s<<a<<z;
  //v<<" "<<s<<std::endl;
  
  /*std::stringstream ss;
  
  EnumIO<Vals>::out(ss,v)<<endl;
  EnumIO<Str>::out(ss,s)<<endl;  
  v=ONE;
  s=SONE;
  EnumIO<Vals>::out(cout,v)<<endl;
  EnumIO<Str>::out(cout,s)<<endl;  

  EnumIO<Vals>::in(ss,v);
  EnumIO<Str>::in(ss,s); 
 
  EnumIO<Vals>::out(cout,v)<<endl;
  EnumIO<Str>::out(cout,s)<<endl;  
*/
/*
}
*/
