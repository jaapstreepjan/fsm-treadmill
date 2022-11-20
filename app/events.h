#ifndef EVENTS_H
#define EVENTS_H

typedef enum {
   E_NO,                ///< Used for initialisation of an event variable
   E_INIT,
   E_LOCK,
   E_COIN,
   E_PASS,
   E_EXCEPTION,         /// to generate and catch an EXECEPTION
} event_t;

#endif
