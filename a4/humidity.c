#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <time.h>
#include "socket.h"
#include "message.h"

#define ASCENT 0.02   //Is used to increase the humidity level as a percentage.
#define DESCENT 0.005 //Is used to decrease the humidity level as a percentage.
#define START_VALUE 31 // defines the starting value for humidity

/* Simulates changes to the humidity sensor device using the FAN_STATUS message.
 */
void read_humidity(struct cignal *cig) {
	if (cig->hdr.type == FAN_STATUS) {
		cig->hdr.type = UPDATE;

		if (cig->dehumid == ON) {
			cig->value -= DESCENT * cig->value;
		} else if (cig->dehumid == OFF) {
			cig->value += ASCENT * cig->value;
		} 

	} else {
		cig->hdr.type = UPDATE;
	}
}


/* This is a helper function for reporting the current status of humidity to 
 * the gateway and receive the command of the gateway in response.  
 */
void update_status(char *cig_serialized, char *msg, int peerfd, 
        struct cignal *cig) {
    cig_serialized = serialize_cignal(*cig);
    if (write(peerfd, cig_serialized, CIGLEN) < 0) {
        perror("humidity: write");
        exit(1);
    }
    if (read(peerfd, msg, CIGLEN) < 0) {
        perror("humidity: read");
        exit(1);
    }
    unpack_cignal(msg, cig);
}


int main(int argc, char **argv) {
	int port;
	char *hostname = "localhost";
	time_t rawtime = time(NULL);
	struct tm *now;
	struct cignal cig;

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s port [hostname]\n", argv[0]);
		exit(1);
	}

	if (argc == 3) {
		hostname = "localhost";
	}

	port = strtol(argv[1], NULL, 0);
	printf("The Cignal Humidity Sensor is now reading...\n\n");
	cig.hdr.device_type = HUMIDITY;
	cig.value = START_VALUE; //initial humidity level
	cig.cooler = OFF;
	cig.dehumid = OFF;

	char *cig_serialized = malloc(sizeof(char) * CIGLEN);
    char msg[CIGLEN]; 
	int msgno = 1;
	// Suppress unused variable messages.  The next two lines can be removed
	// before submitting.

	while (1) {
		int peerfd;
        //Connect to gateway.
		if ((peerfd = connect_to_server(port, hostname)) == -1)	{
			printf("There was an error in connecting to the gateway!\n");
			exit(1);
		} 
		/* TODO: Complete the while loop
		 * If this is the first message, then send a handshake message with
		 * a device id of -1.  If it is a subsequent message, then write
		 * the current state of the message, and read potentially new state
		 * from the server.
		 */

        //Initiating Handshake.
        if (msgno == 1) {
            /* Set up the initial state of the humidity device and report to 
             * the gateway. Also, receive the gateway's response.
             */
            cig.hdr.device_id = -1;
            cig.hdr.type = 1;
            update_status(cig_serialized, msg, peerfd, &cig);
            close(peerfd);
	        msgno++;
        }

        //Later messages.
        else {
            /* Report the current status of humidity to the gateway and receive
             * the gateway's response.
             */
            update_status(cig_serialized, msg, peerfd, &cig);
            close(peerfd);
        }

		if (sleep(INTERVAL) >= 0) {
			rawtime = time(NULL);
			now = localtime(&rawtime);
			read_humidity(&cig);
			printf("[%02d:%02d:%02d] Humidity: %.4f\n", 
					now->tm_hour, 	
			       now->tm_min, 
				   now->tm_sec, 
				   cig.value);
		}
	}
}
