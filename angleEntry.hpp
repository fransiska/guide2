/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


template <typename T>
MoveableEntry<T>::MoveableEntry(Gtk::Fixed *pfixed,T dmin,T dmax):
  helium::correspondingType<HexSpinButton, Gtk::SpinButton,int,double,T>::value
  (
   helium::selectByType<int,double,T>::get(1,0.01), //(int,double) param 1 
   helium::selectByType<int,double,T>::get(10,2) //(int,double) param 2
   ),
  defaultMin(dmin),defaultMax(dmax),
  fixed(pfixed)
{
}



template <typename T>
void MoveableEntry<T>::move(int x, int y) {
  this->unparent(); //remove "Can't set a parent on widget which has a parent" warning
  fixed->put(*this,x,y);
}

template<typename T>
void MoveableEntry<T>::setDefaultRange() {
  this->set_range(defaultMin,defaultMax);
}

template<typename T>
void MoveableEntry<T>::on_activate() {
  Gtk::Entry::on_activate();
  this->hide();
}





template<typename T>
void LinkableEntry<T>::onExternalChange(T v) {
  exChange = true;
  entry.set_value(el->getEntryValue()); //this ended up calling ongtkchange
}

template<typename T>
void LinkableEntry<T>::setRange(T min, T max) {
  entry.set_range(min,max); 
}


template<typename T>
void LinkableEntry<T>::setDefaultRange() {
  entry.setDefaultRange();
}
template<typename T>
void LinkableEntry<T>::setMinRange(T min) {
}
template<typename T>
void LinkableEntry<T>::setMaxRange(T max){
}

template<typename T>
LinkableEntry<T>::LinkableEntry(MoveableEntry<T> &pentry):
  el(NULL),entry(pentry),conn(this),exChange(false){
  entry.setDefaultRange();
}


template<typename T>
void LinkableEntry<T>::setLinked(EntryWidgetLink<T>* pel){
  if (el!=NULL){
    el->setVisibleEntry(true); //visibility
    el->disconnect(conn);
  }
  el=pel;
  el->connect(conn);
  el->setVisibleEntry(false); //visibility
  Point2D loc;
  el->getLocation(loc);
  entry.move(loc.x,loc.y);

  entry.set_value(el->getEntryValue());
  entry.show();
}

template <typename T>
void LinkableEntry<T>::unLink(){
  if(el) {
    el->disconnect(conn);
    
    el->setVisibleEntry(true); //visibility
    entry.hide();
    el = NULL;
  }
}

template <typename T>
void LinkableEntry<T>::onGtkChange() {
  if(!exChange) { //preventing value change from externalChange to execute this function
    if(el!=NULL) {
      if(entry.is_focus()) { //to avoid setting the value twice which would end up with segfau      
	el->setValue(entry.get_value()); //problem: angle-> want to show as degree not radiant
      }
    }
  }
  exChange = false;
}

