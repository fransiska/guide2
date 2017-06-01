/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "motionInfo.h"

MotionInfo:: MotionInfo():
    //prevMotion(NULL),
    framePlayState(std::make_pair(helium::framePlay::STOPPED,0)), 
    framePlayTarget(helium::Void()),
    curTime(0),
    editing(false,false),
    motion(1),
    prevMotion(NULL)
  {
    framePlayState.setCache(std::pair<helium::framePlay::PlayState,helium::Time>(helium::framePlay::STOPPED,0));
  }
