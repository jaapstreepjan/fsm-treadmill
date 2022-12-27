/*!
 * The C program written by C. van Dreumel en J.J. Groenendijk is an
 * implementation of a finite state machine (FSM) model of a treadmill.
 * The FSM is a mathematical model of computation that represents the behavior
 * of a system by specifying the possible states it can be in, the transitions
 * between those states, and the actions that are performed when transitioning
 * between states. In the case of a treadmill, the states might include
 * "stopped," "running at a low speed," "running at a high speed," and so on.
 * The transitions between states would be determined by the actions of the
 * user, such as pressing buttons to change the speed or incline of the
 * treadmill. The actions performed by the FSM would be the physical movements
 * of the treadmill belt and any accompanying changes in the display or other
 * output. This program provides a precise and rigorous way of modeling the
 * behavior of a treadmill, which can be useful for understanding how the
 * treadmill works, testing its performance, and potentially even improving
 * its design.
 */

// Standard C libraries
#include <stdio.h>
#include <unistd.h>

// Finite State Machine library
#include "fsm_functions/fsm.h"

// Development Console libraries
#include "console_functions/keyboard.h"
#include "console_functions/display.h"
#include "console_functions/devConsole.h"

typedef enum {
    INIT,                ///< Used for initialisation of an event variable
    STANDBY,
    DEFAULT,
    DIAGNOSTICS,
    ALTERCONFIG,
    PAUSE,
    EMERGENCY,
} lockStatus_t; //?

char * lockStatusToText[] =
{
    "INIT",                ///< Used for initialisation of an event variable
    "STANDBY",
    "DEFAULT",
    "DIAGNOSTICS",
    "ALTERCONFIG",
    "PAUSE",
    "EMERGENCY",
};

extern char * eventEnumToText[];
extern char * stateEnumToText[];

event_t event;
state_t state;

// Local function prototypes State related

void S_Init_onEntry(void);
void S_Init_onExit(void);

void S_Locked_onEntry(void);
void S_Unlocked_onEntry(void);

void S_Standby_onEntry(void);
void S_Standby_onExit(void);
void S_Default_onEntry(void);
void S_Default_onExit(void);
void S_Diagnostics_onEntry(void);
void S_Diagnostics_onExit(void);
void S_Alterconfig_onEntry(void);
void S_Alterconfig_onExit(void);
void S_Emergency_onEntry(void);
void S_Emergency_onExit(void);
void S_Pause_onEntry(void);
void S_Pause_onExit(void);

// Global used viarable
// Deze naar main verplaatsen?
float Speed;
float Inc;
float Distance;
float TEMP;
// Subsystem initialization (simulation) functions
event_t InitialiseSubsystems(void);

// Subsystem initialization (simulation) functions
// TO DO

// Subsystem1 (simulation) functions
event_t Treadmill(void);
event_t Running_start(void);
event_t Diagnostics_start(void);
event_t Diagnostics_stop(void);
event_t Pause(void);
event_t Resume(void);
event_t Emergency(void);
event_t Emergenct_stop(void);
event_t Config_Change(void);

// Helper function example
void delay_us(uint32_t d);

