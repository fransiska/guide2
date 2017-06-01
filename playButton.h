/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef PLAYBUTTON
#define PLAYBUTTON

#include <app/guide2/guideState.h>

template<bool ZERO>
class RobotFramePlayButton : public Gtk::EventBox {
public:
  RobotFramePlayButton(helium::GtkExceptionHandling &eh,helium::GtkProxy<helium::Connection> &pconn,helium::ValueHub<bool> &pisPlayFrame,helium::ValueHub<bool> &pediting,cfg::guide::Color &probot,cfg::guide::Color &pframe);
 Gtk::ToggleButton button;
protected:
  helium::GtkProxy<helium::Connection> &conn;
  helium::ValueHub<bool> &isPlayFrame;
  helium::ValueHub<bool> &editing;
  cfg::guide::Color &robotColor;
  cfg::guide::Color &frameColor;

  bool on_button_press_event (GdkEventButton*event);  ///< detect right click to change frame/robot, do normal otherwise
  bool onButtonPressEvent (GdkEventButton*event);  ///< detect right click to change frame/robot, do normal otherwise
  void onButtonToggled();
  virtual void onPlayToggled()=0; ///< for adding play button icon
  virtual void onPlayTooltip()=0;
  void onConnectionChange(helium::Connection conn);
  void onEditing(bool edit);
  void onRobotFrameToggle(bool frame);
  void setFramePlayButton(bool frame); ///< set button color for robot/frame
  helium::StaticConnReference<helium::Connection,RobotFramePlayButton,helium::Connection,&RobotFramePlayButton::onConnectionChange> callOnConnectionChange;
  helium::StaticConnReference<bool,RobotFramePlayButton,bool,&RobotFramePlayButton::onRobotFrameToggle> callOnRobotFrameToggle;
  helium::StaticConnReference<bool,RobotFramePlayButton,bool,&RobotFramePlayButton::onEditing> callOnEditing;
};

class PlayButton : public RobotFramePlayButton<false> {
public:
  PlayButton(helium::GtkExceptionHandling &eh,helium::GtkProxy<helium::Connection> &pconn,helium::ValueHub<bool> &pisPlayFrame,helium::ValueHub<bool> &pediting,cfg::guide::Color &probot,cfg::guide::Color &pframe);
private:
  void onPlayToggled();
  void onPlayTooltip();
};
class PlayZeroButton : public RobotFramePlayButton<true> {
public:
  PlayZeroButton(helium::GtkExceptionHandling &eh,helium::GtkProxy<helium::Connection> &pconn,helium::ValueHub<bool> &pplayFrame,helium::ValueHub<bool> &pediting,cfg::guide::Color &probot,cfg::guide::Color &pframe);
private:
  void onPlayToggled();
  void onPlayTooltip();

};

#include <app/guide2/playButton.hpp>

#endif
