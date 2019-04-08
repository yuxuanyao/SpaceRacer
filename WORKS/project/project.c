#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "interrupt_ID.h"
#include "defines.h"
#include "address_map_arm.h"

void disable_A9_interrupts(void);
void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_KEYs(void);
void enable_A9_interrupts(void);


volatile int pixel_buffer_start; // global variable
volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
//extern short game_over_image [240][320];
extern short ship [22][18];
extern short title [240][320];
extern short end1 [240][320];
int score = 0;
int score2 = 0;
bool start;

void wait_for_vsync();
//void clear_screen();
//void plot_pixel(int x, int y, short int line_color);

void write_char(int x, int y, char c);
void load_title();


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
//our stuff
volatile int Ship1dy;
volatile int Ship2dy;

volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_ship(int x, int y);
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
void clear_ship(int x, int y);
void load_end1();

#define numAsteroids 5
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

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

int main(void)
{
    
    disable_A9_interrupts(); // disable interrupts in the A9 processor
	set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
	config_GIC(); // configure the general interrupt controller
	config_KEYs(); // configure pushbutton KEYs to generate interrupts
	enable_A9_interrupts(); // enable interrupts in the A9 processor
    
    initialize();
    
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)
   
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	clear_screen();
	
	/* SW base address */
	volatile int * SW_ptr = (int *) 0xFF200040;	
	int before = * SW_ptr;
	
    
	// start screen
	int current = * SW_ptr;
    start = false;
	//while (current == before) {		
		//load_title();
		//current = * SW_ptr;
	//}
    while (start == false) {		
		load_title();
	}
    
    clear_screen();
 	
    score = 0;
    score2 = 0;
	while (1) {
        // clear_screen_partial();
        clear();

        // code for drawing the boxes and lines
        draw();

        checkKeyPress();
        // code for updating the locations of boxes
        update();
        
        write_char(3, 6, 'S');
		write_char(4, 6, 'C');
		write_char(5, 6, 'O');
		write_char(6, 6, 'R');
		write_char(7, 6, 'E');
		write_char(8, 6, ':');
		write_char(9, 6, ('0'+score));
        
        write_char(63, 6, 'S');
		write_char(64, 6, 'C');
		write_char(65, 6, 'O');
		write_char(66, 6, 'R');
		write_char(67, 6, 'E');
		write_char(68, 6, ':');
		write_char(69, 6, ('0'+score2));
        
        if (score == 5 || score2 == 5){
            start = false;
            score = 0;
            score2 = 0;
            while (start == false) {		
				load_end1();
			}
            clear_screen();
        }        

        wait_for_sync();                            // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	}
	
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    return 1;
}



void draw_ship(int x, int y) {
    //volatile short * pixelbuf = 0xc8000000;
	int i, j;
    for(i = 0; i < 22; i++){
    	for(j = 0; j < 18; j++){
			 plot_pixel(x + j, y + i,ship[i][j]);	
        }
    }
}

void load_title() {
	
    volatile short * pixelbuf = 0xc8000000;
    int i, j;
    for (i=0; i<240; i++) 
        for (j=0; j<320; j++)
        //*(pixelbuf + (j<<0) + (i<<9)) = gameOver[i][j];
		plot_pixel(j, i, title[i][j]);
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
}

