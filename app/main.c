/*!
 * Implementation of the finite statemachine for a turnstile
 * as defined by Robert C. Martin in UML Tutorial: Finite State Machines
 * Code uses a pre-defined state handler (fsm_functions)
 * adapted from code created by Hugo Arends, HAN by
 * John van den Hooven
 *
 * Implementation is based on Figure 2 in the document
 *
 * This version of the code is a monolitic implementation
 * All functions and subsystem code is implemented in main.c
 *
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
    FSM_AddState(S_START,      &(state_funcs_t){  NULL,                  NULL               });
    FSM_AddState(S_INIT,       &(state_funcs_t){  NULL,                  NULL               });
    FSM_AddState(S_STANDBY,    &(state_funcs_t){  NULL,                  NULL               });
    FSM_AddState(S_DEFAULT,    &(state_funcs_t){  NULL,                  NULL               });
    FSM_AddState(S_DIAGNOSTICS,&(state_funcs_t){  NULL,                  NULL               });
    FSM_AddState(S_ALTERCONFIG,&(state_funcs_t){  NULL,                  NULL               });
    FSM_AddState(S_EMERGENCY,  &(state_funcs_t){  NULL,                  NULL               });
    FSM_AddState(S_PAUSE,      &(state_funcs_t){  NULL,                  NULL               });

    /// Second the transitions
    //                                 From            Event                To
    FSM_AddTransition(&(transition_t){ S_START,        E_INIT,              S_INIT     });

    // Should unexpected events in a state be flushed or not?
    FSM_FlushEnexpectedEvents(true);

    /// Start the state machine
    FSM_RunStateMachine(S_START, E_INIT);

    /// Use this test function to test your model
    /// FSM_RevertModel();

    return 0;
}



/// Local function prototypes State related

void S_Init_onEntry(void)
{

}

void S_Init_onExit(void)
{

}

///Subsystem (simulation) functions

// simulate delay in microseconds
void delay_us(uint32_t d)
{

}