// Main
int main(void)
{

    // Define the state machine model
    // First the state and the pointer to the onEntry and onExit functions
    //           State                            onEntry()              onExit()
    FSM_AddState(S_START,      &(state_funcs_t){  NULL,                  NULL                   });
    FSM_AddState(S_INIT,       &(state_funcs_t){  S_Init_onEntry,        NULL                   });
    FSM_AddState(S_STANDBY,    &(state_funcs_t){  S_Standby_onEntry,     NULL                   });
    FSM_AddState(S_DEFAULT,    &(state_funcs_t){  S_Default_onEntry,     NULL                   });
    FSM_AddState(S_DIAGNOSTICS,&(state_funcs_t){  S_Diagnostics_onEntry, NULL                   });
    FSM_AddState(S_ALTERCONFIG,&(state_funcs_t){  S_Alterconfig_onEntry, NULL                   });
    FSM_AddState(S_EMERGENCY,  &(state_funcs_t){  S_Emergency_onEntry,   NULL                   });
    FSM_AddState(S_PAUSE,      &(state_funcs_t){  S_Pause_onEntry,       NULL                   });

    // Second the transitions
    //                                 From           Event                To
    FSM_AddTransition(&(transition_t){ S_START,       E_INIT,              S_INIT        });
    FSM_AddTransition(&(transition_t){ S_INIT,        E_TREADMILL,         S_STANDBY     });
    FSM_AddTransition(&(transition_t){ S_STANDBY,     E_RUNNING_START,     S_DEFAULT     });
    FSM_AddTransition(&(transition_t){ S_DEFAULT,     E_RUNNING_STOP,      S_STANDBY     });
    FSM_AddTransition(&(transition_t){ S_STANDBY,     E_DIAGNOSTICS_START, S_DIAGNOSTICS });
    FSM_AddTransition(&(transition_t){ S_DIAGNOSTICS, E_DIAGNOSTICS_STOP,  S_STANDBY     });
    FSM_AddTransition(&(transition_t){ S_DEFAULT,     E_PAUSE,             S_PAUSE       });
    FSM_AddTransition(&(transition_t){ S_DEFAULT,     E_CONFIG_CHANGE,     S_ALTERCONFIG });
    FSM_AddTransition(&(transition_t){ S_ALTERCONFIG, E_CONFIG_DONE,       S_DEFAULT     });
    FSM_AddTransition(&(transition_t){ S_DEFAULT,     E_EMERGENCY_START,   S_EMERGENCY   });
    FSM_AddTransition(&(transition_t){ S_EMERGENCY,   E_EMERGENCY_STOP,    S_DEFAULT     });
    FSM_AddTransition(&(transition_t){ S_ALTERCONFIG, E_EMERGENCY_START,   S_EMERGENCY   });
    FSM_AddTransition(&(transition_t){ S_EMERGENCY,   E_EMERGENCY_STOP,    S_ALTERCONFIG });

    FSM_RunStateMachine(S_START, E_INIT);

    /// Use this test function to test your model
    /// FSM_RevertModel();

    return 0;
}

// Local function prototypes State related

