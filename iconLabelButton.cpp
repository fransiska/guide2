#include "iconLabelButton.h"
#include <iostream>

IconLabelToggleToolButton::IconLabelToggleToolButton(Gtk::StockID pactiveIcon, std::string pactiveLabel,
						     Gtk::StockID pinactiveIcon, std::string pinactiveLabel) :
  activeIcon   (pactiveIcon, Gtk::ICON_SIZE_LARGE_TOOLBAR),  
  inactiveIcon (pinactiveIcon, Gtk::ICON_SIZE_LARGE_TOOLBAR),
  activeLabel  (pactiveLabel),
  inactiveLabel (pinactiveLabel)
{
  box.pack_start(activeIcon);
  box.pack_start(activeLabel);
  box.pack_start(inactiveIcon);
  box.pack_start(inactiveLabel);
  set_icon_widget(box);
}

void IconLabelToggleToolButton::on_toggled() {
  if(get_active()) {
    inactiveIcon.hide();
    inactiveLabel.hide();
    activeIcon.show();
    activeLabel.show();
  }
  else {
    activeIcon.hide();
    activeLabel.hide();
    inactiveIcon.show();
    inactiveLabel.show();
  }
}

/// using this instead of Gtk::ToggleToolButton::set_active because it doesn't check for sensitive-ness
void IconLabelToggleToolButton::set_active(bool active) {
  if(get_sensitive()) {
    Gtk::ToggleToolButton::set_active(active);
  }
}

IconLabelToolButton::IconLabelToolButton(Gtk::StockID picon, std::string plabel) :
  icon  (picon, Gtk::ICON_SIZE_LARGE_TOOLBAR),  
  label (plabel)
{
  box.pack_start(icon);
  box.pack_start(label);
  set_icon_widget(box);
}

