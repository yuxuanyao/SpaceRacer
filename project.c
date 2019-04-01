#include <stdbool.h>

volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_line(int x_start, int y_start, int x_end, int y_end, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void wait_for_sync();
void drawMeteor();
void update();
void clear();

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
struct prevLocation SDRAM[8];
struct prevLocation ONCHIP[8];
struct box Box[8];

int main(void)
{
    volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

    // initialize
    for (int i = 0; i < 8; i++)
    {
        // initialize direction
        Box[i].dx = ((rand() % 2) * 2) - 1; // 1 or -1
        Box[i].dy = 0;                      // 1 or -1

        // initialize x and y positions
        Box[i].x = rand() % 319;
        Box[i].y = rand() % 239;
        ONCHIP[i].x = Box[i].x;
        ONCHIP[i].y = Box[i].y;
        SDRAM[i].x = Box[i].x;
        SDRAM[i].y = Box[i].y;
    }

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_sync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer

    clear_screen(); // pixel_buffer_start points to the pixel buffer

    bool cleared = false;
    while (1)
    {
        // clear_screen_partial();
        clear_screen();

        // code for drawing the boxes and lines
        drawMeteor();

        // code for updating the locations of boxes
        update();

        wait_for_sync();                            // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

// override the previous lines with black, depending on which was the previous buffer
void clear()
{
    // if start buffer is SDRAM
    if (pixel_buffer_start == 0xC0000000)
    {
        for (int i = 0; i < 8; i++)
        {
            plot_pixel(SDRAM[i].x, SDRAM[i].y, 0x000);
            plot_pixel(SDRAM[i].x + 1, SDRAM[i].y, 0x000);
            plot_pixel(SDRAM[i].x, SDRAM[i].y + 1, 0x0000);
            plot_pixel(SDRAM[i].x + 1, SDRAM[i].y + 1, 0x000);
        }
    }
    // if start buffer is On chip memory
    else if (pixel_buffer_start == 0xC8000000)
    {
        for (int i = 0; i < 8; i++)
        {
            plot_pixel(ONCHIP[i].x, ONCHIP[i].y, 0x000);
            plot_pixel(ONCHIP[i].x + 1, ONCHIP[i].y, 0x000);
            plot_pixel(ONCHIP[i].x, ONCHIP[i].y + 1, 0x0000);
            plot_pixel(ONCHIP[i].x + 1, ONCHIP[i].y + 1, 0x000);
        }
    }
}

// code for subroutines (not shown)
void drawMeteor()
{
    for (int i = 0; i < 8; i++)
    {
        // drawing rectangles
        plot_pixel(Box[i].x, Box[i].y, 0xFFFF);
        plot_pixel(Box[i].x + 1, Box[i].y, 0xFFFF);
        plot_pixel(Box[i].x, Box[i].y + 1, 0xFFFF);
        plot_pixel(Box[i].x + 1, Box[i].y + 1, 0xFFFF);
    }
}

// update movement of boxes
void update()
{
    for (int i = 0; i < 8; i++)
    {
        // save the position to erase
        if (pixel_buffer_start == 0xC0000000)
        {
            SDRAM[i].x = Box[i].x;
            SDRAM[i].y = Box[i].y;
        }
        else if (pixel_buffer_start == 0xC8000000)
        {
            ONCHIP[i].x = Box[i].x;
            ONCHIP[i].y = Box[i].y;
        }

        // if reached right side or left side, reverse direction
        if ((Box[i].x) >= 319 || Box[i].x <= 0)
        {
            Box[i].dx *= -1;
        }

        // actually move
        Box[i].x = Box[i].x + Box[i].dx;
        Box[i].y = Box[i].y + Box[i].dy;
    }
}

// clears screen by drawing all black
void clear_screen()
{
    for (int y = 0; y < 240; ++y)
    {
        for (int x = 0; x < 360; ++x)
        {
            // plot black at every pixel
            plot_pixel(x, y, 0);
        }
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
