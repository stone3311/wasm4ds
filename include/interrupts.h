#ifndef INTERRUPTS_H
#define INTERRUPTS_H

static volatile int frame;
static volatile int semaphore;

void interrupts_init();
void interrupts_reset();
void interrupts_wait_vblank();

int interrupts_framecount();

void Vblank();

#endif