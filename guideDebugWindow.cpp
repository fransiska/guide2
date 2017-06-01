/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "guideDebugWindow.h"
#include <helium/system/basicCell.h>
#include <helium/system/valueProxy.h>
#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
using std::stringstream;

GuideDebugWindow::GuideDebugWindow(helium::Memory &pmem, helium::GtkExceptionHandling *peh)://GuideExceptionHandler *peh): 
  eh(peh),
  mem(pmem),
  buttonQuit(Gtk::Stock::QUIT)
{
  set_title("Guide Debug Window");
  set_border_width(5);
  set_default_size(400, 200);

  //packs
  add(vBox);
  scrolledWindow.add(treeView);
  scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  vBox.pack_start(scrolledWindow, Gtk::PACK_EXPAND_WIDGET);
  vBox.pack_start(buttonBox, Gtk::PACK_SHRINK);
  buttonBox.pack_start(commandLine, Gtk::PACK_EXPAND_WIDGET);
  buttonBox.pack_start(buttonQuit, Gtk::PACK_SHRINK);
  buttonBox.set_border_width(5);
  buttonQuit.signal_clicked().connect(eh->wrap(*this,&GuideDebugWindow::on_button_quit) );
  commandLine.set_max_length(0);

  //Create the Tree model:
  treeModel = Gtk::TreeStore::create(modelColumns);
  treeView.set_model(treeModel);

  //All the items NOT to be reordered with drag-and-drop:
  treeView.set_reorderable(false);



  //Fill the TreeView's model
  Gtk::TreeModel::Row row = *(treeModel->append());
  //row[modelColumns.colVal] = "";
  row[modelColumns.colValValid] = "";
  row[modelColumns.colName] = "Guide Memory";

  Gtk::TreeModel::Row childrow = *(treeModel->append(row.children()));
  //childrow[modelColumns.colVal] = "0";
  row[modelColumns.colValValid] = "0";
  childrow[modelColumns.colName] = "Fake Row";



  //Add the TreeView's view columns:
  treeView.append_column("Item", modelColumns.colName);
  //treeView.append_column("Value", modelColumns.colVal);
  //For this column, we create the CellRenderer ourselves, and connect our own
  //signal handlers, so that we can validate the data that the user enters, and
  //control how it is displayed.
  columnValid.set_title("Value");
  columnValid.pack_start(cellRenderValid);
  treeView.append_column(columnValid);  
  //Tell the view column how to render the model values:
  columnValid.set_cell_data_func(cellRenderValid, eh->wrap(*this, &GuideDebugWindow::treeviewcolumn_validated_on_cell_data) );

  //Make the CellRenderer editable, and handle its editing signals:
#ifdef GLIBMM_PROPERTIES_ENABLED
  cellRenderValid.property_editable() = true;
#else
  cellRenderValid.set_property("editable", true);
#endif

  cellRenderValid.signal_edited().connect(eh->wrap(*this, &GuideDebugWindow::cellrenderer_validated_on_edited) );


  //signals
  treeView.signal_test_expand_row().connect(eh->wrap(*this,&GuideDebugWindow::on_expand_row));

  //right click menu
  {
    Gtk::Menu::MenuList& menulist = treeMenu.items();
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("Refresh _This",eh->wrap(*this,&GuideDebugWindow::refreshThis)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("Refresh _Recursive",eh->wrap(*this,&GuideDebugWindow::refreshRecursive)));
  }
  treeMenu.accelerate(*this);

  treeView.signal_button_press_event().connect_notify(eh->wrap(*this,&GuideDebugWindow::onMouseClick));

  show_all_children();
}

void GuideDebugWindow::getSelectionPath(Gtk::TreeModel::iterator &iter, Gtk::TreeModel::Path& path) {
  Glib::RefPtr<Gtk::TreeView::Selection> refSelection = treeView.get_selection(); //get selected row
  iter = refSelection->get_selected();

  if(refSelection) { //if selected
    std::vector<Gtk::TreeModel::Path> pathlist = refSelection->get_selected_rows(); //changes to path
    if(pathlist.size())
      path = pathlist[0]; 
  }
}

//popup menu
void GuideDebugWindow::refreshThis() {
  Gtk::TreeModel::iterator iter;
  Gtk::TreeModel::Path path;
  getSelectionPath(iter,path);
  if(path.to_string().size()<1) return;

  refreshSingleCell(iter,path);
}//refreshThis

void GuideDebugWindow::refreshRecursive() {
  //cout << "recursive" << endl;
  Gtk::TreeModel::iterator iter;
  Gtk::TreeModel::Path path;
  getSelectionPath(iter,path);
  if(path.to_string().size()<1) return;

  treeView.expand_row(path,false);  

  helium::IDNode<helium::Cell> *m = getCellFromPath(path.to_string());
  //std::cout << m->childrenNum() << " " << path.to_string()<< " "<<(*iter)[modelColumns.colName] << std::endl;
  iter = treeModel->get_iter(path)->children().begin();

  if(iter) {
    path.down();
    //std::cout << m->childrenNum() << " " << path.to_string()<< " "<<(*iter)[modelColumns.colName] << std::endl;
    //std::cout << m->childrenNum() << " " << path.to_string()<< " "<<(*iter)[modelColumns.colName] << std::endl;
    for(size_t i=0;i<m->childrenNum()-1;i++) {
      //std::cout << path.to_string() << " "<<(*iter)[modelColumns.colValName] << std::endl;
      refreshSingleCell(iter,path); 
      iter++;
      path.next();
    }
    refreshSingleCell(iter,path); 
  }

}

