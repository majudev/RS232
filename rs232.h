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


#ifndef rs232_INCLUDED
#define rs232_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>



#if defined(__linux__) || defined(__FreeBSD__)

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/file.h>
#include <errno.h>

int RS232_OpenComport(const char *, int, const char *, struct termios *);
int RS232_PollComport(int, unsigned char *, int);
int RS232_SendByte(int, unsigned char);
int RS232_SendBuf(int, unsigned char *, int);
void RS232_CloseComport(int, struct termios *);
int RS232_IsDCDEnabled(int);
int RS232_IsCTSEnabled(int);
int RS232_IsDSREnabled(int);
void RS232_enableDTR(int);
void RS232_disableDTR(int);
void RS232_enableRTS(int);
void RS232_disableRTS(int);
void RS232_flushRX(int);
void RS232_flushTX(int);
void RS232_flushRXTX(int);	
	
#else

#include <windows.h>

HANDLE RS232_OpenComport(const char *, int, const char *);
int RS232_PollComport(HANDLE, unsigned char *, int);
int RS232_SendByte(HANDLE, unsigned char);
int RS232_SendBuf(HANDLE, unsigned char *, int);
void RS232_CloseComport(HANDLE);
int RS232_IsDCDEnabled(HANDLE);
int RS232_IsCTSEnabled(HANDLE);
int RS232_IsDSREnabled(HANDLE);
void RS232_enableDTR(HANDLE);
void RS232_disableDTR(HANDLE);
void RS232_enableRTS(HANDLE);
void RS232_disableRTS(HANDLE);
void RS232_flushRX(HANDLE);
void RS232_flushTX(HANDLE);
void RS232_flushRXTX(HANDLE);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif


