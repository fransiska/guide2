/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include "guidePostureWindow.h"
#include <iostream>
#include <sstream>
#include <app/guide2/inputDialog.h>
#include "yesNoDialog.h"
#include <helium/xml/xmlLoader.h>
#include "guideDescription.h"

using std::cout;
using std::endl;

PostureTreeView::PostureTreeView(Gtk::Window &pwin, helium::GtkExceptionHandling *peh,RobotModels *prm, int pw, int ph):
  win(pwin),
  eh(peh),rm(prm),
  w(pw),h(ph)
{
  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  set_model(m_refTreeModel);

  //Add the TreeView's view columns:
  //This number will be shown with the default numeric formatting.
  append_column("ID", m_Columns.m_col_id);
  if(rm) append_column("Image", m_Columns.m_pixbuf);

  Gtk::CellRendererText *renderer1 = Gtk::manage(new Gtk::CellRendererText());
  append_column("Name", *renderer1);
  Gtk::TreeViewColumn *col = get_column(rm?2:1);
  col->add_attribute(renderer1->property_markup(), m_Columns.m_col_name);

  //Make all the columns reorderable:
  //This is not necessary, but it's nice to show the feature.
  //You can use TreeView::set_column_drag_function() to more
  //finely control column drag and drop.
  for(guint i = 0; i < (rm?2:1); i++)
  {
    Gtk::TreeView::Column* pColumn = get_column(i);
    pColumn->set_reorderable();
  }

  signal_button_press_event().connect(eh->wrap(*this, &PostureTreeView::on_button_press_event), false);

  //Fill popup menu:
  {
    Gtk::Menu::MenuList& menulist = m_Menu_Popup.items();
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Apply to Current Frame",eh->wrap(*this,&PostureTreeView::onApply)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Add Current Posture",eh->wrap(*this,&PostureTreeView::onAdd)));
    menulist.push_back( Gtk::Menu_Helpers::MenuElem("_Delete",eh->wrap(*this,&PostureTreeView::onDelete)));
  }
  m_Menu_Popup.accelerate(*this);

  m_Menu_Popup.items()[0].set_sensitive(false);
  //m_Menu_Popup.items()[1].set_sensitive(false);
  m_Menu_Popup.items()[2].set_sensitive(false);
}

bool PostureTreeView::on_button_press_event(GdkEventButton* event) {
  bool return_value = false;

  //Call base class, to allow normal handling,
  //such as allowing the row to be selected by the right-click:
  return_value = Gtk::TreeView::on_button_press_event(event);

  //Then do our custom stuff:
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
  {
    m_Menu_Popup.popup(event->button, event->time);
  }

  return return_value;
}


void GuidePostureWindow::clearTree() {
  m_TreeView.m_refTreeModel->clear();

  m_TreeView.m_Menu_Popup.items()[PostureTreeView::APPLYPOST].set_sensitive(false);
  m_TreeView.m_Menu_Popup.items()[PostureTreeView::DELPOST].set_sensitive(false); 

  //for(size_t i=0;i<m_TreeView.postureSnap.size();i++)
  //delete m_TreeView.postureSnap[i]; //delete pixbufptr
  m_TreeView.postureSnap.clear();
}

void PostureTreeView::updateId() {
  typedef Gtk::TreeModel::Children type_children; //minimise code length.
  type_children children = m_refTreeModel->children();
  int i=0;
  for(type_children::iterator iter = children.begin();
      iter != children.end(); ++iter)
    {
      Gtk::TreeModel::Row row = *iter;
      row[m_Columns.m_col_id] = i;
      i++;
    }
}

///tree
void PostureTreeView::onApply() {
  Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
  if(refSelection)
  {
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    if(iter)
    {
      int id;
      std::stringstream ss;
      ss << (*iter)[m_Columns.m_col_id];
      ss >> id;
      //helium::convert(id,(*iter)[m_Columns.m_col_id]);
      signalApplyPost(id);
    }
  }
}


void PostureTreeView::onDelete() {
  Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
  if(refSelection)
  {
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    if(iter)
    {
      int id = (*iter)[m_Columns.m_col_id];
      YesNoDialog d("Warning","Deleted posture will be lost forever, continue?");
      d.set_transient_for(win);

      if(d.run()) {
	m_refTreeModel->erase( iter );
	signalDeletePost(id);

	//delete posturesnap
	std::list<RobotModels::PostureSnap>::iterator iteri;
	iteri = postureSnap.begin();
	advance(iteri,id);
	postureSnap.erase(iteri);
      }//if yes delete
    }//if iter
  }
  updateId();
}

