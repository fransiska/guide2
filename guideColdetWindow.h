/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDECOLDETWINDOW
#define GUIDECOLDETWINDOW
#include <gtkmm.h>
#include "guideState.h"
//#include "guideMotionManagement.h"
#include "coldetManager.h"



class GuideColdetWindow : public Gtk::Window {
  cfg::guide::Joints &joints;
  cfg::guide::Coldet &coldet;
  coldet::ColdetManager &coldetm;
  helium::GtkExceptionHandling *eh;

  struct CpuEnum {
    typedef enum {IDLE,BUSY} Type;
    static const char* strings[];
  };
  const std::string &filename;

  Gtk::RadioButton rbutton[3];

  Gtk::SpinButton entry_cpu_idle; ///< update current idle cpu number here
  Gtk::SpinButton entry_cpu_busy; ///< update current busy cpu number here
  Gtk::Label label_cpu_idle; ///< label for idle cpu
  Gtk::Label label_cpu_busy; ///< label for busy cpu
  Gtk::Label label_default; ///< label to show the default value
  Gtk::Button button_default; ///< button to reset to default

  class TreeView: public Gtk::TreeView {

  public:
    TreeView();

    class ModelColumns : public Gtk::TreeModelColumnRecord {
    public:
      ModelColumns(){ 
	add(m_col_start); 
	add(m_col_end);
	add(m_col_icon); 
	add(m_col_state); 
      }
      
      Gtk::TreeModelColumn<helium::Time> m_col_start;
      Gtk::TreeModelColumn<helium::Time> m_col_end;
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > m_col_icon; 
      Gtk::TreeModelColumn<std::string> m_col_state;  
    };
    ModelColumns modelColumns;
    Glib::RefPtr<Gtk::ListStore> listStore;


  };

  TreeView treeView;

  //wrapper
  Gtk::VButtonBox vbbox_idle, vbbox_busy; ///< vbbox for aligning
  Gtk::HBox hbox_outer;///< box to wrap vbox setting
  Gtk::HBox hbox_turbo;
  Gtk::VBox vbox_turbo;
  Gtk::VBox vbox_cpu;///< box to wrap cpu setting
  Gtk::Frame frame_cpu,frame_turbo; ///< frame for cpu setting

  Gtk::ScrolledWindow scrolledWindow ; ///< scroll window for tree view
  Gtk::VBox vbox_main; ///< box to wrap buttons and entries

  void updateCurCpu(CpuEnum::Type t); ///< update current cpu number
  void save(); ///< saving the current cpu number into xml
  void setToDefault(); ///< set both the current cpu number to default (by setting the entry, which then trigger updateCurCpu()
  void updateSegment(const coldet::SegmentNotification& s); ///< update the treewindow to be filled in with the new information of segment
  void setTurbo(int id); ///< calls coldetManager::setTurbo()

public:
  GuideColdetWindow(cfg::guide::Joints &pjoints, cfg::guide::Coldet &pcoldet,coldet::ColdetManager &pcoldetm,helium::GtkExceptionHandling *peh,const std::string& pfilename);

private:
  
  ///for giving feedback of calc
  helium::StaticConnReference<coldet::SegmentNotification,GuideColdetWindow,const coldet::SegmentNotification&,&GuideColdetWindow::updateSegment> callUpdateSegment;

  void onEnableColdet(bool enable);
  helium::StaticConnReference<bool,GuideColdetWindow,bool,&GuideColdetWindow::onEnableColdet> callOnEnableColdet;

};
#endif
