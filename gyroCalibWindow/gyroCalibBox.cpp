/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "gyroCalibBox.h"
#include <iostream>

using namespace std;

GyroCalibBox::GyroCalibBox(Gtk::Window *pwin):
  win(pwin),
  c_power("Power"),
  c_mmode("Ignore Error")
{
  //Create actions for menus and toolbars:
  m_refActionGroup = Gtk::ActionGroup::create();

  //toolbar button
  m_refActionGroup->add(Gtk::Action::create("Open", Gtk::Stock::OPEN,"_Open"));
  m_refActionGroup->add(Gtk::Action::create("Save", Gtk::Stock::SAVE,"_Save"));
  m_refActionGroup->add(Gtk::Action::create("SaveAs", Gtk::Stock::SAVE_AS,"_Save As"));
  m_refActionGroup->add(Gtk::Action::create("Acquire", Gtk::Stock::CONNECT,"_Acquire"));
  m_refActionGroup->add(Gtk::Action::create("Calc", Gtk::Stock::EXECUTE,"_Calc"));
  m_refActionGroup->add(Gtk::Action::create("CalcAll", Gtk::Stock::EXECUTE,"Ca_lc All"));
  m_refActionGroup->add(Gtk::Action::create("Mean", Gtk::Stock::ABOUT,"_Mean"));
  m_refActionGroup->add(Gtk::Action::create("Use", Gtk::Stock::HARDDISK,"_Use"));
  m_refActionGroup->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT,"_Close"));

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);
  win->add_accel_group(m_refUIManager->get_accel_group());

  Glib::ustring ui_info = 
    "<ui>"
    "  <toolbar name='ToolBar'>"
    "    <toolitem action='Open'/>"
    "    <toolitem action='Save'/>"
    "    <toolitem action='SaveAs'/>"
    "      <separator/>"
    "    <toolitem action='Acquire'/>"
    "      <separator/>"
    "    <toolitem action='Calc'/>"
    "    <toolitem action='CalcAll'/>"
    "    <toolitem action='Mean'/>"
    "      <separator/>"
    "    <toolitem action='Use'/>"
    "      <separator/>"
    "    <toolitem action='Quit'/>"
    "  </toolbar>"
    "</ui>";

  try { m_refUIManager->add_ui_from_string(ui_info); }
  catch(const Glib::Error& ex) { std::cerr << "building menus failed: " <<  ex.what(); }

  Gtk::Toolbar* pToolbar = dynamic_cast<Gtk::Toolbar*>(m_refUIManager->get_widget("/ToolBar")) ;
  if(pToolbar) {
    boxv_main.pack_start(*pToolbar, Gtk::PACK_SHRINK);
    pToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH);
  }
  else 
    cerr << "error: GyroCalib no toolbar" << endl;

  //data,entry,label,mean
  boxv_main.pack_start(separatorh1, Gtk::PACK_SHRINK);
  Gtk::RadioButton::Group group = r_button[0].get_group();
  for(int i=1;i<SETS+1;i++)
    r_button[i].set_group(group);  
  
  for(int i=0;i<=SETS;i++) {
    if(i==SETS)
      boxv_main.pack_start(separatorh2, Gtk::PACK_SHRINK);
    boxh[i].pack_start(r_button[i],Gtk::PACK_SHRINK,5); //radio
    entry[i].set_width_chars(45);    
    boxh[i].pack_start(entry[i],Gtk::PACK_SHRINK,5);    //entry
    label[i].set_width_chars(50);
    boxh[i].pack_start(label[i],Gtk::PACK_SHRINK,5);    //label
    if(i<SETS)
      boxh[i].pack_end(c_button[i], Gtk::PACK_SHRINK); //check
    boxv_main.pack_start(boxh[i], Gtk::PACK_SHRINK);
  }
  boxv_main.pack_start(separatorh3, Gtk::PACK_SHRINK);

  //status bar
  boxh_status.pack_start(l_data, Gtk::PACK_SHRINK, 10);
  boxh_status.pack_end(c_power, Gtk::PACK_SHRINK);
  boxh_status.pack_end(c_mmode, Gtk::PACK_SHRINK);
  boxv_main.pack_start(boxh_status, Gtk::PACK_SHRINK);

  add(boxv_main);
  show_all_children();
}
