#ifndef STATES_H
#define STATES_H

typedef enum {
   S_NO,               ///< Used for initialisation if state is not yet known
   S_START,            /// simulates [*]
   S_INIT,
   S_LOCKED,
   S_UNLOCKED,
   //end
} state_t;

#endif
