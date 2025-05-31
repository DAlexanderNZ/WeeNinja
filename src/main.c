#include <bluetooth/bluetooth.h>
#include <cwiid.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg_array[], struct timespec *timestamp) {
    printf("Got a callback!\n");
    for (int i = 0; i < mesg_count; i++) {
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

    if (cwiid_set_rpt_mode(wiimote, CWIID_RPT_BTN)) {
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
