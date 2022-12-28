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

// Subsystem1 (simulation) functions
// EF_ prefix is used for Event Functions
event_t Treadmill(void);
event_t EF_RUNNING_START(void);
event_t EF_RUNNING_STOP(void);
event_t EF_DIAGNOSTICS_START(void);
event_t EF_DIAGNOSTICS_STOP(void);
event_t EF_PAUSE(void);
event_t EF_RESUME(void);
event_t EF_EMERGENCY_START(void);
event_t EF_EMERGENCY_STOP(void);
event_t EF_CONFIG_CHANGE(void);
event_t EF_CONFIG_DONE(void);

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
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("New state: %s", stateEnumToText[state]);

    // Display information for user
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tChange configuration.\n", Speed, Inc, Distance);

    // Show user options
    event_t nextevent;
    int Navigation;
    Navigation  = DCSsimulationSystemInputChar("\n"
                                               "Press D for diagnostics\n"
                                               "Press S for default running\n",
                                               "D" "S");

    switch (Navigation)
    {
    case 'D':       // Go to state S_DIAGNOSTICS
        nextevent = EF_DIAGNOSTICS_START();
        FSM_AddEvent(nextevent);
        break;
    case 'S':       // Go to state S_DEFAULT
        nextevent = EF_RUNNING_START();
        FSM_AddEvent(nextevent);
        break;
    default:        // Show warning here about invalid input
        DSPshow(1,"Invalid input!\nPlease try again!");
        break;
    }
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
    DCSdebugSystemInfo("New state: %s", stateEnumToText[state]);

    // Display information for user
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tSystem ready!\n", Speed, Inc, Distance);

    // Show user options
    event_t nextevent;
    int Navigation;
    Navigation = DCSsimulationSystemInputChar("\n"
                                              "Press P to Pause\n"
                                              "Press C to change config\n"
                                              "Press E to trigger emergency\n"
                                              "Press Q to stop running\n",
                                              "P" "C" "E" "Q");

    switch (Navigation)
    {
    case 'P':
        nextevent = EF_PAUSE();
        FSM_AddEvent(nextevent);
        break;
    case 'C':
        nextevent = EF_CONFIG_CHANGE();
        FSM_AddEvent(nextevent);
        break;
    case 'E':
        nextevent = EF_EMERGENCY_START();
        FSM_AddEvent(nextevent);
        break;
    case 'Q':
        nextevent = EF_RUNNING_STOP();
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
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("New state: %s", stateEnumToText[state]);

    // Show user information
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tDiagnostic mode\n"
              "\tCleared for maintenance duties.\n", Speed, Inc, Distance);

    // Show user options
    event_t nextevent;
    int Navigation;
    Navigation  = DCSsimulationSystemInputChar("\n"
                                               "Press O for Other things\n"
                                               "Press Q to Quit diagnostics\n",
                                               "Q" "O");

    switch (Navigation)
    {
    case 'Q':
        nextevent = EF_DIAGNOSTICS_STOP();
        FSM_AddEvent(nextevent);
        break;
    case 'O':
        // Other things here that are Diagnostics related
        break;
    default:
        DSPshow(1,"Invalid input!\nPlease try again!");
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
    DCSdebugSystemInfo("New state: %s", stateEnumToText[state]);

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
                                              "Press E for Emergencies\n"
                                              "Press C to commit Change\n",
                                              "S" "I" "D" "E" "C");

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
    case 'E':
        nextevent = EF_EMERGENCY_START();
        FSM_AddEvent(nextevent);
        break;
    case 'C':
        nextevent = EF_CONFIG_DONE();
        FSM_AddEvent(nextevent);
        break;
    default:
        DSPshow(1,"Invalid input!\nPlease try again!");
        break;
    }
}

void S_Alterconfig_onExit(void)
{
    // To Do
}

void S_Emergency_onEntry(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Current state: %s", stateEnumToText[state]);

    // Show user information
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tEmergency mode\n",
            Speed, Inc, Distance);

    // Show user options
    event_t nextevent;
    int Navigation;
    Navigation  = DCSsimulationSystemInputChar("\n"
                                               "Press O for Other things\n"
                                               "Press Q to Quit emergency\n",
                                               "Q" "O");

    switch (Navigation)
    {
    case 'Q':
        nextevent = EF_EMERGENCY_STOP();
        FSM_AddEvent(nextevent);
        break;
    case 'O':
        // Other things here that are Emergency related
        break;
    default:
        DSPshow(1,"Invalid input!\nPlease try again!");
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
        nextevent = EF_RESUME();
        FSM_AddEvent(nextevent);
        break;
    default:
        DCSdebugSystemInfo("Undefined this should not happen");
        DCSdebugSystemInfo("Go to emergency state");
        nextevent = EF_EMERGENCY_START();
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
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);
    DSPshow(2,"System Initialized No errors");

    return(E_TREADMILL);        // Volgens mij moet dit E_INIT zijn, maar dan werkt het niet
}

event_t	Treadmill(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    return (E_TREADMILL);
}

event_t EF_DIAGNOSTICS_START(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // Start diagnostics state.
    // Set incline, speed and distance to zero.
    Speed = 0;
    Inc = 0;
    Distance = 0;

    return (E_DIAGNOSTICS_START);
}

event_t EF_DIAGNOSTICS_STOP(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // Stop diagnostics and go to default state
    // Maybe restore default running configuration?
    Speed = 0.8;
    Inc = 0;
    Distance = 0;

    return (E_DIAGNOSTICS_STOP);
}

event_t EF_RUNNING_START(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // setting starting values
    Speed = 0.8;
    Inc = 0;
    Distance = 0;

    return (E_RUNNING_START);

    //S_Default_onEntry();
}

event_t EF_RUNNING_STOP(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // stopping treadmill with this function
    Speed = 0;
    Inc = 0;

    return (E_RUNNING_STOP);
}

event_t EF_PAUSE(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // Set speed of treadmill to zero. Keep other options the same.
    TEMP = Speed ;
    Speed = 0;
    Inc = Inc;
    Distance = Distance;

    // Let FSM framework know we're done with the event
    return (E_PAUSE);
}

event_t EF_RESUME(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // Restore user configured speed here
    Speed = TEMP;
    TEMP = 0;

    return (E_RESUME);
}

event_t EF_EMERGENCY_START(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // Trigger alarms and emergency things here.
    Speed = 0;
    Inc = 0;

    return (E_EMERGENCY_START);
}

event_t EF_EMERGENCY_STOP(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // Reset emergency triggers here
    // Stop alarm also here
    return (E_EMERGENCY_STOP);
}

event_t EF_CONFIG_CHANGE(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    return (E_CONFIG_CHANGE);
}

event_t EF_CONFIG_DONE(void)
{
    // Show current state
    state_t state;
    state = FSM_GetState();
    DCSdebugSystemInfo("Old state: %s", stateEnumToText[state]);

    // Commit changes to saved configuration

    return (E_CONFIG_DONE);
}

// simulate delay in microseconds
void delay_us(uint32_t d)
{
    DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
    sleep(10000);
}
