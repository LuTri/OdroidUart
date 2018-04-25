#include <unistd.h> 
#include <string.h>

#include <stdio.h>
#include <pty.h>
#include "../uart.h"

void echo_with_sum(int master, int slave) {
    static int calls = 0;
    char buff[BUFF_SIZE];
    int size;

    char uart_buff[1000];
    char uart_echo[3];

    size = read(master, buff, BUFF_SIZE);
    if (buff[size-1] != '\0') { /* WHY THE F ever, sometime the closing byte is
                                   transmitted, sometimes not. Cope with it. */
        read(master, buff + size, 1); // Read mandatory \0 problem
    }
    write_to_buff(buff);

    uart_blocking_read_string(uart_buff, 1000);
    read_from_buff(uart_echo);

    write(master, uart_echo, 2);
    write(master, "\0", 1); // Send string end
}

int cmp_verbose(char* a, char* b) {
    int result;
    result = strcmp(a,b);
    return result;
}

int main(void) {
    char input_buffer[BUFF_SIZE];
    int c;

    int master, slave;
    char slavename[1000];

    openpty(&master, &slave, slavename, NULL, NULL);

    fflush(stdin);
    fgets(input_buffer, BUFF_SIZE, stdin);
    printf("%s\n", slavename);
    fflush(stdout);

    fflush(stdin);
    fgets(input_buffer, BUFF_SIZE, stdin);
    write(master, "Hello Python!", strlen("Hello Python!"));
    printf("Wrote \"Hello Python\"");
    fflush(stdout);

    close(slave);

    fflush(stdin);
    fgets(input_buffer, BUFF_SIZE, stdin);
    while(cmp_verbose(input_buffer, "x\n") != 0) {
        echo_with_sum(master, slave);
        fflush(stdin);
        fgets(input_buffer, BUFF_SIZE, stdin);
    }

    close(master);
    return 0;
}
