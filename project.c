#include <stdbool.h>

volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_line(int x_start, int y_start, int x_end, int y_end, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void wait_for_sync();
void draw();
void drawSquare(int x, int y, int sideLength, short int line_color);
void update();
void clear();
void initialize();

#define numAsteroids 20
#define shipSize 21

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
struct box Ship1SDRAM;
struct box Ship1ONCHIP;

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

        // code for updating the locations of boxes
        update();

        wait_for_sync();                            // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
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

    Ship1.x = 319 / 2 - shipSize / 2;
    Ship1.y = 239 - shipSize - 1;
    Ship1.dy = 1;
    Ship1.dx = 0;

    Ship1SDRAM.x = 319 / 2 - shipSize / 2;
    Ship1SDRAM.y = 239 - shipSize - 1;
    Ship1SDRAM.dy = 1;
    Ship1SDRAM.dx = 0;

    Ship1ONCHIP.x = 319 / 2 - shipSize / 2;
    Ship1ONCHIP.y = 239 - shipSize - 1;
    Ship1ONCHIP.dy = -1;
    Ship1ONCHIP.dx = 0;
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

        drawSquare(Ship1SDRAM.x, Ship1SDRAM.y, shipSize, 0x0000);
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
        drawSquare(Ship1ONCHIP.x, Ship1ONCHIP.y, shipSize, 0x0000);
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
    drawSquare(Ship1.x, Ship1.y, shipSize, 0xFFFF);
}

void drawSquare(int x, int y, int size, short int line_color)
{
    // plot_pixel(x, y, 0xFFFF);
    // plot_pixel(x + size, y, 0xFFFF);
    // plot_pixel(x, y + size, 0xFFFF);
    // plot_pixel(x + size, y + size, 0xFFFF);
    draw_line(x, y, x + size, y, line_color);
    draw_line(x, y, x, y + size, line_color);
    draw_line(x + size, y, x + size, y + size, line_color);
    draw_line(x, y + size, x + size, y + size, line_color);
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
        }

        // if reached top side or bottom side, reverse direction
        if ((Asteroids[i].y) >= 239 || Asteroids[i].y <= 0)
        {
            Asteroids[i].dy *= -1;
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
    }
    else if (pixel_buffer_start == 0xC8000000)
    {
        Ship1ONCHIP.x = Ship1.x;
        Ship1ONCHIP.y = Ship1.y;
    }

    if ((Ship1.y + shipSize) >= 239 || Ship1.y <= 0)
    {
        Ship1.dy *= -1;
    }

    // actually move the ship
    Ship1.y += Ship1.dy;
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