#include <nds.h>
#include <fat.h>
#include <stdio.h>

#include "menu.h"
#include "interrupts.h"

int main()
{
    consoleDemoInit();

    vramSetBankA(VRAM_A_LCD);
    videoSetMode(MODE_FB0);

    interrupts_init();

    if (!fatInitDefault())
    {
        iprintf("FAT init failed\n");
    }

    menu();
}
