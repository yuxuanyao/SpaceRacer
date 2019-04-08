#define EDGE_TRIGGERED 0x1
#define LEVEL_SENSITIVE 0x0
#define CPU0 0x01 // bit-mask; bit 0 represents cpu0
#define ENABLE 0x1

#define KEY0 0
#define KEY1 1
#define NONE 4

#define USER_MODE 0b10000
#define FIQ_MODE 0b10001
#define IRQ_MODE 0b10010
#define SVC_MODE 0b10011
#define ABORT_MODE 0b10111
#define UNDEF_MODE 0b11011
#define SYS_MODE 0b11111

#define INT_ENABLE 0b01000000
#define INT_DISABLE 0b11000000

/* This files provides address values that exist in the system */

#define BOARD "DE1-SoC"

/* Memory */
#define DDR_BASE 0x00000000
#define DDR_END 0x3FFFFFFF
#define A9_ONCHIP_BASE 0xFFFF0000
#define A9_ONCHIP_END 0xFFFFFFFF
#define SDRAM_BASE 0xC0000000
#define SDRAM_END 0xC3FFFFFF
#define FPGA_PIXEL_BUF_BASE 0xC8000000
#define FPGA_PIXEL_BUF_END 0xC803FFFF
#define FPGA_CHAR_BASE 0xC9000000
#define FPGA_CHAR_END 0xC9001FFF

/* Cyclone V FPGA devices */
#define LED_BASE 0xFF200000
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JP1_BASE 0xFF200060
#define JP2_BASE 0xFF200070
#define PS2_BASE 0xFF200100
#define PS2_DUAL_BASE 0xFF200108
#define JTAG_UART_BASE 0xFF201000
#define JTAG_UART_2_BASE 0xFF201008
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000
#define TIMER_2_BASE 0xFF202020
#define AV_CONFIG_BASE 0xFF203000
#define RGB_RESAMPLER_BASE 0xFF203010
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define AUDIO_BASE 0xFF203040
#define VIDEO_IN_BASE 0xFF203060
#define EDGE_DETECT_CTRL_BASE 0xFF203070
#define ADC_BASE 0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE 0xFF709000
#define I2C0_BASE 0xFFC04000
#define I2C1_BASE 0xFFC05000
#define I2C2_BASE 0xFFC06000
#define I2C3_BASE 0xFFC07000
#define HPS_TIMER0_BASE 0xFFC08000
#define HPS_TIMER1_BASE 0xFFC09000
#define HPS_TIMER2_BASE 0xFFD00000
#define HPS_TIMER3_BASE 0xFFD01000
#define FPGA_BRIDGE 0xFFD0501C

/* ARM A9 MPCORE devices */
#define PERIPH_BASE 0xFFFEC000       // base address of peripheral devices
#define MPCORE_PRIV_TIMER 0xFFFEC600 // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF 0xFFFEC100 // PERIPH_BASE + 0x100
#define ICCICR 0x00                 // offset to CPU interface control reg
#define ICCPMR 0x04                 // offset to interrupt priority mask reg
#define ICCIAR 0x0C                 // offset to interrupt acknowledge reg
#define ICCEOIR 0x10                // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST 0xFFFED000 // PERIPH_BASE + 0x1000
#define ICDDCR 0x00                // offset to distributor control reg
#define ICDISER 0x100              // offset to interrupt set-enable regs
#define ICDICER 0x180              // offset to interrupt clear-enable regs
#define ICDIPTR 0x800              // offset to interrupt processor targets regs
#define ICDICFR 0xC00              // offset to interrupt configuration regs

/* This file provides interrupt IDs */

/* FPGA interrupts (there are 64 in total; only a few are defined below) */
#define INTERVAL_TIMER_IRQ 72
#define KEYS_IRQ 73
#define FPGA_IRQ2 74
#define FPGA_IRQ3 75
#define FPGA_IRQ4 76
#define FPGA_IRQ5 77
#define FPGA_IRQ6 78
#define FPGA_IRQ7 79
#define JTAG_IRQ 80
#define FPGA_IRQ9 81
#define FPGA_IRQ10 82
#define JP1_IRQ 83
#define JP7_IRQ 84
#define ARDUINO_IRQ 85
#define FPGA_IRQ14 86
#define FPGA_IRQ15 87
#define FPGA_IRQ16 88
#define FPGA_IRQ17 89
#define FPGA_IRQ18 90
#define FPGA_IRQ19 91

