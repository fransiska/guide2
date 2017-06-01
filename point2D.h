/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef _POINT2D_
#define _POINT2D_

#include <helium/vector/array.h>
#include <helium/core/hasSize.h>
class Point2D:public helium::Array<double,2> {
 public:
  double &x,&y;
 Point2D():x(this->data[0]), y(data[1]){
    data[0] = 0;
    data[1] = 1;
  }
  Point2D(double a, double b):x(data[0]), y(data[1]) {
    data[0]=a;
    data[1]=b;
  }
  
  Point2D(const Point2D &b):helium::Array<double,2>(b),x(data[0]),y(data[1]){
  }
 
  
  Point2D& operator =(const Point2D& b){
    data[0] = b[0];
    data[1] = b[1];
    //std::cout<<"after assignment "<<*this<<"with src"<<b<<"mydata="<<data<<"friend data="<<b.data<<std::endl;
    return *this;
  }

  Point2D& reset(const double &x, const double &y){
    data[0] = x;
    data[1] = y;
    //std::cout<<"after assignment "<<*this<<"with src"<<b<<"mydata="<<data<<"friend data="<<b.data<<std::endl;
    return *this;
  }

};


/**
 * \brief 20130708 to store a pair of values, one default, one current
 *
 * T can be anything, V is the default value
 */
template <typename T,T V>
class DefPairValue : public std::pair<T,T> {
public:
  DefPairValue():std::pair<T,T>(),def(this->first),cur(this->second) {
    def=V;
    cur=V;
  }
  T &def;
  T &cur;
};

inline std::ostream& operator <<(std::ostream& o,const Point2D& l){
  return o<<" "<<l.x<<" "<<l.y;
}

namespace helium {
  template<>
  struct hasSize<Point2D>{
    static const bool value=true;
  };
}
  
#endif
