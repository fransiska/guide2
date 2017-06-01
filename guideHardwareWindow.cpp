
/// DEPRECATED
#include "guideHardwareWindow.h"
#include <iostream>
using std::cout;
using std::endl;

GuideHardwareWindow::GuideHardwareWindow(cfg::guide::Sensors &psensors):
  sensors(psensors)
{
  set_title("Hardware Debug Window");
  set_border_width(5);
  set_default_size(400, 200);
  m_TextView.get_buffer()->set_text("");
  m_TextView.set_buffer(m_TextView.get_buffer());
  m_TextView.set_editable(false);
  m_ScrolledWindow.add(m_TextView);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  add(m_ScrolledWindow);

  show_all_children();
}

void GuideHardwareWindow::on_show() {
  //update text
  cout << "sensor status "<< sensors.getFaultySensorsString() << endl;
  m_TextView.get_buffer()->set_text(sensors.getFaultySensorsString());
  Gtk::Widget::on_show();
}