/* ARM A9 MPCORE devices (there are many; only a few are defined below) */
#define MPCORE_GLOBAL_TIMER_IRQ 27
#define MPCORE_PRIV_TIMER_IRQ 29
#define MPCORE_WATCHDOG_IRQ 30

/* HPS devices (there are many; only a few are defined below) */
#define HPS_UART0_IRQ 194
#define HPS_UART1_IRQ 195
#define HPS_GPIO0_IRQ 196
#define HPS_GPIO1_IRQ 197
#define HPS_GPIO2_IRQ 198
#define HPS_TIMER0_IRQ 199
#define HPS_TIMER1_IRQ 200
#define HPS_TIMER2_IRQ 201
#define HPS_TIMER3_IRQ 202
#define HPS_WATCHDOG0_IRQ 203
#define HPS_WATCHDOG1_IRQ 204

/*-------------Interupts--------------*/
#include <stdbool.h>
volatile int Ship1dy;
volatile int Ship2dy;

// void config_KEYs(void);
// void pushbutton_ISR(void);
// void config_GIC(void);
/*-------------Interupts--------------*/

volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_line(int x_start, int y_start, int x_end, int y_end, short int line_color);
void swap(int *x, int *y);
void plot_pixel(int x, int y, short int line_color);
void wait_for_sync();
void draw();
void drawSquare(int x, int y, int length, int width, short int line_color);
void update();
void clear();
void initialize();
void checkKeyPress();

#define numAsteroids 15
#define shipLength 21
#define shipWidth 11
#define PUSHBUTTONS ((volatile long *)0xFF200050)

struct box
{
    int x;
    int y;
    int dx;
    int dy;
};

struct prevLocation
{
    int x;
    int y;
};

// structs
struct prevLocation SDRAM[numAsteroids];
struct prevLocation ONCHIP[numAsteroids];
struct box Asteroids[numAsteroids];

struct box Ship1;
struct prevLocation Ship1SDRAM;
struct prevLocation Ship1ONCHIP;

struct box Ship2;
struct prevLocation Ship2SDRAM;
struct prevLocation Ship2ONCHIP;

