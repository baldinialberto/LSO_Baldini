#ifndef _SERVER_COMMON_H
#define _SERVER_COMMON_H

#pragma once

#define O_CREATE_FLAG   0x00000010
#define O_LOCK_FLAG     0x00000020
#define O_USR1_FLAG     0x00000040
#define O_USR2_FLAG     0x00000080

#define OPEN_OP     0x00000000
#define CLOSE_OP    0x00000002
#define READ_OP     0x00000004
#define WRITE_OP    0x00000006
#define APPEND_OP   0x00000008
#define REMOVE_OP   0x0000000A
#define LOCK_OP     0x0000000C
#define UNLOCK_OP   0x0000000E

#define MESSG_MASK  0xFFFFFF00
#define FLAG_MASK   0x000000F0
#define OP_MASK     0x0000000F

#define MESSAGE_MAX_LEN 0xFFFFFF
#define mssg_t unsigned int
#define MESSG_SHIFT 8

typedef int server_command_t;
// custom message to server encoded in 4Bytes
// 3Bytes for len of string message 
// 4bits for flags + 4bits for op
// XXXXXXXX XXXXXXXX XXXXXXXX YYYY ZZZZ
//   len of string message    flag  op

#endif