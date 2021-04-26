#ifndef __AT_SERIAL_TEST_H
#define __AT_SERIAL_TEST_H

#ifdef __cplusplus
extern "C" {

}
#endif

typedef struct at_qicsgp{
	unsigned int  context_type;
	unsigned char APN[20];
}at_qicsgp_t;


/* AT COMMAND ERROR */
#define AT_SEND_ERROR     -10
#define AT_RECV_ERROR     -11

/* serial error code */
#define SERIAL_ERROR_OPEN        -1
#define SERIAL_ERROR_READ        -2
#define SERIAL_ERROR_WRITE       -3
#define SERIAL_ERROR_BAUDRATE    -4
#define SERIAL_ERROR_SETTING     -5
#define SERIAL_INVALID_RESOURCE  -6
#define SERIAL_INVALID_FILE      -7

/* data bits */
#define DATA_BITS_8		8
#define DATA_BITS_7     7
#define DATA_BITS_6		6
#define DATA_BITS_5		5


/* flow control */
#define FLOW_CONTROL_NONE         0
#define FLOW_CONTROL_HARDWARE     1
#define FLOW_CONTROL_SOFTWARE     2

/* stop bit */
#define STOP_BIT_1	1
#define STOP_BIT_2  2

/* parity */
#define PARITY_NONE  'N'
#define PARITY_EVEN  'E'
#define PARITY_ODD   'O'
#define PARITY_SPACE 'S'
#define PARITY_MARK  'M'

/* baud rate*/
#define BAUD_RATE_0			0
#define BAUD_RATE_5			5	
#define BAUD_RATE_75		75
#define BAUD_RATE_110		110
#define BAUD_RATE_150		150
#define BAUD_RATE_200		200
#define BAUD_RATE_300		300
#define BAUD_RATE_600		600
#define BAUD_RATE_1200		1200
#define BAUD_RATE_1800		1800
#define BAUD_RATE_2400		2400
#define BAUD_RATE_4800		4800
#define BAUD_RATE_9600		9600
#define BAUD_RATE_19200		19200
#define BAUD_RATE_38400		38400
#define BAUD_RATE_57600		57600
#define BAUD_RATE_115200	115200
#define BAUD_RATE_230400	230400
#define BAUD_RATE_460800	460800
#define BAUD_RATE_500000	500000
#define BAUD_RATE_576000	576000
#define BAUD_RATE_921600	9216000
#define BAUD_RATE_1000000	1000000
#define BAUD_RATE_11520000	11520000
#define BAUD_RATE_15000000	15000000
#define BAUD_RATE_20000000	20000000
#define BAUD_RATE_25000000	25000000
#define BAUD_RATE_30000000	30000000
#define BAUD_RATE_35000000	35000000
#define BAUD_RATE_40000000	40000000
#endif
