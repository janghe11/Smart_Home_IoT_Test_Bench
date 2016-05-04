#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <termios.h>                   
#include <fcntl.h>
#include <mysql.h>

#define CHECKING_TIME 1000								// Manipulate poll time out.

#define MARIA_HOST	"127.0.0.1"
#define MARIA_USER	"root"
#define MARIA_PASS	"raspberry"
#define MARIA_DATABASE	"and_rpi_avr"
#define RPI_CLIENT_ID	"capstonemp12d160422r002"
#define RPI_ID	"rpi2d160423w10"
#define AVR_ID	"avr8535d160423w04"


int avr_mysql_set(void) 
{
	int avr_maria_set;
	char rpi_client_id[23] = {'\0'};
	char avr_read_content = 0;							// Receive data from AVR
	char avr_boiler_temp[2] = {0};						// Insert boiler temp to database
	char avr_query_statement[1024];						// Set AVR query to insert
	
	MYSQL avr_maria;
	MYSQL *maria_connection = NULL;						// Definition of mysql structure
	mysql_init(&avr_maria);								// Initialize mysql structure
	maria_connection = mysql_real_connect(&avr_maria, MARIA_HOST, MARIA_USER, MARIA_PASS, MARIA_DATABASE, 3306, (char *)NULL, 0);
	
	if(maria_connection == NULL) {
		printf("%s\n", mysql_error(&avr_maria));
		exit(1);
	}
	printf("MariaDB connected successfully.\n");
	
	sprintf(avr_query_statement, "INSERT INTO rpi_avr VALUES "
								 "('%s', RPI_CLIENT_ID, RPI_ID, AVR_ID, '%c', '%s')", rpi_client_id, avr_read_content, avr_boiler_temp);
	avr_maria_set = mysql_query(maria_connection, avr_query_statement);
	mysql_close(&avr_maria);
	
	return 0;
}

int main(int argc, char* argv[])
{        
   int tty_usb = -1; 					// Destination device(file) descriptor. Default : Not connected.
   int save_data_file = -1;				// Default : File not exist.
   //int    ndx;
   int    cnt;
   unsigned char   buf[1];				// Variable for receive data.
   struct termios    serial_settings;	// Serial port termios communication settings.
   struct pollfd     poll_events;       // Check polling event.
   int    poll_state = -1;

   tty_usb = open( "/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK );	// Open ttyUSB0 (Read and Write, NO CTTY, NO BLOCK)
   if (tty_usb < 0) {        
      printf("Failed to connect ATmega8535. Please check RS232 cable connection again.\n");
      return -1;
   } else {
	   printf("ATmega8535 is connected by ttyUSB0. Polling standby.\n");
   }
   
   //save_data_file = open("/home/jang/문서/ProjectResources/TestCodes/avr_data.txt", O_WRONLY);
   
   // Serial port termios communication settings
   memset( &serial_settings, 0, sizeof(serial_settings) );			// Initialize serial_settgins termios struct.
   serial_settings.c_cflag       = B9600 | CS8 | CLOCAL | CREAD;	// Control modes. (9600, 8bit data, No parity, 1 Stopbit, No Hardware and Software flow control)
   serial_settings.c_oflag       = 0;	/* ? */						// Output modes.
   serial_settings.c_lflag       = 0;	/* ? */						// Local modes.
   serial_settings.c_cc[VTIME]   = 0;	/* ? */						 
   serial_settings.c_cc[VMIN]    = 1;	/* ? */						// Minimum number of characters for non canonical read.
   
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
            cnt = read(tty_usb, buf, 1);
            printf("Data received. >> %d byte, %s\n", cnt, buf);
            
            if(save_data_file  < 0) {
				printf("\"avr_data.txt\" file dose not exist or not 0byte. Unable to write data to \"avr_data.txt\"\n");
			} else {
				avr_mysql_set();
				printf("Data inserted to database. >> %d byte, %s\n", cnt, buf);
			}
         }
         
         if (poll_events.revents & POLLERR) {	//	Data corrupted
            printf("Communication error. Please check the device connection.\n");
         }
      }
   }
   close(tty_usb);
   return 0;
}
