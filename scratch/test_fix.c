#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void test_trace(const char *buf) {
    char text_of_input[100];
    int bytes_read = 0;
    bool is_backslash = false;
    int res = strlen(buf);

    for (int i = 0; i < res; ++i) {
        if (buf[i] != '\n' && is_backslash) {
            text_of_input[bytes_read++] = '\\';
            if (buf[i] == '\\') {
                text_of_input[bytes_read++] = '\\';
                is_backslash = false;
                continue; // THE FIX
            }
        }
        if (is_backslash && (buf[i] == '\n')) {
            printf("PROMPT > (line continuation triggered!)\n");
        } else if (buf[i] != '\\') {
            text_of_input[bytes_read++] = buf[i];
        }
        if (buf[i] == '\n' && !is_backslash) {
            printf("OUT_OF_LOOP (line finished!)\n");
            break;
        }
        if (is_backslash) {
            is_backslash = false;
        }
        if (buf[i] == '\\' && !is_backslash) {
            is_backslash = true;
        }
    }
    text_of_input[bytes_read] = '\0';
    printf("Result: %s\n", text_of_input);
}

int main() {
    printf("--- Test with 'echo \\\\\\\\n' ---\n");
    test_trace("echo \\\\\n");

    printf("\n--- Test with 'echo \\\\\\\\a\\n' ---\n");
    test_trace("echo \\\\a\n");

    printf("\n--- Test with 'echo a\\\\\\nb\\n' ---\n");
    test_trace("echo a\\\nb\n");

    return 0;
}