void GuideDebugWindow::refreshSingleCell(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path) {
  helium::IDNode<helium::Cell> *m = getCellFromPath(path.to_string()); //get the cell pointer from path 0:0:0 format
  helium::Cell* c = m->getDataP();
  (*iter)[modelColumns.colValValid] = printCell(c,true);
}

std::string GuideDebugWindow::printCell(helium::Cell* c, bool printValue) {
  std::string val="";
  if(c) {
    bool hasGet=false;
    //std::cout<<"here "<<path<<std::endl;
    
    if (helium::isCellFlagSet(c,helium::CELLHOLDER)){
      c=static_cast<helium::CellHolder*>(c)->getRealCell();
    }
    if (helium::isCellFlagSet(c,helium::VALUEPROXY)){
      helium::GenericValueProxy* vp=
	static_cast<helium::GenericValueProxy*>(c);
      if (vp->isConnected()){
	val = "=>";	 
	hasGet=vp->getRemoteCapabilities()&helium::CELLGETS;
	//std::cout<<path<<" is "<<hasGet<<std::endl;
      }else{
      val = "->";	  
      }
    }else{
      hasGet=hasCapability(c,helium::CELLGETS);
      //std::cout<<path<<" (is) "<<hasGet<<std::endl;
    }
    if (helium::isCellFlagSet(c,helium::TYPELESSREMOTECELL)){
      val+="[Remote]";
    }
    
    if (hasGet){
      stringstream sp;
      try{
	if(printValue) c->print(sp);
	else sp<<"[value]";
      }catch(helium::exc::Exception& e){
	val+=e.what();
	eh->handle(e);	  
    }
      val += sp.str();
    }else{
      val+="[Unprintable]";
    }
  }
  return val; 
}

void GuideDebugWindow::onMouseClick (GdkEventButton* event) {
  //cout << "onmouse" << endl;
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) 
    treeMenu.popup(event->button, event->time);
}
//---popup menu end


bool GuideDebugWindow::on_expand_row(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path) {

  //reload data on expand

  Gtk::TreeModel::Row row = *iter;

  //delete children of tree
   Gtk::TreeModel::iterator delIter = treeModel->get_iter(path)->children().begin();
   while(delIter)
   delIter = treeModel->erase(delIter);

  //get pointer to corresponding cell
  //cout << "path " << path.to_string() << " " << iter << endl;
  helium::IDNode<helium::Cell> *m = getCellFromPath(path.to_string());

  //adding gtk::tree rows
  for(size_t i=0;i<m->childrenNum();i++) {
    //add children
    Gtk::TreeModel::Row childrow = *(treeModel->append(row.children()));
    helium::Cell* c=m->getChild(i)->getDataP();
    childrow[modelColumns.colValValid] = printCell(c);;
    childrow[modelColumns.colName] = m->getChild(i)->getMainAlias();
    //std::cout<<"added children"<<childrow[modelColumns.colName] <<std::endl;
      
    //add the expand mark if child has a child
    if(m->getChild(i)->childrenNum()) {
      Gtk::TreeModel::Row grandchildrow = *(treeModel->append(childrow.children()));
      grandchildrow[modelColumns.colValValid] = "0";
      grandchildrow[modelColumns.colName] = "Fake Row";
    }
  }
  return false;
}

helium::IDNode<helium::Cell>* GuideDebugWindow::getCellFromPath(const Glib::ustring &path_string) {

  //the path is a sequence of numbers spaced by colons
  //get the cell from sequence 0:0:2:4

  std::vector<int> pathArray;
  std::stringstream ss(path_string);
  char colon;
  while(ss){
    int x;
    ss>>x;
    if (ss){
      pathArray.push_back(x);
    }
    ss>>colon;
  }

  //move to the selected memory tree
  helium::IDNode<helium::Cell> *m = &mem;
  for(size_t i=1;i<pathArray.size();i++)
    m = m->getChild(pathArray[i]);

  return m;
}

void GuideDebugWindow::treeviewcolumn_validated_on_cell_data(Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
  //Get the value from the model and show it appropriately in the view:
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    Glib::ustring view_text = row[modelColumns.colValValid];

#ifdef GLIBMM_PROPERTIES_ENABLED
    cellRenderValid.property_text() = view_text;
#else
    cellRenderValid.set_property("text", view_text);
#endif
  }
 
}

void GuideDebugWindow::cellrenderer_validated_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text) {
  Gtk::TreePath path(path_string);
  helium::IDNode<helium::Cell> *m = getCellFromPath(path_string);
  stringstream ss;
  ss << new_text;
  if(m->getDataP()) {
    
    //trying to set data
    if ((m->getDataP()->getProperties()!=NULL)&&(m->getDataP()->getProperties()->getCapabilities()&helium::CELLSET)){
      m->getDataP()->streamSet(ss);
    }
    
      //Get the row from the path:
    Gtk::TreeModel::iterator iter = treeModel->get_iter(path);
    if(iter) {
      Gtk::TreeModel::Row row = *iter;
      //Put the new value in the model:
      row[modelColumns.colValValid] = new_text;
    }
  }
}

