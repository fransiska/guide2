#include <iostream>
#include <helium/vector/array.h>
#include <gtkmm.h>
#include "guideTeachList.h"

 #include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/constReference.h>
 #include <helium/core/basicClass.h>
#include <helium/system/valueHub.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/callback/tsSignal.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>

#include <helium/callback/ehSignal.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/system/valueProxy.h>
 #include <helium/core/isConvertible.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>
 #include <helium/util/debug.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/thread.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
#include <helium/system/remoteCell.h>
 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>
#include <helium/system/remoteCell.hpp>
#include <helium/core/selectByValue.h>
 #include <helium/gtk/gtkSignal.h>
#include <helium/gtk/gtkExceptionHandling.h>

#include <helium/util/mathUtil.h>

 #include <helium/callback/staticConnReference.h>

#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/core.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/basicClass.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>
#include <helium/core/constReference.h>

#include <helium/thread/thread.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/tsSignal.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/vector/operations.h>
 #include <helium/system/cellProtocol.h>
#include <helium/callback/ehSignal.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/util/debug.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/basicCell.h>
 #include <helium/system/cellSignature.h>
 #include <helium/system/remoteCellImporter.h>

#include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>

#include <helium/thread/sharedAccess.h>
#include <helium/callback/wrapInfo.h>

#include <helium/destroy/autoDestroy.h>
#include <helium/core/selectByValue.h>

#include <helium/callback/staticConnReference.h>


#include <helium/system/cellDescription.h>
#include <helium/core/deletable.h>

#include <helium/system/cellMapping.h>

#include <helium/system/volatile.h>
#include <helium/system/indexableProxy.h>

#include <helium/core/isSpecialized.h>
#include <helium/system/valueHub.h>

#include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>
 #include <helium/gtk/protectedGtkProxy.h>

#include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>

#include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>

#include <helium/system/signalledArray.h>
#include <helium/spooling/spooler.h>

#include <helium/spooling/job.h>

#include <helium/os/time.h>
#include <helium/robot/hwInterface.h>

#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 

#include <helium/system/signalledValue.h>


#include <helium/robot/motorOffMode.h>


#include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>
 
#include <helium/robot/timeSource.h>
 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>

#include <helium/robot/motorFramePlayMode.h>
 

#include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/enum.h>
#include <helium/callback/tsSignal.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/core/constReference.h>
#include <helium/core/basicClass.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/ioUtil.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>

#include <helium/system/cellDescription.h>
 #include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/vector/operations.h>
 #include <helium/system/cellProtocol.h>
#include <helium/callback/ehSignal.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/util/debug.h>
#include <helium/system/cellMapping.hpp>
 #include <helium/system/volatile.h>
#include <helium/system/valueProxy.h>
 #include <helium/core/isConvertible.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/thread.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>

 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>

 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>
 #include <helium/system/valueHub.h>
#include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>

 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/system/remoteCellImporter.h>
#include <helium/callback/staticConnReference.h>

#include <helium/convex/drawConvex.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/graphics/keybMouseInteraction.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/math/algebra.h>
 #include <helium/convex/convexModel.h>
#include <helium/convex/jointParser.h>

#include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>

 #include <helium/callback/tsSignal.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/constReference.h>
 #include <helium/core/basicClass.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>

 #include <helium/robot/framePolicy.h>
#include <helium/robot/frame.h>
 #include <helium/vector/array.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/basicClass.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>
 #include <helium/core/constReference.h>
#include <helium/smartpointer/ndref.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/ioUtil.h>
#include <helium/core/enum.h>

 #include <helium/system/valueHub.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/destroy/destroy.h>
 #include <helium/callback/signalType.h>
#include <helium/callback/tsSignal.h>
 #include <helium/callback/ehSignal.h>
#include <helium/robot/motionParser.h>
#include <helium/gtk/gtkExceptionHandling.h>
#include <app/guide2/guideState.h>
 #include <helium/system/cellDescription.h>
