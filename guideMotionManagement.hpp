/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

template<cfg::guide::CursorType::Type c>
void GuideMotionManagement::onSyncStateUpdate(cfg::guide::SyncType::Type t) { ///sets all joints synced
  helium::Posture currentFrame(gs.joints.size());
  try{
    for(size_t i=0;i<gs.joints.size();i++) {
      currentFrame[i] = gs.joints[i].frameValue.get().second;
      gs.joints[i].syncState[c].assure(t);
    }
  }catch(helium::exc::Exception& ){
  }

  //only send signal when there is a change in the frame (ie. copy/connect) differing from robot
  if(c == cfg::guide::CursorType::ROBOT && t < cfg::guide::SyncType::DISCONNECT) {
    for(size_t i=0;i<gs.joints.size();i++) {
      if(currentFrame[i] != gs.joints[i].displayedTarget.get()) {
	gs.joints.postureChange(GuideEvent::SYNCPOSTURE);
	break;
      }
    }
  }
}


template<GuideEvent::MotionChangeReason REASON>
void GuideMotionManagement::insertFrameReason(const helium::Posture &post,helium::Time t) {
  if(t<0) t = gs.motionInfo.curTime.get();
  gs.motionInfo.curTime.setCache(t);
  gs.motionInfo.motionEdit<REASON,void,helium::Time,const helium::Posture&,std::string,&helium::Motion::insertFrame>(t,post,post.name); 
}

template<GuideEvent::PostureChangeReason REASON, GuideEvent::JointChangeReason REASONJ>
void GuideMotionManagement::pasteFrameReason(const helium::Posture &post,helium::Time t) {
  if(t<0) t = gs.motionInfo.curTime.get();
  try {
    //check joint reason
    if(REASONJ < GuideEvent::PASTEPOSTUREJOINTCHANGE || REASONJ > GuideEvent::PASTESTRINGJOINTCHANGE)
      throw helium::exc::InvalidDataValue(REASONJ);
  }
  catch(helium::exc::Exception& e){
    std::cerr << "Invalid joint change reason " << e.what();
  }
  gs.motionInfo.curTime.assure(t);
  for(size_t i=0;i<gs.joints.size();i++) {
    gs.joints[i].frameValue.assure<REASONJ>(std::make_pair(isnan(post[i])?helium::INTERPOLATEFRAME:helium::VALUEFRAME,post[i]));
  }  
  gs.joints.postureChange(REASON);
}

