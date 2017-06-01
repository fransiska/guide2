/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GYROCALIBWINDOW
#define GYROCALIBWINDOW

//#include "gyroCalibMenu.h"
//≈#include <guiObjects/guiConnectionLine.h>

#include <gtkmm.h>
#include <vector>

#include "gyroCalib.h"
#include "gyroCalibBox.h"



class GyroCalibWindow : public Gtk::Window {
  GyroCalib gyroCalib;
  cfg::guide::Joints &joints;
  cfg::guide::Gyro &gyro;
  helium::GtkProxy<helium::Connection> &conn;
  helium::GtkExceptionHandling *eh;
  GyroCalibBox gBox;

  Gtk::AspectFrame frame_gl; //for gtkgl

  //GyroCalibMenu gyroCalibMenu;
 public:

  Gtk::VBox boxv_main;

 protected:
  int radio;
  std::vector<std::pair<int,std::string> > check;
  std::vector<std::vector<double> > meanData;
  const std::string &calibFile;
  Gtk::FileChooserDialog* file_chooser;
  bool all;
  //double data[SETS]; //for passing to save calib data
  helium::Array<std::vector<helium::Array<double,3> > ,SETS> dataarray;
  //helium::Array<helium::Array<double>,SETS> dataarrayh;
  int dataarraysize[SETS];
  bool gyroConnected;
  bool visible;
  bool powerClicked;
  bool monitorModeClicked;
  helium::ValueHub<bool> acquire;

 public:
  void update_radio_status();
  void update_check_status();
  void update_calc_status();
  void update_label();

  /**
   * replaces selected radio button entry with the one on the file
   */
  void onOpen();
  /**
   * calls saveSelectedCalib with default name specified in guide config
   * @see saveSelectedCalib()
   */
  void onSave();
  /**
   * calls saveSelectedCalib with specified name
   * @see saveSelectedCalib()
   */
  void onSaveAs();
  void calc();
  void onCalcAll();
  void onCalcOne();
  void toggleAcquire(); ///< connected to menu button
  void onAcquire(bool b);
  /**
   * set selected calib as current calib
   * WARNING! calib is not automatically saved
   */
  void onUse();
  void onMean();
  void onQuit();

  void onShowWindow();
  void onHideWindow();
  void gyroDataConnect(bool b);

  void onPowerClick();
  void onMonitorModeClick();

  //moved to main: GuiConnectionLine guiConnectionLine;

  GyroCalibWindow(//int* argc,char** argv
		  cfg::guide::Joints &joints,
		  cfg::guide::Gyro &gyro,
		  helium::GtkProxy<helium::Connection> &pconn,
		  const std::string &pcalibFile,
		  cfg::guide::Colors &pcolors,
		  helium::GtkExceptionHandling *peh, 
		  int pdesample=1,
		  RobotModels::ModelArea *pma=NULL);

  void setCurrentLabel(double* c);
  /**
   * set selected calib as current calib
   * saves current calib to specified file
   */
  void saveSelectedCalib(std::string filename);
  void setPower(bool state);
  void setMonitorMode(bool b);
  void onPowerChange();

  void onConnection(const helium::Connection &c);
  void onStateChange(const std::pair<int,helium::HwState>& hwpair);
  
  void onMonitorModeChange();
 
  helium::StaticConnReference<bool,GyroCalibWindow,bool,&GyroCalibWindow::onAcquire> callOnAcquire;

 

struct Callers;
std::auto_ptr<Callers> callers;

};

#endif //GYROCALIBWINDOW
