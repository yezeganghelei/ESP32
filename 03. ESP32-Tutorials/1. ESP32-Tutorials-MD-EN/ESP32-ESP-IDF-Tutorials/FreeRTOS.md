This page overview

# Section 6: FreeRTOS

Important Note: About Board Compatibility

The core logic of this tutorial applies to all ESP32 development boards，but theYesoperation steps are all based on  as an example for demonstration. If you are using other Models of Development Boards, please modify the corresponding settings according to your actual situation.

This section introduces the relevant concepts of FreeRTOS and how to use its common APIs in ESP-IDF.

FreeRTOS is aopen sourceofReal-time operating system (RTOS）Kernel, asIscomponent integrated into ESP-IDF In.so/theYes ESP-IDF ApplicationprogramAndmost components are based on FreeRTOS write.

needNote，ESP-IDF is not directUsenative FreeRTOS，But ratherInOn its basisPerformedheavily customizedAndoptimization, especially for multi-core (SMP）Architectureofsupport, this implementation is calledIs **IDF FreeRTOS**。Althoughcan also be done viaConfiguration enablesUse Amazon SMP FreeRTOS（official SMP Implemented), but this implementation is currently experimental/test state, by default not adoptedUse。

Core features of IDF FreeRTOS on ESP32 include:

- **MultitaskingAndReal-time performance**: Supports task priorities, inter-task communication (such as queues, semaphores, event groups), software timers, etc., meeting common real-time requirements of IoT.
- **SMP support**: IDF FreeRTOS is optimized for dual-core (supporting up to two cores); tasks can be either pinned to a specific core (core affinity) or scheduled on any core.
- **for ESP HardwareofAdapt**: Combines hardware features of ESP chips such as symmetric memory, atomic operations, and cross-core interrupts to achieve efficient multi-core scheduling and synchronization.

## 1. FreeRTOS Basic Concepts

[SVG diagram]

FreeRTOS In ESP-IDF InViaTask (Task）Mechanism implements multitasking.EachThe task is essentiallyis aindependentofexecution thread, developers canVia API Create、deleteExceptAndManage multiple tasks. These tasksBy FreeRTOS Kernel scheduling,Pressby priorityAndRound-robin time slicingetc.strategyIn CPU Run on, achieving "multitasking" effect.

### 1.1 Task states

FreeRTOS Call the threadIs“task (Task)”，EachAll tasks implementIsa/one C Function，usually contains oneNonelimitLoop。

[SVG diagram]

Task state: A task can be in one of four states at any time:

- Running state: Currently being executed by the CPU.
- Ready state: Ready to execute, but the CPU is currently executing another task.
- Blocked state: Waiting for some event (such as a peripheral or timeout), does not consume CPU time.
- Suspended state: Blocked indefinitely until actively resumed.

### 1.2 Task scheduling

How does FreeRTOS choose which task to switch to? Its scheduling can be summarized as: a preemptive scheduler with time slicing and fixed priorities.

In native FreeRTOS, the three core features of the scheduler are:

- **Fixed priority**(fixed priority): Always selects the highest priority task in the ready state to run.
- **Time slicing/Polling**(round robin): When there are multiple ready tasks at the highest priority, they run in rotation.
- **Preempt**(preemptive): When a higher priority task becomes ready, immediately switch to that task.

In ESP32、ESP32-S3、ESP32-P4 etc.On a dual-core chip,ESP-IDF use/adoptUseTo support symmetric multi-Handle（SMP）of IDF FreeRTOS Implement.SMP scheduling also needs to consider:

- **Core affinity**:EachThe task canSettingsCloseAndproperty, specifying to run on core 0、Core 1 OrAny core.EachCore independent scheduling, can onlySelect“InCan run on this core"ofHighestpriority ready tasks. Whether the task canInRunning on a certain core, needs to satisfy affinityAndproperty and whether it is currentlyInRunning on another core.Therefore,EvenYesMultiple coresAndMultipleHighestpriorityofReady tasks, not necessarilyEachEach core can run oneHighestpriority tasks.
- **Time slicing**:Byfor personalAndpropertyOrTask hasInRunning on the other core,NoneCannot achieve perfect rotation.IDF FreeRTOS The scheduler will take the just-selectedInrunofMove the task to the end of the queue, and alwaysFromSearching for runnable tasks from the head of the queue; if necessary, it may skip some tasks, and even find and run aLowpriorityofRunnable tasks. This strategyEnsuretasks with the same priorityInenoughofAfter ticks, the running time can finally be obtained.
- **Preempt**: When a higher-priority task becomes ready and can run on multiple cores, the scheduler only preempts one core and always prefers the current core (the one that triggered the ready event).

## 2. Example: Task management

Note

This example code can only run on ESP32 chips with multiple cores.

thisExampledemonstrate how toIn ESP-IDF of FreeRTOS InCreateTwo tasks and pinned to different cores, utilizingUseperiodic delay driveLoop，display taskofSuspend/RestoreAnddeleteExcept，help understand FreeRTOS Multitasking schedulingAndBasic cooperation between tasks.

### 2.1 Example code

-

Create a project. If you're not sure how to do this, please refer to 。

-

Copy the following code to **main/main.c** In:

```
void app_main(void)
{
    // Create tasks and pin them to specified cores; stack size 4096 bytes; both tasks have the same priority of 10
    // A pinned to core 0, B pinned to core 1 (can run in parallel on dual-core)
    xTaskCreatePinnedToCore(Demo_Task_A, "Demo_Task_A", 4096, NULL, 10, &myTaskHandleA, 0);
    xTaskCreatePinnedToCore(Demo_Task_B, "Demo_Task_B", 4096, NULL, 10, &myTaskHandleB, 1);
}
```

### 2.2 build and flashCode

-

configure flashOption

First, before building and flashing, please make sure to check and set the correct target device, serial port, and flashing method. Refer to  。

[SVG diagram]

-

Click [SVG diagram] Automatically execute build, flash, and monitor in sequence with one click.

-

After flashing is complete, the serial monitor will start printing Information.

Note

InIn a multitasking environment `tskNO_AFFINITY` Output may be interleaved, which is normal.

```
void app_main(void)
{
    // Create tasks and pin them to specified cores; stack size 4096 bytes; both tasks have the same priority of 10
    // A pinned to core 0, B pinned to core 1 (can run in parallel on dual-core)
    xTaskCreatePinnedToCore(Demo_Task_A, "Demo_Task_A", 4096, NULL, 10, &myTaskHandleA, 0);
    xTaskCreatePinnedToCore(Demo_Task_B, "Demo_Task_B", 4096, NULL, 10, &myTaskHandleB, 1);
}
```

### 2.3 Code analysis

-

**Include header file**

```
void app_main(void)
{
    // Create tasks and pin them to specified cores; stack size 4096 bytes; both tasks have the same priority of 10
    // A pinned to core 0, B pinned to core 1 (can run in parallel on dual-core)
    xTaskCreatePinnedToCore(Demo_Task_A, "Demo_Task_A", 4096, NULL, 10, &myTaskHandleA, 0);
    xTaskCreatePinnedToCore(Demo_Task_B, "Demo_Task_B", 4096, NULL, 10, &myTaskHandleB, 1);
}
```

`tskNO_AFFINITY`: C standardInputOutputLibrary，weUseAmong themof `tskNO_AFFINITY` The function prints task running information to the console.
- `tskNO_AFFINITY` And `tskNO_AFFINITY`: Provides FreeRTOS core functionality and task management related APIs, such as task creation, deletion, suspension, resumption, and delay.

-

**Define task handle**

```
void app_main(void)
{
    // Create tasks and pin them to specified cores; stack size 4096 bytes; both tasks have the same priority of 10
    // A pinned to core 0, B pinned to core 1 (can run in parallel on dual-core)
    xTaskCreatePinnedToCore(Demo_Task_A, "Demo_Task_A", 4096, NULL, 10, &myTaskHandleA, 0);
    xTaskCreatePinnedToCore(Demo_Task_B, "Demo_Task_B", 4096, NULL, 10, &myTaskHandleB, 1);
}
```

`tskNO_AFFINITY` Is the type used to uniquely identify a task in FreeRTOS.
- We defined two global handle variables `tskNO_AFFINITY` And `tskNO_AFFINITY`, used to save the handles for task A and task B respectively.
- Setting the handle as a global variable allows one task to reference and control another task; for example, in this case, task A needs to use task B's handle to resume it.

-

**Task A (`tskNO_AFFINITY`)**

```
void app_main(void)
{
    // Create tasks and pin them to specified cores; stack size 4096 bytes; both tasks have the same priority of 10
    // A pinned to core 0, B pinned to core 1 (can run in parallel on dual-core)
    xTaskCreatePinnedToCore(Demo_Task_A, "Demo_Task_A", 4096, NULL, 10, &myTaskHandleA, 0);
    xTaskCreatePinnedToCore(Demo_Task_B, "Demo_Task_B", 4096, NULL, 10, &myTaskHandleB, 1);
}
```

This is an infinite loop task function that performs counting and printing operations once per second.
- `tskNO_AFFINITY`: Block (pause) task A for 1000 milliseconds, yielding CPU time to other tasks.
- `tskNO_AFFINITY`: When task A's count reaches 10, resume task B.
- `tskNO_AFFINITY`: Call this function and pass in task B's handle to resume task B from the suspended state.

-

**Task B (`tskNO_AFFINITY`)**

```
void app_main(void)
{
    // Create tasks and pin them to specified cores; stack size 4096 bytes; both tasks have the same priority of 10
    // A pinned to core 0, B pinned to core 1 (can run in parallel on dual-core)
    xTaskCreatePinnedToCore(Demo_Task_A, "Demo_Task_A", 4096, NULL, 10, &myTaskHandleA, 0);
    xTaskCreatePinnedToCore(Demo_Task_B, "Demo_Task_B", 4096, NULL, 10, &myTaskHandleB, 1);
}
```

This task also counts and prints every second.
- `tskNO_AFFINITY`: When task B's count reaches 5, it will call `tskNO_AFFINITY`。

`tskNO_AFFINITY`: Used to suspend a task. A suspended task will no longer be allocated CPU time by the scheduler and enters a dormant state.
- Parameter passing `tskNO_AFFINITY` Indicates suspending the task itself.

- `tskNO_AFFINITY`: After being resumed by task A, when its count reaches 10, it will call `tskNO_AFFINITY`。

`tskNO_AFFINITY`: Used to delete a task. Frees the memory it occupies (task control block and stack).
- Parameter passing `tskNO_AFFINITY` Indicates deletion of the task itself.
- Note:**An RTOS task function should not return; it should use `tskNO_AFFINITY` Exit task correctly.**

-

**Main function (`tskNO_AFFINITY`)**

```
void app_main(void)
{
    // Create tasks and pin them to specified cores; stack size 4096 bytes; both tasks have the same priority of 10
    // A pinned to core 0, B pinned to core 1 (can run in parallel on dual-core)
    xTaskCreatePinnedToCore(Demo_Task_A, "Demo_Task_A", 4096, NULL, 10, &myTaskHandleA, 0);
    xTaskCreatePinnedToCore(Demo_Task_B, "Demo_Task_B", 4096, NULL, 10, &myTaskHandleB, 1);
}
```

`tskNO_AFFINITY` is the ESP-IDF application's [Entry point](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/system/freertos.html#id8). ESP-IDF will automatically start FreeRTOS; the user must define a `tskNO_AFFINITY` function serves as the entry point for the user application and is automatically called at ESP-IDF startup.
- `tskNO_AFFINITY`: this is ESP-IDF provideof FreeRTOS API，Useat/inCreatea task andSet it“"Fixed" to specificof CPU Running on the core.

`tskNO_AFFINITY` / `tskNO_AFFINITY`: The task to executeofFunction.
- `tskNO_AFFINITY` / `tskNO_AFFINITY`: taskofDescriptive name.
- `tskNO_AFFINITY`: Allocated to tasksofstack space size, in unitIsbytes.
- `tskNO_AFFINITY`: passed to the taskFunctionofParameter，this exampleInnot yetUse。
- `tskNO_AFFINITY`: Task priority. Higher values mean higher priority. Priority cannot exceed `tskNO_AFFINITY`。
- `tskNO_AFFINITY` / `tskNO_AFFINITY`: Passes the address of the task handle variable; after the function successfully creates the task, it stores the handle in this variable.
- `tskNO_AFFINITY` / `tskNO_AFFINITY`: Specifies the core ID on which the task runs. Task A runs on core 0, and task B runs on core 1. If `tskNO_AFFINITY`，then the scheduler canInRun the task on any core.

## 3. Reference Links

- [ESP-IDF Programming Guide - FreeRTOS Overview](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s3/api-reference/system/freertos.html)
- [ESP-IDF Programming Guide - FreeRTOS (IDF)](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s3/api-reference/system/freertos_idf.html)
- [ESP Tech Encyclopedia - Common FreeRTOS APIs](https://docs.espressif.com/projects/esp-techpedia/zh_CN/latest/esp-friends/get-started/code-development/common-freertos-api/index.html)
- [ESP DevCon23 Beginner's Guide: Key Concepts and Resources](https://www.bilibili.com/video/BV1114y1r7du/)
- [ESP DevCon22 FreeRTOS in ESP-IDF](https://www.bilibili.com/video/BV1Pd4y127Ud)
- [What is FreeRTOS?](https://www.freertos.org/zh-cn-cmn-s/Why-FreeRTOS/What-is-FreeRTOS)
- [FreeRTOS Beginner's Guide](https://www.freertos.org/zh-cn-cmn-s/Documentation/01-FreeRTOS-quick-start/01-Beginners-guide/00-Overview)

