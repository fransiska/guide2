/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include <algorithm>

template<bool ZERO>
RobotFramePlayButton<ZERO>::RobotFramePlayButton(helium::GtkExceptionHandling &eh,helium::GtkProxy<helium::Connection> &pconn, helium::ValueHub<bool> &pisPlayFrame,helium::ValueHub<bool> &pediting,cfg::guide::Color &probot,cfg::guide::Color &pframe):
  conn(pconn),
  isPlayFrame(pisPlayFrame),
  editing(pediting),
  robotColor(probot),frameColor(pframe),
  callOnConnectionChange(this),
  callOnRobotFrameToggle(this),
  callOnEditing(this)
{
  add(button);
  add_events(Gdk::BUTTON_PRESS_MASK);
  set_above_child(false);
  signal_button_press_event().connect(eh.wrap(*this,&RobotFramePlayButton::on_button_press_event));
  button.signal_button_press_event().connect(eh.wrap(*this,&RobotFramePlayButton::onButtonPressEvent),false);
  button.signal_toggled().connect(eh.wrap(*this,&RobotFramePlayButton::onButtonToggled));
  conn.connect(callOnConnectionChange);
  isPlayFrame.connect(callOnRobotFrameToggle);
  editing.connect(callOnEditing);

  button.set_has_tooltip();
}

/// eventbox press event
template<bool ZERO>
bool RobotFramePlayButton<ZERO>::on_button_press_event (GdkEventButton*event){
  //std::cout << "on_button_press_event"<< button.get_active() << std::endl;
  if(event->button == 3 && !editing.get()) {
    isPlayFrame = !isPlayFrame;
    return true;
  }
  //std::cout << "_returning false" << std::endl;
  return false;
}

/// button press event
template<bool ZERO>
bool RobotFramePlayButton<ZERO>::onButtonPressEvent (GdkEventButton*event){
  //std::cout << "onbuttonpressevent"<< button.get_active() << std::endl;
  if(event->button == 3 && !button.get_active() && !editing.get()) {
    isPlayFrame = !isPlayFrame;
    //std::cout << "returning true" << std::endl;
    return true;
  }
  //std::cout << "returning false" << std::endl;
  return false;
}


template<bool ZERO>
void RobotFramePlayButton<ZERO>::onButtonToggled(){
  if(button.get_active()) {
    Gtk::Widget *stopS;
    stopS = Gtk::manage (new Gtk::Image (Gtk::Stock::MEDIA_STOP, Gtk::ICON_SIZE_SMALL_TOOLBAR)); 
    stopS->show();
    button.remove();
    button.add(*stopS);
  }
  else
    onPlayToggled();
  onPlayTooltip();
}

template<bool ZERO>
void RobotFramePlayButton<ZERO>::onConnectionChange(helium::Connection connection) {
  if(editing.get() || (!isPlayFrame.get() && connection.state != helium::Connection::CONNECTED)) {
    button.set_sensitive(false);
    set_above_child(true);    
  }
  else { 
    button.set_sensitive(true);
    set_above_child(false);
  }
  setFramePlayButton(isPlayFrame.get());
}

template<bool ZERO>
void RobotFramePlayButton<ZERO>::onEditing(bool edit) {
  if(edit || (!isPlayFrame && conn.get().state != helium::Connection::CONNECTED)) {
    button.set_sensitive(false);
    set_above_child(true);    
  }
  else { 
    button.set_sensitive(true);
    set_above_child(false);
  }
}

template<bool ZERO>
void RobotFramePlayButton<ZERO>::onRobotFrameToggle(bool frame) {
  setFramePlayButton(frame);
  onConnectionChange(conn.get());
  onPlayTooltip();
}

template<bool ZERO>
void RobotFramePlayButton<ZERO>::setFramePlayButton(bool frame) {
  if(frame) {
    Gdk::Color color;
    color.set_rgb_p(frameColor[0],frameColor[1],frameColor[2]);
    button.modify_bg(Gtk::STATE_NORMAL,color);

    color.set_rgb_p(std::min(1.0,frameColor[0]*1.3),std::min(1.0,frameColor[1]*1.3),std::min(1.0,frameColor[2]*1.3));
    button.modify_bg(Gtk::STATE_PRELIGHT,color);

    color.set_rgb_p(frameColor[0]*0.7,frameColor[1]*0.7,frameColor[2]*0.7);
    button.modify_bg(Gtk::STATE_ACTIVE,color);
    button.modify_bg(Gtk::STATE_SELECTED,color);
  }
  else {
    Gdk::Color color;
    color.set_rgb_p(robotColor[0],robotColor[1],robotColor[2]);
    button.modify_bg(Gtk::STATE_NORMAL,color);

    color.set_rgb_p(std::min(1.0,robotColor[0]*1.3),std::min(1.0,robotColor[1]*1.3),std::min(1.0,robotColor[2]*1.3));
    button.modify_bg(Gtk::STATE_PRELIGHT,color);

    color.set_rgb_p(robotColor[0]*0.7,robotColor[1]*0.7,robotColor[2]*0.7);
    button.modify_bg(Gtk::STATE_ACTIVE,color);
    button.modify_bg(Gtk::STATE_SELECTED,color);
  }
}
