
/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef GUIDEUTIL
#define GUIDEUTIL

#include <iostream>

namespace GuideUtil{
  inline void trim(std::string &str) {
    std::size_t first = str.find_first_not_of(' ');
    std::size_t last  = str.find_last_not_of(' ');
    str = str.substr(first, last-first+1);
  }
};

#endif
