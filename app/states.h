#ifndef STATES_H
#define STATES_H

typedef enum {
    S_NO,               ///< Used for initialisation if state is not yet known
    S_START,            /// simulates [*]
    S_INIT,
    S_STANDBY,
    S_DEFAULT,            // QTCreator vind de naam 'default' niet leuk
    S_DIAGNOSTICS,
    S_ALTERCONFIG,
    S_EMERGENCY,
    S_PAUSE
    //end
} state_t;

#endif
