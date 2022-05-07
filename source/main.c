#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <jpeg/jpgogc.h>
#include <unistd.h>

//Control Flow Definitions
#define MAIN_MENU 1
#define SHOW_SETTINGS 2
#define CONTROLLER_TEST 3
#define ANALOG_CALIBRATION 4
#define CSTICK_CALIBRATION 5
#define TRIGGER_SETUP 6
#define EXIT_APP 7

//Rendering Vars
static u32 *xfb;
static GXRModeObj *rmode;

//Image Vars
extern char picdata[];
extern int  piclength;

//Images
JPEGIMG phost;

//Control Flow Vars
int menu_pointer = 2;
int current_screen = 1;

//Controller Communication Vars
int message_received = 0;
int Ax = 0;
int Ay = 0;
int Cx = 0;
int Cy = 0;


/*
* Initialize Function handles setting up the VT Terminal used for printing text
* to the screen and frame buffer for images.
*/
void Initialise() {

	VIDEO_Init();
	PAD_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
}

char *convert(uint8_t *a){
  char* buffer2;
  int i;

  buffer2 = malloc(9);
  if (!buffer2)
    return NULL;

  buffer2[8] = 0;
  for (i = 0; i <= 7; i++)
    buffer2[7 - i] = (((*a) >> i) & (0x01)) + '0';

  puts(buffer2);

  return buffer2;
}

/*
* takes in an image and places it on the screen with a location to put it
*/
void display_jpeg(JPEGIMG jpeg, int x, int y) {

	unsigned int *jpegout = (unsigned int *) jpeg.outbuffer;

	int i,j;
	int height = jpeg.height;
	int width = jpeg.width/2;
		for(i=0;i<=width;i++)
			for(j=0;j<=height-2;j++)
				xfb[(i+x)+320*(j+16+y)]=jpegout[i+width*j];

	free(jpeg.outbuffer);

}

/*
* handles rendering the menu options and the arrow next to them using VT
* Terminal commands.
*/
void display_menu (int men_val) {

	printf("\x1b[10;30H");
	printf("PhobGCC Configuration Tool");

	printf("\x1b[12;35H");
	printf("See Current Controller Settings");

	printf("\x1b[13;35H");
	printf("Controller Test");

	printf("\x1b[14;35H");
	printf("Analog Stick Calibration");

	printf("\x1b[15;35H");
	printf("C-Stick Calibration");

	printf("\x1b[16;35H");
	printf("Trigger Setup");

	printf("\x1b[17;35H");
	printf("Exit");

	if(men_val == 2) {
			printf("\x1b[12;32H");
			printf("->");

	} else if(men_val == 3) {
			printf("\x1b[13;32H");
			printf("->");
	} else if(men_val == 4) {
			printf("\x1b[14;32H");
			printf("->");
	} else if(men_val == 5) {
			printf("\x1b[15;32H");
			printf("->");
	} else if(men_val == 6) {
			printf("\x1b[16;32H");
			printf("->");
	} else if(men_val == 7) {
			printf("\x1b[17;32H");
			printf("->");
	}
}

/*
* handles main menu operation
*/
void main_menu () {
		while(1) {

		PAD_ScanPads();

		u16 buttonsDown = PAD_ButtonsDown(0);

		if(buttonsDown & PAD_BUTTON_START) {
			current_screen = menu_pointer;
			break;
		}

		if(buttonsDown & PAD_BUTTON_UP) {
			if(menu_pointer == 2){
			} else {
				menu_pointer--;
			}
		}

		if(buttonsDown & PAD_BUTTON_DOWN) {
			if(menu_pointer == 7){
			} else {
				menu_pointer++;
			}
		}

		VIDEO_WaitVSync();

		VIDEO_ClearFrameBuffer (rmode, xfb, COLOR_BLACK);

		display_menu(menu_pointer);
		display_jpeg(phost, 60, 100);
	}
}

