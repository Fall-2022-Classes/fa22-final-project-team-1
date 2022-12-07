/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "xadc_core.h"
#include "spi_core.h"
#include "i2c_core.h"
#include "ps2_core.h"
#include "ddfs_core.h"
#include "adsr_core.h"
#include "timer_core.h"

void test_start(GpoCore *led_p) {
   int i;

   for (i = 0; i < 20; i++) {
      led_p->write(0xff00);
      sleep_ms(50);
      led_p->write(0x0000);
      sleep_ms(50);
   }
}

/**
 * check bar generator core
 * @param bar_p pointer to Gpv instance
 */
void bar_check(GpvCore *bar_p) {
   bar_p->bypass(0);
   sleep_ms(3000);
}

/**
 * check color-to-grayscale core
 * @param gray_p pointer to Gpv instance
 */
void gray_check(GpvCore *gray_p) {
   gray_p->bypass(0);
   sleep_ms(3000);
   gray_p->bypass(1);
}

/**
 * check osd core
 * @param osd_p pointer to osd instance
 */
void osd_check(OsdCore *osd_p) {
   osd_p->set_color(0x0f0, 0x001); // dark gray/green
   osd_p->bypass(0);
   osd_p->clr_screen();
   for (int i = 0; i < 64; i++) {
      osd_p->wr_char(8 + i, 20, i);
      osd_p->wr_char(8 + i, 21, 64 + i, 1);
      sleep_ms(100);
   }
   sleep_ms(3000);
}

/**
 * test frame buffer core
 * @param frame_p pointer to frame buffer instance
 */
void frame_check(FrameCore *frame_p) {
   int x, y, color;

   frame_p->bypass(0);
   for (int i = 0; i < 10; i++) {
      frame_p->clr_screen(0x008);  // dark green
      for (int j = 0; j < 20; j++) {
         x = rand() % 640;
         y = rand() % 480;
         color = rand() % 512;
         frame_p->plot_line(400, 200, x, y, color);
      }
      sleep_ms(300);
   }
   sleep_ms(3000);
}
/**
 * test ghost sprite
 * @param ghost_p pointer to ghost sprite instance
 */
void ghost_check(SpriteCore *ghost_p, Ps2Core *ps2_p) {
	 int x, y;
	   char ch;
	   unsigned long last;

	   ghost_p->bypass(0);

	     // clear top and bottom lines
	     for (int i = 0; i < 32; i++) {
	        ghost_p->wr_mem(i, 0);
	        ghost_p->wr_mem(31 * 32 + i, 0);
	     }
	     last = now_ms();
	   x = 300;
	   y = 300;
	   do{
		   if (ps2_p->get_kb_ch(&ch)) {
	              if(ch == 'l')
	              {
	            	  x = x + 1;
	              }
	              if(ch == 'j')
	              {
	            	  x = x - 1;
	              }
	              if(ch == 'i')
	              {
	            	  y = y - 1;
	              }
	              if(ch == 'k')
	              {
	            	  y = y + 1;
	              }
	               last = now_ms();
	            } // end get_kb_ch()
	   		ghost_p->move_xy(x,y);

	   }while (now_ms() - last < 5000);
}
/*if(time != 0)
		     		{
		     			frame_p -> clr_screen(0x000);
		     			start = 0;
		     		}*/

		     		/*if(time < 999999)
		     		{
		     			time = time + 1;
		     			sleep_ms(100);
		     		}

		     		if(time > 10 && time <19)
		     		{
		     			//  (50, 50)	(600, 50)
		     			//	(50, 450)	(600, 450)
		     			frame_p -> clr_screen(0xfff);
		     			frame_p->plot_line(175, 100, 175, 400, 0x00f); //left side
		     			frame_p->plot_line(175, 100, 475, 100, 0x0f);	//top side
		     			frame_p->plot_line(175, 400, 475, 400, 0xf09);	//bottom side
		     			frame_p->plot_line(475, 100, 475, 400, 0xff0); // right side
		     		}
		     		if(time > 20 && time < 29)
		     		{
		     			frame_p -> clr_screen(0xfff);
		     			frame_p->plot_line(200, 125, 200, 375, 0x00f); //left side
		     			frame_p->plot_line(200, 125, 450, 125, 0x0f);	//top side
		     			frame_p->plot_line(200, 375, 450, 375, 0xf09);	//bottom side
		     			frame_p->plot_line(450, 125, 450, 375, 0xff0); // right side
		     		}*/

/**
 * test ghost sprite
 * @param ghost_p pointer to mouse sprite instance
 */
