/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef GUIDEDEBUGWINDOW
#define GUIDEDEBUGWINDOW

#include <gtkmm.h>
#include <helium/system/cell.h>
#include <helium/gtk/gtkExceptionHandling.h>

class GuideDebugWindow: public Gtk::Window {
  helium::GtkExceptionHandling *eh;
 public:
  GuideDebugWindow(helium::Memory &pmem, helium::GtkExceptionHandling *geh);
protected:
  //Signal handlers:
  virtual void on_button_quit() {hide();}
  bool on_expand_row(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path);
  void fillCells(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path, bool print=false);

  //editable validating cells
  virtual void treeviewcolumn_validated_on_cell_data(Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);
  virtual void cellrenderer_validated_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text);

  helium::IDNode<helium::Cell>* getCellFromPath(const Glib::ustring &path);

  //Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns()
    { add(colName);
      //add(colVal);
      add(colValValid); }

    Gtk::TreeModelColumn<Glib::ustring> colName;
    //Gtk::TreeModelColumn<Glib::ustring> colVal;
    Gtk::TreeModelColumn<Glib::ustring> colValValid;
  };

  helium::Memory &mem;

  ModelColumns modelColumns;

  //Child widgets:
  Gtk::VBox vBox;

  Gtk::ScrolledWindow scrolledWindow;
  Gtk::TreeView treeView;
  Glib::RefPtr<Gtk::TreeStore> treeModel;

  Gtk::HBox buttonBox;
  Gtk::Entry commandLine;
  Gtk::Button buttonQuit;

  //For the validated column:
  Gtk::CellRendererText cellRenderValid;
  Gtk::TreeView::Column columnValid;
  bool validateRetry;
  Glib::ustring invalidRetry;

  Gtk::Menu treeMenu;
  void refreshThis();
  void refreshRecursive();
  void refreshSingleCell(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path);
  std::string printCell(helium::Cell* c, bool printValue=false);
  void getSelectionPath(Gtk::TreeModel::iterator& iter, Gtk::TreeModel::Path& path);
  void onMouseClick (GdkEventButton* event);
};

#endif
