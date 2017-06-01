/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.06.19
 * \version 0.1
 * Release_flags g
 */

#ifndef ICONLABELBUTTON
#define ICONLABELBUTTON

#include <gtkmm.h>

/*! ToolButton with Icon and Text
 */
class IconLabelToolButton : public Gtk::ToolButton {
  Gtk::VBox box;
  Gtk::Image icon;
  Gtk::Label label;

public:
  IconLabelToolButton(Gtk::StockID pactiveIcon, std::string pactiveLabel);
};


/*! ToggleToolButton with an icon and text at bottom
  different icon and text for different toggled state
 */
class IconLabelToggleToolButton : public Gtk::ToggleToolButton {
  Gtk::VBox box;
  Gtk::Image activeIcon,inactiveIcon;
  Gtk::Label activeLabel,inactiveLabel;

public:
  IconLabelToggleToolButton(Gtk::StockID pactiveIcon, std::string pactiveLabel, Gtk::StockID pinactiveIcon, std::string pinactiveLabel);
  void set_active(bool active);
  void on_toggled();
};



#endif
