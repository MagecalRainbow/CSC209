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
#include "controller.h"

#define MAXFD(x,y) ((x) >= (y)) ? (x) : (y)

int main(int argc, char *argv[]){
	int port;
	struct cignal cig;
	// A buffer to store a serialized message
	char *cig_serialized = malloc(sizeof(char)*CIGLEN);
	// An array to registered sensor devices
	int device_record[MAXDEV] = {0};
	
	if(argc == 2){
		port = strtol(argv[1], NULL, 0);
	} else{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(1);
	}

	int gatewayfd = set_up_server_socket(port);
	printf("\nThe Cignal Gateway is now started on port: %d\n\n", port);
	int peerfd;
	
	/* TODO: Implement the body of the server.  
	 *
	 * Use select so that the server process never blocks on any call except
	 * select. If no sensors connect and/or send messsages in a timespan of
	 * 5 seconds then select will return and print the message "Waiting for
	 * Sensors update..." and go back to waiting for an event.
	 * 
	 * The server will handle connections from devices, will read a message from
	 * a sensor, process the message (using process_message), write back
	 * a response message to the sensor client, and close the connection.
	 * After reading a message, your program must print the "RAW MESSAGE"
	 * message below, which shows the serialized message received from the *
	 * client.
	 * 
	 *  Print statements you must use:
     * 	printf("Waiting for Sensors update...\n");
	 * 	printf("RAW MESSAGE: %s\n", YOUR_VARIABLE);
	 */

	// TODO implement select loop


	// Suppress unused variable warning.  The next 5 ilnes can be removed 
	// after the variables are used.

    /* The variables for file descriptor sets and the file descriptors from 
     * sockets.
     */
    fd_set all_fds;
    int fds_ready; 
    int numfd = gatewayfd;

    /* Timer required for the 5 sec delay that needs to be set up for the select 
     * function.
     */
    struct timeval timer;

    FD_ZERO(&all_fds); //set of descriptors being empty.
    FD_SET(gatewayfd, &all_fds); //add file descriptors from gateway.
    char msg[CIGLEN];

	while(1) {
		timer.tv_sec = 5;
    	timer.tv_usec = 0;
        fd_set temp_fds = all_fds;

        //perform on temp_fds because select modifies the fd set.
        if ((fds_ready = select(numfd + 1, &temp_fds, NULL, NULL, 
                        &timer)) < 0) {
            perror("select");
            exit(1);
        }

        //None of the sensors are ready to connect to gateway.
        if (!fds_ready) {
            printf("Waiting for Sensors update...\n");
			continue;
        }

        //Setting gatewayfd into fd set we are working on.
		if (FD_ISSET(gatewayfd, &temp_fds)) {
			peerfd = accept_connection(gatewayfd);
            if (peerfd < 0) {
				perror("accept");
            	exit(1);
            }
            FD_SET(peerfd, &all_fds);
            numfd = MAXFD(numfd, peerfd); 
		}


        /*Check the sensors and get the sensors that are ready to  exchange the 
         * messages and process.
         */
	    for (int index = 0; index <= numfd; index++) {
            if (index != gatewayfd && FD_ISSET(index, &temp_fds)) {
                //Read from sensor.
                if (read(index, msg, CIGLEN) < 0) {
                    perror("read");
                    FD_CLR(index, &all_fds);
                    exit(1);
                }

                //Convert cignal into message - human readable form.
                unpack_cignal(msg, &cig);
                printf("RAW MESSAGE: %s\n", msg);

                //Process the message  
                if (process_message(&cig, device_record) < 0) {
                    FD_CLR(index, &all_fds);
                    close(index);
                    break;
                }

                //Report the response status to censors.
                cig_serialized = serialize_cignal(cig);
                if (write(index, cig_serialized, CIGLEN) < 0) {
                    perror("write");
                    FD_CLR(index, &all_fds);
                    exit(1);
                }

                //Close and discard already reported message from the server.
                FD_CLR(index, &all_fds);
                close(index);
            }
        }
	}
	return 0;
}