void mouse_check(SpriteCore *mouse_p, Ps2Core *ps2_p, SpriteCore *ghost_p, FrameCore *frame_p, OsdCore *osd_p, uint32_t speed, TimerCore *time_p,int &start) {
   int mx, my , gx, gy;
   char ch;
   unsigned long last;
   int xdis, ydis;
   int xHigh, xLow, yHigh, yLow;
   int time_value = 0;
   static int mouse_string[10] = {77,79,85,83,69,0,76,79,83,69};
   static int ghost_string[10] = {71,72,79,83,84,0,76,79,83,69};
   static int win_string[9]={71,72,79,83,84,0,87,73,78};
   mouse_p->bypass(0);
   ghost_p->bypass(0);
   frame_p->bypass(0);
   osd_p -> bypass(0);

    // dark gray/green
     // clear top and bottom lines
     /*for (int i = 0; i < 32; i++) {
        mouse_p->wr_mem(i, 0);
        ghost_p->wr_mem(i, 0);
        mouse_p->wr_mem(31 * 32 + i, 0);
        ghost_p->wr_mem(31 * 32 + i, 0);
     }*/

     last = now_ms();
   mx = 400; gx = 300;
   my = 400; gy = 300;
   xHigh = 600; xLow = 50;
   yHigh = 450; yLow = 50;

		  // wasd for mouse movement
		  // ijkl for ghost movement
   	   	   //r for stopping
   while(start == 1)

   {
		  if (ps2_p->get_kb_ch(&ch)) {
		                if(ch == 'd')
		                {
		              	  mx = mx + 4;
		                }
		                if(ch == 'a')
		                {
		              	  mx = mx - 4;
		                }
		                if(ch == 'w')
		                {
		              	  my = my - 4;
		                }
		                if(ch == 's')
		                {
		              	  my = my + 4;
		                }
		                if(ch == 'l')
		                {
		                	  gx = gx + speed;
		                }
		                if(ch == 'j')
		                {
		              	  gx = gx - speed;
		                }
		                if(ch == 'i')
		                {
		                	  gy = gy - speed;
		                }
		                if(ch == 'k')
		                {
		                	  gy = gy + speed;
		                }
		                if(ch == 'r')
		                {
		                	start = 0;

		                }
		                 //last = now_ms();
		              } // end get_kb_ch()
		     		mouse_p->move_xy(mx,my); // move mouse
		     		ghost_p -> move_xy(gx,gy); // move ghost

		     		xdis = gx - mx;
		     		ydis = gy - my;

		     		if(xdis < 0) // absolute value
		     		{
		     			xdis = xdis * -1;
		     		}
		     		if(ydis <0)
		     		{
		     			ydis = ydis * -1;
		     		}

		     		if(xdis <= 20 && ydis<= 20) // if ghost and mouse touch
		     		{

		     			frame_p->clr_screen(0xfff);
		     			for (int i = 0; i < 9; i++)
		     				{
		     					osd_p->wr_char(275+i, 0, win_string[i]);// display ghost win

		     				}

		     			start = 0;

		     		}

		     		if(mx >= 600 || mx <= 50 || my >= 450 || my <= 50) //mouse touches edges it loses
		     		{

		     			frame_p->clr_screen(0xfff);
		     			for (int i = 0; i < 10; i++)
		     				{
		     					osd_p->wr_char(275+i, 0, mouse_string[i]);// display mouse lose
		     				}

		     			start = 0;
		     		}
		     		if(gx >= 600 || gx <= 50 || gy >= 450 || gy <= 50) // ghost touches edges it loses
		     		{

		     			frame_p -> clr_screen(0xfff);
		     			for (int i = 0; i < 10; i++)
		     			{
		     				osd_p->wr_char(275+i, 0, ghost_string[i]);// display ghost lose
		     			}


		     		}

	   	   //}
	   	   /*time_p -> clear();
	   	   time_p -> pause();
	   	   start = 0;*/

	}

}

uint32_t ghostSpeed(XadcCore *adc_p)
{
	double reading;
	int final;
	uart.disp("analog channel/voltage: ");
	uart.disp(0);
	uart.disp(" / ");
	reading = 1000*(adc_p->read_adc_in(0)); // 0 to 1 Volt
	uart.disp(reading, 3);
	uart.disp("\n\r");
	//sleep_ms(500);
	if (reading > 0 && reading < 333)
		{
			final = 2;
		}
		if (reading >= 333 && reading < 666)
			{
			final = 8;
			}
		if (reading >= 666 && reading <= 1000)
		{
			final = 12;
		}
		uart.disp("final reading: ");
		uart.disp(final);
		uart.disp("\n\r"); //Return reading
		return final;
}

void draw_square(FrameCore *frame_p)
{
	//  (50, 50)	(600, 50)
	//	(50, 450)	(600, 450)

	int x, y, color;

	//frame_p->clr_screen(0x000);  // dark green
	frame_p->plot_line(50, 50, 50, 450, 0x00f); //left side
	frame_p->plot_line(50, 50, 600, 50, 0x0f);	//top side
	frame_p->plot_line(50, 450, 600, 450, 0xf09);	//bottom side
	frame_p->plot_line(600, 50, 600, 450, 0xff0); // right side
}

