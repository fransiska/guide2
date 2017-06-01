/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */


#ifndef MOTIONLINE
#define MOTIONLINE

#include <app/guide2/inputDialog.h>
#include <set>
#include <helium/system/valueHub.h>
#include <app/guide2/guideMotionManagement.h>
#include <app/guide2/robotModels.h>
#include <app/guide2/coldetManager.h>
#include <app/guide2/playButton.h>
#include <gtkmm.h>


/** \brief showing frame info when hovering frame
 */
class TooltipWindow:public Gtk::Window {
  RobotModels::ModelArea *modelArea;
  Gtk::VBox tooltipBox;
  Gtk::Label tooltipLabel;
public:
  TooltipWindow(RobotModels::ModelArea* pmodelArea);
  void setPosture(const double* data);
  void setBG(const double* b);
  void show(Glib::ustring &tip);
};

class TimeSlider: public Gtk::HScale {
  helium::ValueHub<helium::Time> &curTime;
  helium::GtkExceptionHandling &eh;
  Gtk::ScrolledWindow &scrolledWindow;
public:
  TimeSlider(helium::ValueHub<helium::Time> &pcurTime, helium::GtkExceptionHandling &peh,Gtk::ScrolledWindow &scrolledWindow, helium::ValueHub<bool> &editing);
  bool onScrollEvent(GdkEventScroll* ev);

  void disableClicks(bool editing);
  void setValue(helium::Time time);
private:
  void onSetValue();
  void setRange(helium::Time endTime);
  void setScroll();
  helium::StaticConnReference<helium::Time,TimeSlider,helium::Time,&TimeSlider::setValue> callSetValue;
  helium::StaticConnReference<bool,TimeSlider,bool,&TimeSlider::disableClicks> callOnEditing;

public:
  void setRange(const MotionInfo& mi);
  helium::StaticConnReference<std::pair<helium::framePlay::PlayState,helium::Time>, TimeSlider, helium::Time, &TimeSlider::setValue> callSetCurrPlayFrame;
};

/** \brief drawing area where the keyframes' marks are drawn
 */
class TicksArea: public Gtk::DrawingArea{
private:
  Gtk::Window &win; 
  GuideMotionManagement &gmm;
  std::map<helium::Time,cfg::guide::TicksType::Type> &ticks; ///< keyframe and coldet TicksArea
  Gtk::ScrolledWindow &scrolledWindow;
  coldet::ColdetManager *coldetm;
  TooltipWindow tooltipWindow; ///< the tooltip showing keyframe details
  int zoom;
  int width;
  int height;
  static const int offset = 9;
  helium::Time clickCursorTime; ///< saves the time under the cursor position
  helium::Time clickFrameTime; ///< saves the nearest frame time under the cursor position

  Gtk::Menu frameMenu;
enum TicksMenu {NEW,ADJUST,DEL,COPYFRAME,COPYFRAMETO,PASTE,PASTEFRAME,LINK,UNLINK,RENAME,FIX};

  helium::Mutex ticksMutex;
public:
  TicksArea(Gtk::Window &pwin, GuideMotionManagement &pgmm, std::map<helium::Time,cfg::guide::TicksType::Type> &pticks, Gtk::ScrolledWindow &pscrolledWindow, RobotModels::ModelArea *pmodelArea,  coldet::ColdetManager *pcoldetm);


  //draw
  virtual bool on_expose_event(GdkEventExpose* event);
  void drawFrameTick(Cairo::RefPtr<Cairo::Context> &cr, helium::Time time, bool red);
  void drawColdetTick(Cairo::RefPtr<Cairo::Context> &cr, helium::Time time);
  void drawFrameTime(Cairo::RefPtr<Cairo::Context> &cr, helium::Time time, bool red);
  void refresh();

  //ticks
  helium::Time getFrameTime(int pos); ///< get nearest tick (including coldet)
  helium::Time getCursorTime(int pos); ///< get nearest time
  void zoomTicks(bool in);///< zoom frames in or out
  void disableClicks(bool editing);

  bool onKeyPress(GdkEventKey* event);

private:
  bool onMouseClick (GdkEventButton* event); ///< handles mouse click (moves to the nearest tick)
  void onRightMouseClick (GdkEventButton* event); ///< handles right mouse click to open menu

  //menu
  void newFrame();
  void adjustTime();
  void deleteFrame();
  void copyFrame();
  void copyFrameTo();
  void pasteFrame();
  void pasteNearbyFrame();
  void linkFrame();
  void unlinkFrame();
  void renameFrame();
  void fixFrame();

  void setTicks(); ///< get ticks from motion, coldet
  void setColdetTicks(coldet::SegmentNotification sm); ///< from coldet update, add coldet ticks

  bool onTicksTooltip(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip); ///< \todo tooltip for ticksarea
  helium::StaticConnReference<helium::Time,TicksArea,void,&TicksArea::refresh> callRefresh;
  helium::StaticConnReference<bool,TicksArea,bool,&TicksArea::disableClicks> callOnEditing;

  helium::StaticConnReference<GuideEvent::MotionChangeReason,TicksArea,void,&TicksArea::setTicks> callSetTicks;
  helium::StaticConnReference<coldet::SegmentNotification,TicksArea,coldet::SegmentNotification,&TicksArea::setColdetTicks> callSetColdetTicks;

  void onEnableColdet(bool enable);
  helium::StaticConnReference<bool,TicksArea,bool,&TicksArea::onEnableColdet> callOnEnableColdet;

};

class FrameEntry : public Gtk::Entry {
  GuideMotionManagement &gmm;

  Gtk::Entry &repeatEntry;
  Gtk::Entry &speedEntry;

