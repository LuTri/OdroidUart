#include "handlers.h"
#include "commands.h"

/* INTERNAL API */

typedef struct {
    uint8_t cmd;
    CMD_HANDLER* handler;
} CMD_HANDLER_ASSOC;

CMD_HANDLER_ASSOC _cmd_assocs[MAX_CMD_HANDLERS] = { { 0, 0} };
uint8_t _n_registered_handlers = 0;

uint8_t _find_registered_cmd(uint8_t cmd) {
    for (uint8_t idx = 0; idx < MAX_CMD_HANDLERS; idx++) {
        if (_cmd_assocs[idx].cmd == cmd && _cmd_assocs[0].handler != 0)
            return idx + 1;
    }
    return 0;
}

/* PUBLIC API */

uint8_t register_handler(uint8_t cmd, CMD_HANDLER* handler) {
    CMD_HANDLER_ASSOC *current_assoc = _cmd_assocs;

    if (_find_registered_cmd(cmd)) return CMD_IS_REGISTERED;
    if (_n_registered_handlers >= MAX_CMD_HANDLERS) return CMD_REGISTER_FULL;

    while (current_assoc->handler != 0) current_assoc++;
    current_assoc->handler = handler;
    current_assoc->cmd = cmd;

    _n_registered_handlers++;
    return REGISTER_OK;
}

uint8_t unregister_handler(uint8_t cmd) {
    uint8_t idx = _find_registered_cmd(cmd);
    if (!(idx)) return CMD_NOT_REGISTERED;

    _cmd_assocs[idx - 1].handler = 0;
    _cmd_assocs[idx - 1].cmd = 0;

    _n_registered_handlers--;
    return REGISTER_OK;
}

uint8_t num_registered_handlers(void) {
    return _n_registered_handlers;
}

CMD_HANDLER* get_handler(uint8_t cmd) {
    uint8_t idx = _find_registered_cmd(cmd);
    if (!(idx)) return 0;

    return _cmd_assocs[idx -1].handler;
}
