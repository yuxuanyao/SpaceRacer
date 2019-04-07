volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_line(int x_start, int y_start, int x_end, int y_end, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void wait_for_sync();
void draw();
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
        Box[i].dy = ((rand() % 2) * 2) - 1; // 1 or -1

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
            plot_pixel(SDRAM[i].x + 1, SDRAM[i].y + 1, 0x00);
            if (i == 7)
            {
                draw_line(SDRAM[i].x, SDRAM[i].y, SDRAM[0].x, SDRAM[0].y, 0x0000);
            }
            else
            {
                draw_line(SDRAM[i].x, SDRAM[i].y, SDRAM[i + 1].x, SDRAM[i + 1].y, 0x0000);
            }
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
            plot_pixel(ONCHIP[i].x + 1, ONCHIP[i].y + 1, 0x00);
            if (i == 7)
            {
                draw_line(ONCHIP[i].x, ONCHIP[i].y, ONCHIP[0].x, ONCHIP[0].y, 0x0000);
            }
            else
            {
                draw_line(ONCHIP[i].x, ONCHIP[i].y, ONCHIP[i + 1].x, ONCHIP[i + 1].y, 0x0000);
            }
        }
    }
}

// code for subroutines (not shown)
void draw()
{
    for (int i = 0; i < 8; i++)
    {

        // drawing rectangles
        plot_pixel(Box[i].x, Box[i].y, 0x001F);
        plot_pixel(Box[i].x + 1, Box[i].y, 0x001F);
        plot_pixel(Box[i].x, Box[i].y + 1, 0x001F);
        plot_pixel(Box[i].x + 1, Box[i].y + 1, 0x001F);

        // connect last point to first point
        if (i == 7)
        {
            draw_line(Box[i].x, Box[i].y, Box[0].x, Box[0].y, 0x001F);
        }
        // connect to next rectangle
        else
        {
            draw_line(Box[i].x, Box[i].y, Box[i + 1].x, Box[i + 1].y, 0x001F);
        }
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

        // if reached top side or bottom side, reverse direction
        if ((Box[i].y) >= 239 || Box[i].y <= 0)
        {
            Box[i].dy *= -1;
        }

        // actually move
        Box[i].x = Box[i].x + Box[i].dx;
        Box[i].y = Box[i].y + Box[i].dy;
    }
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

// draw line function
void draw_line(int x_start, int y_start, int x_end, int y_end, short int line_color)
{

    int is_steep = (abs(y_end - y_start)) - (abs(x_end - x_start));
    //bool is_steep = (abs(y_end - y_start)) > (abs(x_end - x_start))
    if (is_steep > 0)
    {
        int temp_x_start = x_start;
        int temp_x_end = x_end;
        x_start = y_start;
        y_start = temp_x_start;
        x_end = y_end;
        y_end = temp_x_end;
    }

    if (x_start > x_end)
    {
        int temp_x = x_start;
        int temp_y = y_start;
        x_start = x_end;
        y_start = y_end;
        x_end = temp_x;
        y_end = temp_y;
    }

    int delta_x = x_end - x_start;      //initialize change of x
    int delta_y = abs(y_end - y_start); //change of y
    int error = -(delta_x / 2);         // set error
    int x, y, y_step;

    if (y_start < y_end)
        y_step = 1;
    else
        y_step = -1;

    for (x = x_start, y = y_start; x < (x_end + 1); x++)
    {
        if (is_steep > 0)
            plot_pixel(y, x, line_color);
        else
            plot_pixel(x, y, line_color);
        error = error + delta_y;
        if (error >= 0)
        {
            y = y + y_step;
            error = error - delta_x;
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
