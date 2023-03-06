#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <dirent.h>

#include "menu.h"

#include "emulator.h"
#include "copyright_notice.h"
#include "interrupts.h"

#include "../data/game.h"

void menu_print_options()
{
    iprintf("WASM-4 DS");

    iprintf("\x1b[4;3HPlay platformer test");
    iprintf("\x1b[6;3HLoad WASM from file");
    iprintf("\x1b[8;3HCopyright notice");
}

void menu()
{
    menu_print_options();

    int sel = 0;

    while (1)
    {
        // Print the cursor at the appropriate line
        for (int i = 0; i < 3; ++i)
        {
            iprintf("\x1b[%i;1H%c", 2*i + 4, sel == i ? '>' : ' ');
        }

        scanKeys();

        if (keysDown() & KEY_DOWN && sel < 2)
        {
            sel++;
        }
        else if (keysDown() & KEY_UP && sel > 0)
        {
            sel--;
        }
        else if (keysDown() & KEY_A)
        {
            consoleClear();

            switch (sel)
            {
                case 0: emulator_loop(game_wasm, game_wasm_len); break;
                case 1: menu_file_selection(); break;
                case 2: copyright_notice(); break;
            }

            consoleClear();
            menu_print_options();
        }
    }
}

void menu_file_selection()
{
    DIR* directory = opendir("/wasm");

    if (directory == NULL)
    {
        return;
    }

    // Read the directory file by file
    struct dirent *entry = readdir(directory);
    while (entry != NULL)
    {
        if(entry->d_type != DT_DIR)
        {
            consoleClear();

            iprintf(entry->d_name);
            iprintf("\x1b[20;0H(A) Next file  (X) Load file)");

            scanKeys();
            while (!keysDown()) {interrupts_wait_vblank(); scanKeys();}

            if (keysDown() & KEY_X)
            {
                chdir("/wasm");
                FILE* cart = fopen(entry->d_name, "rb");

                int len;
                fseek(cart, 0, SEEK_END);
                len = ftell(cart);
                fseek(cart, 0, SEEK_SET);

                uint8_t *data = (uint8_t*)malloc(len);
                fread(data, 1, len, cart);

                emulator_loop(data, len);

                free(data);
            }
        }

        entry = readdir(directory);
    }

    closedir(directory);
}