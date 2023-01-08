#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#endif // PROTOTYPES_H

// Function prototypes related to code efficiency
void showCurrentState(void);
void saveStat(void);
void getStat(void);
void updateDis(void);
void resetStat(void);
void keepTimeStart(void);
void keepTimeStop(void);

// Local function prototypes State related
void S_initOnEntry(void);
void S_standbyOnEntry(void);
void S_defaultOnEntry(void);
void S_diagnosticsOnEntry(void);
void S_alterconfigOnEntry(void);
void S_emergencyOnEntry(void);
void S_pauseOnEntry(void);


