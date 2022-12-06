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

/*	if(speed <= 299 && speed >= 200)
   		{
			if (ps2_p->get_kb_ch(&ch)) {
              if(ch == 'l')
              {
              	  gx = gx + 2;
              }
              if(ch == 'j')
              {
            	  gx = gx - 2;
              }
              if(ch == 'i')
              {
              	  gy = gy - 2;
              }
              if(ch == 'k')
              {
              	  gy = gy + 2;
              }
            } // end get_kb_ch()
         if(speed <= 399 && speed >= 300)
         {
         	 if (ps2_p->get_kb_ch(&ch)) {
              if(ch == 'l')
              {
              	  gx = gx + 3;
              }
              if(ch == 'j')
              {
            	  gx = gx - 3;
              }
              if(ch == 'i')
              {
              	  gy = gy - 3;
              }
              if(ch == 'k')
              {
              	  gy = gy + 3;
              }
            } // end get_kb_ch()
         }

         else
         {

         }
   		}*/

/**
 * test ghost sprite
 * @param ghost_p pointer to mouse sprite instance
 */
void mouse_check(SpriteCore *mouse_p, Ps2Core *ps2_p, SpriteCore *ghost_p, FrameCore *frame_p, OsdCore *osd_p, uint32_t speed) {
   int mx, my , gx, gy;
   char ch;
   unsigned long last;
   int xdis, ydis;
   int mxPrev, myPrev, gxPrev, gyPrev;
   int start = 0;
   static int time_string [7] = {84, 73, 77, 69, 32, 61, 32};
   int timer_value = 60;

   mouse_p->bypass(0);
   ghost_p->bypass(0);
   //frame_p->bypass(0);
   //osd_p -> bypass(0);

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
   mxPrev = mx; myPrev = my;
   gxPrev = gx; gyPrev = gy;

   ps2_p->get_kb_ch(&ch);
   if(ch == 'c')
   {
	   start = 1;
   }
   else
   {
	   start = 0;
   }
	while(start == 1){

		  // wasd for mouse movement
		  // ijkl for ghost movement
		  if (ps2_p->get_kb_ch(&ch)) {
		                if(ch == 'd')
		                {
		              	  mx = mx + 1;
		                }
		                if(ch == 'a')
		                {
		              	  mx = mx - 1;
		                }
		                if(ch == 'w')
		                {
		              	  my = my - 1;
		                }
		                if(ch == 's')
		                {
		              	  my = my + 1;
		                }
		                if(ch == 'l')
		                {
		                	  gx = gx + 1;
		                }
		                if(ch == 'j')
		                {
		              	  gx = gx - 1;
		                }
		                if(ch == 'i')
		                {
		                	  gy = gy - 1;
		                }
		                if(ch == 'k')
		                {
		                	  gy = gy + 1;
		                }

		                 last = now_ms();
		              } // end get_kb_ch()
		     		mouse_p->move_xy(mx,my);
		     		ghost_p -> move_xy(gx,gy);

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

		     		if(xdis <= 20 && ydis<= 20) // if ghost and mouse touch clear screen
		     		{
		     			frame_p->clr_screen(0x123);

		     		}
		     		if(mx >= 650 && mx <= 50 && my >= 450 && my <= 50) //mouse touches edges it loses
		     		{
		     			frame_p->clr_screen(0x345);
		     		}
		     		else if(gx >= 650 && gx <= 50 && gy >= 450 && gy <= 50) // ghost touches edges it loses
		     		{
		     			frame_p -> clr_screen(0x456);
		     		}

	}

}

uint32_t ghostSpeed(XadcCore *adc_p)
{
	double reading;
	uart.disp("analog channel/voltage: ");
	uart.disp(0);
	uart.disp(" / ");
	reading = 1000*(adc_p->read_adc_in(0)); // 0 to 1 Volt
	uart.disp(reading, 3);
	uart.disp("\n\r");
	//sleep_ms(500);
	return reading; // Return reading
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

int main() {

	int timer_value = 60;
	double speed = 0;

	frame.clr_screen(0xfff);
	frame.plot_line(50, 50, 50, 450, 0x00f); //left side
	frame.plot_line(50, 50, 600, 50, 0x0f);	//top side
	frame.plot_line(50, 450, 600, 450, 0xf09);	//bottom side
	frame.plot_line(600, 50, 600, 450, 0xff0); // right side
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
      //do {
    	  display_time_const(&osd, &timer_value);
    	  speed = ghostSpeed(&adc);
      mouse_check(&mouse,&ps2,&ghost, &frame,&osd,speed);
      //timer_value = timer_value - 1;
     // }while(timer_value != 0);
      //timer_value = 60;


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
