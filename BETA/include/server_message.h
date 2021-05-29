#ifndef _SERVER_MESSAGE_H
#define _SERVER_MESSAGE_H

#define SAPI_SUCCESS        0x00
#define SAPI_FAILURE        0x01
#define SAPI_EVICT          0x02
#define SAPI_FNF            0x03

#define O_CREATE            0x01
#define O_LOCK              0x02

//#define SAPI_OPENCONN       0x04
//#define SAPI_CLOSECONN      0x08
#define SAPI_OPENFILE       0x0C
#define SAPI_READFILE       0x10
#define SAPI_READNFILES     0x14
#define SAPI_WRITEFILE      0x18 
#define SAPI_APPENDFILE     0x1C
#define SAPI_LOCKFILE       0x20
#define SAPI_UNLOCKFILE     0x24
#define SAPI_CLOSEFILE      0x28
#define SAPI_REMOVEFILE     0x2C

#define SAPI_MSSLEN_SHFT    0x08

#define SAPI_MESSAGE_LEN(FLAG, OP, MESSAGELEN) ((unsigned int)FLAG | (unsigned int)OP | (unsigned int)(MESSAGELEN << SAPI_MSSLEN_SHFT))

typedef unsigned int s_message;



#endif