#ifndef VARIABLES_H
#define VARIABLES_H

#endif // VARIABLES_H

typedef enum {
    INIT,                ///< Used for initialisation of an event variable
    STANDBY,
    DEFAULT,
    DIAGNOSTICS,
    ALTERCONFIG,
    PAUSE,
    EMERGENCY,
} treadmillStatus_t; // Need to find out what this is used for. Comment Colin: Name of the enum.

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

// Struct for vallues. Comment Colin: couldn't find out how to pass refferense to state. had to resolve to global struct. if time allow fix.
struct Variables
{
    float
    speed,
    inc,
    distance,
    tSpeed,
    tInc;
};

struct Variables myStruct;
