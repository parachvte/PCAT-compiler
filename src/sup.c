#include <stdio.h>

int read_int(){
    int a;
    scanf("%d",&a);
    return a;
}

int read_real(){
    float a;
    scanf("%f",&a);
    int b = *(int*)(&a);
    return b;
}

void print_int( int a ){
    printf("%d",a);
}

void print_real( int a ){
    float r = *(float*)(&a);
    printf("%f",r);
}

void print_str( int a ){
    char* p = (char*)a;
    printf("%s",p);
}

void print_bool( int a ){
    if ( a ) printf("TRUE"); else printf("FALSE");
}

void print_line(){
    printf("\n");
}

void MainEntry();

int main(){
    MainEntry();
    return 0;
}
