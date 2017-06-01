#ifndef GUIDESTATE
#define GUIDESTATE

#include <iostream>

#include <helium/robot/framePolicy.h>

struct GuideProp {
  helium::Posture robotPost;
  helium::Motion motion;
  helium::Posture curPost;
  helium::Posture copyPost;
};

class GuideState {
  std::stack<GuideProp> undoStack;
  std::stack<GuideProp> redoStack;
};

#endif
