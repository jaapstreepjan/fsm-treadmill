/*!
 * The C program written by C. van Dreumel en J.J. Groenendijk is an implementation of a finite state machine (FSM) model of a treadmill.
 * The FSM is a mathematical model of computation that represents the behavior of a system by specifying the possible states it can be in, the transitions between those states, and the actions that are performed when transitioning between states.
 * In the case of a treadmill, the states might include "stopped," "running at a low speed," "running at a high speed," and so on.
 * The transitions between states would be determined by the actions of the user, such as pressing buttons to change the speed or incline of the treadmill.
 * The actions performed by the FSM would be the physical movements of the treadmill belt and any accompanying changes in the display or other output.
 * This program provides a precise and rigorous way of modeling the behavior of a treadmill, which can be useful for understanding how the treadmill works, testing its performance, and potentially even improving its design.
 */

#include <stdio.h>
#include <unistd.h>

/// Finite State Machine Library
#include "fsm_functions/fsm.h"

/// Development Console Library
#include "console_functions/keyboard.h"
#include "console_functions/display.h"
#include "console_functions/devConsole.h"

typedef enum {
    UNLOCKED,                ///< Used for initialisation of an event variable
    LOCKED,
} lockStatus_t;

char * lockStatusToText[] =
{
    "UNLOCKED",
    "LOCKED",
};


extern char * eventEnumToText[];
extern char * stateEnumToText[];

event_t event;
state_t state;

int LockStatus = LOCKED;

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


///Subsystem initialization (simulation) functions
event_t InitialiseSubsystems(void);

///Subsystem initialization (simulation) functions
event_t CoinAcceptor(void);

///Subsystem1 (simulation) functions

int LockTurnstile(const int LockStatus);
int UnlockTurnstile(const int LockStatus);

event_t TurnstileLock(void);

///Helper function example
void delay_us(uint32_t d);

// Main
int main(void)
{

    /// Define the state machine model
    /// First the state and the pointer to the onEntry and onExit functions
    //           State                           onEntry()               onExit()
    FSM_AddState(S_START,      &(state_funcs_t){  NULL,                  NULL                   });
    FSM_AddState(S_INIT,       &(state_funcs_t){  S_Init_onEntry,        S_Init_onExit          });
    FSM_AddState(S_STANDBY,    &(state_funcs_t){  S_Standby_onEntry,     S_Standby_onExit       });
    FSM_AddState(S_DEFAULT,    &(state_funcs_t){  S_Default_onEntry,     S_Default_onExit       });
    FSM_AddState(S_DIAGNOSTICS,&(state_funcs_t){  S_Diagnostics_onEntry, S_Diagnostics_onExit   });
    FSM_AddState(S_ALTERCONFIG,&(state_funcs_t){  S_Alterconfig_onEntry, S_Alterconfig_onExit   });
    FSM_AddState(S_EMERGENCY,  &(state_funcs_t){  S_Emergency_onEntry,   S_Emergency_onExit     });
    FSM_AddState(S_PAUSE,      &(state_funcs_t){  S_Pause_onEntry,       S_Pause_onExit         });

    /// Second the transitions
    //                                 From            Event                To
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



/// Local function prototypes State related

void S_Init_onEntry(void)
{
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = InitialiseSubsystems();

    FSM_AddEvent(nextevent);           /// Internal generated event

}

void S_Init_onExit(void)
{

}

void S_Standby_onEntry(void)
{
    // To Do
}

void S_Standby_onExit(void)
{
    // To Do
}

void S_Default_onEntry(void)
{
    // To Do
}

void S_Default_onExit(void)
{
    // To Do
}

void S_Diagnostics_onEntry(void)
{
    // To Do
}

void S_Diagnostics_onExit(void)
{
    // To Do
}

void S_Alterconfig_onEntry(void)
{
    // To Do
}

void S_Alterconfig_onExit(void)
{
    // To Do
}

void S_Emergency_onEntry(void)
{
    // To Do
}

void S_Emergency_onExit(void)
{
    // To Do
}

void S_Pause_onEntry(void)
{
    // To Do
}

void S_Pause_onExit(void)
{
    // To Do
}

///Subsystem (simulation) functions
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
    // LockStatus = LockTurnstile(LockStatus);
    DSPshow(3,"Turnstile locked");

    return(E_TREADMILL);
}

// simulate delay in microseconds
void delay_us(uint32_t d)
{
    DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
    sleep(10000);
}
