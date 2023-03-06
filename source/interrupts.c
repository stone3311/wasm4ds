#include <nds.h>

#include "interrupts.h"

void interrupts_init()
{
    interrupts_reset();
    irqSet(IRQ_VBLANK, Vblank);
}

void interrupts_reset()
{
    frame = 0;
    semaphore = 0;
}

void interrupts_wait_vblank()
{
    while (!semaphore) {}
    semaphore = 0;
}

int interrupts_framecount()
{
    return frame;
}

void Vblank()
{
    frame++;
    semaphore = 1;
}