int main(void)
{
    volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
    // initialize asteroids
    initialize();

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_sync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer

    while (1)
    {
        // clear_screen_partial();
        clear();

        // code for drawing the boxes and lines
        draw();

        checkKeyPress();
        // code for updating the locations of boxes
        update();

        wait_for_sync();                            // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

void checkKeyPress()
{

    if (*PUSHBUTTONS == 0b0001 || *PUSHBUTTONS == 0b1101)
    {
        Ship1dy = 0;
        Ship2dy = 1;
    }
    else if (*PUSHBUTTONS == 0b0010 || *PUSHBUTTONS == 0b1110)
    {
        Ship1dy = 0;
        Ship2dy = -1;
    }
    else if (*PUSHBUTTONS == 0b0100 || *PUSHBUTTONS == 0b0111)
    {
        Ship2dy = 0;
        Ship1dy = 1;
    }
    else if (*PUSHBUTTONS == 0b1000 || *PUSHBUTTONS == 0b1011)
    {
        Ship2dy = 0;
        Ship1dy = -1;
    }
    else if (*PUSHBUTTONS == 0b1001)
    {
        Ship2dy = 1;
        Ship1dy = -1;
    }
    else if (*PUSHBUTTONS == 0b0110)
    {
        Ship1dy = 1;
        Ship2dy = -1;
    }
    else if (*PUSHBUTTONS == 0b0101)
    {
        Ship1dy = 1;
        Ship2dy = 1;
    }
    else if (*PUSHBUTTONS == 0b1010)
    {
        Ship1dy = -1;
        Ship2dy = -1;
    }
    else
    {
        Ship1dy = 0;
        Ship2dy = 0;
    }
}

void initialize()
{
    // initialize
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)
    for (int i = 0; i < numAsteroids; i++)
    {
        // initialize direction
        Asteroids[i].dx = 3 * (((rand() % 2) * 2) - 1); // 1 or -1
        Asteroids[i].dy = 0;

        // initialize x and y positions
        Asteroids[i].x = rand() % 319;
        Asteroids[i].y = rand() % 239;
        ONCHIP[i].x = Asteroids[i].x;
        ONCHIP[i].y = Asteroids[i].y;
        SDRAM[i].x = Asteroids[i].x;
        SDRAM[i].y = Asteroids[i].y;
    }

    Ship1.x = 319 / 4 - shipWidth / 2;
    Ship1.y = 239 - shipLength - 1;
    Ship1dy = 0; // initialize ship1dy

    Ship1SDRAM.x = 319 / 4 - shipWidth / 2;
    Ship1SDRAM.y = 239 - shipLength - 1;

    Ship1ONCHIP.x = 319 / 4 - shipWidth / 2;
    Ship1ONCHIP.y = 239 - shipLength - 1;

    Ship2.x = 319 * 3 / 4 - shipWidth / 2;
    Ship2.y = 239 - shipLength - 1;
    Ship2dy = 0; // initialize ship1dy

    Ship2SDRAM.x = 319 * 3 / 4 - shipWidth / 2;
    Ship2SDRAM.y = 239 - shipLength - 1;

    Ship2ONCHIP.x = 319 * 3 / 4 - shipWidth / 2;
    Ship2ONCHIP.y = 239 - shipLength - 1;
}

// override the previous lines with black, depending on which was the previous buffer
void clear()
{
    // if start buffer is SDRAM
    if (pixel_buffer_start == 0xC0000000)
    {
        for (int i = 0; i < numAsteroids; i++)
        {
            plot_pixel(SDRAM[i].x, SDRAM[i].y, 0x0000);
            plot_pixel(SDRAM[i].x + 1, SDRAM[i].y, 0x0000);
            plot_pixel(SDRAM[i].x, SDRAM[i].y + 1, 0x0000);
            plot_pixel(SDRAM[i].x + 1, SDRAM[i].y + 1, 0x0000);
        }

        drawSquare(Ship1SDRAM.x, Ship1SDRAM.y, shipLength, shipWidth, 0x0000);
        drawSquare(Ship2SDRAM.x, Ship2SDRAM.y, shipLength, shipWidth, 0x0000);
    }
    // if start buffer is On chip memory
    else if (pixel_buffer_start == 0xC8000000)
    {
        for (int i = 0; i < numAsteroids; i++)
        {
            plot_pixel(ONCHIP[i].x, ONCHIP[i].y, 0x0000);
            plot_pixel(ONCHIP[i].x + 1, ONCHIP[i].y, 0x0000);
            plot_pixel(ONCHIP[i].x, ONCHIP[i].y + 1, 0x0000);
            plot_pixel(ONCHIP[i].x + 1, ONCHIP[i].y + 1, 0x0000);
        }
        drawSquare(Ship1ONCHIP.x, Ship1ONCHIP.y, shipLength, shipWidth, 0x0000);
        drawSquare(Ship2ONCHIP.x, Ship2ONCHIP.y, shipLength, shipWidth, 0x0000);
    }
}

// code for subroutines (not shown)
void draw()
{

    // draw asteroids
    for (int i = 0; i < numAsteroids; i++)
    {
        // drawing rectangles
        plot_pixel(Asteroids[i].x, Asteroids[i].y, 0xFFFF);
        plot_pixel(Asteroids[i].x + 1, Asteroids[i].y, 0xFFFF);
        plot_pixel(Asteroids[i].x, Asteroids[i].y + 1, 0xFFFF);
        plot_pixel(Asteroids[i].x + 1, Asteroids[i].y + 1, 0xFFFF);
    }

    // draw ship
    drawSquare(Ship1.x, Ship1.y, shipLength, shipWidth, 0xFFFF);
    drawSquare(Ship2.x, Ship2.y, shipLength, shipWidth, 0xFFFF);
}

void drawSquare(int x, int y, int length, int width, short int line_color)
{
    draw_line(x, y, x + width, y, line_color);
    draw_line(x, y, x, y + length, line_color);
    draw_line(x + width, y, x + width, y + length, line_color);
    draw_line(x, y + length, x + width, y + length, line_color);
}

// update movement of boxes
void update()
{
    for (int i = 0; i < numAsteroids; i++)
    {
        // save the position to erase
        if (pixel_buffer_start == 0xC0000000)
        {
            SDRAM[i].x = Asteroids[i].x;
            SDRAM[i].y = Asteroids[i].y;
        }
        else if (pixel_buffer_start == 0xC8000000)
        {
            ONCHIP[i].x = Asteroids[i].x;
            ONCHIP[i].y = Asteroids[i].y;
        }

        // if reached right side or left side, reverse direction
        if ((Asteroids[i].x) >= 319 || Asteroids[i].x <= 0)
        {
            Asteroids[i].dx *= -1;
            Asteroids[i].y = rand() % 239;
        }

        // actually move
        Asteroids[i].x = Asteroids[i].x + Asteroids[i].dx;
        Asteroids[i].y = Asteroids[i].y + Asteroids[i].dy;
    }
    // update ship
    if (pixel_buffer_start == 0xC0000000)
    {
        Ship1SDRAM.x = Ship1.x;
        Ship1SDRAM.y = Ship1.y;
        Ship2SDRAM.x = Ship2.x;
        Ship2SDRAM.y = Ship2.y;
    }
    else if (pixel_buffer_start == 0xC8000000)
    {
        Ship1ONCHIP.x = Ship1.x;
        Ship1ONCHIP.y = Ship1.y;
        Ship2ONCHIP.x = Ship2.x;
        Ship2ONCHIP.y = Ship2.y;
    }

    if (Ship1.y <= 0 || Ship1.y + shipLength >= 239)
    {
        Ship1.y = 239 - shipLength - 1;
    }
    if (Ship2.y <= 0 || Ship2.y + shipLength >= 239)
    {
        Ship2.y = 239 - shipLength - 1;
    }

    for (int i = 0; i < numAsteroids; ++i)
    {
        if ((Asteroids[i].x <= Ship1.x + shipWidth) && (Asteroids[i].x >= Ship1.x) && (Asteroids[i].y <= Ship1.y + shipLength) && (Asteroids[i].y >= Ship1.y))
        {
            Ship1.y = 239 - shipLength - 1;
        }

        if ((Asteroids[i].x <= Ship2.x + shipWidth) && (Asteroids[i].x >= Ship2.x) && (Asteroids[i].y <= Ship2.y + shipLength) && (Asteroids[i].y >= Ship2.y))
        {
            Ship2.y = 239 - shipLength - 1;
        }
    }

    // actually move the ship
    Ship1.y += Ship1dy;
    Ship2.y += Ship2dy;
}

// clears screen by drawing all black
void clear_screen()
{
    for (int i = 0xC8000000; i < 0xC803BE7E; i = i + 2)
    {
        *(short int *)(i) = 0x0000;
    }

    for (int i = 0xC0000000; i < 0xC003BE7E; i = i + 2)
    {
        *(short int *)(i) = 0x0000;
    }
}

// plot pixel function (provided)
void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// wait for sync function
void wait_for_sync()
{
    volatile int *pixel_ctrl_ptr = 0xFF203020;
    register int status;

    *pixel_ctrl_ptr = 1;

    status = *(pixel_ctrl_ptr + 3);
    while ((status & 0x01) != 0)
    {
        status = *(pixel_ctrl_ptr + 3);
    }
    return;
}

void draw_line(int x0, int y0, int x1, int y1, short int line_color)
{
    bool is_steep = abs(y1 - y0) > abs(x1 - x0);

    if (is_steep)
    {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }
    if (x0 > x1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step;

    if (y0 < y1)
    {
        y_step = 1;
    }
    else
    {
        y_step = -1;
    }

    for (int x = x0; x < x1; ++x)
    {
        if (is_steep)
        {
            plot_pixel(y, x, line_color);
        }
        else
        {
            plot_pixel(x, y, line_color);
        }
        error = error + deltay;
        if (error >= 0)
        {
            y = y + y_step;
            error = error - deltax;
        }
    }
}

void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}
