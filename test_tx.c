#include <stdio.h>
#include "rs232.h"

int main(){
	int baud = 9600;
	char mode[] = { '8', 'N', '1', 0 };
	struct termios old_settings;
	int port = RS232_OpenComport("/dev/pts/2", baud, mode, &old_settings);
	if(port < 0){
		perror("couldn't open port");
		printf("%d\n", port);
		return -1;
	}
	printf("Sending data...\n");
	for(char a = 'a'; a <= 'z'; ++a){
		RS232_SendByte(port, a);
		sleep(1);
	}
	RS232_SendByte(port, '\n');
	RS232_CloseComport(port, &old_settings);
	return 0;
}
