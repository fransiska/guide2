/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#include "drawable.h"
namespace gwidget{



  class DrawingContextHolder{
    DrawingContext* dc;


    //DrawingContext::_drawingContext = 0;
  public:
     DrawingContextHolder():dc(NULL){
    }

    ~DrawingContextHolder(){
      if (dc) delete dc;
    }
    
    DrawingContext *get(){
      if (!dc) {
	dc=new DrawingContext();
      }
      return dc;
    }

  };

  static DrawingContextHolder _drawingContext;



DrawingContext *DrawingContext::get() {

  return _drawingContext.get();
}


};