void S_Init_onEntry(void)
{
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = InitialiseSubsystems();

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_Standby_onEntry(void)
{
    event_t nextevent;

    nextevent = Treadmill();

    FSM_AddEvent(nextevent);

    // To Do
}

void S_Standby_onExit(void)
{
    // To Do
}

void S_Default_onEntry(void)
{
    state_t state;

    // Display information for user
    DSPshow(2,"Speed: %f Km/H", Speed);
    DSPshow(3,"Inclination: %f %%", Inc);
    DSPshow(4,"Distance: %f M", Distance);
    DSPshow(5,"All systems go!.");

    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    int Navigation;

    Navigation  = DCSsimulationSystemInputChar("enter A to pause", "A" "B" "C");

    switch (Navigation)
    {
    case 'A':
        Pause();
        S_Pause_onEntry();
        break;
    case 'B':
        Config_Change();
        S_Alterconfig_onEntry();
        break;
    case 'C':
        Emergency();
        S_Emergency_onEntry();
        break;
    default:
        break;
    }    // To Do
}

void S_Default_onExit(void)
{
    // To Do
}

void S_Diagnostics_onEntry(void)
{
    event_t nextevent;

    nextevent = Diagnostics_start();

    FSM_AddEvent(nextevent);

    DSPshow(2,"Speed: %f Km/H", Speed);
    DSPshow(3,"Inclination: %f %%", Inc);
    DSPshow(4,"Distance: %f M", Distance);
    DSPshow(5,"System in Diagnostic mode please perform duties.");

    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    int Navigation;

    Navigation  = DCSsimulationSystemInputChar("enter A to return to default operations", "A");

    switch (Navigation)
    {
    case 'A':
        Diagnostics_stop();
        S_Default_onEntry();
        break;
    default:
        Diagnostics_stop();
        S_Default_onEntry();
        break;
    }
}

void S_Diagnostics_onExit(void)
{
    // To Do
}

void S_Alterconfig_onEntry(void)
{
    state_t state;

    // Display information for user
    DSPshow(2,"Speed: %f Km/H", Speed);
    DSPshow(3,"Inclination: %f %%", Inc);
    DSPshow(4,"Distance: %f M", Distance);
    DSPshow(5,"All systems go!.");

    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    int Navigation;

    Navigation  = DCSsimulationSystemInputChar("enter A to commit change", "A");

    switch (Navigation)
    case 'A':

        break;
        // To Do
}

void S_Alterconfig_onExit(void)
{
    // To Do
}

void S_Emergency_onEntry(void)
{
    state_t state;

    // Display information for user
    DSPshow(2,"Speed: %f Km/H", Speed);
    DSPshow(3,"Inclination: %f %%", Inc);
    DSPshow(4,"Distance: %f M", Distance);
    DSPshow(5,"Emergency! all systems shut down.");

    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    int Navigation;

    Navigation  = DCSsimulationSystemInputChar("enter A to reset", "A");

    switch (Navigation)
    {
    case 'A':
        Emergenct_stop();
        Running_start();
        break;
    }
    // To Do
}

void S_Emergency_onExit(void)
{
    // To Do
}

void S_Pause_onEntry(void)
{
    state_t state;

    // Display information for user
    DSPshow(2,"Speed: %f Km/H", Speed);
    DSPshow(3,"Inclination: %f %%", Inc);
    DSPshow(4,"Distance: %f M", Distance);
    DSPshow(5,"Treadmill on pause.");

    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    int Navigation;

    Navigation  = DCSsimulationSystemInputChar("enter A to resume", "A");

    switch (Navigation)
    {
    case 'A':
        Resume();
        S_Default_onEntry();
        break;
    default:
        break;
        // To Do
    }
}
void S_Pause_onExit(void)
{
    // To Do
}

// Subsystem (simulation) functions
event_t InitialiseSubsystems(void)
{
    state_t state;
    DSPinitialise();
    DSPshowDisplay();
    KYBinitialise();

    state = FSM_GetState();
    DCSdebugSystemInfo("S_Init_onEntry:");
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);
    DSPshow(2,"System Initialized No errors");

    return(E_TREADMILL);
}

event_t	Treadmill(void)
{
    int Navigation;

    Navigation  = DCSsimulationSystemInputChar("Hit D for diagnostics operations or S for default operations", "D" "S");

    switch (Navigation)
    {
    case 'D':
        S_Diagnostics_onEntry();
        break;
    case 'S':
        Running_start();
        break;
        //  TODO
        //  case 'Q':
        //      Running_stop();
        //      break;
    default:
        Running_start();
        break;
    }
}

event_t Diagnostics_start(void)
{
    Speed = 0;
    Inc = 0;
    Distance = 0;

    return (E_DIAGNOSTICS_START);
}

event_t Diagnostics_stop(void)
{
    Speed = 0.8;
    Inc = 0;
    Distance = 0;

    return (E_DIAGNOSTICS_STOP);
}

event_t Running_start(void)
{
    // setting starting values
    Speed = 0.8;
    Inc = 0;
    Distance = 0;

    return (E_RUNNING_START);

    S_Default_onEntry();
}

event_t Pause(void)
{
    TEMP = Speed ;
    Speed = 0;
    Inc = Inc;
    Distance = Distance;

    return (E_PAUSE);
}

event_t Resume(void)
{
    Speed = TEMP;
    TEMP = 0;
    return (E_RESUME);
}

event_t Emergency(void)
{
    Speed = 0;
    Inc = 0;

    return (E_EMERGENCY_START);
}

event_t Emergenct_stop(void)
{
    return (E_EMERGENCY_STOP);
}

event_t Config_Change(void)
{
    return (E_CONFIG_CHANGE);
}
// simulate delay in microseconds
void delay_us(uint32_t d)
{
    DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
    sleep(10000);
}
