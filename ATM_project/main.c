#include "main.h"

int main(){
    read_from_csv("data.csv");
    initialize_transaction(&trans_head, head);
    while (1){
        system("cls");
        authenticate_user(head);
        printf("Enter 0 to exit: ");
    }
    return 0;
}