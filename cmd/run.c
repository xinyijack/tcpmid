//
// Created by jack on 23-8-24.
//
#include "../lib/common.h"

int main() {
    char *input0 = "ls -l ";
    char output[1024];
    FILE *f0 = popen(input0, "r");
    while (fgets(output, sizeof(output), f0) != NULL) {
        printf("cmd result: %s", output);
    }
    pclose(f0);
//  /root/ClionProjects/tcpmid
    char *input1 = "ls -l /root/ClionProjects/tcpmid";
    FILE *f1 = popen(input1, "r");
    fwrite(input1, 1, sizeof(input1), f1);
    while (fgets(output, sizeof(output), f1) != NULL) {
        printf("cmd result: %s", output);
    }
    pclose(f1);
}