#include <stdio.h>
#include <string.h>

int main(void) {
    char line[] = "Hello: World";
    char *colon_ptr = strchr(line, ':');
    if (colon_ptr != NULL) {
        printf("Found colon at index %ld\n", colon_ptr - line);
    } else {
        printf("Colon not found\n");
    }
    return 0;
}