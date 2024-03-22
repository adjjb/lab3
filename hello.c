/*
 * Userspace program that communicates with the vga_ball device driver
 * through ioctls
 *
 * Stephen A. Edwards
 * Columbia University
 */

#include <stdio.h>
#include "vga_ball.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int vga_ball_fd;

/* Read and print the background color */
void print_background_color() {
  vga_ball_arg_t vla;
  
  if (ioctl(vga_ball_fd, VGA_BALL_READ_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_READ_BACKGROUND) failed");
      return;
  }
  printf("%02x %02x %02x %02x %02x\n",
	 vla.background.red, vla.background.green, vla.background.blue,vla.background.x,vla.background.y);
}

/* Set the background color */
void set_background_color(const vga_ball_color_t *c)
{
  vga_ball_arg_t vla;
  vla.background = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_SET_BACKGROUND) failed");
      return;
  }
}

int main()
{
  vga_ball_arg_t vla;
  int i = 0;
  static const char filename[] = "/dev/vga_ball";

  static const vga_ball_color_t colors[] = {
    { 0xff, 0xff, 0xff,0x00,0x00 },
    { 0xff, 0x00, 0x00,0x00,0x00 },
    { 0x00, 0xff, 0x00,0x00,0x00 }, 
    { 0x00, 0x00, 0xff,0x00,0x00 }, 
    { 0xff, 0xff, 0x00,0x00,0x00 }, 
    { 0x00, 0xff, 0xff,0x00,0x00 }, 
    { 0xff, 0x00, 0xff,0x00,0x00 }, 
    { 0x80, 0x80, 0x80,0x00,0x00 }
  };

  static vga_ball_color_t background ;
# define COLORS 9

  printf("VGA ball Userspace program started\n");

  if ( (vga_ball_fd = open(filename, O_RDWR)) == -1) {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }

  printf("initial state: ");
  print_background_color();
  
  int offset = 50;
  int x = 200;
  int y = 160;
  int dx = 5;
  int dy = 4;

  int color_num = 1;
  background = colors[1];
  while(i < 500 ){
    
    background.x = x / 5;
    background.y = y / 4;
    set_background_color(&background);
    print_background_color();
    usleep(50000);
    x += dx;
    y += dy;
    if(y > 480 - 42){
	dy = -4;
	background = colors[color_num%8+1];
	color_num ++;
    }

    else if(y < 42){
	dy = 4;
	background = colors[color_num%8+1];
	color_num ++;
    }

    if(x>1280-78) {
	dx = -5;
	background = colors[color_num%8+1];
	color_num ++;
    }
    else if(x<78) {
	dx = 5;
	background = colors[color_num%8+1];
	color_num ++;
    }

    i ++;
  }
  /*for (i = 0 ; i < 24 ; i++) {
    set_background_color(&colors[i % COLORS ]);
    print_background_color();
    usleep(400000);
  }*/
  
  printf("VGA BALL Userspace program terminating\n");
  return 0;
}
