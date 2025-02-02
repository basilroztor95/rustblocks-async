#include <stdio.h>
#include <stdlib.h>

int main() {
    int status = system("ping -c 1 -q 8.8.8.8 &> /dev/null");
    
    if (status == 0) {
        printf("OK");
    } else {
        printf("FAIL");
    }

    return 0;
}

