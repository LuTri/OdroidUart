#include <string.h>
#include <unistd.h>

#include <pty.h>
#include <stdio.h>
#include "../uart.h"

#define MOCK_BUFFER BUFF_SIZE * 10

int read_zero_safe(int fd, char* buff, int buff_size) {
    int read_bytes = 0;

    buff[0] = '\0';
    while(buff[0] == '\0') {
        /* WHY THE F ever a previous closing byte is sometimes appended to the
         * next transmission... cope with that */
        read_bytes = read(fd, buff, 1);
    }

    read_bytes += read(fd, buff + read_bytes, buff_size);

    if (buff[read_bytes - 1] !=
        '\0') { /* ALSO WHY THE F ever, sometimes the closing byte is
             transmitted, sometimes not. Cope with it. */
        read(fd, buff + read_bytes, 1);  // Read mandatory \0 problem
    }
    return read_bytes;
}

void mock_binary(int master, const char* mock_answer) {
    static int call=0;
    char buff[MOCK_BUFFER];
    int size;

    uint8_t status;
    char uart_buff[BUFF_SIZE];
    char uart_echo[3];

    size = read_zero_safe(master, buff, MOCK_BUFFER);

    write_to_buff_binary(buff, size);

    uart_prot_read(uart_buff, BUFF_SIZE, &status);
    read_from_buff(uart_echo);

    if (mock_answer == NULL) {
        write(master, uart_echo, 2);
    } else {
        write(master, mock_answer, 2);
    }
}

int main(void) {
    char input_buffer[MOCK_BUFFER];
    int c;

    int master, slave;
    char slavename[MOCK_BUFFER];

    openpty(&master, &slave, slavename, NULL, NULL);

    fflush(stdin);
    fgets(input_buffer, MOCK_BUFFER, stdin);
    printf("%s\n", slavename);
    fflush(stdout);

    close(slave);

    fflush(stdin);
    fgets(input_buffer, MOCK_BUFFER, stdin);
    while (strcmp(input_buffer, "x\n") != 0) {
        if (strcmp(input_buffer, "b\n") == 0) {
            mock_binary(master, NULL);
        } else if (strcmp(input_buffer, "p\n") == 0) {
            mock_binary(master, MSG_PARITY_ERROR);
        } else if (strcmp(input_buffer, "f\n") == 0) {
            mock_binary(master, MSG_FRAME_ERROR);
        } else if (strcmp(input_buffer, "d\n") == 0) {
            mock_binary(master, MSG_DATA_OVERRUN);
        }
        fflush(stdin);
        fgets(input_buffer, MOCK_BUFFER, stdin);
    }

    close(master);
    return 0;
}
