# ifndef NORMAL_HEADER_HPP
# define NORMAL_HEADER_HPP

#include "pthread.h"

void InitActiveCodePage(){
    system("chcp 65001");
    system("cls");
}
bool CheckPointer(void* p){
    return p != nullptr;
}
void AlertPointer(void* p,int exitCode = 0){
    if (!CheckPointer(p))    exit(exitCode);
}
template <class T>  
void SwapValue(T& a,T& b){  //  浅拷贝
    T tmp = a;
    a = b;
    b = tmp;
}
# endif