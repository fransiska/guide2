/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "sensorCalibMenu.h"

SensorCalibMenu::SensorCalibMenu()
{
}

void SensorCalibMenu::addTo(Gtk::Window* win, Gtk::VBox& c){
  menubar_action = Gtk::ActionGroup::create();
  menubar_action->add(Gtk::Action::create("MenuFile","_File"));
  menubar_action->add(Gtk::Action::create("Save",Gtk::Stock::SAVE),
  		      sigc::mem_fun(*this,&SensorCalibMenu::onFileSave));
  menubar_action->add(Gtk::Action::create("Close",Gtk::Stock::QUIT),
		      sigc::mem_fun(*this,&SensorCalibMenu::onFileQuit));

  menubar_uimanager = Gtk::UIManager::create();
  menubar_uimanager->insert_action_group(menubar_action);

  //add keyboard shortcut
  win->add_accel_group(menubar_uimanager->get_accel_group());

  Glib::ustring ui_info =
    "<ui>"
      "<menubar name='MenuBar'>"
        "<menu action='MenuFile'>"
          "<menuitem action='Save' />"
          "<menuitem action='Quit' />"
        "</menu>"
      "</menubar>"
    "</ui>";
  menubar_uimanager->add_ui_from_string(ui_info);
  menubar = menubar_uimanager->get_widget("/MenuBar");
  c.pack_start(*menubar,Gtk::PACK_SHRINK);
}

void SensorCalibMenu::onFileSave() {
  /* //TODO DEBUG
#ifndef SENSORXML 
  scm->storeCalibration(min,max);
#else
  scm->storeCalibration();
#endif
  */
}

void SensorCalibMenu::onFileQuit() {
  exit(1);
}
