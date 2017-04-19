/*
***************************************************************************
*
* Author: Teunis van Beelen
*         Michał Maj
*
* Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016 Teunis van Beelen
* Copyright (C) 2017 Michał Maj
*
* Email: teuniz@gmail.com
*        maju@majudev.net
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/


/* Last revision: July 10, 2016 */

/* For more info and how to use this library, visit: http://www.teuniz.net/RS-232/ */


#include "rs232.h"


#if defined(__linux__) || defined(__FreeBSD__)   /* Linux & FreeBSD */

int RS232_OpenComport(const char * comport, int baudrate, const char *mode, struct termios * old_port_settings)
{
  int baudr,
      status;

  switch(baudrate)
  {
    case      50 : baudr = B50;
                   break;
    case      75 : baudr = B75;
                   break;
    case     110 : baudr = B110;
                   break;
    case     134 : baudr = B134;
                   break;
    case     150 : baudr = B150;
                   break;
    case     200 : baudr = B200;
                   break;
    case     300 : baudr = B300;
                   break;
    case     600 : baudr = B600;
                   break;
    case    1200 : baudr = B1200;
                   break;
    case    1800 : baudr = B1800;
                   break;
    case    2400 : baudr = B2400;
                   break;
    case    4800 : baudr = B4800;
                   break;
    case    9600 : baudr = B9600;
                   break;
    case   19200 : baudr = B19200;
                   break;
    case   38400 : baudr = B38400;
                   break;
    case   57600 : baudr = B57600;
                   break;
    case  115200 : baudr = B115200;
                   break;
    case  230400 : baudr = B230400;
                   break;
    case  460800 : baudr = B460800;
                   break;
    case  500000 : baudr = B500000;
                   break;
    case  576000 : baudr = B576000;
                   break;
    case  921600 : baudr = B921600;
                   break;
    case 1000000 : baudr = B1000000;
                   break;
    case 1152000 : baudr = B1152000;
                   break;
    case 1500000 : baudr = B1500000;
                   break;
    case 2000000 : baudr = B2000000;
                   break;
    case 2500000 : baudr = B2500000;
                   break;
    case 3000000 : baudr = B3000000;
                   break;
    case 3500000 : baudr = B3500000;
                   break;
    case 4000000 : baudr = B4000000;
                   break;
    default      : printf("invalid baudrate\n");
                   return(-1);
                   break;
  }

  int cbits=CS8,
      cpar=0,
      ipar=IGNPAR,
      bstop=0;

  if(strlen(mode) != 3)
  {
    printf("invalid mode \"%s\"\n", mode);
    return(-1);
  }

  switch(mode[0])
  {
    case '8': cbits = CS8;
              break;
    case '7': cbits = CS7;
              break;
    case '6': cbits = CS6;
              break;
    case '5': cbits = CS5;
              break;
    default : printf("invalid number of data-bits '%c'\n", mode[0]);
              return(-1);
              break;
  }

  switch(mode[1])
  {
    case 'N':
    case 'n': cpar = 0;
              ipar = IGNPAR;
              break;
    case 'E':
    case 'e': cpar = PARENB;
              ipar = INPCK;
              break;
    case 'O':
    case 'o': cpar = (PARENB | PARODD);
              ipar = INPCK;
              break;
    default : printf("invalid parity '%c'\n", mode[1]);
              return(-1);
              break;
  }

  switch(mode[2])
  {
    case '1': bstop = 0;
              break;
    case '2': bstop = CSTOPB;
              break;
    default : printf("invalid number of stop bits '%c'\n", mode[2]);
              return(-1);
              break;
  }

/*
http://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html

http://man7.org/linux/man-pages/man3/termios.3.html
*/

  int port = open(comport, O_RDWR | O_NOCTTY | O_NDELAY);
  if(port==-1)
  {
    perror("unable to open comport ");
    return(-1);
  }

  /* lock access so that another process can't also use the port */
  if(flock(port, LOCK_EX | LOCK_NB) != 0)
  {
    close(port);
    perror("Another process has locked the comport.");
    return(-1);
  }

  struct termios new_port_settings;
  int error = tcgetattr(port, old_port_settings);
  if(error==-1)
  {
    close(port);
    flock(port, LOCK_UN);  // free the port so that others can use it.
    perror("unable to read portsettings ");
    return(-1);
  }
  memset(&new_port_settings, 0, sizeof(struct termios));  /* clear the new struct */

  new_port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
  new_port_settings.c_iflag = ipar;
  new_port_settings.c_oflag = 0;
  new_port_settings.c_lflag = 0;
  new_port_settings.c_cc[VMIN] = 0;      /* block untill n bytes are received */
  new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

  cfsetispeed(&new_port_settings, baudr);
  cfsetospeed(&new_port_settings, baudr);

  error = tcsetattr(port, TCSANOW, &new_port_settings);
  if(error==-1)
  {
    tcsetattr(port, TCSANOW, old_port_settings);
    close(port);
    flock(port, LOCK_UN);  /* free the port so that others can use it. */
    perror("unable to adjust portsettings ");
    return(-1);
  }

/* http://man7.org/linux/man-pages/man4/tty_ioctl.4.html */

  if(ioctl(port, TIOCMGET, &status) == -1)
  {
    tcsetattr(port, TCSANOW, old_port_settings);
    flock(port, LOCK_UN);  // free the port so that others can use it.
    perror("unable to get portstatus");
    return(-1);
  }

  status |= TIOCM_DTR;    // turn on DTR
  status |= TIOCM_RTS;    // turn on RTS

  if(ioctl(port, TIOCMSET, &status) == -1)
  {
    tcsetattr(port, TCSANOW, old_port_settings);
    flock(port, LOCK_UN);  // free the port so that others can use it.
    perror("unable to set portstatus");
    return(-1);
  }

  return port;
}


