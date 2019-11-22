#pragma once
#include <iostream>

void print();
template<typename Head, typename... Args>
void print(const Head& head, const Args&... args );

void print(){ std::cout << std::endl;};
template<typename Head, typename... Args>
void print(const Head& head, const Args&... args )
{
    std::cout << head << " ";
    print(args...);
};