/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDELOG
#define GUIDELOG

#include <iostream>
#include <fstream>
#include <helium/os/time.h>

class GuideLog {
  std::string filename;
  std::fstream* log;
  helium::Time startTime;

public:
  GuideLog(std::string pfilename); 
  ~GuideLog();

  GuideLog& operator<< (const std::string &s);  
};

#endif
