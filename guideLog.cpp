/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "guideLog.h"
#include <iomanip>

GuideLog::GuideLog(std::string pfilename):filename(pfilename) {
  log=new std::fstream(filename.c_str(),std::fstream::out);
  if (log->fail()){
    std::cerr<<"error: GuideLog, in creation of "<<filename<<std::endl;
  }
  else {
    std::cout << "logging data to " << filename <<".txt"<< std::endl;
  }
  startTime = helium::getSystemMillis();
}

GuideLog::~GuideLog() {
  std::cout << "close guideLog" << std::endl;
  log->close();
  delete log;
}

GuideLog& GuideLog::operator << (const std::string &s) {
  *log << std::setprecision(17) << helium::getSystemMillis() << " " <<s;
  return *this;
}
