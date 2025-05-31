#include <bluetooth/bluetooth.h>
#include <cwiid.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct {
    float x;
    float y;
} pos2d;

#define FOV_X 45.0
#define FOV_Y 45.0
#define CY 384.0
#define CX 512.0

void print_buttons(uint16_t buttons) {
    printf("Buttons pressed:");
    if (buttons & CWIID_BTN_LEFT)
        printf(" LEFT");
    if (buttons & CWIID_BTN_RIGHT)
        printf(" RIGHT");
    if (buttons & CWIID_BTN_UP)
        printf(" UP");
    if (buttons & CWIID_BTN_DOWN)
        printf(" DOWN");
    if (buttons & CWIID_BTN_A)
        printf(" A");
    if (buttons & CWIID_BTN_B)
        printf(" B");
    if (buttons & CWIID_BTN_1)
        printf(" 1");
    if (buttons & CWIID_BTN_2)
        printf(" 2");
    if (buttons & CWIID_BTN_MINUS)
        printf(" MINUS");
    if (buttons & CWIID_BTN_PLUS)
        printf(" PLUS");
    if (buttons & CWIID_BTN_HOME)
        printf(" HOME");
    printf("\n");
}

void ir_to_real_space(uint16_t px1, uint16_t py1, uint16_t px2, uint16_t py2,
                      int *screen_x, int *screen_y, float *distance) {

    float mid_y =
        ((float)(py1 + py2)) / 2.0 +
        30; // + 30 = manual offset since you are typically above the screen
    float mid_x = ((float)(px1 + px2)) / 2.0;
    float offset_y = (CY - mid_y) / 768;
    float offset_x = (CX - mid_x) / 1024;
    *screen_y = (int)(offset_y + 0.5);
    *screen_x = (int)(offset_x + 0.5);
}

void print_ir_event(struct cwiid_ir_src srcs[]) {
    int screen_x = 0;
    int screen_y = 0;
    float distance = 0.0;
    uint16_t px1 = 0;
    uint16_t px2 = 0;
    uint16_t py1 = 0;
    uint16_t py2 = 0;
    int blob_count = 0;

    for (int i = 0; i < CWIID_IR_SRC_COUNT; i++) {
        if (srcs[i].valid) {
            if (!blob_count) {
                px1 = srcs[i].pos[CWIID_X];
                py1 = srcs[i].pos[CWIID_Y];
                blob_count++;
            } else {
                px2 = srcs[i].pos[CWIID_X];
                py2 = srcs[i].pos[CWIID_Y];
            }
        }
    }
    if (blob_count) {
        ir_to_real_space(px1, py1, px2, py2, &screen_x, &screen_y, &distance);
        printf("Screen space x = %d, y = %d", screen_x, screen_y);
    }
}

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg_array[], struct timespec *timestamp) {
    printf("Got a callback!\n");
    for (int i = 0; i < mesg_count; i++) {
        union cwiid_mesg msg = mesg_array[i];
        switch (msg.type) {
        case CWIID_MESG_BTN:
            print_buttons(msg.btn_mesg.buttons);
            break;
        case CWIID_MESG_IR:
            print_ir_event(msg.ir_mesg.src);
            break;
        default:
            break;
        }
        if (mesg_array[i].type == CWIID_MESG_BTN) {
            print_buttons(mesg_array[i].btn_mesg.buttons);
        }
    }
}

int main(int argc, char *argv[]) {
    cwiid_wiimote_t *wiimote;
    bdaddr_t bdaddr = *BDADDR_ANY;
    if (argc > 1) {
        if (str2ba(argv[1], &bdaddr)) {
            fprintf(stderr, "Invalid Bluetooth address\n");
            return 1;
        }
    }

    printf("Press buttons 1+2 on your Wiimote now...\n");
    wiimote = cwiid_open(&bdaddr, CWIID_FLAG_MESG_IFC);
    printf("HUH!\n");
    if (!wiimote) {
        fprintf(stderr, "Unable to connect\n");
        return 1;
    }
    printf("Connected.\n");

    if (cwiid_set_mesg_callback(wiimote, &cwiid_callback)) {
        fprintf(stderr, "Unable to set callback\n");
        cwiid_close(wiimote);
        return 1;
    }

    if (cwiid_set_rpt_mode(wiimote, CWIID_RPT_BTN | CWIID_RPT_IR)) {
        fprintf(stderr, "Unable to set report mode\n");
        cwiid_close(wiimote);
        return 1;
    }

    printf("Listening for button presses. Press Ctrl+C to exit.\n");
    while (1) {
        sleep(1);
    }

    cwiid_close(wiimote);
    return 0;
}
