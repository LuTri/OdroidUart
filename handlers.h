#ifndef _OUART_HANDLERS_H
#define _OUART_HANDLERS_H

#include "commands.h"

#ifndef UNITTEST
#include <avr/io.h>
#endif

#define MAX_CMD_HANDLERS 20

#define CMD_REGISTER_FULL 0x02
#define CMD_IS_REGISTERED 0x01
#define CMD_NOT_REGISTERED 0x03
#define REGISTER_OK 0xFF

typedef uint8_t (CMD_HANDLER)(COMMAND_BUFFER* command);

uint8_t register_handler(uint8_t cmd, CMD_HANDLER* handler);
uint8_t unregister_handler(uint8_t cmd);
uint8_t num_registered_handlers(void);
CMD_HANDLER* get_handler(uint8_t cmd);
void handle_loop(void);

#endif /* ifndef _OUART_HANDLERS_H */