#include <raylib.h>
#include <stdio.h>

#include <bluetooth/bluetooth.h>
#include <cwiid.h>

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg_array[], struct timespec *timestamp) {
    printf("Received a wiimote event!");
}

int main(int argc, char **argv) {

    bdaddr_t bdaddr;
    cwiid_wiimote_t *wiimote = cwiid_open(&bdaddr, CWIID_FLAG_MESG_IFC);

    if (!wiimote) {
        printf("Failed to connect to wiimote!");
    } else if (cwiid_set_mesg_callback(wiimote, &cwiid_callback)) {
        printf("Failed to set callback");
    } else {
        cwiid_enable(wiimote, CWIID_FLAG_MOTIONPLUS);
        cwiid_request_status(wiimote);
    }
    while (1) {
    }
    return 0;
}
