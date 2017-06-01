/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

const char* JointChangeReasonStr[] = {"UNSPECIFIEDJOINTCHANGE",
				      "INITIALIZEJOINT",
				      
				      "VALUECHANGE",
				      //"ENABLEJOINTCHANGE", //frame change as a result of enable change
				      //"FRAMETYPEDISABLE", //1 joint is disabled
				      //"FRAMETYPEENABLE", //1 joint is enabled
				      
				      "SYNCJOINTCHANGE",
				      "SYNCALLJOINTCHANGE",
				      
				      //minor change
				      "TIMESELECTIONJOINTCHANGE", //curTime change
				      "DIRECTJOINTCHANGE", //change a joint frameValue
				      
				      //all joint change
				      "ALLJOINTCHANGE", //pasting a frame
				      "ALLZEROJOINTCHANGE", //zero all frameValue
				      "ALLENABLEJOINTCHANGE", //enable all frame
				      "COPYCOLDETAUTOFIX", //copying the coldet auto fix
				      "MIRRORPOSTURE",
				      "PASTESTRING",

				      "UNDOJOINTCHANGE",
				      "REDOJOINTCHANGE"

};

const char* MotionChangeReasonStr[] = {"SAVEFRAME",
			   "SAVESELECTFRAME",
			   "DISCARDFRAME",
			   "INSERTFRAME",
			   "INSERTTEMPFRAME",			   
			   "ADJUSTTIME",
			   "RENAMEFRAME",
			   "LINKFRAME",
			   "UNLINKFRAME",
			   "PASTEFRAME",


			   "UNDOMOTION",
			   "REDOMOTION",
			   "OPENMOTION",
			   "NEWMOTION",

			   //ignore change
			   "INITIALIZATION",
				       "ENABLECHANGE"};

const char* CalibChangeReasonStr[] = {"MAXCHANGE",
				      "MINCHANGE",
				      "ZEROCHANGE",
				      "OPENCALIB",

				      //ignore change
				      "UNDOCALIB",
				      "REDOCALIB"
};
