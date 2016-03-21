#include <wiringSerial.h>

int main(int argc, char* argv[])
{
	int fd;
	int data;
	
	if((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0)
	{
		fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
	}
	
	printf("\nRaspberry Pi UART Test");
	
	while(1)
	{
		data = serialGetchar(fd);
		printf("\nATmega8535 > RPi = %c", (char)data);
		serialPutchar(id, data);
		serialPuts(fd, "\n");
		fflush(stdout);
	}
		
	return 0;
}
