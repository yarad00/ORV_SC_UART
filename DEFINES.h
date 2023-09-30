#ifndef DEFINES
#define DEFINES

#define START_BIT 0
#define STOP_BIT 1
#define IDLE_BIT 1

typedef enum {
    IDLE = 0,
    RECEIVING = 1,
    STOP = 2
} State;

#endif