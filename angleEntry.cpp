/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include <helium/util/ioUtil.h>
#include "angleEntry.h"


using namespace std;


HexSpinButton::HexSpinButton(int min,int max) {
  set_numeric(false);
  set_range(0,100);
  set_increments(min,max);
}

int HexSpinButton::on_input(double* new_value) {
  std::string now = get_text();
  
  //erase trailing whitespaces
  std::string whitespaces (" \t\f\v\n\r");
  size_t found = now.find_last_not_of(whitespaces);
  if (found!=std::string::npos)
    now.erase(found+1);
  
  //erase left whitespace
  found = now.find_first_not_of(whitespaces);
  if (found!=std::string::npos)
    now = now.substr(found);
  
  //find non valid characters
  found = now.find_first_not_of("abcdef1234567890");
  
  int p;
  std::stringstream ss;
  ss << now;
  ss >> std::hex >> p;
  
  //limitting to range
  double max;
  double min;
  get_range(min,max);
  
  if(p<min || p > max || found != std::string::npos) { 
    *new_value = get_value_as_int();
    return Gtk::INPUT_ERROR;
  }
  
  *new_value = p++;
  return true;
  
}
bool HexSpinButton::on_output() {
  std::stringstream sss;
  sss << std::hex << get_value_as_int();
  set_text(sss.str());
  return true;
}


AngleEntry::AngleEntry(Gtk::Fixed *pfixed,double dmin,double dmax):
  MoveableEntry<double>(pfixed,dmin,dmax)
{ 
  this->set_increments(0.1,1);
  this->set_width_chars(5);
  this->set_update_policy(Gtk::UPDATE_IF_VALID);

  this->set_digits(2);
  this->set_numeric();
}

HexEntry::HexEntry(Gtk::Fixed *pfixed,int dmin,int dmax):
  MoveableEntry<int>(pfixed,dmin,dmax)
{ 
  this->set_increments(1,10);
  this->set_width_chars(5);
  this->set_update_policy(Gtk::UPDATE_IF_VALID);
}
