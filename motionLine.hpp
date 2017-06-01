/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


template<typename T>
ValidatingEntry<T>::ValidatingEntry(T& pvalue, helium::GtkExceptionHandling &eh, const cfg::guide::Color &pvalid, const cfg::guide::Color &pinvalid):
  value(pvalue),
  valid(pvalid),
  invalid(pinvalid)
{
  entry.signal_activate().connect_notify(eh.wrap(*this,&ValidatingEntry::onActivate));
  entry.signal_changed().connect_notify(eh.wrap(*this,&ValidatingEntry::onChange));
  entry.signal_focus_out_event().connect(eh.wrap(*this,&ValidatingEntry::onFocusOut));
  signal_leave_notify_event().connect(eh.wrap(*this,&ValidatingEntry::onLeave));


  add_events(Gdk::LEAVE_NOTIFY_MASK);
  entry.set_width_chars(2);
  add(entry);
  std::stringstream ss;
  ss << value;
  entry.set_text(ss.str());
}

///set the invalid value to red (focus is still on entry)
template<typename T>
void ValidatingEntry<T>::onActivate() {
  std::string txt = entry.get_text();
  Gdk::Color color;

  try {
    setValue(txt);
    color.set_rgb_p(valid[0],valid[1],valid[2]);
  }  
  catch(helium::exc::Exception& e){
    color.set_rgb_p(invalid[0],invalid[1],invalid[2]);
    entry.modify_text(Gtk::STATE_NORMAL, color);
    e.rethrow();
  }
  entry.modify_text(Gtk::STATE_NORMAL, color);
}

///set to previous valid value (focus away from entry)
template<typename T>
bool ValidatingEntry<T>::validateValue() {
  std::string txt = entry.get_text();
  Gdk::Color color;
  color.set_rgb_p(valid[0],valid[1],valid[2]);
  entry.modify_text(Gtk::STATE_NORMAL, color);

  try {
    setValue(txt);
    std::stringstream ss;
    ss << getValue();
    entry.set_text(ss.str());
  }  
  catch(helium::exc::Exception& e){
    std::stringstream ss;
    ss << getValue();
    entry.set_text(ss.str());
    e.rethrow();
  }
  return false;
}

template<typename T>
void ValidatingEntry<T>::onChange() {
  //changed from outside
  if(!entry.is_focus()) {
    validateValue();
  }
  //user edit
  else {
    Gdk::Color color;
    color.set_rgb_p(valid[0],valid[1],valid[2]);  
    entry.modify_text(Gtk::STATE_NORMAL, color);
  }
}

template<typename T>
bool ValidatingEntry<T>::onLeave(GdkEventCrossing* event) {
  return validateValue();
}

template<typename T>
bool ValidatingEntry<T>::onFocusOut(GdkEventFocus* event) {
  return validateValue(); //continue event handling
}

template<typename T>
T ValidatingEntry<T>::getValue() {
  return value;
}