//setting data to a branch of the tree
void PostureTreeView::addEntry(const int id, const std::string name, const helium::Posture pose) {
  //cout << "posture add entry" << endl;
  Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  row[m_Columns.m_col_id] = id;

  std::stringstream ss;
  ss << pose;
  row[m_Columns.m_col_name] = "<b>"+name+"</b>\n"+ss.str();

  if(rm) {
    postureSnap.resize(postureSnap.size()+1);
    rm->getPostureSnap(postureSnap.back(),pose,w,h);

    row[m_Columns.m_pixbuf] = Gdk::Pixbuf::create_from_data(postureSnap.back().data,Gdk::COLORSPACE_RGB,false,8,w,h,w*3);
  }

  m_Menu_Popup.items()[APPLYPOST].set_sensitive(true);
  m_Menu_Popup.items()[DELPOST].set_sensitive(true);
}


//adding for the first time
void GuidePostureWindow::addEntry(const int id) {
  m_TreeView.addEntry(id,gmm.gs.postures[id].name,gmm.gs.postures[id]);
}


void PostureTreeView::onAdd() {
  signalAddCurPost();
  updateId();
}



/*
PostureTreeView::~PostureTreeView() {
  for(size_t i=0; i<postureSnap.size(); i++)
    delete postureSnap[i];
}
*/

//GuidePostureWindow::GuidePostureWindow(GuidePostureList &pgpl,GuideExceptionHandler *peh) :
GuidePostureWindow::GuidePostureWindow(GuideMotionManagement &pgmm,RobotModels *prm):
  //pl(ppl),
  //eh(peh),
  gmm(pgmm),
  //cmw(pcmw),
  //postures(gmm.gs.postures),
  m_TreeView(*this,&gmm.eh,prm,gmm.gs.settings.sizes.model.w,gmm.gs.settings.sizes.model.h),
  m_Button_Quit(Gtk::Stock::QUIT),
  m_Button_Open(Gtk::Stock::OPEN),
  m_Button_Save(Gtk::Stock::SAVE_AS),
  addCurPostureConn(this,helium::FuncPlaceHolder<GuidePostureWindow,void,&GuidePostureWindow::addCurPosture>()),
  applyPostureConn(this,helium::FuncPlaceHolder<GuidePostureWindow,int,&GuidePostureWindow::applyPosture>()),
  callDeleteEntry(this,helium::FuncPlaceHolder<GuidePostureWindow,int,&GuidePostureWindow::deleteEntry>())
{
  set_title("Posture List");
  set_border_width(5);
  set_default_size(500, 200);

  add(m_VBox);

  //Add the TreeView, inside a ScrolledWindow, with the button underneath:
  m_ScrolledWindow.add(m_TreeView);

  //Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_VBox.pack_start(m_ScrolledWindow);
  m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  m_ButtonBox.pack_start(m_Button_Open, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_Save, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_Quit, Gtk::PACK_SHRINK);
  m_ButtonBox.set_border_width(5);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
  m_Button_Open.signal_clicked().connect( gmm.eh.wrap(*this,&GuidePostureWindow::onFileAction,OPEN) );
  m_Button_Save.signal_clicked().connect( gmm.eh.wrap(*this,&GuidePostureWindow::onFileAction,SAVEAS) );
  m_Button_Quit.signal_clicked().connect( gmm.eh.wrap(*this,&GuidePostureWindow::on_button_quit) );
  //signal_show().connect(eh->wrap(*this,&GuidePostureWindow::refreshSnap));    
  m_TreeView.signalDeletePost.connect(callDeleteEntry);

  m_TreeView.signalAddCurPost.connect(addCurPostureConn);
  m_TreeView.signalApplyPost.connect(applyPostureConn);

  show_all_children();

  signal_realize().connect(gmm.eh.wrap(*this,&GuidePostureWindow::onRealize));
}

void GuidePostureWindow::addCurPosture() {
  std::string name;
  InputDialog inputDialog("Input a posture name",name,&gmm.eh);
  inputDialog.set_transient_for(*this);
  if(inputDialog.run()) {
    if(name.size() == 0)
      name = "New " + gmm.gs.pref.robot + " Posture "+helium::toString(gmm.gs.postures.size());
    helium::Posture p(gmm.gs.joints.size(),name);
    for(size_t i=0;i<gmm.gs.joints.size();i++)
      p[i] = gmm.gs.joints[i].frameValue.get().second;

    
    //int id = guidePostureList.addPosture(name,ss.str());
    //guidePostureWindow.addEntry(id); gmm.gs.postures.add(p); 

    gmm.gs.postures.resize(gmm.gs.postures.size()+1);
    gmm.gs.postures[gmm.gs.postures.size()-1] = p;

    //cout << "posture is now " << postures.size() << endl;

    m_TreeView.addEntry(gmm.gs.postures.size()-1,name,p);
    
    savePostures(gmm.gs.pref.plFilename);
  }
}

void GuidePostureWindow::savePostures(std::string f) {
  helium::Memory outmem;
  helium::XmlLoader out;
  dexport(outmem,gmm.gs.postures);
  //std::cout << "saving to " << f << " "<< outmem << std::endl;
  out.save(f.c_str(),outmem);  
}

