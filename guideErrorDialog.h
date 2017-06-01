/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef GUIDEERRORDIALOG
#define GUIDEERRORDIALOG
#include <iostream>
#include <gtkmm.h>
#include "guideMotionManagement.h"

class GuideErrorDialog : public Gtk::Dialog{
  std::vector<std::string> &em;
  Gtk::Entry dEntry;
  Gtk::VBox vBox;
  Gtk::HBox dBox;
  Gtk::HBox dbBox;
  Gtk::Image image;
  Gtk::TextView m_TextView;
  Gtk::Button nextButton;
  Gtk::Button prevButton;
  Gtk::Button okButton;
  Gtk::Button clearButton;
  Gtk::Button lastButton;
  Gtk::Button firstButton;
  Gtk::CheckButton followTick;
  Gtk::ScrolledWindow scrolledWin;
  Gtk::Label number;
  int mId;
  //helium::TsSignal<int> &updateErrorNumber;
  helium::GtkProxy<int> &errorn;
  helium::GtkExceptionHandling *eh;

 public:
  GuideErrorDialog(std::vector<std::string> &pem,helium::GtkExceptionHandling *peh,helium::GtkProxy<int> &perrorn);
  void onPrev();
  void onNext();
  void onFirst();
  void onLast();
  void onOk();
  void onClear();
  void onFollow();
  void setMessage();
  void updateMessage();
};

class ErrorDialogEH:public helium::exc::ExceptionHandler,public helium::Callback<const std::string& >{
  std::vector<std::string> errorMessages;
  GuideErrorDialog dialog;
  //Gtk::MessageDialog dialog;
  void dialogClosed(int reply);
  void operator()(const std::string& s);
  helium::GtkSignal<const std::string&,std::string,false,true> dialogDisp;

  public:
  ErrorDialogEH(Gtk::Window& powner, //helium::TsSignal<int> &puen,
		helium::GtkExceptionHandling *peh,
		helium::GtkProxy<int> &perrorn);
  GuideErrorDialog& getErrorDialog() {return dialog;}
  void handle(helium::exc::Exception& e);
  //helium::TsSignal<int> &updateErrorNumber;
  helium::GtkProxy<int> &errorn;
};






#endif
