/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef HE_GUIDEEVENTS
#define HE_GUIDEEVENTS

namespace GuideEvent {
  //! Joints change
  enum JointChangeReason{
    //event
    DIRECTJOINTCHANGE,         /*!< Direct joint change from widget */
    ENABLEJOINTCHANGE,         /*!< Enable joint change from widget */
    SYNCJOINTCHANGE,           /*!< Click/drag on joint widget cursor */
    COPYJOINTCHANGE,           /*!<  */

    //all joints
    ALLCOPYJOINTCHANGE,        /*!< Left click on the sync arrow to frame */
    ALLSYNCJOINTCHANGE,        /*!< Right click on the sync arrow to frame */
    ALLZEROJOINTCHANGE,        /*!< Click on the all zero widget */
    ALLENABLEJOINTCHANGE,      /*!< Click on the all enable widget */

    PASTEPOSTUREJOINTCHANGE,              /*!< Changing each joint by pasting a posture to a frame */
    FIXPOSTUREJOINTCHANGE,
    PASTESTRINGJOINTCHANGE,
    PASTETEACHPOSTURE,
    PASTELISTJOINT,

    MIRRORJOINTS,

    //ignore
    TIMESELECTIONJOINTCHANGE,  /*!< Change to current time (Read out from motion) */
    INITIALIZEJOINT,

    UNDOJOINT,           /*!< Undo a joint change */
    REDOJOINT,           /*!< Redo a joint change */
  };

  //! Posture change
  enum PostureChangeReason {
    ALLZEROPOSTURECHANGE,
    ALLENABLEPOSTURECHANGE,
    MIRRORPOSTURE,
    SYNCPOSTURE,
    PASTELISTPOSTURE,
    PASTESTRING,
    PASTETEACHPOSTURECHANGE,
    FIXPOSTURE,                /*!< Fix a frame */

    UNDOPOSTURE,        /*!< Undo multiple joints change */
    REDOPOSTURE,         /*!< Redo multiple joints change */

    //ignore
    NONPOSTURECHANGE
  };

  //! Motion change
  enum MotionChangeReason {
    SAVETHISFRAME,
    SAVESELECTFRAME,

    INSERTFRAME,
    INSERTFRAMEFROMROBOT,
    DELETEFRAME,    
    PASTEFRAME,

    LINKFRAME,

    NEWMOTION,
    OPENMOTION,

    DISCARDFRAME,
    ADJUSTFRAME,
    RENAMEFRAME,
    UNLINKFRAME,

    //ignore
    INITIALIZEMOTION,
    DIRECTFRAMECHANGE,
    INSERTTEMPFRAME,
    DISCARDTEMPFRAME,
    REPLACEBACKFRAME,

    UNDOMOTION,
    REDOMOTION,
    UNDOINSERTTEMPFRAME,
    UNDOFRAME,

  }; 

  //! Overall events type
  enum EventType {
    SENTINELEVENT,    /*!< Is only used for the first item in the stack */
    JOINTEVENT, /*!< Change to a joint's value */
    POSTUREEVENT,    /*!< Change to all joint's value at one time */
    MOTIONEVENT,     /*!< When the user is done editing a keyframe */
    CALIBEVENT,
    EVENTTYPENUM      /*!< Number of EventType, not used */
  };

  enum CalibEventType {
    CALIBSENTINELEVENT,
    CALIBMINCHANGEEVENT,
    CALIBZEROCHANGEEVENT,
    CALIBMAXCHANGEEVENT,
    CALIBUNDOEVENT,
    CALIBREDOEVENT,
    CALIBEVENTTYPENUM
  };
};

inline std::ostream& operator<<( std::ostream& os, const GuideEvent::JointChangeReason&  jcr)
{
  switch( jcr ) {
  case GuideEvent::DIRECTJOINTCHANGE: os << "DirectJointChange";break;
  case GuideEvent::ENABLEJOINTCHANGE: os << "EnableJointChange";break;
  case GuideEvent::SYNCJOINTCHANGE: os << "SyncJointChange";break;
  case GuideEvent::COPYJOINTCHANGE: os << "CopyJointChange";break;
    
    //all joints
  case GuideEvent::ALLCOPYJOINTCHANGE: os << "AllCopyJointChange";break;      
  case GuideEvent::ALLSYNCJOINTCHANGE: os << "AllSyncJointChange";break;      
  case GuideEvent::ALLZEROJOINTCHANGE: os << "AllZeroJointChange";break;
  case GuideEvent::ALLENABLEJOINTCHANGE: os << "AllEnableJointChange";break;

  case GuideEvent::PASTELISTJOINT: os << "PasteListJoint";break;

    //ignore
  case GuideEvent::TIMESELECTIONJOINTCHANGE: os << "TimeSelectionJointChange";break;
  case GuideEvent::INITIALIZEJOINT: os << "InitializeJoint"; break;

  case GuideEvent::UNDOJOINT: os << "UndoJoint";break;
  case GuideEvent::REDOJOINT: os << "RedoJoint";break;

  case GuideEvent::PASTETEACHPOSTURE: os << "PasteTeachPosture"; break;

  case GuideEvent::PASTEPOSTUREJOINTCHANGE: os << "PastePostureJointChange";break;
  case GuideEvent::FIXPOSTUREJOINTCHANGE: os << "FixPostureJointChange";break;
  case GuideEvent::PASTESTRINGJOINTCHANGE: os << "PasteStringJointChange";break;
  default:
    throw helium::exc::MissingData("Undefined JointChangeReason string");
    break;
  }
  return os;
}
inline std::ostream& operator<<( std::ostream& os, const GuideEvent::PostureChangeReason& pcr) {
  switch( pcr ) {
  case GuideEvent::PASTELISTPOSTURE: os << "PasteListPosture"; break;    
  case GuideEvent::PASTESTRING: os << "PasteString"; break;
  case GuideEvent::FIXPOSTURE: os << "FixPosture"; break;
  case GuideEvent::ALLZEROPOSTURECHANGE: os << "AllZeroPostureChange"; break;
  case GuideEvent::ALLENABLEPOSTURECHANGE: os << "AllEnablePostureChange"; break;
  case GuideEvent::MIRRORPOSTURE: os << "MirrorPosture"; break;
  case GuideEvent::SYNCPOSTURE: os << "SyncPosture"; break;
  case GuideEvent::UNDOPOSTURE: os << "UndoPostureChange";break;
  case GuideEvent::REDOPOSTURE: os << "RedoPostureChange";break;
  case GuideEvent::PASTETEACHPOSTURECHANGE: os << "PasteTeachPostureChange"; break;

    //ignore    
  case GuideEvent::NONPOSTURECHANGE: os << "NonPostureChange"; break;
  default:
    throw helium::exc::MissingData("Undefined PostureChangeReason string");
    break;
  }
  return os;
}

