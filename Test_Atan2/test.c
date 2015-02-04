#include <stdio.h>
#include <math.h>

int main (void)
{
float a,b, output;
printf("Input x,y\n");
scanf("%f %f",&a,&b);
output=atan2(b,a)*180/M_PI;
if (output<0)
	output+=360;
printf("%f\n",output);
}