void display_time_const(OsdCore *osd_p, int *timer_value)
{
	static int time_string [7] = {84, 73, 77, 69, 32, 61, 32};

	osd_p->set_color(0x001, 0x000);
	for (int i = 0; i < 7; i++)
	{
		osd_p->wr_char(10+i, 0, time_string[i]);
	}

	if(*timer_value < 99999999999 && *timer_value >= 30)
	{
		osd_p->wr_char(18, 0, 43);
		sleep_ms(500);
		osd_p->wr_char(18, 0, 0);
		sleep_ms(500);
	}
	else if (*timer_value < 30 && *timer_value >= 10)
	{
		osd_p->wr_char(18, 0, 45);
		sleep_ms(500);
		osd_p->wr_char(18, 0, 0);
		sleep_ms(500);
	}
	else
	{
		osd_p->wr_char(18, 0, 33);
		sleep_ms(500);
		osd_p->wr_char(18, 0, 0);
		sleep_ms(500);
	}

}


void startKey(Ps2Core *ps2_p, int &start, int &time, FrameCore *frame_p)
{
	char ch;

	ps2_p->get_kb_ch(&ch);
	if(ch == 'c')
	{
		start = 1;
		frame_p->clr_screen(0xfff);
		draw_square(frame_p);
	}
	else
	{
		start = 0;
	}
}

void display_speed(OsdCore *osd_p,int speed)
{
	static int speed_string[5] = {83,80,69,69,68};
	uint8_t x = 308;
	uint8_t y = 0;
	uint8_t speed1 = 49;
	uint8_t speed2 = 50;
	uint8_t speed3 = 51;
	for (int i = 0; i < 5; i++)
			{
				osd_p->wr_char(300+i, y, speed_string[i]); //display speed
			}
	if(speed == 2)
	{
		osd_p->wr_char(x, y, speed1); //display 1
	}
	if(speed == 8)
	{
		osd_p->wr_char(x, y, speed2); //display 2
	}
	if(speed == 12)
	{
		osd_p->wr_char(x, y, speed3);// display 3
	}
}
uint64_t getTime(TimerCore *time_p)
{
	uint64_t time_value;
	time_value = time_p ->read_time();
	return time_value;
}

void display_time_value(TimerCore *time_p)
{
	uint64_t time_value;
	int startClk = 1;
	if(startClk == 1)
	{
		time_p->clear();
		time_p ->pause();
		startClk = 0;
		time_value = time_p ->read_time();
	}
	time_p -> go();
	time_value = time_p ->read_time();

	uart.disp("The time is: ");
	uart.disp(time_value,10,3);
	}
//uint64_t read_time()
// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));
DebounceCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));
I2cCore adt7420(get_slot_addr(BRIDGE_BASE, S10_I2C));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
DdfsCore ddfs(get_slot_addr(BRIDGE_BASE, S12_DDFS));
AdsrCore adsr(get_slot_addr(BRIDGE_BASE, S13_ADSR), &ddfs);
TimerCore timer(get_slot_addr(BRIDGE_BASE, S0_SYS_TIMER));

int main() {

	int timer_value = 30;
	double speed = 0;
	int start = 0;
	int time = 500;
	uint64_t showTime;
	frame.clr_screen(0xfff);
	draw_square(&frame);
   while (1) {

     // test_start(&led);
      // bypass all cores
     // frame.bypass(1);
      //bar.bypass(1);
      //gray.bypass(1);
      //ghost.bypass(1);
      //osd.bypass(1);
      //mouse.bypass(1);
      //sleep_ms(3000);

      // enable cores one by one
     // frame_check(&frame);
     // bar_check(&bar);
     // gray_check(&gray);
      //ghost_check(&ghost,&ps2);
      //osd_check(&osd);
	  // showTime = getTime(&timer);
	  // display_time_value(&timer);
	   startKey(&ps2,start,time,&frame); // press c to start game
	   speed = ghostSpeed(&adc); // gets the speed of the ghost
	   display_speed(&osd, speed); // display the ghost speed on the screen
      while(start == 1)
      {

    	  osd.clr_screen();
    	  display_time_const(&osd, &timer_value);
    	  display_speed(&osd, speed);
    	  mouse_check(&mouse,&ps2,&ghost, &frame,&osd,speed,&timer,start);

      }



      while (sw.read(0)) {
         // test composition with different overlays if sw(0) is 1
         mouse.bypass(sw.read(1));
         //osd.bypass(sw.read(2));
        // ghost.bypass(sw.read(3));
         //gray.bypass(sw.read(6));
         //bar.bypass(sw.read(7));
         //frame.bypass(sw.read(8));
         // set osd background color to transparent
        // osd.set_color(0x0f0, sw.read(9));
         // set color/animation of ghost sprite
         //ghost.wr_ctrl(sw.read() >> 11);
      } //while
   } // while
} //main
