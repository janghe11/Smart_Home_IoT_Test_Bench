#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <termios.h>                   
#include <fcntl.h>
#include <mysql.h>
#include <time.h>

#define CHECKING_TIME 	1000							// Manipulate poll time out.

#define MARIA_HOST		"127.0.0.1"
#define MARIA_USER		"root"
#define MARIA_PASS		"raspberry"
#define MARIA_DATABASE	"and_rpi_avr"
#define CLIENT_ID		"capstonemp12d160422r002"
#define RPI_ID			"rpi2d160423w10"
#define AVR_ID			"avr8535d160423w04"


int avr_maria_set(int avr_get_count, char avr_read_content) 
{
	int rpi_query_check = -1;							// Check query is proceeded (Fail : -1, Proceeded : 0)
	int query_retry_count = 0;							// Plus count when mysql_query is failed (Maximum : 5)
	char cid[30] = {'\0'};								// Command ID (Primary key of table)
	char avr_boiler_temp[2] = {0};						// Insert boiler temp to database
	char avr_query_statement[1024];						// Set AVR query to insert
	
	time_t server_time = time(NULL);					// Get current time function
	struct tm *cid_timestamp;							// Get each year, month, day, hour, min, sec from time function (defined in time.h)
	char cid_model_name[10] = "gal5";					// Device name which sent data to RPi
	
	// Get Raspberry Pi server time and generate cid.
	cid_timestamp = localtime(&server_time);
	sprintf(cid, "%sd%04d%02d%02d%02d%02d%02dc%04d", cid_model_name, cid_timestamp->tm_year + 1900, cid_timestamp->tm_mon + 1, cid_timestamp->tm_mday, 
													cid_timestamp->tm_hour, cid_timestamp->tm_min, cid_timestamp->tm_sec, avr_get_count);
	
	// Establish mariadb connection and insert values to rpi_avr table.
	MYSQL avr_maria;
	MYSQL *maria_connection = NULL;						// Definition of mysql structure
	mysql_init(&avr_maria);								// Initialize mysql structure
	maria_connection = mysql_real_connect(&avr_maria, MARIA_HOST, MARIA_USER, MARIA_PASS, MARIA_DATABASE, 3306, (char *)NULL, 0);
	
	if(maria_connection == NULL) {						// Check mariadb connection fail
		printf("%s\n", mysql_error(&avr_maria));
		exit(1);
	} else {
		printf("MariaDB connected successfully.\n");
	}
	
	// Make insert query and send query
	sprintf(avr_query_statement, "INSERT INTO rpi_avr (cid, client_id, rpi_id, avr_id, avr_data, avr_param) VALUES "
								 "('%s', '%s', '%s', '%s', '%c', '%s')", cid, CLIENT_ID, RPI_ID, AVR_ID, avr_read_content, avr_boiler_temp);
								 
	for(query_retry_count = 0; query_retry_count < 5; query_retry_count++) {
		rpi_query_check = mysql_query(maria_connection, avr_query_statement);
		if(rpi_query_check == 0) {
			printf("cid %s data successfully inserted.\n", cid);
			break;
		} else {
			printf("Failed to insert mariadb query. Retry. (Retry attempt : %d / 5)\n", query_retry_count + 1);
			printf("%s\n", mysql_error(&avr_maria));
			if(query_retry_count == 4) {
				printf("Insert query is not available. Please check rpi_avr table and query again.\n");
			}
		}
	}
	
	mysql_close(&avr_maria);
	
	return 0;
}

int main(int argc, char* argv[])
{        
   int	tty_usb = -1; 					// Destination device(file) descriptor. Default : Not connected.
   int	read_check = -1;				// Check read() function read data (Default : -1, Data read : byte size, Not read : -1)
   unsigned char	buf[2] = {'\0'};	// Variable for receive data. (buf[0] : Real data, buf[1] : Noises after data)
   struct	termios	serial_settings;	// Serial port termios communication settings.
   struct	pollfd	poll_events;        // Check polling event structure.
   int	poll_state = -1;				// Change poll_state when data is received. (Default : -1)
   int	avr_get_count = 0;				// Check count when data is sent from AVR (Set 0 when RPi reboot)

   // Open ttyUSB0 (Read and Write, NO CTTY, NO BLOCK)
   tty_usb = open( "/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK );
   if (tty_usb < 0) {        
      printf("Failed to connect ATmega8535. Please check RS232 cable connection again.\n");
      return -1;
   } else {
	   printf("ATmega8535 is connected by ttyUSB0. Polling standby.\n");
   }
   
   // Serial port termios communication settings
   memset(&serial_settings, 0, sizeof(serial_settings));			// Initialize serial_settgins termios struct.
   serial_settings.c_cflag       = B9600 | CS8 | CLOCAL | CREAD;	// Control modes. (9600, 8bit data, No parity, 1 Stopbit, No Hardware and Software flow control)
   serial_settings.c_oflag       = 0;	/* ? */						// Output modes.
   serial_settings.c_lflag       = 0;	/* ? */						// Local modes.
   serial_settings.c_cc[VTIME]   = 0;	/* ? */						 
   serial_settings.c_cc[VMIN]    = 1;								// Minimum number of characters for non canonical read.
   
   tcflush(tty_usb, TCIFLUSH ); /* ? */
   tcsetattr(tty_usb, TCSANOW, &serial_settings );	/* ? */
   fcntl(tty_usb, F_SETFL, FNDELAY); 	/* ? */

   // Settings for polling
   poll_events.fd        = tty_usb;					  // Get device(file) open location
   poll_events.events    = POLLIN | POLLERR;          // Check poll in and error
   poll_events.revents   = 0; 						  // Check returned event. Default : 0


   // Receive data from AVR and Save to avr_data.txt
   while (1)
   {
	  /*
	   * Call poll() -> check event
       * poll(event registration var, Amount of checking pollfd (Only 1 in this file : tty_usb), set time out (1000 = 1sec))
	   */
      poll_state = poll((struct pollfd*)&poll_events, 1, CHECKING_TIME);

      if (poll_state > 0) {						// Have poll events
         if (poll_events.revents & POLLIN)	{	// Received poll data
            read_check = read(tty_usb, buf, 1);
            printf("Data received. >> %d byte, %s\n", read_check, buf);
            printf("buf[0] print : %c\n", buf[0]);
            
            avr_get_count++;
			avr_maria_set(avr_get_count, (char)buf[0]);
			printf("Data inserted to database. >> %d byte, %c\n", read_check, buf[0]);
		 }
         
         if (poll_events.revents & POLLERR) {	//	Data corrupted
            printf("Communication error. Please check the device connection.\n");
         }
      }
   }
   close(tty_usb);
   return 0;
}