#include <helium/core/deletable.h>
 #include <helium/system/cellMapping.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>
 #include <helium/util/debug.h>
#include <helium/system/cellMapping.hpp>
 #include <helium/system/volatile.h>
#include <helium/system/valueProxy.h>
 #include <helium/core/isConvertible.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/thread.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>

#include <helium/system/remoteCell.h>
 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>

 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>

 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>


#include <app/guide2/connectionCell.h>
 #include <helium/system/remoteCellImporter.h>
#include <helium/callback/staticConnReference.h>
#include <app/guide2/point2D.h>
#include <helium/container/historyStack.h>
#include <app/guide2/guideLog.h>
#include <helium/util/mathUtil.h>
#include <helium/system/cell.h>
 #include <helium/util/ioUtil.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/core.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/enum.h>
 #include <helium/vector/array.h>
#include <helium/core/basicClass.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>
 #include <helium/core/constReference.h>

 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>
 #include <helium/callback/ehSignal.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/destroy/destroy.h>
 #include <helium/callback/signalType.h>
#include <helium/callback/tsSignal.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/debug.h>
#include <helium/gtk/gtkExceptionHandling.h>
 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>
 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/thread/semaphore.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/system/cellClient.h>
#include <helium/net/clientSocket.h>
 #include <helium/thread/thread.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/core/selectByValue.h>

#include <helium/system/cellDescription.h>
 #include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cell.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/core.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/basicClass.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>
#include <helium/core/constReference.h>

 #include <helium/util/stringUtil.h>
#include <helium/vector/operations.h>
 #include <helium/system/cellProtocol.h>
#include <helium/callback/ehSignal.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/callback/tsSignal.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/debug.h>

 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/thread/semaphore.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/system/cellClient.h>
#include <helium/net/clientSocket.h>
 #include <helium/thread/thread.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/basicCell.h>
 #include <helium/system/cellSignature.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/core/selectByValue.h>
#include <helium/system/indexableProxy.h>
 #include <helium/core/isSpecialized.h>
#include <helium/system/hexMapping.h>

#include <helium/system/valueHub.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/system/remoteCellImporter.h>


#include <helium/robot/motorPropertyManager.h>
 #include <helium/robot/framePolicy.h>
#include <helium/robot/frame.h>
 #include <helium/vector/array.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/basicClass.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>
 #include <helium/core/constReference.h>
#include <helium/smartpointer/ndref.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/ioUtil.h>
#include <helium/core/enum.h>

 #include <helium/system/valueHub.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/destroy/destroy.h>
 #include <helium/callback/signalType.h>
#include <helium/callback/tsSignal.h>
 #include <helium/callback/ehSignal.h>
#include <helium/robot/motionParser.h>

#include <app/guide2/guideState.h>
 #include <helium/system/cellDescription.h>
#include <helium/core/deletable.h>
 #include <helium/system/cellMapping.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>
 #include <helium/util/debug.h>
#include <helium/system/cellMapping.hpp>
 #include <helium/system/volatile.h>
#include <helium/system/valueProxy.h>
 #include <helium/core/isConvertible.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/thread.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>

 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>

 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

#include <app/guide2/connectionCell.h>
 #include <helium/system/remoteCellImporter.h>

#include <app/guide2/point2D.h>

#include <app/guide2/guideLog.h>

#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/enum.h>
 #include <helium/callback/tsSignal.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/core/constReference.h>
 #include <helium/core/basicClass.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/vector/array.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>

 #include <helium/system/cellDescription.h>
#include <helium/core/deletable.h>
 #include <helium/system/cellMapping.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>
 #include <helium/callback/ehSignal.h>
#include <helium/destroy/destroy.h>
 #include <helium/callback/signalType.h>
#include <helium/util/debug.h>

 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/thread/semaphore.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/system/cellClient.h>