inline std::ostream& operator<<( std::ostream& os, const GuideEvent::EventType& et) {
  switch( et ) {
  case GuideEvent::SENTINELEVENT: os << "SentinelEvent";break;
  case GuideEvent::JOINTEVENT: os << "JointEvent";break;
  case GuideEvent::POSTUREEVENT: os << "PostureEvent";break;
  case GuideEvent::MOTIONEVENT: os << "MotionEvent";break;
  case GuideEvent::CALIBEVENT: os << "CalibEvent";break;

  case GuideEvent::PASTESTRING: os << "PasteString";break;
  default:
    throw helium::exc::MissingData("Undefined EventChangeReason string");
    break;
  }
  return os;
}
inline std::ostream& operator<<( std::ostream& os, const GuideEvent::CalibEventType& et) {
  switch( et ) {
  case GuideEvent::CALIBSENTINELEVENT: os << "CalibSentinelEvent";break;
  case GuideEvent::CALIBMAXCHANGEEVENT: os << "CalibMaxChangeEvent";break;
  case GuideEvent::CALIBMINCHANGEEVENT: os << "CalibMinChangeEvent";break;
  case GuideEvent::CALIBZEROCHANGEEVENT: os << "CalibZeroChangeEvent";break;
  case GuideEvent::CALIBUNDOEVENT: os << "CalibUndoEvent";break;
  case GuideEvent::CALIBREDOEVENT: os << "CalibRedoEvent";break;
  default:
    throw helium::exc::MissingData("Undefined CalibEventChangeReason string");
    break;
  }
  return os;
}

inline std::ostream& operator<<( std::ostream& os, const GuideEvent::MotionChangeReason&  mcr)
{
  switch( mcr ) {
  case GuideEvent::SAVETHISFRAME: os << "SaveThisFrame"; break;
  case GuideEvent::SAVESELECTFRAME: os << "SaveSelectFrame"; break;
  case GuideEvent::DISCARDFRAME: os << "DiscardFrame"; break;
  case GuideEvent::PASTEFRAME: os << "PasteFrame"; break;
  case GuideEvent::INSERTFRAME: os << "InsertFrame"; break;
  case GuideEvent::INSERTFRAMEFROMROBOT: os << "InsertFrameFromRobot"; break;
  case GuideEvent::DELETEFRAME: os << "DeleteFrame"; break;   

  case GuideEvent::ADJUSTFRAME: os << "AdjustFrame"; break;
  case GuideEvent::RENAMEFRAME: os << "RenameFrame"; break;
    
  case GuideEvent::LINKFRAME: os << "LinkFrame"; break;
  case GuideEvent::UNLINKFRAME: os << "UnlinkFrame"; break;
    
  case GuideEvent::NEWMOTION: os << "NewMotion"; break;
  case GuideEvent::OPENMOTION: os << "OpenMotion"; break;

    //ignore
  case GuideEvent::UNDOMOTION: os << "UndoMotion"; break;
  case GuideEvent::REDOMOTION: os << "RedoMotion"; break;
  case GuideEvent::UNDOINSERTTEMPFRAME: os << "UndoInsertTempFrame"; break;
  case GuideEvent::UNDOFRAME: os << "UndoFrame"; break;
  case GuideEvent::INSERTTEMPFRAME: os << "InsertTempFrame"; break;
  case GuideEvent::DISCARDTEMPFRAME: os << "DiscardTempFrame"; break;
  case GuideEvent::DIRECTFRAMECHANGE: os << "DirectFrameChange"; break;
  case GuideEvent::REPLACEBACKFRAME:os << "ReplaceBackFrame"; break;


  default:
    throw helium::exc::MissingData("Undefined MotionChangeReason string");
    break;
  }
  return os;
}

#endif
