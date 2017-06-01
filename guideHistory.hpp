/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

template<typename CR> void GuideHistory::writeLog(GuideEvent::EventType e, CR r, std::string s) {
  std::stringstream ss;
  ss << e << " " << r;
  if(s.length())
    ss << " " << s;
  writeLog(ss.str());
}

