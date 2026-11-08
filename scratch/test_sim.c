#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char* simulate_read_input(const char *input, int buf_size) {
    int bytes_read = 0;
    int size_of_text_of_input = 100;
    char *text_of_input = malloc(size_of_text_of_input);
    bool pending_backslash = false;

    int total_len = strlen(input);
    int pos = 0;

    while (pos < total_len) {
        int chunk = total_len - pos;
        if (chunk > buf_size) chunk = buf_size;

        char buf[16];
        memcpy(buf, input + pos, chunk);
        pos += chunk;

        for (int i = 0; i < chunk; ++i) {
            char c = buf[i];

            if (pending_backslash) {
                if (c == '\n') {
                    // \<newline>: drop both \ and \n
                    pending_backslash = false;
                    continue;
                } else if (c == '\\') {
                    // \\: both backslashes written, neither escapes next char
                    text_of_input[bytes_read++] = '\\';
                    text_of_input[bytes_read++] = '\\';
                    pending_backslash = false;
                } else {
                    // \ followed by normal char
                    text_of_input[bytes_read++] = '\\';
                    text_of_input[bytes_read++] = c;
                    pending_backslash = false;
                }
            } else {
                if (c == '\\') {
                    pending_backslash = true;
                } else if (c == '\n') {
                    text_of_input[bytes_read++] = '\n';
                    goto out;
                } else {
                    text_of_input[bytes_read++] = c;
                }
            }
        }
    }

out:
    if (pending_backslash) {
        text_of_input[bytes_read++] = '\\';
    }
    text_of_input[bytes_read] = '\0';
    return text_of_input;
}

int main() {
    // Test 1: normal command
    char *s1 = simulate_read_input("echo 1\n", 4);
    printf("Test 1: %s", s1);

    // Test 2: double backslash at end of line
    char *s2 = simulate_read_input("echo \\\\\n", 4);
    printf("Test 2: %s", s2);

    // Test 3: line continuation
    char *s3 = simulate_read_input("echo a\\\nb\n", 4);
    printf("Test 3: %s", s3);

    // Test 4: backslash split across buffer boundary
    // "echo 12345678901\\" len is 16, so \ is at chunk boundary!
    char *s4 = simulate_read_input("echo 12345678901\\\nb\n", 16);
    printf("Test 4: %s", s4);

    return 0;
}
