#include <stdio.h>
#include "rs232.h"

int main(){
	int baud = 9600;
	char mode[] = { '8', 'N', '1', 0 };
	struct termios old_settings;
	int port = RS232_OpenComport("/dev/pts/4", baud, mode, &old_settings);
	if(port < 0){
		perror("couldn't open port");
		return -1;
	}
	char buffer[1024];
	int rx;
	printf("Receiving data...\n");
	do{
		rx = RS232_PollComport(port, buffer, 1024);
		for(int i = 0; i < rx; ++i) printf("%c", buffer[i]);
	}while(buffer[rx-1] != 'z');
	printf("\n");
	RS232_CloseComport(port, &old_settings);
	return 0;
}
