#include <stdio.h>
#include <string.h>

int readInt(int min, int max){
    char line[100];
    int num;
    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) continue;
        if (sscanf(line, "%d", &num) == 1 && num >= min && num <= max) {
            return num;
        }
        printf("Invalid input, enter a number from %d to %d: ", min, max);
    }
}

void readString(char buf[], int size) {
    if (fgets(buf, size, stdin) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}
int main(){
    
}
