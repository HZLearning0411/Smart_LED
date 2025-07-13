#ifndef __SYSTEM_H
#define __SYSTEM_H

void IWDG_Init(void);

typedef enum {
    AUTO_MODE,
    MANUAL_MODE
} SystemMode;

#endif