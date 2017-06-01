/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "guideConfigTypes.h" 


namespace helium{
const char* EnumStrings<cfg::guide::Orientation::Type>::
  strings[EnumStrings<cfg::guide::Orientation::Type>::NUM]={"right","left"};

  const char* EnumStrings<cfg::guide::CursorType::Type>::
  strings[EnumStrings<cfg::guide::CursorType::Type>::NUM]={"Robot","File","Pot"};
  //const char* SyncType::strings[] = {"Disconnect","Connect","Copy","CopyAll"};

}