#include <helium/net/clientSocket.h>
 #include <helium/thread/thread.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/basicCell.h>
 #include <helium/system/cellSignature.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/core/selectByValue.h>
#include <helium/system/indexableProxy.h>
 #include <helium/core/isSpecialized.h>
#include <helium/system/valueHub.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/system/remoteCellImporter.h>

 #include <helium/container/historyStack.h>
 #include <helium/os/time.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>

#include <helium/robot/motorPropertyManager.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/spooling/spooler.h>
 #include <helium/thread/thread.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/constReference.h>
 #include <helium/core/basicClass.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>

 #include <helium/callback/tsSignal.h>
#include <helium/spooling/job.h>

 #include <helium/os/time.h>
#include <helium/robot/motorInterface.h>
 #include <helium/core/triple.h>
#include <helium/core/deletable.h>
 #include <helium/smartpointer/reference.h>
#include <helium/robot/hwInterface.h>

 #include <helium/system/signalledValue.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/system/cellProtocol.h>
#include <helium/callback/ehSignal.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>
 #include <helium/system/cellSignal.h>
#include <helium/core/selectByValue.h>
 #include <helium/system/cellDescription.h>
#include <helium/system/cellMapping.h>

 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>
 
 #include <helium/robot/motorFramePlayMode.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
#include <helium/robot/motorActiveMode.h>
 #include <helium/callback/valueWaiter.h>

 #include <helium/system/valueHub.h>
#include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
 #include <helium/robot/hardwareException.h>
#include <helium/robot/motorOffMode.h>

 #include <helium/robot/motionAdapter.h>
#include <helium/robot/sensorSpooler.h>
 #include <helium/robot/sensorInterface.h>
#include <helium/container/bufferPipe.h>
 #include <helium/container/minimalList.h>
#include <helium/core/referenceType.h>
 #include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/system/remoteCellImporter.h>

#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/core.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/basicClass.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>
#include <helium/core/constReference.h>

 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/tsSignal.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/vector/operations.h>
 #include <helium/system/cellProtocol.h>
#include <helium/callback/ehSignal.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/util/debug.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/basicCell.h>
 #include <helium/system/cellSignature.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/system/remoteCellImporter.h>
 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/core/selectByValue.h>
 #include <helium/callback/staticConnReference.h>
 #include <helium/system/cellSignal.h>
#include <helium/robot/application.h>
 #include <helium/robot/config.h>
#include <helium/system/cellMapping.h>

 #include <helium/system/indexableProxy.h>
#include <helium/xml/xmlLoader.h>
 #include <helium/system/helpCell.h>
#include <helium/system/cellDescription.h>
 #include <helium/core/deletable.h>
#include <helium/core/isSpecialized.h>
 #include <helium/os/time.h>
#include <helium/system/remoteCellExporter.h>

#include <helium/system/valueHub.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/spooling/spooler.h>
 #include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/robot/framePolicy.h>
#include <helium/robot/policy.h>

#include <app/guide2/angleEntry.h>
#include <helium/util/mathUtil.h>
 #include <helium/convex/drawConvex.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/graphics/keybMouseInteraction.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/math/algebra.h>
 #include <helium/convex/convexModel.h>
 #include <helium/convex/boxNode.h>
 #include <helium/convex/jointParser.h>
 #include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>
 #include <helium/graphics/autoresizingBuffer.h>
 #include <heliumBased/robot/_sensorCalibManager.h>

#include <helium/graphics/glDraw.h>
 #include <helium/math/filter.h>
#include <heliumBased/robot/gyroCalibManager.h>

#include <app/guide2/readablePostureDrawer.h>
#include <helium/graphics/glUtil.h>
 #include <helium/system/keyboardCellServer.h>
 #include <helium/system/cellRequestParser.h>
 #include <helium/system/cellRequestParser.hpp>
 #include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/enum.h>
#include <helium/callback/tsSignal.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/core/constReference.h>
#include <helium/core/basicClass.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/ioUtil.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>

 #include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/vector/operations.h>
 #include <helium/system/cellProtocol.h>
