#include <stdio.h>

int main(){
    int m1,m2,m3,total,roll_no;
    float avg;
    scanf("%d\n",&roll_no);
    scanf("%d %d %d",&m1,&m2,&m3);
    total =m1 + m2 + m3;
    avg = total/3;
    printf("Roll Number: %d\n",roll_no);
    printf("Total Marks: %d\n",total);
    printf("Average Marks:%.2f\n",avg);
    return 0;
}