void GuidePostureWindow::onFileAction(ActionType action) {
  std::string title = "";
  switch(action) {
  case OPEN: title="Please choose a posture file";break;
  case SAVEAS: title="Please input a posture file";break;
  }

  Gtk::FileChooserDialog dialog(title, action==OPEN?Gtk::FILE_CHOOSER_ACTION_OPEN:Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(action==OPEN?Gtk::Stock::OPEN:Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);
  dialog.set_current_folder(gmm.gs.pref.plFilename.substr(0,gmm.gs.pref.plFilename.find_last_of("/\\")));

  Gtk::CheckButton replace("Replace current list");
  if(action==OPEN) {
    replace.show();
    replace.set_active(false);
    dialog.set_extra_widget(replace);
  }

  //Add filters, so that only certain file types can be selected:
  Gtk::FileFilter filter_xml;
  filter_xml.set_name("XML files");
  filter_xml.add_pattern("*.xml");
  dialog.add_filter(filter_xml);
  
  if (action==SAVEAS) {
    dialog.set_do_overwrite_confirmation(true);
    dialog.set_current_name("postures.xml");
  }

  if(dialog.run()==Gtk::RESPONSE_OK) {
    if(action==SAVEAS) {
      //gtl.get().saveTeachList(dialog.get_filename().c_str());
      savePostures(dialog.get_filename());
    }
    else {//OPEN file (replace current)
      cout << "backing up current postures to " << gmm.gs.pref.plFilename << ".backup.xml" << endl;
      savePostures(gmm.gs.pref.plFilename+".backup.xml");
      
      if(replace.get_active()) {
	gmm.gs.postures.clear();
	loadPostures(dialog.get_filename(), gmm.gs.postures);
      }
      else {
	cfg::guide::Postures pl;
	loadPostures(dialog.get_filename(), pl);
	appendPostures(pl);
      }
      
      clearTree();
      reloadTree();
      
      savePostures(gmm.gs.pref.plFilename);
    }
  }  
}

void GuidePostureWindow::appendPostures(cfg::guide::Postures &pl) {
  for(size_t i=0;i<pl.size();i++) {
    gmm.gs.postures.push_back(pl[i]);
  }
}

void GuidePostureWindow::reloadTree() {
  for(size_t i=0;i<gmm.gs.postures.size();i++) {
    addEntry(i);
  }
}

void GuidePostureWindow::loadPostures(std::string f, cfg::guide::Postures &pl) {
  //cout << "adding postures to list" << endl;

  helium::Memory inmem;
  helium::XmlLoader xml;
  xml.loadDataAndModel(inmem,f);
  helium::dimport(pl,inmem);
  //cout << inmem << endl;
}

void GuidePostureWindow::onRealize() {
  for(size_t i=0;i<gmm.gs.postures.size();i++) 
  //for(size_t i=0;i<5;i++)
    addEntry(i);
}


void GuidePostureWindow::applyPosture(int id) { //apply one posture from posturelistwindow
  for(size_t i=0;i<gmm.gs.joints.size();i++) 
    gmm.gs.joints[i].frameValue.assure<GuideEvent::PASTELISTJOINT>(std::make_pair(gmm.gs.joints[i].frameValue.get().first,gmm.gs.postures[id][i]));
  gmm.gs.joints.postureChange(GuideEvent::PASTELISTPOSTURE);

  /// \todo fix this, this causes 2 times event
  gmm.gs.motionInfo.motionEdit<GuideEvent::RENAMEFRAME,void,helium::Time,std::string,&helium::Motion::rename>
    (gmm.gs.motionInfo.curTime.get(),gmm.gs.postures[id].name);
}


void GuidePostureWindow::enableApplyMenu(bool b) {
  //cout << "enable menu" << endl;
  m_TreeView.m_Menu_Popup.items()[PostureTreeView::APPLYPOST].set_sensitive(b);
  m_TreeView.m_Menu_Popup.items()[PostureTreeView::ADDPOST].set_sensitive(b);
}

void GuidePostureWindow::deleteEntry(int id) {
  for(size_t i=id;i<gmm.gs.postures.size()-1;i++)
    gmm.gs.postures[i] = gmm.gs.postures[i+1];
  gmm.gs.postures.resize(gmm.gs.postures.size()-1);   //TODO delete posture DEBUG fabio how to add/remove postures xml?

  //std::cout << "Deleted selected ID " << id << " remaining " << gmm.gs.postures.size()<< std::endl;
  if(gmm.gs.postures.size() < 1) {
    //cout << "no more posture" << endl;
    m_TreeView.m_Menu_Popup.items()[PostureTreeView::APPLYPOST].set_sensitive(false);
    m_TreeView.m_Menu_Popup.items()[PostureTreeView::DELPOST].set_sensitive(false);
  }//if no more remaining postures -> disable menu
  savePostures(gmm.gs.pref.plFilename);
}
