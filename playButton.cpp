/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#include <app/guide2/playButton.h>

PlayButton::PlayButton(helium::GtkExceptionHandling &eh,helium::GtkProxy<helium::Connection> &pconn,helium::ValueHub<bool> &pplayFrame,helium::ValueHub<bool> &pediting,cfg::guide::Color &probot,cfg::guide::Color &pframe):
  RobotFramePlayButton<false>(eh,pconn,pplayFrame,pediting,probot,pframe)
{
  onPlayToggled();
}

PlayZeroButton::PlayZeroButton(helium::GtkExceptionHandling &eh,helium::GtkProxy<helium::Connection> &pconn,helium::ValueHub<bool> &pplayFrame,helium::ValueHub<bool> &pediting,cfg::guide::Color &probot,cfg::guide::Color &pframe):
  RobotFramePlayButton<true>(eh,pconn,pplayFrame,pediting,probot,pframe)
{
  onPlayToggled();
}

void PlayButton::onPlayTooltip(){
  if(button.get_active())
    button.set_tooltip_markup("Stop playing motion");
  else {
    if(isPlayFrame.get())
      button.set_tooltip_markup("Play on model from current frame");
    else
      button.set_tooltip_markup("Play on robot from current frame");
  }
}
void PlayZeroButton::onPlayTooltip(){
  if(button.get_active())
    button.set_tooltip_markup("Stop playing motion");
  else {
    if(isPlayFrame.get())
      button.set_tooltip_markup("Play on model from frame 0");
    else
      button.set_tooltip_markup("Play on robot from frame 0");
  }
}

void PlayButton::onPlayToggled() {
  Gtk::Widget *playS;
  playS = Gtk::manage (new Gtk::Image (Gtk::Stock::MEDIA_PLAY, Gtk::ICON_SIZE_SMALL_TOOLBAR)); 
  playS->show(); 
  button.remove();
  button.add (*playS); 
  if(isPlayFrame.get())
    button.set_tooltip_markup("Play on model from current frame");
  else
    button.set_tooltip_markup("Play on robot from current frame");

}
void PlayZeroButton::onPlayToggled() {
  Gtk::Widget *playZS;
  Gtk::HBox *containerZero;
  Gtk::Label *labelZero;
  playZS = Gtk::manage (new Gtk::Image (Gtk::Stock::MEDIA_PLAY, Gtk::ICON_SIZE_SMALL_TOOLBAR)); 
  playZS->show(); 
  labelZero = Gtk::manage( new Gtk::Label() ); 
  labelZero->set_label("0");
  labelZero->show();
  containerZero = Gtk::manage( new Gtk::HBox(false, 0) ); 
  containerZero->add( *labelZero ); 
  containerZero->add( *playZS ); 
  containerZero->show();
  button.remove();
  button.add(*containerZero);
    if(isPlayFrame.get())
      button.set_tooltip_markup("Play on model from frame 0");
    else
      button.set_tooltip_markup("Play on robot from frame 0");
}

