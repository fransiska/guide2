/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef CHANGEABLE
#define CHANGEABLE

template <typename C> class changeable{
 public:
  const C def;
  C curr;
 changeable(const C& c):def(c),curr(c){
  }
  C& operator()(){ return curr;}

};

template <typename C> 
std::ostream& operator<<(std::ostream& o,const changeable<C>& c){
    o<<c.def<<" "<<c.curr;
    return o;
}

#endif
