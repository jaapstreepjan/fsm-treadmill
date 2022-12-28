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
 *
 * TO DO:
 *  - Finish running_stop function
 *  - Fix pause state
 *  - Exit diagnostics should show main menu first?
 *  -
 *
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
} treadmillStatus_t; // Need to find out what this is used for.

char * treadmillStatusToText[] =
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
//void S_Locked_onEntry(void);      // LEGACY
//void S_Unlocked_onEntry(void);    // LEGACY
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
event_t resumeButton(void);

// Subsystem1 (simulation) functions
event_t Treadmill(void);
event_t Running_start(void);
event_t Running_stop(void);
event_t Diagnostics_start(void);
event_t Diagnostics_stop(void);
event_t Pause(void);
event_t Resume(void);
event_t Emergency(void);
event_t Emergency_stop(void);
event_t Config_Change(void);
event_t Config_Done(void);

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
    FSM_AddTransition(&(transition_t){ S_PAUSE,       E_RESUME,            S_DEFAULT     });
    FSM_AddTransition(&(transition_t){ S_DEFAULT,     E_CONFIG_CHANGE,     S_ALTERCONFIG });
    FSM_AddTransition(&(transition_t){ S_ALTERCONFIG, E_CONFIG_DONE,       S_DEFAULT     });
    FSM_AddTransition(&(transition_t){ S_DEFAULT,     E_EMERGENCY_START,   S_EMERGENCY   });
    FSM_AddTransition(&(transition_t){ S_EMERGENCY,   E_EMERGENCY_STOP,    S_DEFAULT     });
    FSM_AddTransition(&(transition_t){ S_ALTERCONFIG, E_EMERGENCY_START,   S_EMERGENCY   });
    FSM_AddTransition(&(transition_t){ S_EMERGENCY,   E_EMERGENCY_STOP,    S_ALTERCONFIG });

    FSM_RunStateMachine(S_START, E_INIT);

    // Use this test function to test your model
    // FSM_RevertModel();

    //    FSM_FlushEnexpectedEvents(true);

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
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    event_t nextevent;

    // Display information for user
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tSystem ready!\n", Speed, Inc, Distance);

    // Show user options
    int Navigation;
    Navigation = DCSsimulationSystemInputChar("\nPress P to Pause."
                                              "\nPress C to change config."
                                              "\nPress E to trigger emergency."
                                              "\nPress Q to stop running",
                                              "P" "C" "E" "Q");

    switch (Navigation)
    {
    case 'P':
        nextevent = Pause();
        FSM_AddEvent(nextevent);
        break;
    case 'C':
        nextevent = Config_Change();
        FSM_AddEvent(nextevent);
        break;
    case 'E':
        nextevent = Emergency();
        FSM_AddEvent(nextevent);
        break;
    case 'Q':
        nextevent = Running_stop();
        FSM_AddEvent(nextevent);
    default:
        DSPshow(1,"Invalid input!\nPlease try again!");
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

    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tSystem in Diagnostic mode.\n"
              "\tCleared for maintenance duties.\n", Speed, Inc, Distance);

    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    int Navigation;

    Navigation  = DCSsimulationSystemInputChar("\nPress A to return to default operations.", "A");

    switch (Navigation)
    {
    case 'A':
        Diagnostics_stop();
        S_Default_onEntry();
        break;
    default:
        //        Diagnostics_stop();           // Niet nodig?
        //        S_Default_onEntry();          //
        break;
    }
}

void S_Diagnostics_onExit(void)
{
    // To Do
}

void S_Alterconfig_onEntry(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    // Display information for user
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tChange configuration.\n", Speed, Inc, Distance);

    // Show user information
    int Navigation;
    Navigation = DCSsimulationSystemInputChar("\n"
                                              "Press S to change Speed\n"
                                              "Press I to change Incline\n"
                                              "Press D to change Distance\n"
                                              "Press C to commit Change\n",
                                              "S" "I" "D" "C");

    event_t nextevent;
    switch (Navigation)
    {
    case 'S':
        // change speed here
        break;
    case 'I':
        // chagne Incline here
        break;
    case 'D':
        // change Distance here
        break;
    case 'C':
        nextevent = Config_Done();
        FSM_AddEvent(nextevent);
        break;
    default:
        break;
    }
}

void S_Alterconfig_onExit(void)
{
    // To Do
}

void S_Emergency_onEntry(void)
{
    state_t state;

    // Display information for user
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tEmergency! System halting!.\n", Speed, Inc, Distance);

    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    int Navigation;

    Navigation  = DCSsimulationSystemInputChar("\nPress A to reset.", "A");

    switch (Navigation)
    {
    case 'A':
        Emergency_stop();
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
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    // Initialize variables
    event_t nextevent;
    int response;

    // Show user information
    DSPshow(2,"Treadmill paused.");
    response = DCSsimulationSystemInputChar("Press C to continue", "C");

    switch (response)
    {
    case 'C':
        DSPshow(3,"Resuming operations");
        nextevent = Resume();
        FSM_AddEvent(nextevent);
        break;
    default:
        DCSdebugSystemInfo("Undefined this should not happen");
        DCSdebugSystemInfo("Go to emergency state");
        nextevent = Emergency();
        FSM_AddEvent(nextevent);
    }
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

    Navigation  = DCSsimulationSystemInputChar("\nPress D for diagnostics operations."
                                               "\nPress S for default operations."
                                               "\nPress Q to stop running."
                                               "\n",
                                               "D" "S" "Q");

    switch (Navigation)
    {
    case 'D':
        S_Diagnostics_onEntry();
        break;
    case 'S':
        Running_start();
        break;
    case 'Q':
        Running_stop();
        break;
    default:
        Running_start();
        break;
    }
}

event_t Diagnostics_start(void)
{
    // Start diagnostics state.
    // Set incline, speed and distance to zero.
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

    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    return (E_RUNNING_START);

    S_Default_onEntry();
}

event_t Running_stop(void)
{
    // stopping treadmill with this function
    Speed = 0;
    Inc = 0;

    return (E_RUNNING_STOP);

    S_Standby_onEntry();
}

event_t Pause(void)
{
    // Set speed of treadmill to zero. Keep other options the same.
    TEMP = Speed ;
    Speed = 0;
    Inc = Inc;
    Distance = Distance;

    // Let FSM framework know we're done with the event
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

event_t Emergency_stop(void)
{
    return (E_EMERGENCY_STOP);
}

event_t Config_Change(void)
{
    return (E_CONFIG_CHANGE);
}

event_t Config_Done(void)
{
    // Commit changes to saved configuration

    return (E_CONFIG_DONE);
}

// simulate delay in microseconds
void delay_us(uint32_t d)
{
    DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
    sleep(10000);
}
