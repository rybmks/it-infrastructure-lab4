#include <ctype.h>
#include <stdio.h>

void process_string(char *str) {
    for (int i = 0; str[i] != '\0' && str[i] != '\n'; i++) {
        if (i % 2 == 0) {
            str[i] = tolower(str[i]);
        } else {
            str[i] = toupper(str[i]);
        }
    }
}

int main() {
    char input[100];

    int variant = 15;

    printf("%d\n", variant);

    if (fgets(input, sizeof(input), stdin) != NULL) {
        process_string(input);
        printf("%s", input);
    }

    return 0;
}