  struct FrameEntryStatus {
    typedef enum {ESHELP,ESADJUST,ESNEWFRAME,ESSPEED,ESREPEAT,ESEDITING,ESINVALID,ESSTATUSNUM} Type;
    static const char* strings[];
  };///< frame entry status indicating the result of the last action
  FrameEntryStatus::Type entryStatus;

  void deleteHelpText();///< make sure help text is deleted when present
  bool onFrameEntryKey(GdkEventKey* event); ///< to delete help text before user input
  void onFrameEntryActivate(); ///< handles frame entry text after enter key
  bool onFrameEntryFocusOut(GdkEventFocus* event); ///< puts on default text
  bool onFrameEntryFocusIn(GdkEventFocus* event); ///< delete or leave current text depending on status

public:
  FrameEntry(GuideMotionManagement &pgmm, Gtk::Entry &pspeedEntry, Gtk::Entry &prepeatEntry);
  void disableClicks(bool editing);
private:
  helium::StaticConnReference<bool,FrameEntry,bool,&FrameEntry::disableClicks> callOnEditing;
};

template<typename T>
class ValidatingEntry : public Gtk::EventBox {
protected:
  T& value;
private:
  const cfg::guide::Color &valid;
  const cfg::guide::Color &invalid;
public:
  Gtk::Entry entry;
  ValidatingEntry(T& value, helium::GtkExceptionHandling &eh, const cfg::guide::Color &valid, const cfg::guide::Color &invalid);
protected:
  void onActivate();
  void onChange();
  bool validateValue();
  bool onFocusOut(GdkEventFocus* event);
  bool onLeave(GdkEventCrossing* event);
  virtual void setValue(std::string s) {}
  virtual T getValue();
};

class RepeatEntry : public ValidatingEntry<int> {
public:
  RepeatEntry(int& value, helium::GtkExceptionHandling &eh, const cfg::guide::Color &valid, const cfg::guide::Color &invalid);
  void test();
protected:
  void setValue(std::string s);
};

class SpeedEntry : public ValidatingEntry<double> {
public:
  SpeedEntry(double& value, helium::GtkExceptionHandling &eh, const cfg::guide::Color &valid, const cfg::guide::Color &invalid);
protected:
  void setValue(std::string s);
};

/** \brief Horizontal bar showing keyframes and buttons
 */
class MotionLine: public Gtk::HBox {
  GuideMotionManagement &gmm;   ///<   
  Gtk::Window &guideWindow;     ///< to set transient of dialog windows
  coldet::ColdetManager *coldetm;
  std::map<helium::Time,cfg::guide::TicksType::Type> ticks; ///< keyframe and coldet TicksArea
  TicksArea ticksArea;    ///< place to draw keyframe marks
  TimeSlider timeSlider;      ///< time scroller

  Gtk::VBox timeBox;
  Gtk::VBox zoomBox;
  Gtk::VBox controlBox;
  Gtk::HBox leftControlBox;
  Gtk::VBox rightFrameBox1;
  Gtk::VBox rightFrameBox2;
  Gtk::HBox frameBox1;
  Gtk::HBox frameBox2;
  Gtk::HBox rightControlBox;

  Gtk::Table mTable;

  Gtk::Label speedLabel;
  Gtk::Label repeatLabel;
  SpeedEntry speedEntry;
  RepeatEntry repeatEntry;
  FrameEntry frameEntry;

  Gtk::ScrolledWindow scrolledWindow; 
  Gtk::VSeparator vSep;

  Gtk::Button prevFrameButton;
  Gtk::Button nextFrameButton;
  Gtk::Button zeroFrameButton;
  Gtk::Button zoomInSliderButton;
  Gtk::Button zoomOutSliderButton;

  helium::ValueHub<bool> isPlayFrame;
  bool isPlayZeroButtonClicked;

  PlayButton playButton;
  PlayZeroButton playZeroButton;



public:
  class SameStartEndFrameDialog : public InputDialog {
  protected:
    Gtk::Button *button2;
  public:    
    SameStartEndFrameDialog(std::string &pstring,helium::GtkExceptionHandling *eh);
  };

  MotionLine(GuideMotionManagement &pgmm, Gtk::Window &pguideWindow,RobotModels::ModelArea *pma, coldet::ColdetManager *pcoldetm);
  bool onKeyPress(GdkEventKey* event); ///< \todo key actions, including delete

private:
  //button actions
  void moveFrame(int type);  ///< move to next or previous frame
  void onPlayFrameClicked(bool zero); ///< play motion
  void disableClicksOnPlay(bool play); ///< enable/disable buttons according to play state
  void onPlaySignal(std::pair<helium::framePlay::PlayState,helium::Time> state); ///< action for play signal

  void confirmColdet();
  void confirmPlaySpeed();
  void confirmPlayRepeat();
  void confirmSameStartEndFrame();
  
  bool onScrolledWindowMouseClick (GdkEventButton* event);
  void disableClicks(bool disable);

  //ticks
  helium::Time getNextTick(helium::Time time);
  helium::Time getPrevTick(helium::Time time);

  helium::StaticConnReference<bool,MotionLine,bool,&MotionLine::disableClicks> callOnEditing;
  helium::StaticConnReference<std::pair<helium::framePlay::PlayState,helium::Time>, MotionLine, std::pair<helium::framePlay::PlayState,helium::Time>, &MotionLine::onPlaySignal> callOnPlaySignal;

  helium::ReplaceParamStaticConnReference<GuideEvent::MotionChangeReason,TimeSlider,const MotionInfo&,&TimeSlider::setRange> callSetRange;
};

#include "motionLine.hpp"

#endif

