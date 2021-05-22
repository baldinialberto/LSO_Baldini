#ifndef _SERVER_COMMON_H
#define _SERVER_COMMON_H

#pragma once

// command flags
#define O_CREATE_FLAG   0x00000010
#define O_LOCK_FLAG     0x00000020
#define O_DIR_FLAG      0x00000040
#define O_USR_FLAG      0x00000080

// error / return flags
#define WRBACK_FLAG     0x00000002
#define ALREADY_FLAG    0x00000004
#define NOTCONN_FLAG    0x00000006
#define FNF_FLAG        0x00000008
#define LOCKED_FLAG     0x0000000A

// command op codes
#define OPEN_OP     0x00000000
#define CLOSE_OP    0x00000001
#define READ_OP     0x00000002
#define WRITE_OP    0x00000003
#define APPEND_OP   0x00000004
#define REMOVE_OP   0x00000005
#define LOCK_OP     0x00000006
#define UNLOCK_OP   0x00000007
#define CLSCONN_OP  0x00000008

#define MESSG_MASK  0xFFFFFF00
#define FLAG_MASK   0x000000F0
#define OP_MASK     0x0000000F

#define MESSAGE_MAX_LEN 0xFFFFFF
#define MESSG_SHIFT 8

typedef unsigned int server_command_t;
// custom message to server encoded in 4Bytes
// 3Bytes for len of string message 
// 4bits for flags + 4bits for op
// XXXXXXXX XXXXXXXX XXXXXXXX YYYY ZZZZ
//   len of string message    flag  op

// on return from server :
// 0 success
// WRBACK_OP is set if the server has evicted a file
// LOCK_FLAG is set if the file is already locked
// FNF_FLAG is set if the file is not stored
// esle error has occurred

#endif