#include <helium/callback/ehSignal.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/util/debug.h>
#include <helium/system/cellMapping.hpp>
 #include <helium/system/volatile.h>
#include <helium/system/valueProxy.h>
 #include <helium/core/isConvertible.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/thread.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>

 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>

 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>
 #include <helium/system/valueHub.h>
#include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

#include <app/guide2/connectionCell.h>
 #include <helium/system/remoteCellImporter.h>

#include <app/guide2/point2D.h>
 #include <heliumBased/robot/_sensorCalibManager.h>

#include <helium/graphics/glDraw.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/graphics/keybMouseInteraction.h>
#include <helium/math/filter.h>
 #include <heliumBased/robot/gyroCalibManager.h>

#include <app/guide2/postureDrawer.h>
#include <helium/convex/drawConvex.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/math/algebra.h>
 #include <helium/convex/convexModel.h>
#include <helium/convex/jointParser.h>
 #include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>
 #include <helium/graphics/autoresizingBuffer.h>
 #include <helium/container/historyStack.h>

#include <app/guide2/readablePostureDrawer.h>
#include <helium/graphics/glUtil.h>
 #include <helium/robot/framePolicy.h>
#include <helium/robot/policy.h>

#include <app/guide2/guideTeachList.h>

#include <app/guide2/angleEntry.h>
#include <helium/util/mathUtil.h>
 #include <helium/convex/drawConvex.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/callback/tsSignal.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/constReference.h>
 #include <helium/core/basicClass.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>
 
 #include <helium/graphics/keybMouseInteraction.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/core/basicEnum.h>
 #include <helium/math/algebra.h>
#include <helium/vector/operations.h>
 #include <helium/convex/convexModel.h>

#include <helium/convex/jointParser.h>

 #include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>
#include <helium/thread/semaphore.h>

 #include <helium/robot/framePolicy.h>
#include <helium/robot/frame.h>
 #include <helium/vector/array.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/basicClass.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>
 #include <helium/core/constReference.h>
#include <helium/smartpointer/ndref.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/ioUtil.h>
#include <helium/core/enum.h>

 #include <helium/system/valueHub.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/destroy/destroy.h>
 #include <helium/callback/signalType.h>
#include <helium/callback/tsSignal.h>
 #include <helium/callback/ehSignal.h>
#include <helium/robot/motionParser.h>

#include <app/guide2/guideState.h>
 #include <helium/system/cellDescription.h>
#include <helium/core/deletable.h>
 #include <helium/system/cellMapping.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>
 #include <helium/util/debug.h>
#include <helium/system/cellMapping.hpp>
 #include <helium/system/volatile.h>
#include <helium/system/valueProxy.h>
 #include <helium/core/isConvertible.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/thread.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>

 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>

 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>

 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

#include <app/guide2/connectionCell.h>
 #include <helium/system/remoteCellImporter.h>

#include <app/guide2/point2D.h>
#include <helium/container/historyStack.h>
#include <app/guide2/guideLog.h>

#include <app/guide2/inputDialog.h>
#include <helium/util/mathUtil.h>

#include <helium/callback/tsSignal.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/constReference.h>
#include <helium/core/basicClass.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/ioUtil.h>
#include <helium/core/enum.h>
 #include <helium/vector/array.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>


#include <helium/convex/drawConvex.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/graphics/keybMouseInteraction.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/core/basicEnum.h>
 #include <helium/math/algebra.h>
#include <helium/vector/operations.h>
 #include <helium/convex/convexModel.h>

#include <helium/convex/jointParser.h>
 #include <helium/destroy/autoDestroy.h>
 #include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>
#include <helium/thread/semaphore.h>
 #include <helium/graphics/autoresizingBuffer.h>

 #include <helium/system/cellDescription.h>
#include <helium/core/deletable.h>
 #include <helium/system/cellMapping.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/system/cellProtocol.h>
