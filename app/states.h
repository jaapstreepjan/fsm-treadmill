#ifndef STATES_H
#define STATES_H

typedef enum {
    S_NO,               ///< Used for initialisation if state is not yet known
    S_START,            /// simulates [*]
    init,
    standby,
    default,            // QTCreator vind de naam 'default' niet leuk
    diagnostics,
    alterConfig,
    emergency,
    pause
    //end
} state_t;

#endif
