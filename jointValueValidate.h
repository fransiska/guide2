#ifndef JOINTVALUEVALIDATE
#define JOINTVALUEVALIDATE

#include <helium/util/mathUtil.h>

template<typename T>
class JointValueValidate {
  const T absMinValue,absMaxValue; ///<limit of view (height of bar)
  T minValue,maxValue; ///<limit (red area), change only at selected joint change
  T curMinValue,curMaxValue;///< current limit of cursor/entry
  
public:
  JointValueValidate(T pminValue, T pmaxValue);
  void validateValue(T &val, bool noLimit=false);
  /**
   * set range to fit if value is outside limit
   */
  void expandRange(T val);
  void setRange(T min,T max); ///<set curmin and curmax directly
  void setDefaultRange(bool noLimit=false);
  //void setMaxRange(); /// set to maximum possible range (abs limit)
  bool isWithinLimit(const T &val);
  T getAbsMin(); ///< bar min displayable value
  T getAbsMax(); ///< bar max displayable value  
  T getCurMin(); /// curminValue
  T getCurMax(); /// curminValue
  void setMin(T v); ///< set redLimit
  void setMax(T v); ///< set redLimit 
};

template<typename T>
JointValueValidate<T>::JointValueValidate(T pminValue, T pmaxValue):
  absMinValue(pminValue),absMaxValue(pmaxValue),minValue(pminValue),maxValue(pmaxValue),curMinValue(minValue),curMaxValue(maxValue)
{
  minValue = absMinValue;
  maxValue = absMaxValue;
  curMinValue = absMinValue;
  curMaxValue = absMaxValue;
}

template<typename T>
void JointValueValidate<T>::validateValue(T &val, bool noLimit){
  //limiting the draggable position of cursor0 (until the red position)

  if(val<absMinValue)
    val = absMinValue;
  else if(val > absMaxValue)
    val = absMaxValue;
  if(!noLimit) {
    if(val<curMinValue)
      val = curMinValue;
    else if(val > curMaxValue)
      val = curMaxValue;
  }
  //std::cout << "val,cur " << val << " " << curMinValue << " " << curMaxValue << std::endl;
}



template<typename T>
T JointValueValidate<T>::getAbsMin() {
  return absMinValue;
}
template<typename T>
T JointValueValidate<T>::getAbsMax() {
  return absMaxValue;
}

template<typename T>
T JointValueValidate<T>::getCurMin() {
  return curMinValue;
}
template<typename T>
T JointValueValidate<T>::getCurMax() {
  return curMaxValue;
}

/** call this when changing selected joint */
template<typename T>
void JointValueValidate<T>::setMin(T v) {
  if(v >= absMinValue)
    minValue = v;
}
/** call this when changing selected joint */
template<typename T>
void JointValueValidate<T>::setMax(T v) {
  if(v <= absMaxValue)
    maxValue = v;
}

/** set beyond curmin and curmax depends on cur value
 * need to set curmin and curmax first
 */
template<typename T>
void JointValueValidate<T>::expandRange(T val){
  //setDefaultRange(false);
  //in the case when current value is over limit
  if(val < curMinValue  && val >= absMinValue)
    curMinValue = val;
  else if(val > curMaxValue && val <= absMaxValue)
    curMaxValue = val;
}

///set curmin curmax directly
template<typename T>
void JointValueValidate<T>::setRange(T min, T max){
  setDefaultRange(true);

  if(min > curMinValue)
    curMinValue = min;
  if(max < curMaxValue)
    curMaxValue = max;

}

/** default range
 *  call when finished changing value
 */
template<typename T>
void JointValueValidate<T>::setDefaultRange(bool noLimit){
  curMinValue = absMinValue;
  curMaxValue = absMaxValue;
  
  if(!noLimit) {
    if(minValue > absMinValue)
      curMinValue = minValue;
    if(maxValue < absMaxValue)
      curMaxValue = maxValue;
  }
}


#endif