#include <helium/callback/ehSignal.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/util/debug.h>
#include <helium/system/cellMapping.hpp>
 #include <helium/system/volatile.h>
#include <helium/system/valueProxy.h>
 #include <helium/core/isConvertible.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/thread.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>

 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>

 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>
 #include <helium/system/valueHub.h>
#include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/gtk/gtkProxy.h>
 #include <helium/gtk/gtkSignal.h>
#include <helium/gtk/gtkExceptionHandling.h>
 
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>
 
 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>
 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/system/remoteCellImporter.h>

#include <helium/robot/framePolicy.h>
 #include <helium/robot/policy.h>
#include <helium/robot/motionParser.h>
 #include <helium/robot/motorPropertyManager.h>

#include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/enum.h>
#include <helium/callback/tsSignal.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/core/constReference.h>
#include <helium/core/basicClass.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/ioUtil.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>

 #include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/vector/operations.h>
 #include <helium/system/cellProtocol.h>
#include <helium/callback/ehSignal.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/util/debug.h>
#include <helium/system/cellMapping.hpp>
 #include <helium/system/volatile.h>
#include <helium/system/valueProxy.h>
 #include <helium/core/isConvertible.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/wrapInfo.h>
 #include <helium/destroy/autoDestroy.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/thread/thread.h>
 #include <helium/net/netSocket.h>
#include <helium/net/commandSplitter.h>

 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>

 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>
 #include <helium/system/valueHub.h>
#include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/system/remoteCellImporter.h>

 #include <helium/xml/xmlLoader.h>
#include <helium/gtk/gtkSpool.h>

#include <helium/robot/frame.h>
#include <helium/vector/array.h>
 #include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/basicClass.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>
#include <helium/core/constReference.h>
 #include <helium/smartpointer/ndref.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>


 #include <helium/callback/tsSignal.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/system/cellDescription.h>
 #include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/system/cellProtocol.h>
 #include <helium/callback/ehSignal.h>
#include <helium/destroy/destroy.h>
 #include <helium/callback/signalType.h>
#include <helium/util/debug.h>

 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/thread/semaphore.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/system/cellClient.h>
#include <helium/net/clientSocket.h>
 #include <helium/thread/thread.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/basicCell.h>
 #include <helium/system/cellSignature.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/core/selectByValue.h>
#include <helium/system/indexableProxy.h>
 #include <helium/core/isSpecialized.h>
#include <helium/system/valueHub.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
 #include <helium/robot/motorFramePlayMode.h>


 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/container/historyStack.h>
 #include <helium/robot/frame.h>
#include <helium/vector/array.h>
 #include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/basicClass.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>
#include <helium/core/constReference.h>
 #include <helium/smartpointer/ndref.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>


 #include <helium/callback/tsSignal.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/system/cellDescription.h>
 #include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/system/cellProtocol.h>
 #include <helium/callback/ehSignal.h>
#include <helium/destroy/destroy.h>
 #include <helium/callback/signalType.h>
#include <helium/util/debug.h>

 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/thread/semaphore.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/system/cellClient.h>
#include <helium/net/clientSocket.h>
 #include <helium/thread/thread.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/basicCell.h>
 #include <helium/system/cellSignature.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/core/selectByValue.h>
#include <helium/system/indexableProxy.h>
 #include <helium/core/isSpecialized.h>
#include <helium/system/valueHub.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>
 #include <helium/gtk/gtkExceptionHandling.h>

 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
 #include <helium/robot/motorFramePlayMode.h>

 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

 #include <helium/system/remoteCellImporter.h>
 #include <helium/convex/drawConvex.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/graphics/keybMouseInteraction.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/math/algebra.h>
 #include <helium/convex/convexModel.h>
 #include <helium/convex/boxNode.h>
 #include <helium/convex/jointParser.h>
 #include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>
 #include <helium/graphics/autoresizingBuffer.h>

 #include <helium/robot/framePolicy.h>
