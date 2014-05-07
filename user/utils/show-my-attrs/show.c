#include <stdio.h>
#include <unistd.h>

#include "../../include/dcac.h"

int main() {
    char str[1024];
    int i;
    int fds[256];
    printf("Attribute set of process %d\n", getpid());
    int size = dcac_get_attr_fd_list(fds, 256);
    for (i = 0; i < size; i++) {
        dcac_get_attr_name(fds[i], str, 1024);
        printf("%d: %s\n", fds[i], str);
    }
    printf("Total count: %d\n", i);
}
