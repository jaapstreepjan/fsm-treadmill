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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Finite State Machine library
#include "fsm_functions/fsm.h"

// Development Console libraries
#include "console_functions/keyboard.h"
#include "console_functions/display.h"
#include "console_functions/devConsole.h"

// Protoypes and Variables
#include "prototypes.h"
#include "variables.h"

// Declare variables for the start and end times
time_t start_time, end_time;
double elapsed_time;

extern char * eventEnumToText[];
extern char * stateEnumToText[];

event_t event;
state_t state;

// Subsystem initialization (simulation) functions
event_t InitialiseSubsystems(void);

// Subsystem1 (simulation) functions
// EF_ prefix is used for Event Functions
event_t TREADMILL(void);
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
    // sets all vallues to 0
    resetStat();

    // Define the state machine model
    // First the state and the pointer to the onEntry and onExit functions
    //           State                            onEntry()              onExit()
    FSM_AddState(S_START,      &(state_funcs_t){  NULL,                  NULL                   });
    FSM_AddState(S_INIT,       &(state_funcs_t){  S_initOnEntry,        NULL                   });
    FSM_AddState(S_STANDBY,    &(state_funcs_t){  S_standbyOnEntry,     NULL                   });
    FSM_AddState(S_DEFAULT,    &(state_funcs_t){  S_defaultOnEntry,     NULL                   });
    FSM_AddState(S_DIAGNOSTICS,&(state_funcs_t){  S_diagnosticsOnEntry, NULL                   });
    FSM_AddState(S_ALTERCONFIG,&(state_funcs_t){  S_alterconfigOnEntry, NULL                   });
    FSM_AddState(S_EMERGENCY,  &(state_funcs_t){  S_emergencyOnEntry,   NULL                   });
    FSM_AddState(S_PAUSE,      &(state_funcs_t){  S_pauseOnEntry,       NULL                   });

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

