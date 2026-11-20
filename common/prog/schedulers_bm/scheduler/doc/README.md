# Scheduler
---
[⮨ Main Page Framework help](../../../../README.md)


### Contents

1. [Introduction](#Introduction)
2. [Requisites](#Requisites)
3. [Scheduler Tables](#SchedulerTables)</br>
    3.1 [Module ID](#ModuleID)</br>
    3.2 [Dependecies](#Dependecies)</br>
    3.3 [Task Table and Priority](#Priority)</br>
    3.4 [Task Handlers](#TaskHandlers)</br>
4. [Comparison with the Old Scheduler](#Comparison)</br>
5. [Appendix A Examples](#Examples)
6. [Appendix B Interface](#Interface)



## 1. Introduction <a name="Introduction"></a>
The job of the scheduler is to call the task handlers of each module according to their priority.
The scheduler replaces the call of task handlers from main loop. The scheduler is also responsible for dependencies check. Before
call the first task handler, the scheduler makes sure that all the modules are initialized. 
The scheduler is able to handle unlimited number of tasks listed in TasksList table. Each task to schedule is an entry in this table 
and will be called frequently from scheduler. The scheduler can be run either in normal mode or optimize mode, which can be selected during compilation time.
<br><br>Each task has three attributes:
* Mode: enabled/disabled
* State: initialized/ not initialized / TASK_TERMINATE
* Priority: High/ Low  

The state and priority may be changed dynamically.  
The state will be set to initialized once a task handler returns TASK_INITIALISED or TASK_TERMINATE.


## 2. Requisites <a name="Requisites"></a>
To use the scheduler the module task handlers must return the initialization state of module.
```c
uint8_t XXX_handleTask(void)
```
Each module should define its dependencies in the header file. Please refer WDT_handleTask Module. 

## 3. Schedule Tables <a name="SchedulerTables"></a>
To schedule the module following information must be defined in corresponding tables

1 Module ID

2 Dependecies

3 Task Table and Priority

4 Task Handlers

### 3.1 Module ID <a name="ModuleID"></a>
The module identifiers must be defined in ModuleID table. Each module must have an unambiguous identifier like MOD_XXX. 

```c
Table location: App/.../prog/scheduler_bm/scheduler/scheduler_cfg.h
---------------------------------------------------------------- 
typedef enum SCH_ModuleIDs
{
    MOD_TIM,          ///< module ID of timer library   
    MOD_BAL,          ///< module ID of dbus bus layer
    MOD_DBPL,         ///< module ID of dbus presentation layer
    MOD_DLL,          ///< module ID of dbus data link layer
    MOD_WDT,          ///< module ID of watchdogtimer
    /*! place your projects modules here */

    TOTAL_MODULES     ///< last entry, do not change
} TaskID;
```
### 3.2 Dependecies <a name="Dependecies"></a>
#### 3.2.1 Dependency define in module
Each module that has to be called from the scheduler must define its dependencies. when the module has no dependencies then empty define has to be placed in the module header. Each module must only have one dependency definition in its header, 
when a module has no dependency definition then temporarily it can be defined in App/.../scheduler_cfg.c

A module with module token XXX depends on a timer (token TIM) has to define the following dependency in the header of the module:
#define DEP_XXX MOD_TIM,


**Examples:**
```c
#define DEP_PHYP MOD_TIM,             /* one dependency */
#define DEP_LED                       /* no dependency  */
#define DEP_DBPL MOD_DLL, MOD_TIM,    /* Two dependency */
```
**NOTE:**</br>
*No comma is needed to terminate empty definition  

#### 3.2.1 Dependency define in Scheduler_cfg.c
The module identifier is known for Scheduler_cfg.c, so an array of dependencies can be defined for each module and each dependency table must be terminated with END_STAMP.
The name of the table is in format XXX_dep where XXX represents the module token. 
```c
Table location: App/.../prog/schedulers_bm/scheduler/scheduler_cfg.c
---------------------------------------------------------------- 
static const TaskID BAL_dep[]                 = {DEPLIB_BAL  END_STAMP};
static const TaskID DBPL_dep[]                = {DEPLIB_DBPL END_STAMP};
static const TaskID DLL_dep[]                 = {DEPLIB_DLL  END_STAMP};
static const TaskID TIM_dep[]                 = {DEP_TIM     END_STAMP};
static const TaskID WDT_dep[]                 = {DEP_WDT     END_STAMP};
/*! place your module dependencies here */
```
Now the DependenciesList pointer table must be filled with dependencies table of the module.
```c
Table location: App/.../prog/schedulers_bm/scheduler/scheduler_cfg.c
---------------------------------------------------------------- 
.DependenciesList =
 {
    [MOD_TIM]       = TIM_dep,
    [MOD_BAL]       = BAL_dep,
    [MOD_DBPL]      = DBPL_dep,
    [MOD_DLL]       = DLL_dep,
    [MOD_WDT]       = WDT_dep,
    /* place your modules dependencies arrays here */
 }
```
### 3.3 Task Table and Priority <a name="Priority"></a>
The task tables (not to confound with the task handler table) define the tasks to be called. Priority for the corresponding module identifier must be filled with appropriate priorities. 
Possible priority levels are HIGH_PRIORITY, LOW_PRIORITY. The scheduler runs all high priorities tasks and then the first low priority task from the table, the same process repeats for the next low priority task. 

```c
Table location: App/.../prog/schedulers_bm/scheduler/scheduler_cfg.c
---------------------------------------------------------------- 
.DependenciesList =
 {
    {MOD_TIM,       HIGH_PRIORITY},
    {MOD_BAL,       HIGH_PRIORITY},
    {MOD_DBPL,      HIGH_PRIORITY},
    {MOD_DLL,       LOW_PRIORITY},
    {MOD_WDT,       LOW_PRIORITY},
    /*! place your project modules and their priorities here */
    {END_STAMP,     PRIO_NONE}
 }

 ```
### 3.4 Task Handlers<a name="TaskHandlers"></a>
The Scheduler need the address of each task handler to call it, so the each module must register its task handler in task handler table.
```c
Table location: App/.../prog/schedulers_bm/scheduler/scheduler_cfg.c
---------------------------------------------------------------- 
.TaskHandlersList =
    {
        [MOD_TIM]   = TIM_handleTask16,  /* TIM_handleTask if no 16Bit timer used */
        [MOD_BAL]   = BAL_handleTask,
        [MOD_DBPL]  = DBPL_handleTask,
        [MOD_DLL]   = DLL_handleTask,
        [MOD_WDT]   = WDT_handleTask,
        /*! place your task handlers here */

    },
 ```
**Prototype of task handler is listed below** which should be describe in module 
```c
uint8_t XXX_handleTask(void)
```
## 4. Comparison with the Old Scheduler <a name="Comparison"></a>
| Features  | New Scheduler  | Old Scheduler |
| :------------ |:---------------| :-----|
| Operational Mode    | Can run in two mode namely the Optimize Performance mode and the Normal mode.In the Optimize Performance mode the Performance and the RAM space are increased.| Only one mode|
| Time Monitor Feature| Time Properties of the task can be monitor including Task Time period, Task Jitter Average, Task Runtime                                    | Does not exist, but still user can get these info by adding few lines of code|
| Debug Mode          | Debug Single module or Debug all modules                                                                                                   | Does not exist|
|Global Functions|SCH_areAllModulesInitialized </br> SCH_isTaskInitialized </br> SCH_changeModulePriority </br> SCH_reactivateModule, SCH_terminateModule </br> SCH_isModuleActive </br> SCH_getActiveTaskID|SCH_bGetInitMainTaskTableStatus </br> SCH_bGetInitModuleStatus </br> SCH_vSetPriority </br> SCH_vSwitchModule </br> Does not exist </br> Does not exist|

## Appendix A Examples <a name="Examples"></a>
### A.1 scheduler_cfg.c
```c
#include "bsh_stdinc.h"
#include "scheduler_cfg.h"
#include "scheduler_types.h"
#include "bal.h"
#include "dbuspresentation.h" 
#include "dbusdll.h"
#include "timerlib.h"
#include "ped_fw/watchdogtimer/watchdogtimer.h"
#ifdef CCSS
#include "CCSS_SyStTask.h"
#endif
#ifdef CCSS_DOM_REF
#include "CCDOM_DomainRefImpl.h"
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
#include "FuncSafe/fSafe.h"      // for handle task's
#include "FuncSafe/fsafeXs.h"    // needed for DEP_xxx
#endif

/***************************************************************************************************
 * DEPENDENCY ARRAYS
 * *************************************************************************************************
 * \brief: Create Module dependency arrays for each module here, terminate the array with END_STAMP
 * *************************************************************************************************
 * \note: No Circular Dependencies allowed
 ***************************************************************************************************/
static const TaskID BAL_dep[]                 = {DEPLIB_BAL  END_STAMP};
static const TaskID DBPL_dep[]                = {DEPLIB_DBPL END_STAMP};
static const TaskID DLL_dep[]                 = {DEPLIB_DLL  END_STAMP};
static const TaskID TIM_dep[]                 = {DEP_TIM     END_STAMP};
static const TaskID WDT_dep[]                 = {DEP_WDT     END_STAMP};
#ifdef CCSS
    static const TaskID CCSS_ptDep[]          = {DEP_CCSS    END_STAMP};
#endif
#ifdef CCSS_DOM_REF
    static const TaskID CCDOM_ptDep[]         = {DEP_CCDOM   END_STAMP};
#endif
#if !defined(TH_FSCOM) && defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED)
    #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
        static const TaskID FSF_REG_ptDep[]   = { DEP_REG END_STAMP};       //!< FSF REG task dependency structure
    #endif
    #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
        static const TaskID FSF_RAM_ptDep[]   = { DEP_FSF END_STAMP};       ///< dependency list of module #MOD_RAM
    #endif
    #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
        static const TaskID FSF_ROM_ptDep[]   = { DEP_ROM END_STAMP};       ///< dependency list of module #MOD_ROM
    #endif
    #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
        static const TaskID FSF_STACK_ptDep[] = { DEP_STACK END_STAMP};   ///< dependency list of module #MOD_STACK
    #endif
    #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
        static const TaskID FSF_ISR_ptDep[]   = { DEP_ISR END_STAMP};       ///< dependency list of module #MOD_ISR
    #endif
    #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
        static const TaskID FSF_FSCLKptDep[]  = { DEP_FSCLK END_STAMP};
    #endif
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    static const TaskID FSF_ptDep[] = { DEP_FSF END_STAMP};
#endif
/*! place your module dependencies here */


/**!
 * \brief       Main Project Catalog Structure
 *
 * \details     It contains following Tables,
 *              TasksList           - Task line-up for scheduling
 *              TaskHandlersList    - TaskHandler functions
 *              DependenciesList    - Module Dependency
 */
struct SCH_ProjectInventory SCH_DefaultProjectCatalogue =
{


    /***************************************************************************************************
     * MAIN TASKS LIST
     * *************************************************************************************************
     * \brief: Add ModuleIDs from SCH_ModuleIDs and assign them appropriate priorities here
     ***************************************************************************************************/
    .TasksList =
    {
        {MOD_TIM,       HIGH_PRIORITY},
        {MOD_BAL,       HIGH_PRIORITY},
        {MOD_DBPL,      HIGH_PRIORITY},
        {MOD_DLL,       LOW_PRIORITY},
        {MOD_WDT,       LOW_PRIORITY},
        #ifdef CCSS
            {MOD_CCSS,  LOW_PRIORITY},
        #endif
        #ifdef CCSS_DOM_REF
            {MOD_CCDOM, LOW_PRIORITY},
        #endif
        #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
            {MOD_REG,   HIGH_PRIORITY},          ///< Functional Safety - register test
        #endif
        #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
            {MOD_RAM,   HIGH_PRIORITY},          ///< Functional Safety - ram test
        #endif
        #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
            {MOD_ROM,   HIGH_PRIORITY},          ///< Functional Safety - rom test
        #endif
        #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
            {MOD_STACK, HIGH_PRIORITY},          ///< Functional Safety - stack test
        #endif
        #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
            {MOD_ISR,   HIGH_PRIORITY},
        #endif
        #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
            {MOD_CLK,   HIGH_PRIORITY},
        #endif
        #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
            {MOD_FSF,   HIGH_PRIORITY},
        #endif
        /*! place your project modules and their priorities here */

        {END_STAMP,     PRIO_NONE}
    },


    /***************************************************************************************************
     * TASK HANDLERS LIST
     * *************************************************************************************************
     * \brief: Add corresponding task handlers for each module here
     ***************************************************************************************************/
    .TaskHandlersList =
    {
        [MOD_TIM]       = TIM_handleTask16,  /* TIM_handleTask if no 16Bit timer used */
        [MOD_BAL]       = BAL_HandleTask,
        [MOD_DBPL]      = DBPL_HandleTask,
        [MOD_DLL]       = DLL_HandleTask,
        [MOD_WDT]       = WDT_handleTask,
        #ifdef CCSS
            [MOD_CCSS]  = CCSS_RunSyStTask,
        #endif
        #ifdef CCSS_DOM_REF
            [MOD_CCDOM] = CCDOM_RunDomRefTask,
        #endif
        #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
            [MOD_REG]   = FSF_ucRegHandleTask,
        #endif
        #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
            [MOD_RAM]   = FSF_ucRamHandleTask,
        #endif
        #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
            [MOD_ROM]   = FSF_ucRomHandleTask,
        #endif
        #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
            [MOD_STACK] = FSF_ucStackHandleTask,
        #endif
        #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
            [MOD_ISR]   = FSF_ucIsrHandleTask,
        #endif
        #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
            [MOD_CLK]   = FSCLK_u8HandleTask,
        #endif
        #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
            [MOD_FSF]   = FSF_ucComHandleTask,
        #endif

        /*! place your task handlers here */

    },


    /***************************************************************************************************
     * DEPENDENCIES LIST
     * *************************************************************************************************
     * \brief: Add above module dependency arrays in the following Table
     * *************************************************************************************************
     * \note: maintain the same order as enumeration SCH_ModuleIDs in scheduler_config.h
     ***************************************************************************************************/
    .DependenciesList =
    {
        [MOD_TIM]       = TIM_dep,
        [MOD_BAL]       = BAL_dep,
        [MOD_DBPL]      = DBPL_dep,
        [MOD_DLL]       = DLL_dep,
        [MOD_WDT]       = WDT_dep,
        #ifdef CCSS
            [MOD_CCSS]  = CCSS_ptDep,
        #endif
        #ifdef CCSS_DOM_REF
            [MOD_CCDOM] = CCDOM_ptDep,
        #endif
        #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
            [MOD_REG]   = FSF_REG_ptDep,
        #endif
        #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
            [MOD_RAM]   = FSF_RAM_ptDep,
        #endif
        #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
            [MOD_ROM]   = FSF_ROM_ptDep,
        #endif
        #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
            [MOD_STACK] = FSF_STACK_ptDep,
        #endif
        #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
            [MOD_ISR]   = FSF_ISR_ptDep,
        #endif
        #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
            [MOD_CLK]   = FSF_FSCLKptDep,
        #endif
        #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
            [MOD_FSF]   = FSF_ptDep,
        #endif

        /* place your modules dependencies arrays here */

    },
};
 ```
### A.2 scheduler_cfg.h
```c
typedef enum SCH_ModuleIDs
{
        MOD_TIM,            ///< module ID of timer library
        MOD_BAL,            ///< module ID of dbus bus layer
        MOD_DBPL,           ///< module ID of dbus presentation layer
        MOD_DLL,            ///< module ID of dbus data link layer
        MOD_WDT,            ///< module ID of watchdogtimer
    #ifdef CCSS
        MOD_CCSS,           ///< module ID of SystemStates Taskhandler
    #endif
    #ifdef CCSS_DOM_REF
        MOD_CCDOM,          ///< module ID of SystemStates Domain Reference implementation. to be replaced with an application specific implementation.
    #endif
    #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
        MOD_REG,            ///< Functional Safety - register test
    #endif
    #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
        MOD_RAM,            ///< Functional Safety - ram test
    #endif
    #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
        MOD_ROM,            ///< Functional Safety - rom test
    #endif
    #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
        MOD_STACK,          ///< Functional Safety - stack test
    #endif
    #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
        MOD_ISR,            ///< Functional Safety - ISR test
    #endif
    #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
        MOD_CLK,            ///< Functional Safety - SYS clock test
    #endif
    #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
        MOD_FSF,
    #endif
        /*! place your projects modules here */

        TOTAL_MODULES /* last entry, do not change */
} TaskID;
 ```
## Appendix B Interface <a name="Interface"></a>

### B.1 SCH_changeModulePriority
**Prototype**&emsp;&emsp;&emsp;&emsp;&emsp;void SCH_changeModulePriority(TtaskId tTaskId, BOOL bPriority)</br>
**Description**&emsp;&emsp;&emsp;&emsp;SCH_changeModulePriority sets the priority of a task.</br>
**Parameters**</br>
module_id:&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;Identifier of module to switch</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;TaskID</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;0&nbsp;...&nbsp;0x7F</br>
module_priority:&emsp;&emsp;&nbsp;Specify the priority to set for module</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;TaskPriority</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;HIGH_PRIORITY, LOW_PRIORITY</br>
**Return Value**</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;void</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;--</br></br>

### B.2 SCH_reactivateModule
**Prototype**&emsp;&emsp;&emsp;&emsp;&emsp;void SCH_reactivateModule(const TaskID module_id)</br>
**Description**&emsp;&emsp;&emsp;&emsp;SCH_reactivateModule Activate the previously deactivated Module.</br>
**Parameters**</br>
module_id:&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;Module Identifier to Activate</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;TaskID</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;0&nbsp;...&nbsp;0x7F</br>

**Return Value**</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;void</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;--</br></br>


### B.3 SCH_getActiveTaskID
**Prototype**&emsp;&emsp;&emsp;&emsp;&emsp;TaskID SCH_getActiveTaskID(void)</br>
**Description**&emsp;&emsp;&emsp;&emsp;SCH_getActiveTaskID returns the current active task ID.</br>
**Parameters**</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;void</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;--</br>
**Return Value**</br>
module_id:&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;Module Identifier of Activate task</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;TaskID</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;0&nbsp;...&nbsp;0x7F</br></br>

### B.4 SCH_terminateModule
**Prototype**&emsp;&emsp;&emsp;&emsp;&emsp;void SCH_terminateModule(const TaskID module_id)</br>
**Description**&emsp;&emsp;&emsp;&emsp;SCH_terminateModule Terminates the module.</br>
**Parameters**</br>
module_id:&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;Module Identifier to Activate</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;TaskID</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;0&nbsp;...&nbsp;0x7F</br>

**Return Value**</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;void</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;--</br></br>

### B.4 SCH_isTaskInitialized
**Prototype**&emsp;&emsp;&emsp;&emsp;&emsp;bool SCH_isTaskInitialized(const TaskID task_id)</br>
**Description**&emsp;&emsp;&emsp;&emsp;SCH_isTaskInitialized returns InitState of a module.</br>
**Parameters**</br>
module_id:&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;Module Identifier</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;TaskID</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;0&nbsp;...&nbsp;0x7F</br>

**Return Value**</br>
state:&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Module Identifier Init state</br>
&emsp;&emsp;Type:&emsp;&emsp;&emsp;&emsp;&emsp;BOOL</br>
&emsp;&emsp;Range:&emsp;&emsp;&emsp;&emsp;&nbsp;True&nbsp;or&nbsp;False</br></br>

[⮨ Main Page Framework help](../../../../README.md)