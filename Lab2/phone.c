#include <stdio.h>
#include <stdlib.h>

int main(){
    char phone[11];
    int i;
    scanf("%s", phone);
    scanf("%d", &i);

    if (i == -1) {
        printf("%s", phone);
        return 0;
    } else if (i >= 0 && i <= 9) {
        printf("%c", phone[i]);
        return 0;
    } else {
        printf("ERROR");
        return 1;
    }

}
