#include <nds.h>
#include <stdio.h>

#include "emulator.h"

#include "interrupts.h"

#include "apu.h"
#include "runtime.h"
#include "wasm.h"
#include "window.h"

const int x_offset = (SCREEN_WIDTH  - 160) / 2;
const int y_offset = (SCREEN_HEIGHT - 160) / 2;

// Callback from w4_runtimeUpdate to update the framebuffer
void w4_windowComposite (const uint32_t* palette, const uint8_t* framebuffer)
{
    // Convert the 32 bit color palette to RGB15
    uint16_t palette_rgb15[4];

    for (int i = 0; i < 4; ++i)
    {
        uint32_t color = palette[i];

        uint8_t b = (color & 0xFF) >> 3;
        uint8_t g = ((color >> 8) & 0xFF) >> 3;
        uint8_t r = ((color >> 16) & 0xFF) >> 3;

        palette_rgb15[i] = RGB15(r, g, b);
    }

    // Only draw during Vblank to avoid tearing
    interrupts_wait_vblank();

    // Unpack the 2bpp framebuffer to VRAM_A
    for (int y = 0; y < 160; ++y) {
        for (int x = 0; x < 160/4; ++x) {
            int n = x + y * (160/4);

            uint8_t packed = framebuffer[n];
            int pixel1 = (packed & 0b00000011);
            int pixel2 = (packed & 0b00001100) >> 2;
            int pixel3 = (packed & 0b00110000) >> 4;
            int pixel4 = (packed & 0b11000000) >> 6;

            int vram_index = 4 * x + x_offset + (y + y_offset) * SCREEN_WIDTH;

            VRAM_A[vram_index]     = palette_rgb15[pixel1];
            VRAM_A[vram_index + 1] = palette_rgb15[pixel2];
            VRAM_A[vram_index + 2] = palette_rgb15[pixel3];
            VRAM_A[vram_index + 3] = palette_rgb15[pixel4];
        }
    }
}

void emulator_loop(uint8_t* game_data, int game_data_length)
{
    interrupts_reset();

    consoleClear();
    iprintf("Initializing WASM4 runtime...\n");

    w4_Disk disk = {0};
    uint8_t* memory = w4_wasmInit();
    w4_runtimeInit(memory, &disk);

    iprintf("Loading cartridge... ");
    w4_wasmLoadModule(game_data, game_data_length);

    iprintf("done\n");
    iprintf("\x1b[20;0H(Start) Return to menu\n(Select) Swap screens");

    int w4_frame = 0;
    while (!(keysCurrent() & KEY_START))
    {
        scanKeys();

        if (keysDown() & KEY_SELECT)
        {
            lcdSwap();
        }

        uint8_t gamepad = 0;

        if (keysCurrent() & KEY_X)
        {
            gamepad |= W4_BUTTON_X;
        }
        if (keysCurrent() & KEY_Y)
        {
            gamepad |= W4_BUTTON_Z;
        }

        if (keysCurrent() & KEY_UP)
        {
            gamepad |= W4_BUTTON_UP;
        }
        if (keysCurrent() & KEY_DOWN)
        {
            gamepad |= W4_BUTTON_DOWN;
        }
        if (keysCurrent() & KEY_LEFT)
        {
            gamepad |= W4_BUTTON_LEFT;
        }
        if (keysCurrent() & KEY_RIGHT)
        {
            gamepad |= W4_BUTTON_RIGHT;
        }
        w4_runtimeSetGamepad(0, gamepad);

        uint8_t mouseButtons = 0;

        touchPosition touch_pos;
        touchRead(&touch_pos);

        if (keysCurrent() & KEY_TOUCH)
        {
            mouseButtons |= W4_MOUSE_LEFT;
        }
        if (keysCurrent() & KEY_R)
        {
            mouseButtons |= W4_MOUSE_RIGHT;
        }
        if (keysCurrent() & KEY_L)
        {
            mouseButtons |= W4_MOUSE_MIDDLE;
        }
        w4_runtimeSetMouse(touch_pos.px - x_offset, touch_pos.py - y_offset, mouseButtons);

        w4_runtimeUpdate();
        w4_frame++;

        iprintf("\x1b[10;0Hframe = %i, w4_frame = %i", interrupts_framecount(), w4_frame);
    }

    w4_wasmDestroy();
}