void load_end1() {
	
    volatile short * pixelbuf = 0xc8000000;
    int i, j;
    for (i=0; i<240; i++) 
        for (j=0; j<320; j++)
        //*(pixelbuf + (j<<0) + (i<<9)) = gameOver[i][j];
		plot_pixel(j, i, end1[i][j]);
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
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

void clear_ship(int x, int y) {
    //volatile short * pixelbuf = 0xc8000000;
	int i, j;
    for(i = 0; i < 22; i++){
    	for(j = 0; j < 18; j++){
             //*(pixel_buffer_start + (j<<0) + (i<<9)) = bird[i][j];
			 plot_pixel(x + j, y + i, 0x0000);	
        }
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

        clear_ship(Ship1SDRAM.x, Ship1SDRAM.y);
        clear_ship(Ship2SDRAM.x, Ship2SDRAM.y);
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
        clear_ship(Ship1ONCHIP.x, Ship1ONCHIP.y);
        clear_ship(Ship2ONCHIP.x, Ship2ONCHIP.y);
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
    
    draw_ship(Ship1.x, Ship1.y);
    draw_ship(Ship2.x, Ship2.y);
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
    	if (Ship1.y <=1){
        	score++;
        }
        Ship1.y = 239 - shipLength - 1;
    }
    if (Ship2.y <= 0 || Ship2.y + shipLength >= 239)
    {
    	if (Ship2.y <= 1){
        	score2++;
        }
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

// wait for sync function
void wait_for_vsync()
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
















/* setup the KEY interrupts in the FPGA */
void config_KEYs() {
volatile int * KEY_ptr = (int *) 0xFF200050; // pushbutton KEY base address
*(KEY_ptr + 2) = 0xF; // enable interrupts for the two KEYs
}

/* This file:
* 1. defines exception vectors for the A9 processor
* 2. provides code that sets the IRQ mode stack, and that dis/enables
* interrupts
* 3. provides code that initializes the generic interrupt controller
*/
void pushbutton_ISR(void);
void config_interrupt(int, int);
// Define the IRQ exception handler
void __attribute__((interrupt)) __cs3_isr_irq(void) {
	// Read the ICCIAR from the CPU Interface in the GIC
	int interrupt_ID = *((int *)0xFFFEC10C);
	if (interrupt_ID == 73) // check if interrupt is from the KEYs
		pushbutton_ISR();
	else
		while (1); // if unexpected, then stay here
	// Write to the End of Interrupt Register (ICCEOIR)
	*((int *)0xFFFEC110) = interrupt_ID;
}

// Define the remaining exception handlers
void __attribute__((interrupt)) __cs3_reset(void) {
while (1);
}
void __attribute__((interrupt)) __cs3_isr_undef(void) {
while (1);
}
void __attribute__((interrupt)) __cs3_isr_swi(void) {
while (1);
}
void __attribute__((interrupt)) __cs3_isr_pabort(void) {
while (1);
}
void __attribute__((interrupt)) __cs3_isr_dabort(void) {
while (1);
}
void __attribute__((interrupt)) __cs3_isr_fiq(void) {
while (1);
}
/*
* Turn off interrupts in the ARM processor
*/
void disable_A9_interrupts(void) {
	int status = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}
/*
* Initialize the banked stack pointer register for IRQ mode
*/
void set_A9_IRQ_stack(void) {
	int stack, mode;
	stack = 0xFFFFFFFF - 7; // top of A9 onchip memory, aligned to 8 bytes
	/* change processor to IRQ mode with interrupts disabled */
	mode = 0b11010010;
	asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
	/* set banked stack pointer */
	asm("mov sp, %[ps]" : : [ps] "r"(stack));
	/* go back to SVC mode before executing subroutine return! */
	mode = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
}

/*
* Turn on interrupts in the ARM processor
*/
void enable_A9_interrupts(void) {
	int status = 0b01010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

/*
* Configure the Generic Interrupt Controller (GIC)
*/
void config_GIC(void) {
	config_interrupt (73, 1); // configure the FPGA KEYs interrupt (73)
	// Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all
	// priorities
	*((int *) 0xFFFEC104) = 0xFFFF;
	// Set CPU Interface Control Register (ICCICR). Enable signaling of
	// interrupts
	*((int *) 0xFFFEC100) = 1;
	// Configure the Distributor Control Register (ICDDCR) to send pending
	// interrupts to CPUs
	*((int *) 0xFFFED000) = 1;
}
/*
* Configure Set Enable Registers (ICDISERn) and Interrupt Processor Target
* Registers (ICDIPTRn). The default (reset) values are used for other registers
* in the GIC.
*/
void config_interrupt(int N, int CPU_target) {
	int reg_offset, index, value, address;
	/* Configure the Interrupt Set-Enable Registers (ICDISERn).
	* reg_offset = (integer_div(N / 32) * 4
	* value = 1 << (N mod 32) */
	reg_offset = (N >> 3) & 0xFFFFFFFC;
	index = N & 0x1F;
	value = 0x1 << index;
	address = 0xFFFED100 + reg_offset;
	/* Now that we know the register address and value, set the appropriate bit */
	*(int *)address |= value;
	/* Configure the Interrupt Processor Targets Register (ICDIPTRn)
	* reg_offset = integer_div(N / 4) * 4
	* index = N mod 4 */
	reg_offset = (N & 0xFFFFFFFC);
	index = N & 0x3;
	address = 0xFFFED800 + reg_offset + index;
	/* Now that we know the register address and value, write to (only) the
	* appropriate byte */
	*(char *)address = (char)CPU_target;
}

/********************************************************************
* Pushbutton - Interrupt Service Routine
*
* This routine checks which KEY has been pressed. It writes to HEX0
*******************************************************************/
void pushbutton_ISR(void) {
	/* KEY base address */
	volatile int * KEY_ptr = (int *) 0xFF200050;
	/* HEX display base address */
	volatile int * HEX3_HEX0_ptr = (int *) 0xFF200020;
	
	int press, HEX_bits;
	
	press = *(KEY_ptr + 3); // read the pushbutton interrupt register
	*(KEY_ptr + 3) = press; // Clear the interrupt
	if (press & 0x1) { // KEY0
		start = true;
	}
	else if (press & 0x2) { // KEY1
	}	
	else if (press & 0x4) { // KEY2
		start = true;
	}
	else { // press & 0x8, which is KEY3

	}	
	return;
}

void write_char(int x, int y, char c) {
  // VGA character buffer
  volatile char * character_buffer = (char *) (0xC9000000 + (y<<7) + x);
  *character_buffer = c;
}


