#ifndef EVENTS_H
#define EVENTS_H

typedef enum {
    E_NO,
    E_INIT,
    E_TREADMILL,
    E_RUNNING_START,
    E_RUNNING_STOP,
    E_DIAGNOSTICS_START,
    E_DIAGNOSTICS_STOP,
    E_CONFIG_CHANGE,
    E_CONFIG_DONE,
    E_PAUSE,
    E_RESUME,
    E_EMERGENCY_START,
    E_EMERGENCY_STOP
} event_t;

#endif
