/************************************************************************************

Filename    :   OVR_OSX_DeviceManager.h
Content     :   OSX specific DeviceManager header.
Created     :   March 14, 2013
Authors     :   Lee Cooper

Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR SDK License Version 2.0 (the "License"); 
you may not use the Oculus VR SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#ifndef OVR_OSX_DeviceManager_h
#define OVR_OSX_DeviceManager_h

#include "OVR_DeviceImpl.h"

#include "Kernel/OVR_Timer.h"

#include <IOKit/hid/IOHIDManager.h>
#include <CoreGraphics/CGDirectDisplay.h>
#include <CoreGraphics/CGDisplayConfiguration.h>


namespace OVR { namespace OSX {

class DeviceManagerThread;

//-------------------------------------------------------------------------------------
// ***** OSX DeviceManager

class DeviceManager : public DeviceManagerImpl
{
public:
    DeviceManager();
    ~DeviceManager();

    // Initialize/Shutdown manager thread.
    virtual bool Initialize(DeviceBase* parent);
    virtual void Shutdown();

    virtual ThreadCommandQueue* GetThreadQueue();
    virtual ThreadId GetThreadId() const;
    
    virtual DeviceEnumerator<> EnumerateDevicesEx(const DeviceEnumerationArgs& args);

    virtual bool  GetDeviceInfo(DeviceInfo* info) const;

protected:
    static void displayReconfigurationCallBack (CGDirectDisplayID display,
                                                CGDisplayChangeSummaryFlags flags,
                                                void *userInfo);
  
public: // data
    Ptr<DeviceManagerThread> pThread;
};

//-------------------------------------------------------------------------------------
// ***** Device Manager Background Thread

class DeviceManagerThread : public Thread, public ThreadCommandQueue
{
    friend class DeviceManager;
    enum { ThreadStackSize = 32 * 1024 };
public:
    DeviceManagerThread();
    ~DeviceManagerThread();

    virtual int Run();

    // ThreadCommandQueue notifications for CommandEvent handling.
    virtual void OnPushNonEmpty_Locked()
    {
        CFRunLoopSourceSignal(CommandQueueSource);
        CFRunLoopWakeUp(RunLoop);
    }
    
    virtual void OnPopEmpty_Locked()     {}


    // Notifier used for different updates (EVENT or regular timing or messages).
    class Notifier  
    {
    public:

        // Called when timing ticks are updated. // Returns the largest number of microseconds
        // this function can wait till next call.
        virtual UInt64  OnTicks(UInt64 ticksMks)
        { OVR_UNUSED1(ticksMks);  return Timer::MksPerSecond * 1000; }
    };
 
    // Add notifier that will be called at regular intervals. 
    bool AddTicksNotifier(Notifier* notify);
    bool RemoveTicksNotifier(Notifier* notify);

    CFRunLoopRef        GetRunLoop()
    { return RunLoop; }
    
    void                Shutdown();
private:
    CFRunLoopRef        RunLoop;

    CFRunLoopSourceRef  CommandQueueSource;
    
    static void staticCommandQueueSourceCallback(void* pContext);
    void commandQueueSourceCallback();

    Event               StartupEvent;
    
    // Ticks notifiers. Used for time-dependent events such as keep-alive.
    Array<Notifier*>    TicksNotifiers;
};

}} // namespace OSX::OVR

#endif // OVR_OSX_DeviceManager_h
