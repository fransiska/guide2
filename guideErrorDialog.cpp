/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#include <app/guide2/guideErrorDialog.h>
#include <helium/gtk/gtkExceptionHandling.h>
#include <vector>
#include <iostream>
using std::vector;
using std::string;
using std::stringstream;

GuideErrorDialog::GuideErrorDialog(vector<string> &pem,
				   //helium::TsSignal<int> &puen,
				   helium::GtkExceptionHandling *peh,
				   helium::GtkProxy<int> &perrorn):
  em(pem),
  image(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DND),
  okButton(Gtk::Stock::OK),
  followTick("follow"),
  number("0/0"),
  mId(0),
  //updateErrorNumber(puen),
  errorn(perrorn),
  eh(peh)
{
  //nextButton.set_label("_Next");
  Gtk::Widget* imNext = Gtk::manage(new Gtk::Image(Gtk::Stock::GO_FORWARD, Gtk::ICON_SIZE_BUTTON));
  nextButton.set_image(*imNext);
  nextButton.set_use_underline();
  //prevButton.set_label("_Prev");
  Gtk::Widget* imPrev = Gtk::manage(new Gtk::Image(Gtk::Stock::GO_BACK, Gtk::ICON_SIZE_BUTTON));
  prevButton.set_image(*imPrev);
  prevButton.set_use_underline();
  Gtk::Widget* imClear = Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_BUTTON));
  clearButton.set_image(*imClear);
  clearButton.set_label("_Clear");
  clearButton.set_use_underline();

  //firstButton.set_label("_First");
  Gtk::Widget* imFirst = Gtk::manage(new Gtk::Image(Gtk::Stock::GOTO_FIRST, Gtk::ICON_SIZE_BUTTON));
  firstButton.set_image(*imFirst);
  firstButton.set_use_underline();
  //lastButton.set_label("_Last");
  Gtk::Widget* imLast = Gtk::manage(new Gtk::Image(Gtk::Stock::GOTO_LAST, Gtk::ICON_SIZE_BUTTON));
  lastButton.set_image(*imLast);
  lastButton.set_use_underline();


  set_size_request(425,200);
  image.show();
  image.set_size_request(50,50);
  scrolledWin.add(m_TextView);
  vBox.pack_start(image,Gtk::PACK_SHRINK);
  vBox.pack_start(number,Gtk::PACK_SHRINK);
  dBox.pack_start(vBox,Gtk::PACK_SHRINK);
  dBox.pack_start(scrolledWin,Gtk::PACK_EXPAND_WIDGET);
  if(em.size()) {
    m_TextView.get_buffer()->set_text(em[mId]);
    std::cout << "emsize" << std::endl;
  }
  m_TextView.set_editable(false);
  dbBox.pack_start(followTick,Gtk::PACK_SHRINK);
  dbBox.pack_start(clearButton,Gtk::PACK_SHRINK);
  dbBox.pack_start(firstButton,Gtk::PACK_SHRINK);
  dbBox.pack_start(prevButton,Gtk::PACK_SHRINK);
  dbBox.pack_start(nextButton,Gtk::PACK_SHRINK);
  dbBox.pack_start(lastButton,Gtk::PACK_SHRINK);
  dbBox.pack_start(okButton,Gtk::PACK_SHRINK);
  get_vbox()->pack_start(dBox,Gtk::PACK_EXPAND_WIDGET);
  get_action_area()->pack_start(dbBox,Gtk::PACK_SHRINK);
  //get_action_area()->pack_start(okButton,Gtk::PACK_SHRINK);

  followTick.set_active(true);

  clearButton.signal_clicked().connect(eh->wrap(*this,&GuideErrorDialog::onClear));
  prevButton.signal_clicked().connect(eh->wrap(*this,&GuideErrorDialog::onPrev));
  nextButton.signal_clicked().connect(eh->wrap(*this,&GuideErrorDialog::onNext));
  okButton.signal_clicked().connect(eh->wrap(*this,&GuideErrorDialog::onOk));
  firstButton.signal_clicked().connect(eh->wrap(*this,&GuideErrorDialog::onFirst));
  lastButton.signal_clicked().connect(eh->wrap(*this,&GuideErrorDialog::onLast));
  followTick.signal_toggled().connect(eh->wrap(*this,&GuideErrorDialog::onFollow));

  show_all_children();
}

void GuideErrorDialog::onFollow() {
  onLast();
}

void GuideErrorDialog::onPrev() {
  if(em.size()) {
    mId--;
    updateMessage();
  }
}
void GuideErrorDialog::onNext() {
  if(em.size()) {
    mId++;
    updateMessage();
  }
}
void GuideErrorDialog::onFirst() {
  if(em.size()) {
    mId = 0;
    updateMessage();
  }
}
void GuideErrorDialog::onLast() {
  if(em.size()) {
    mId = (int)em.size()-1;
    updateMessage();
  }
}

void GuideErrorDialog::updateMessage() {
  if(mId<0) mId = 0;
  if(mId>=(int)em.size()) mId = em.size()-1;
  m_TextView.get_buffer()->set_text(em[mId]);
  stringstream ss;
  ss << mId+1 << "/" << em.size();
  number.set_label(ss.str());  
}

void GuideErrorDialog::onOk() {
  hide();
}
void GuideErrorDialog::onClear() {
  em.clear();
  m_TextView.get_buffer()->set_text("");
  number.set_label("0/0");
  mId = 0;
  //updateErrorNumber(0);
  errorn = 0;
}

void GuideErrorDialog::setMessage() {
  if(em.size()>0) {
    if(followTick.get_active())
      mId = em.size()-1;
    updateMessage();
  }
  else
    std::cerr << "error: no error messages found" << std::endl;
}


void ErrorDialogEH::dialogClosed(int reply){
  //std::cout<<"reply is "<<reply<<std::endl;
  dialog.hide();
}

ErrorDialogEH::ErrorDialogEH(Gtk::Window& owner, 
			     //helium::TsSignal<int> &puen,
			     helium::GtkExceptionHandling *eh,
			     helium::GtkProxy<int> &perrorn):
  dialog(errorMessages,eh,perrorn),
  //updateErrorNumber(puen)
  errorn(perrorn)
{
  dialog.signal_response().connect(sigc::mem_fun(*this,&ErrorDialogEH::dialogClosed));
  dialogDisp.connect(this);
}


void ErrorDialogEH::operator()(const std::string& s){
  //std::cout<<"OPERATOR "<<s<<std::endl;
  std::vector<std::string> tokens;
  helium::tokenize(tokens,s,":");
  std::stringstream ss;
  helium::print("\n",tokens,ss);
  errorMessages.push_back(ss.str());
  dialog.setMessage();
  dialog.show();
  //updateErrorNumber((int)errorMessages.size());
  errorn = errorMessages.size();
};

void ErrorDialogEH::handle(helium::exc::Exception& e){
  //std::cout<<"HANDLING "<<e.what()<<std::endl;
 dialogDisp(e.what());
}