#include <helium/robot/policy.h>
#include <helium/robot/motionParser.h>
#include <app/guide2/guideTeachList.h>
 #include <helium/system/cellClient.h>
#include <helium/net/clientSocket.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/util/debug.h>
 #include <helium/util/ioUtil.h>
#include <helium/core/core.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>

 #include <helium/thread/thread.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/core/basicClass.h>
 #include <helium/thread/semaphore.h>
#include <helium/callback/tsSignal.h>
 #include <helium/net/netSocket.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>
 #include <helium/callback/ehSignal.h>
#include <helium/destroy/destroy.h>

 #include <helium/system/basicCell.h>
#include <helium/system/cellSignature.h>
 #include <helium/core/isSame.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/system/remoteCellImporter.h>
 #include <helium/system/valueProxy.h>
#include <helium/thread/sharedAccess.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/core/selectByValue.h>
#include <helium/robot/application.h>
 #include <helium/robot/config.h>
#include <helium/system/cellMapping.h>

 #include <helium/system/indexableProxy.h>
#include <helium/xml/xmlLoader.h>
 #include <helium/system/helpCell.h>
#include <helium/system/cellDescription.h>
 #include <helium/core/deletable.h>
#include <helium/system/remoteCellExporter.h>

#include <helium/system/valueHub.h>
 #include <helium/core/basicEnum.h>
#include <helium/gtk/gtkProxy.h>

 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/robot/hwInterface.h>
 #include <helium/smartpointer/reference.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledValue.h>
#include <helium/spooling/spooler.h>
 #include <helium/spooling/job.h>

 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 #include <helium/core/triple.h>


 #include <helium/robot/motorPropertyManager.h>
 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>
 #include <helium/robot/timeSource.h>
#include <helium/os/time.h>
 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <app/guide2/point2D.h>
#include <helium/vector/array.h>
 #include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/basicClass.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>
#include <helium/core/constReference.h>
 #include <helium/robot/framePolicy.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>

 #include <helium/robot/policy.h>
#include <helium/system/valueHub.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/callback/tsSignal.h>
#include <helium/callback/ehSignal.h>
#include <helium/gtk/gtkExceptionHandling.h>

#include <app/guide2/guideState.h>
#include <helium/system/cellDescription.h>
 #include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/vector/operations.h>
 #include <helium/system/cellProtocol.h>
#include <helium/util/debug.h>

 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/thread/semaphore.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/system/cellClient.h>
#include <helium/net/clientSocket.h>
 #include <helium/thread/thread.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/basicCell.h>
 #include <helium/system/cellSignature.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/core/selectByValue.h>
#include <helium/system/indexableProxy.h>
 #include <helium/core/isSpecialized.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>

 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

#include <helium/os/time.h>
 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

#include <app/guide2/connectionCell.h>
 #include <helium/system/remoteCellImporter.h>
 #include <helium/callback/staticConnReference.h>
#include <helium/container/historyStack.h>
#include <app/guide2/guideLog.h>

#include <app/guide2/angleEntry.h>
#include <helium/util/mathUtil.h>
 #include <helium/convex/drawConvex.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/graphics/keybMouseInteraction.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/math/algebra.h>
 #include <helium/convex/convexModel.h>
 #include <helium/convex/boxNode.h>
 #include <helium/convex/jointParser.h>
 #include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>
 #include <helium/graphics/autoresizingBuffer.h>
 #include <heliumBased/robot/_sensorCalibManager.h>

#include <helium/graphics/glDraw.h>
 #include <helium/math/filter.h>
#include <heliumBased/robot/gyroCalibManager.h>

#include <app/guide2/readablePostureDrawer.h>
#include <helium/graphics/glUtil.h>
 #include <helium/gtk/gtkExceptionHandling.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>

#include <helium/system/valueHub.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/constReference.h>
#include <helium/core/basicClass.h>
 #include <helium/destroy/destroy.h>