int RS232_PollComport(int port, unsigned char *buf, int size)
{
  int n;

  n = read(port, buf, size);

  if(n < 0)
  {
    if(errno == EAGAIN)  return 0;
  }

  return(n);
}


int RS232_SendByte(int port, unsigned char byte)
{
  int n = write(port, &byte, 1);
  if(n < 0)
  {
    if(errno == EAGAIN)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }

  return(0);
}


int RS232_SendBuf(int port, unsigned char *buf, int size)
{
  int n = write(port, buf, size);
  if(n < 0)
  {
    if(errno == EAGAIN)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }

  return(0);
}


void RS232_CloseComport(int port, struct termios * old_port_settings)
{
  int status;

  if(ioctl(port, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_DTR;    /* turn off DTR */
  status &= ~TIOCM_RTS;    /* turn off RTS */

  if(ioctl(port, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }

  tcsetattr(port, TCSANOW, old_port_settings);
  close(port);

  flock(port, LOCK_UN);  /* free the port so that others can use it. */
}

/*
Constant  Description
TIOCM_LE        DSR (data set ready/line enable)
TIOCM_DTR       DTR (data terminal ready)
TIOCM_RTS       RTS (request to send)
TIOCM_ST        Secondary TXD (transmit)
TIOCM_SR        Secondary RXD (receive)
TIOCM_CTS       CTS (clear to send)
TIOCM_CAR       DCD (data carrier detect)
TIOCM_CD        see TIOCM_CAR
TIOCM_RNG       RNG (ring)
TIOCM_RI        see TIOCM_RNG
TIOCM_DSR       DSR (data set ready)

http://man7.org/linux/man-pages/man4/tty_ioctl.4.html
*/

int RS232_IsDCDEnabled(int port)
{
  int status;

  ioctl(port, TIOCMGET, &status);

  if(status&TIOCM_CAR) return(1);
  else return(0);
}


int RS232_IsCTSEnabled(int port)
{
  int status;

  ioctl(port, TIOCMGET, &status);

  if(status&TIOCM_CTS) return(1);
  else return(0);
}


int RS232_IsDSREnabled(int port)
{
  int status;

  ioctl(port, TIOCMGET, &status);

  if(status&TIOCM_DSR) return(1);
  else return(0);
}


void RS232_enableDTR(int port)
{
  int status;

  if(ioctl(port, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status |= TIOCM_DTR;    /* turn on DTR */

  if(ioctl(port, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}


void RS232_disableDTR(int port)
{
  int status;

  if(ioctl(port, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_DTR;    /* turn off DTR */

  if(ioctl(port, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}


void RS232_enableRTS(int port)
{
  int status;

  if(ioctl(port, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status |= TIOCM_RTS;    /* turn on RTS */

  if(ioctl(port, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}


void RS232_disableRTS(int port)
{
  int status;

  if(ioctl(port, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_RTS;    /* turn off RTS */

  if(ioctl(port, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}


void RS232_flushRX(int port)
{
  tcflush(port, TCIFLUSH);
}


void RS232_flushTX(int port)
{
  tcflush(port, TCOFLUSH);
}


void RS232_flushRXTX(int port)
{
  tcflush(port, TCIOFLUSH);
}


#else  /* windows */

char mode_str[128];


HANDLE RS232_OpenComport(const char * port, int baudrate, const char *mode)
{

  switch(baudrate)
  {
    case     110 : strcpy(mode_str, "baud=110");
                   break;
    case     300 : strcpy(mode_str, "baud=300");
                   break;
    case     600 : strcpy(mode_str, "baud=600");
                   break;
    case    1200 : strcpy(mode_str, "baud=1200");
                   break;
    case    2400 : strcpy(mode_str, "baud=2400");
                   break;
    case    4800 : strcpy(mode_str, "baud=4800");
                   break;
    case    9600 : strcpy(mode_str, "baud=9600");
                   break;
    case   19200 : strcpy(mode_str, "baud=19200");
                   break;
    case   38400 : strcpy(mode_str, "baud=38400");
                   break;
    case   57600 : strcpy(mode_str, "baud=57600");
                   break;
    case  115200 : strcpy(mode_str, "baud=115200");
                   break;
    case  128000 : strcpy(mode_str, "baud=128000");
                   break;
    case  256000 : strcpy(mode_str, "baud=256000");
                   break;
    case  500000 : strcpy(mode_str, "baud=500000");
                   break;
    case 1000000 : strcpy(mode_str, "baud=1000000");
                   break;
    default      : printf("invalid baudrate\n");
                   return(1);
                   break;
  }

  if(strlen(mode) != 3)
  {
    printf("invalid mode \"%s\"\n", mode);
    return(1);
  }

  switch(mode[0])
  {
    case '8': strcat(mode_str, " data=8");
              break;
    case '7': strcat(mode_str, " data=7");
              break;
    case '6': strcat(mode_str, " data=6");
              break;
    case '5': strcat(mode_str, " data=5");
              break;
    default : printf("invalid number of data-bits '%c'\n", mode[0]);
              return(1);
              break;
  }

  switch(mode[1])
  {
    case 'N':
    case 'n': strcat(mode_str, " parity=n");
              break;
    case 'E':
    case 'e': strcat(mode_str, " parity=e");
              break;
    case 'O':
    case 'o': strcat(mode_str, " parity=o");
              break;
    default : printf("invalid parity '%c'\n", mode[1]);
              return(1);
              break;
  }

  switch(mode[2])
  {
    case '1': strcat(mode_str, " stop=1");
              break;
    case '2': strcat(mode_str, " stop=2");
              break;
    default : printf("invalid number of stop bits '%c'\n", mode[2]);
              return(1);
              break;
  }

  strcat(mode_str, " dtr=on rts=on");

/*
http://msdn.microsoft.com/en-us/library/windows/desktop/aa363145%28v=vs.85%29.aspx

http://technet.microsoft.com/en-us/library/cc732236.aspx
*/

  HANDLE port = CreateFileA(comports[comport_number],
                      GENERIC_READ|GENERIC_WRITE,
                      0,                          /* no share  */
                      NULL,                       /* no security */
                      OPEN_EXISTING,
                      0,                          /* no threads */
                      NULL);                      /* no templates */

  if(port==INVALID_HANDLE_VALUE)
  {
    printf("unable to open comport\n");
    return(1);
  }

  DCB port_settings;
  memset(&port_settings, 0, sizeof(port_settings));  /* clear the new struct  */
  port_settings.DCBlength = sizeof(port_settings);

  if(!BuildCommDCBA(mode_str, &port_settings))
  {
    printf("unable to set comport dcb settings\n");
    CloseHandle(port);
    return(1);
  }

  if(!SetCommState(port, &port_settings))
  {
    printf("unable to set comport cfg settings\n");
    CloseHandle(port);
    return(1);
  }

  COMMTIMEOUTS Cptimeouts;

  Cptimeouts.ReadIntervalTimeout         = MAXDWORD;
  Cptimeouts.ReadTotalTimeoutMultiplier  = 0;
  Cptimeouts.ReadTotalTimeoutConstant    = 0;
  Cptimeouts.WriteTotalTimeoutMultiplier = 0;
  Cptimeouts.WriteTotalTimeoutConstant   = 0;

  if(!SetCommTimeouts(port, &Cptimeouts))
  {
    printf("unable to set comport time-out settings\n");
    CloseHandle(port);
    return NULL;
  }

  return HANDLE;
}


int RS232_PollComport(HANDLE port, unsigned char *buf, int size)
{
  int n;

/* added the void pointer cast, otherwise gcc will complain about */
/* "warning: dereferencing type-punned pointer will break strict aliasing rules" */

  ReadFile(port, buf, size, (LPDWORD)((void *)&n), NULL);

  return(n);
}


int RS232_SendByte(HANDLE port, unsigned char byte)
{
  int n;

  WriteFile(port, &byte, 1, (LPDWORD)((void *)&n), NULL);

  if(n<0)  return(1);

  return(0);
}


int RS232_SendBuf(HANDLE port, unsigned char *buf, int size)
{
  int n;

  if(WriteFile(port, buf, size, (LPDWORD)((void *)&n), NULL))
  {
    return(n);
  }

  return(-1);
}


void RS232_CloseComport(HANDLE port)
{
  CloseHandle(port);
}

/*
http://msdn.microsoft.com/en-us/library/windows/desktop/aa363258%28v=vs.85%29.aspx
*/

int RS232_IsDCDEnabled(HANDLE port)
{
  int status;

  GetCommModemStatus(port, (LPDWORD)((void *)&status));

  if(status&MS_RLSD_ON) return(1);
  else return(0);
}


int RS232_IsCTSEnabled(HANDLE port)
{
  int status;

  GetCommModemStatus(port, (LPDWORD)((void *)&status));

  if(status&MS_CTS_ON) return(1);
  else return(0);
}


int RS232_IsDSREnabled(HANDLE port)
{
  int status;

  GetCommModemStatus(port, (LPDWORD)((void *)&status));

  if(status&MS_DSR_ON) return(1);
  else return(0);
}


void RS232_enableDTR(HANDLE port)
{
  EscapeCommFunction(port, SETDTR);
}


void RS232_disableDTR(HANDLE port)
{
  EscapeCommFunction(port, CLRDTR);
}


void RS232_enableRTS(HANDLE port)
{
  EscapeCommFunction(port, SETRTS);
}


void RS232_disableRTS(HANDLE port)
{
  EscapeCommFunction(port, CLRRTS);
}

/*
https://msdn.microsoft.com/en-us/library/windows/desktop/aa363428%28v=vs.85%29.aspx
*/

void RS232_flushRX(HANDLE port)
{
  PurgeComm(port, PURGE_RXCLEAR | PURGE_RXABORT);
}


void RS232_flushTX(HANDLE port)
{
  PurgeComm(port, PURGE_TXCLEAR | PURGE_TXABORT);
}


void RS232_flushRXTX(HANDLE port)
{
  PurgeComm(port, PURGE_RXCLEAR | PURGE_RXABORT);
  PurgeComm(port, PURGE_TXCLEAR | PURGE_TXABORT);
}

#endif