void S_initOnEntry(void)
{
    event_t nextevent;

    /// Simulate the initialisation
    nextevent = InitialiseSubsystems();

    FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_standbyOnEntry(void)
{
    showCurrentState();

    // Display information for user
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tChange configuration.\n", myStruct.speed, myStruct.inc, myStruct.distance);

    // Show user options
    event_t nextevent;
    int navigation;
    navigation  = DCSsimulationSystemInputChar("\n"
                                               "Press D for diagnostics\n"
                                               "Press S for default running\n",
                                               "D" "S");

    switch (navigation)
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

void S_defaultOnEntry(void)
{
    // start timer to keep track of time
    keepTimeStart();

    showCurrentState();

    // Display information for user
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tSystem ready!\n", myStruct.speed, myStruct.inc, myStruct.distance);

    // Show user options
    event_t nextevent;
    int navigation;
    navigation = DCSsimulationSystemInputChar("\n"
                                              "Press P to Pause\n"
                                              "Press C to change config\n"
                                              "Press E to trigger emergency\n"
                                              "Press Q to stop running\n",
                                              "P" "C" "E" "Q");

    switch (navigation)
    {
    case 'P':
        // Function call to update Distance based on time.
        keepTimeStop();
        updateDis();

        nextevent = EF_PAUSE();
        FSM_AddEvent(nextevent);
        break;
    case 'C':
        // Function call to update Distance based on time.
        keepTimeStop();
        updateDis();

        nextevent = EF_CONFIG_CHANGE();
        FSM_AddEvent(nextevent);
        break;
    case 'E':
        // Function call to update Distance based on time.
        keepTimeStop();
        updateDis();

        nextevent = EF_EMERGENCY_START();
        FSM_AddEvent(nextevent);
        break;
    case 'Q':
        // Function call to update Distance based on time.
        keepTimeStop();
        updateDis();

        nextevent = EF_RUNNING_STOP();
        FSM_AddEvent(nextevent);
        break;
    default:
        DSPshow(1,"Invalid input!\nPlease try again!");
        break;
    }
}

void S_diagnosticsOnEntry(void)
{
    showCurrentState();

    // Show user information
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tDiagnostic mode\n"
              "\tCleared for maintenance duties.\n", myStruct.speed, myStruct.inc, myStruct.distance);

    // Show user options
    event_t nextevent;
    int navigation;
    navigation  = DCSsimulationSystemInputChar("\n"
                                               "Press O for Other things\n"
                                               "Press Q to Quit diagnostics\n",
                                               "Q" "O");

    switch (navigation)
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


void S_alterconfigOnEntry(void)
{
    // start timer to keep track of time
    keepTimeStart();

    showCurrentState();

    // Display information for user
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tChange configuration.\n", myStruct.speed, myStruct.inc, myStruct.distance);

    // Show user information
    int navigation;
    navigation = DCSsimulationSystemInputChar("\n"
                                              "Press S to change Speed\n"
                                              "Press I to change Incline\n"
                                              "Press D to change Distance\n"
                                              "Press E for Emergencies\n"
                                              "Press C to commit Change\n",
                                              "S" "I" "D" "E" "C");

    event_t nextevent;
    char input[10]; // input buffer

    switch (navigation)
    {
    case 'S':
        // change speed here
        printf("Enter a float value: ");
        fgets(input, sizeof(input), stdin); // get user input

        myStruct.speed = atof(input); // convert input string to float and assign to struct value

        printf("Struct value: %f\n", myStruct.speed);

        S_alterconfigOnEntry();
        break;
    case 'I':
        // change Incline here
        printf("Enter a float value: ");
        fgets(input, sizeof(input), stdin); // get user input

        myStruct.inc = atof(input); // convert input string to float and assign to struct value

        printf("Struct value: %f\n", myStruct.inc);

        S_alterconfigOnEntry();
        break;
    case 'D':
        // change Incline here
        printf("Enter a float value: ");
        fgets(input, sizeof(input), stdin); // get user input

        myStruct.distance = atof(input); // convert input string to float and assign to struct value

        printf("Struct value: %f\n", myStruct.distance);

        S_alterconfigOnEntry();
        break;
    case 'E':
        // Function call to update Distance based on time.
        keepTimeStop();
        updateDis();

        nextevent = EF_EMERGENCY_START();
        FSM_AddEvent(nextevent);
        break;
    case 'C':
        // Function call to update Distance based on time.
        keepTimeStop();
        updateDis();

        nextevent = EF_CONFIG_DONE();
        FSM_AddEvent(nextevent);
        break;
    default:
        DSPshow(1,"Invalid input!\nPlease try again!");
        break;
    }
}

void S_emergencyOnEntry(void)
{
    showCurrentState();

    // Show user information
    DSPshow(2,"\tSpeed: %.1f Km/H\n"
              "\tInclination: %.1f %%\n"
              "\tDistance: %.1f M\n"
              "\tEmergency mode\n",
            myStruct.speed, myStruct.inc, myStruct.distance);

    // Show user options
    event_t nextevent;
    int navigation;
    navigation  = DCSsimulationSystemInputChar("\n"
                                               "Press O for Other things\n"
                                               "Press Q to Quit emergency\n",
                                               "Q" "O");

    switch (navigation)
    {
    case 'Q':
        nextevent = EF_EMERGENCY_STOP();
        FSM_AddEvent(nextevent);
        break;
    case 'O':
        printf("This is a Simulated error log, Reseting to Emergency");
        S_emergencyOnEntry();
        // Other things here that are Emergency related
        break;
    default:
        DSPshow(1,"Invalid input!\nPlease try again!");
        break;
    }
}

void S_pauseOnEntry(void)
{
    showCurrentState();

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
    // state_t state;       // Deze moet misschien blijven staan? Comment Colin: exces function? look if time allows.
    DSPinitialise();
    DSPshowDisplay();
    KYBinitialise();

    DSPshow(2,"System Initialized No errors");

    showCurrentState();
    return(E_TREADMILL);        // Volgens mij moet dit E_INIT zijn, maar dan werkt het niet
}

// Event for transitioning from S_INIT to S_STANDBY
event_t	TREADMILL(void)
{
    // Startup phase here

    showCurrentState();
    return (E_TREADMILL);
}

// Event function for transitioning from S_DEFAULT to S_DIAGNOSTICS
event_t EF_DIAGNOSTICS_START(void)
{
    // Trigger diagnostic things here
    // Set incline, speed and distance to zero.
    saveStat();

    showCurrentState();
    return (E_DIAGNOSTICS_START);
}

// Event function for transitioning from S_DIAGNOSTICS to S_DEFAULT
event_t EF_DIAGNOSTICS_STOP(void)
{
    // Stop diagnostics and go to default state
    // restore default running configuration
    getStat();

    showCurrentState();
    return (E_DIAGNOSTICS_STOP);
}

// Event function for transitioning from S_STANDBY to S_DEFAULT
event_t EF_RUNNING_START(void)
{
    // Allows access to Variables in Struct Note:Use s1. before variable
    // setting starting values
    myStruct.speed = 0.8;
    myStruct.inc = 0;

    showCurrentState();
    return (E_RUNNING_START);
}

// Event function for transitioning from S_DEFAULT to S_STANDBY
event_t EF_RUNNING_STOP(void)
{
    // stopping treadmill with this function
    saveStat();

    showCurrentState();
    return (E_RUNNING_STOP);
}

// Event function for transitioning from S_DEFAULT to S_PAUSE
event_t EF_PAUSE(void)
{    
    // Set speed of treadmill to zero. Keep other options the same.
    saveStat();

    showCurrentState();
    return (E_PAUSE);
}

// Event function for transitioning from S_PAUSE to S_DEFAULT
event_t EF_RESUME(void)
{
    // Restore user configured speed here
    getStat();

    showCurrentState();
    return (E_RESUME);
}

// Event function for transitioning from S_DEFAULT to S_EMERGENCY
event_t EF_EMERGENCY_START(void)
{
    // Trigger alarms and emergency things here.
    getStat();

    showCurrentState();
    return (E_EMERGENCY_START);
}

// Event function from transitioning from S_EMERGENCY to S_DEFAULT
event_t EF_EMERGENCY_STOP(void)
{
    // Reset emergency triggers here
    // Stop alarm also here
    saveStat();

    showCurrentState();
    return (E_EMERGENCY_STOP);
}

// Function for transtitioning from state default to state alterConfig
event_t EF_CONFIG_CHANGE(void)
{
    // Allow changes to happen to configuration
    showCurrentState();
    return (E_CONFIG_CHANGE);
}

// Function for transitioning from state alterConfig to state default
event_t EF_CONFIG_DONE(void)
{
    // Commit changes to saved configuration HERE

    showCurrentState();
    return (E_CONFIG_DONE);
}

// simulate delay in microseconds
void delay_us(uint32_t d)
{
    DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
    sleep(10000);
}

// Function for showing the state to end user for debugging purposes.
void showCurrentState(void)
{
    // initialize needed variable
    state_t state;

    // fetch current state from FSM-framework
    state = FSM_GetState();

    // Show current state to user
    DCSdebugSystemInfo("State: %s", stateEnumToText[state]);
}

// Function for keeping track of current stats
void saveStat(void)
{
    // Setting current vallue in Temp for later pull.
    myStruct.tSpeed = myStruct.speed;
    myStruct.tInc = myStruct.inc;

    myStruct.speed = 0;
    myStruct.inc = 0;
}

// Function for returning saved stats
void getStat(void)
{
    // Pulling Vallues from Temps.
    myStruct.speed = myStruct.tSpeed;
    myStruct.inc = myStruct.tInc;

    myStruct.tSpeed = 0;
    myStruct.tInc = 0;
}

// Function for keeping track of distance
void updateDis(void)
{
    float TDistance;
    // unfortionatly due to time constrains this function will not be impletemented
    // my knowledge of keeping track of timers in C is limited and will take to long to learn/ implement in thi project. (Colin)


    TDistance = elapsed_time * (myStruct.speed / 3.6);
    myStruct.distance = myStruct.distance + TDistance;
}

// Function to reset all stats
void resetStat(void)
{
    // Allows access to Variables in Struct Note:Use Ptr -> before variable
    myStruct.tSpeed = 0;
    myStruct.tInc = 0;
    myStruct.speed =0;
    myStruct.inc =0;
    myStruct.distance =0;
}

void keepTimeStart(void)
{
    // Record the start time
    time(&start_time);
}

void keepTimeStop(void)
{
    // Record the end time
    time(&end_time);

    // Calculate the elapsed time
    elapsed_time = difftime(end_time, start_time);
}