#include <helium/callback/signalType.h>
 #include <helium/callback/tsSignal.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>

#include <helium/callback/ehSignal.h>
 #include <helium/robot/framePolicy.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
#include <helium/robot/policy.h>
#include <helium/robot/motionParser.h>

#include <helium/gtk/gtkExceptionHandling.h>

#include <app/guide2/guideState.h>
#include <helium/system/cellDescription.h>
 #include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cell.h>
#include <helium/system/idTree.h>
 #include <helium/util/stringUtil.h>
#include <helium/vector/operations.h>
 #include <helium/system/cellProtocol.h>
#include <helium/util/debug.h>

 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/thread/semaphore.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/system/cellClient.h>
#include <helium/net/clientSocket.h>
 #include <helium/thread/thread.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/basicCell.h>
 #include <helium/system/cellSignature.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/core/selectByValue.h>
#include <helium/system/indexableProxy.h>
 #include <helium/core/isSpecialized.h>
 #include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/gtkProxy.h>
#include <helium/gtk/gtkSignal.h>

 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/spooling/spooler.h>
#include <helium/spooling/job.h>

 #include <helium/robot/hwInterface.h>
#include <helium/smartpointer/reference.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>

 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>

 #include <helium/robot/motionAdapter.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

#include <app/guide2/connectionCell.h>
 #include <helium/system/remoteCellImporter.h>
#include <helium/callback/staticConnReference.h>
#include <app/guide2/point2D.h>
#include <helium/container/historyStack.h>
#include <app/guide2/guideLog.h>
 #include <helium/convex/drawConvex.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/graphics/keybMouseInteraction.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/math/algebra.h>
 #include <helium/convex/convexModel.h>
 #include <helium/convex/boxNode.h>
 #include <helium/convex/jointParser.h>
 #include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>
 #include <helium/graphics/autoresizingBuffer.h>
 #include <helium/callback/tsSignal.h>
#include <helium/callback/signal.h>
 #include <helium/callback/callback.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/constReference.h>
 #include <helium/core/basicClass.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>
#include <helium/vector/array.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>

 #include <helium/convex/drawConvex.h>
 #include <helium/graphics/glPrimitiveCanvas.h>
 #include <helium/graphics/keybMouseInteraction.h>
 #include <helium/graphics/basicKeybInter.h>
#include <helium/core/basicEnum.h>
 #include <helium/math/algebra.h>
#include <helium/vector/operations.h>
 #include <helium/convex/convexModel.h>
#include <helium/convex/jointParser.h>
 #include <helium/destroy/autoDestroy.h>
 #include <helium/convex/convexModel.h>
 #include <helium/graphics/pixelReader.h>
#include <helium/thread/semaphore.h>
 #include <helium/graphics/autoresizingBuffer.h>
 #include <helium/graphics/glUtil.h>
 #include <helium/vector/array.h>
#include <helium/core/core.h>
 #include <helium/core/exception.h>
#include <helium/core/typenames.h>
 #include <helium/core/debugLevel.h>
#include <helium/core/basicClass.h>
 #include <helium/core/isBaseOf.h>
#include <helium/core/isSame.h>
 #include <helium/core/constReference.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
#include <helium/thread/mutex.h>
 #include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
 #include <helium/core/enum.h>


#include <helium/callback/tsSignal.h>
 #include <helium/callback/signal.h>
#include <helium/callback/callback.h>
 #include <helium/core/core.h>
#include <helium/core/exception.h>
 #include <helium/core/typenames.h>
#include <helium/core/debugLevel.h>
 #include <helium/core/constReference.h>
#include <helium/core/basicClass.h>
 #include <helium/thread/mutex.h>
#include <helium/thread/thread_private.h>
 #include <helium/util/ioUtil.h>
#include <helium/core/enum.h>
 #include <helium/vector/array.h>
#include <helium/core/isBaseOf.h>
 #include <helium/core/isSame.h>

 #include <helium/system/valueHub.h>