void show_settings() {
	while (1) {

				PAD_ScanPads();
				u16 buttonsHeld = PAD_ButtonsHeld(0);
				u16 buttonsDown = PAD_ButtonsDown(0);

				if(message_received == 1) {
					printf("\x1b[2;0H");
					printf("Communicating with PhobGCC");
					usleep(500000); //sleep for half a second

					Ax = PAD_StickX(0);
					Ay = PAD_StickY(0);

					Cx = PAD_SubStickX(0);
					Cy = PAD_SubStickY(0);

					message_received = 2;
				} else if(message_received == 0) {
					printf("\x1b[2;0H");
					printf("Please Send Settings (LR+Start)");
					if ((buttonsHeld & PAD_TRIGGER_L) && (buttonsHeld & PAD_TRIGGER_R) && (buttonsHeld & PAD_BUTTON_START)) {
							message_received = 1;
					}
				} else if (message_received == 2) {

					printf("\x1b[2;0H");
					printf("X axis Delay Setting: %d", Ax);

					printf("\x1b[3;0H");
					printf("Y Axis Delay Setting: %d", Ay);

					printf("\x1b[4;0H");
					printf("X axis Snapback Setting: %d", Cx);

					printf("\x1b[5;0H");
					printf("Y axis Snapback Setting: %d", Cy);

					if(buttonsDown & PAD_BUTTON_START) {
						current_screen = 1;
						message_received = 0;
						break;
					}

				}
				VIDEO_WaitVSync();
				VIDEO_ClearFrameBuffer (rmode, xfb, COLOR_BLACK);
			}
}

/*
* handles controller test operation
*/
void controller_test() {
	while(1) {

		PAD_ScanPads();
		u16 buttonsDown = PAD_ButtonsDown(0);
		if(buttonsDown & PAD_BUTTON_START) {
			current_screen = 1;
			break;
		}

		printf("\x1b[2;0H");
		printf("Controller Test WIP");

		VIDEO_WaitVSync();
	}
}

/*
* handles analog calibration operation
*/
void analog_calib() {
	while(1) {

		PAD_ScanPads();
		u16 buttonsDown = PAD_ButtonsDown(0);
		if(buttonsDown & PAD_BUTTON_START) {
			current_screen = 1;
			break;
		}

		printf("\x1b[2;0H");
		printf("Analog Stick Calibration WIP");

		VIDEO_WaitVSync();
	}
}

/*
* handles c-stick calibration operation
*/
void cstick_calib() {
	while(1) {

		PAD_ScanPads();
		u16 buttonsDown = PAD_ButtonsDown(0);
		if(buttonsDown & PAD_BUTTON_START) {
			current_screen = 1;
			break;
		}

		printf("\x1b[2;0H");
		printf("C-stick Calibration WIP");

		VIDEO_WaitVSync();
	}
}

/*
* handles trigger setup operation
*/
void trigger_setup() {
	while(1) {

		PAD_ScanPads();

		u16 buttonsDown = PAD_ButtonsDown(0);
		u16 buttonsHeld = PAD_ButtonsHeld(0);

		if(buttonsDown & PAD_BUTTON_START) {
			current_screen = 1;
			break;
		}

		printf("\x1b[2;0H");
		int lTrigger = PAD_TriggerL(0);
		printf("L Trigger Value: %d", lTrigger);

		printf("\x1b[3;0H");
		if(buttonsHeld & PAD_TRIGGER_L) {
			printf("L Trigger Digital Active");
		}

		printf("\x1b[2;40H");
		int rTrigger = PAD_TriggerR(0);
		printf("R Trigger Value: %d", rTrigger);

		printf("\x1b[3;40H");
		if(buttonsHeld & PAD_TRIGGER_R) {
			printf("R Trigger Digital Active");
		}

		VIDEO_WaitVSync();
		VIDEO_ClearFrameBuffer (rmode, xfb, COLOR_BLACK);
	}
}


/*
* main execution loop
*/
int main() {

	memset(&phost, 0, sizeof(JPEGIMG));
	phost.inbuffer = picdata;
	phost.inbufferlength = piclength;
	JPEG_Decompress(&phost);
	Initialise();
	display_jpeg(phost, 60, 100);

	while (1) {

		VIDEO_WaitVSync();
		VIDEO_ClearFrameBuffer (rmode, xfb, COLOR_BLACK);

		if(current_screen == MAIN_MENU) {
			main_menu();
		} else if(current_screen == SHOW_SETTINGS) {
			show_settings();
		} else if(current_screen == CONTROLLER_TEST) {
			controller_test();
		} else if(current_screen == ANALOG_CALIBRATION) {
			analog_calib();
		} else if(current_screen == CSTICK_CALIBRATION) {
			cstick_calib();
		} else if(current_screen == TRIGGER_SETUP) {
			trigger_setup();
		} else if(current_screen == EXIT_APP) {
			exit(0);
		}

	}

	return 0;

}
