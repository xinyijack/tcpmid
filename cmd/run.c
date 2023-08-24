//
// Created by jack on 23-8-24.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    char *input0 = "ls -l ";
    char output[1024];
    FILE *f0 = popen(input0, "r");
    while (fgets(output, sizeof(output), f0) != NULL) {
        printf("cmd result: %s", output);
    }
    pclose(f0);

    char *input1 = "ls -l /home/jack/CLionProjects/tcpmid";
    FILE *f1 = popen(input1, "r");
    fwrite(input1, 1, sizeof(input1), f1);
    while (fgets(output, sizeof(output), f1) != NULL) {
        printf("cmd result: %s", output);
    }
    pclose(f1);
}