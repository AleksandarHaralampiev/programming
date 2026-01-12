#include <stdio.h>
unsigned floor(unsigned x, unsigned p2)
{
    return x - (x % p2);
}
unsigned ceil(unsigned x, unsigned p2)
{
    return floor(x, p2) + p2;
}