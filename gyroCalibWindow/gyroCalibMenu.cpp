#include "gyroCalibWindow.h"
#include "gyroCalibMenu.h"
#include <gtkmm/stock.h>
#include <iostream>
#include <fstream>
#include <sstream>

GyroCalibMenu::GyroCalibMenu(GyroCalib* pgyroCalib, std::string pcalibFile, GyroCalibWindow* pwin) :
  gyroCalib(pgyroCalib),calibFile(pcalibFile),win(pwin) {
}

GyroCalibMenu::~GyroCalibMenu() {
}

void GyroCalibMenu::addTo(Gtk::Window* win, Gtk::VBox& c){
  menubar_action = Gtk::ActionGroup::create();
  menubar_action->add(Gtk::Action::create("MenuFile","_File"));
  menubar_action->add(Gtk::Action::create("Open",Gtk::Stock::OPEN),
  		      sigc::mem_fun(*this,&GyroCalibMenu::onFileOpen));
  menubar_action->add(Gtk::Action::create("Save",Gtk::Stock::SAVE),
  		      sigc::mem_fun(*this,&GyroCalibMenu::onFileSave));
  menubar_action->add(Gtk::Action::create("SaveAs",Gtk::Stock::SAVE_AS),
  		      sigc::mem_fun(*this,&GyroCalibMenu::onFileSaveAs));
  menubar_action->add(Gtk::Action::create("Quit",Gtk::Stock::QUIT),
		      sigc::mem_fun(*this,&GyroCalibMenu::onFileQuit));

  menubar_uimanager = Gtk::UIManager::create();
  menubar_uimanager->insert_action_group(menubar_action);

  //add keyboard shortcut
  win->add_accel_group(menubar_uimanager->get_accel_group());

  Glib::ustring ui_info =
    "<ui>"
      "<menubar name='MenuBar'>"
        "<menu action='MenuFile'>"
          "<menuitem action='Open' />"
          "<menuitem action='Save' />"
          "<menuitem action='SaveAs' />"
          "<menuitem action='Quit' />"
        "</menu>"
      "</menubar>"
    "</ui>";
  menubar_uimanager->add_ui_from_string(ui_info);
  menubar = menubar_uimanager->get_widget("/MenuBar");
  c.pack_start(*menubar,Gtk::PACK_SHRINK);
}

void GyroCalibMenu::onFileOpen() {
  file_chooser = new Gtk::FileChooserDialog(*win, "FileChoose", Gtk::FILE_CHOOSER_ACTION_OPEN);
  file_chooser->add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
  file_chooser->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  //Add filters, so that only certain file types can be selected:
  Gtk::FileFilter filter_text;
  filter_text.set_name("Text files");
  filter_text.add_mime_type("text/plain");
  file_chooser->add_filter(filter_text);

  file_chooser->run();
  calibFile = file_chooser->get_filename();

  double c[5];
  std::string s;
  std::fstream in (calibFile.c_str(),std::fstream::in);
  getline(in,s);
  in.close();
  std::stringstream ss;
  ss << s;

  ss >> c[0] >> c[1] >> c[2] >> c[3] >> c[4] ;
  win->setCurrentLabel(c);

  delete file_chooser;
}

void GyroCalibMenu::onFileSave() {
  win->saveSelectedCalib(calibFile);
}

void GyroCalibMenu::onFileSaveAs() {
  file_chooser = new Gtk::FileChooserDialog(*win, "FileSaveAs", Gtk::FILE_CHOOSER_ACTION_SAVE);
  file_chooser->add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
  file_chooser->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  //Add filters, so that only certain file types can be selected:
  Gtk::FileFilter filter_text;
  filter_text.set_name("Text files");
  filter_text.add_mime_type("text/plain");
  file_chooser->add_filter(filter_text);

  file_chooser->set_do_overwrite_confirmation(true);
  int result = file_chooser->run();
  if(result==Gtk::RESPONSE_OK) {
    calibFile = file_chooser->get_filename();
    win->saveSelectedCalib(calibFile);
  }

  delete file_chooser;
}

void GyroCalibMenu::onFileQuit() {
  exit(1);
}