#include <helium/destroy/destroy.h>
 #include <helium/callback/signalType.h>
#include <helium/callback/ehSignal.h>
 #include <helium/robot/motorExceptionHandler.h>
 #include <helium/robot/hwStateMonitor.h>
#include <helium/core/slContainer.h>
 #include <helium/callback/wrapInfo.h>
#include <helium/destroy/autoDestroy.h>
 #include <helium/system/signalledArray.h>
#include <helium/system/cellSignal.h>
 #include <helium/core/selectByValue.h>
#include <helium/system/cell.h>
 #include <helium/system/idTree.h>
#include <helium/util/stringUtil.h>
 #include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>
 #include <helium/util/debug.h>
#include <helium/system/cellSignature.h>
 #include <helium/system/basicCell.h>
#include <helium/spooling/spooler.h>
 #include <helium/thread/thread.h>
#include <helium/thread/semaphore.h>
 #include <helium/spooling/job.h>

 #include <helium/os/time.h>
#include <helium/robot/hwInterface.h>
 #include <helium/smartpointer/reference.h>
#include <helium/core/deletable.h>
 #include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>
 #include <helium/robot/motorSpooler.h>
#include <helium/robot/motorInterface.h>
 
 #include <helium/system/signalledValue.h>
 #include <helium/system/cellDescription.h>
#include <helium/system/cellMapping.h>
 #include <helium/system/cellMapping.hpp>
#include <helium/system/volatile.h>
 #include <helium/system/valueProxy.h>
#include <helium/core/isConvertible.h>
 #include <helium/thread/sharedAccess.h>
#include <helium/system/cellClient.h>
 #include <helium/net/clientSocket.h>
#include <helium/net/netSocket.h>
 #include <helium/net/commandSplitter.h>
#include <helium/system/cellClient.hpp>
 #include <helium/system/remoteCell.h>
#include <helium/system/remoteCell.hpp>
 #include <helium/system/indexableProxy.h>
#include <helium/core/isSpecialized.h>

#include <helium/gtk/gtkProxy.h>
 #include <helium/gtk/gtkSignal.h>
#include <helium/gtk/gtkExceptionHandling.h>
 
#include <helium/util/mathUtil.h>
 #include <helium/callback/staticConnReference.h>
 #include "../../app/guide2/guideState.h"
#include <helium/system/protectedValueHub.h>
 #include <helium/system/protectedSetter.h>
#include <helium/core/basicEnum.h>
 #include <helium/gtk/protectedGtkProxy.h>
 #include <helium/system/protectedValueProxy.h>
 #include <helium/robot/motorOffMode.h>

 #include <helium/robot/motorLinearMode.h>
 #include <helium/robot/motorActiveMode.h>
#include <helium/callback/valueWaiter.h>
 
#include <helium/robot/timeSource.h>
 #include <helium/robot/motionAdapter.h>
#include <helium/robot/frame.h>
 #include <helium/smartpointer/ndref.h>
 #include <helium/robot/motorFramePlayMode.h>
 
 #include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorInterface.h>
 #include <helium/container/bufferPipe.h>
#include <helium/container/minimalList.h>
 #include <helium/core/referenceType.h>
#include <helium/robot/genericMotorSign.h>
 #include <helium/robot/motorStructures.h>
#include <helium/system/hexMapping.h>
 #include <helium/robot/knownPosNormalizer.h>
 #include <helium/spooling/spoolerControl.h>
#include <helium/util/moutPrinter.h>
 #include <helium/util/mout.h>
#include <helium/util/mtLog.h>

#include "../../app/guide2/connectionCell.h"

#include "../../app/guide2/point2D.h"
 #include <helium/robot/framePolicy.h>
#include <helium/robot/policy.h>
#include <helium/robot/motionParser.h>
#include <tinyxml/tinyxml.h>
#include "guideTeachList.h"
#include <helium/container/historyStack.h>
