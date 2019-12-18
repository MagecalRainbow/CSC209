#include <stdio.h>
#include <stdlib.h>

int main() {
    char phone[11];
    int i;
    int error = 0;

    scanf("%s", phone);

    while (scanf("%d", &i) != EOF) {
        if (i == -1) {
            printf("%s\n", phone);
        } else if (i >= 0 && i <= 9) {
            printf("%c\n", phone[i]);
        } else {
            printf("ERROR\n");
            error += 1;
        }
    }

    if (error == 0) {
        return 0;
    } else {
        return 1;
    } 


}
