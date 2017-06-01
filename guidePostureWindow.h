/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef GUIDEPOSTURELISTWINDOW
#define GUIDEPOSTURELISTWINDOW

#include <gtkmm.h>
#include <helium/signal/tsSignal.h>
//#include <helium/gtk/gtkErrorHandler.h>
#include "guideState.h"
#include "guideMotionManagement.h"
#include "robotModels.h"

class GuidePostureWindow;

/** class to make the posture capture list
 */
class PostureTreeView: public Gtk::TreeView {
  Gtk::Window &win;
  enum postureMenu{APPLYPOST,ADDPOST,DELPOST};
  friend class GuidePostureWindow;
  // Types and classes
  helium::GtkExceptionHandling *eh;
  RobotModels *rm;
  int w, h;
  std::list<RobotModels::PostureSnap> postureSnap; ///< a list of helium::Array, allowing the created Gdk::Pixbuf address to remain the same
public:
  /**
   * @param peh for exception handling
   * @param prm for getting posture capture
   * @param pw width
   * @param ph height
   */
  PostureTreeView(Gtk::Window &pwin, helium::GtkExceptionHandling *peh,RobotModels *prm, int pw, int ph);
  //virtual ~PostureTreeView();
protected:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    ModelColumns() {
      add(m_col_id);  add(m_pixbuf); add(m_col_name); //add(m_col_angle); 
    }
    Gtk::TreeModelColumn<unsigned int> m_col_id;
    Gtk::TreeModelColumn<RobotModels::PixbufPtr> m_pixbuf;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };
  ModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Gtk::Menu m_Menu_Popup;
  virtual bool on_button_press_event(GdkEventButton* event);
  void onDelete();
  void onApply();
  void onAdd();
  /**
   * @param id id to be added
   * @param name name of the posture
   * @param pose the angle of the robot
   */
  void addEntry(const int id, const std::string name, const helium::Posture pose);
  void updateId();

public:
  helium::TsSignal<void> signalAddCurPost;
  helium::TsSignal<int> signalApplyPost;
  helium::TsSignal<int> signalDeletePost;
};

class GuidePostureWindow: public Gtk::Window {
  //GuidePostureList &gpl;
  //cfg::guide::Postures &pl;
  //helium::GtkExceptionHandling *eh;
  GuideMotionManagement &gmm;
  //cfg::guide::Postures postures;

  //helium::Array<helium::Posture> postures;

  int defw; int defh;
  //PostureCaptureWindow *pcw;
public:
  enum ActionType{OPEN,SAVEAS};
  GuidePostureWindow(GuideMotionManagement &pgmm,RobotModels *prm);
  //virtual ~GuidePostureWindow();
  helium::TsSignal<void> signalAddCurPost;
  helium::TsSignal<int> signalApplyPost;
  void addCurPosture();
  void applyPosture(int i);
  void addEntry(const int id, const std::string name, const helium::Posture angle, const unsigned char* pose, int w, int h, int r);
  void addEntry(const int id);
  void deleteEntry(const int id);
  void onRealize();
  void enableApplyMenu(bool b);
  void savePostures(std::string f);
  void loadPostures(std::string f, cfg::guide::Postures &pl);
  void appendPostures(cfg::guide::Postures &pl);

protected:
  virtual void on_button_quit() {hide();}
  void onFileAction(ActionType action);
  void clearTree();
  void reloadTree();

  //Child widgets:
  Gtk::VBox m_VBox;

  Gtk::ScrolledWindow m_ScrolledWindow;
  PostureTreeView m_TreeView;

  Gtk::HButtonBox m_ButtonBox;
  Gtk::Button m_Button_Quit;
  Gtk::Button m_Button_Open;
  Gtk::Button m_Button_Save;

  helium::InternalConnReference<void> addCurPostureConn;
  helium::InternalConnReference<int> applyPostureConn;
  helium::InternalConnReference<int> callDeleteEntry;
};

#endif
