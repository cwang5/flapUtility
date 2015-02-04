#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE* fp;
	FILE* collision;
	char buff[BUFSIZ];
	float absvalue(float);
	int i,j;
	float angle, moment, angle2, moment2;
	angle2=90;
	moment2=1;
	i=0;
	while(i==0)
	{
	system("bash ./utilities/test_script");
	fp=fopen("./forceBalance.dat","r");
	if (fp != NULL)
	{
		while(fgets(buff, sizeof buff, fp) != NULL)
		{
			sscanf(buff, "%f %f", &angle, &moment);
		}
		if (absvalue(angle-angle2) < 5e-4)
		{
			i=1;
		}
		if ((absvalue(angle-angle2) < 1e-3) && (moment2*moment > 0) && (absvalue(moment2-moment) <1e-4))
		{
			i=1;
		}
		angle2=angle;
		moment2=moment;
	}
	else
	{
		printf("Error!! forcebalance.dat not found!\n");
		return 1;
	}
	fclose(fp);
	}
}

float absvalue(float x)
{
	if (x < 0.0)
		return -x;
	else
		return x;

}
