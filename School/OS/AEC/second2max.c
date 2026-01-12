#include <stdio.h>
void sort2(int *a, int *b)
{
if (*a>*b)
{
int tmp=*b;
*b=*a;
*a=tmp;
}
}
int second2max(int a, int b, int c, int d)
{
sort2(&a, &b);
sort2(&c, &d);
sort2(&b, &d);
sort2(&a, &c);
sort2(&b, &c);
return c;
}