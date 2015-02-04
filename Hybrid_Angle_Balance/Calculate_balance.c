#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main()

{
    FILE *input;
    FILE *plane;
    FILE *output;
    int i;
    char C[60];
    char *brac;
    char buff[BUFSIZ];
    float time,angle;
    float pf[3],vf[3],pm[3],vm[3];
    float forceSum[3],normal[3],momentSum[3];

    forceSum[1]=0.0;
    forceSum[2]=0.0;
    forceSum[3]=0.0;
    input=fopen("./balance/0/forces.dat","r");
    if (input==NULL)
    {
	printf("Error, unable to find forces.dat, please check your folder name/structure... \n");
	return 1;
    }
    else
    {
	printf("Reading force balance across the flap... \n");
	brac=fgets(C, 60, input);
	/* reading in the "title" line */
	i = 0;
	while (fgets(buff, sizeof buff, input) != NULL)
	{
	    sscanf(buff, "%f (((%f %f %f) (%f %f %f)) ((%f %f %f) (%f %f %f)))",&time,&pf[1],&pf[2],&pf[3],&vf[1],&vf[2],&vf[3],&pm[1],&pm[2],&pm[3],&vm[1],&vm[2],&vm[3]);
	    /* I honestly don't know what the %f-> &time actually read in, but it's most certainly not accurate, although the current arrangement seems to make all other numbers accurate, so that's that. The time variable won't be used for anything anyway. */
/* 	    printf("%s",buff); */
/* 	    printf("%i %f %f %f %f %f %f %f %f %f %f %f %f\n",i,pf[1],pf[2],pf[3],vf[1],vf[2],vf[3],pm[1],pm[2],pm[3],vm[1],vm[2],vm[3]); */
	    /*summing up the forces in each of the x, y, z direction */
	    if (i == 0)
	    {
		forceSum[1]=0.0;
		forceSum[2]=0.0;
		forceSum[3]=0.0;
		momentSum[1]=0.0;
		momentSum[2]=0.0;
		momentSum[3]=0.0;
	    }
	    else
	    {
		forceSum[1]+=pf[1]+vf[1];
		forceSum[2]+=pf[2]+vf[2];
		forceSum[3]+=pf[3]+vf[3];
		momentSum[1]+=pm[1]+vm[1];
		momentSum[2]+=pm[2]+vm[2];
		momentSum[3]+=pm[3]+vm[3];
	    }
	    i++;
	}
	forceSum[1]/=(float)i;
	forceSum[2]/=(float)i;
	forceSum[3]/=(float)i;
	momentSum[1]/=(float)i;
	momentSum[2]/=(float)i;
	momentSum[3]/=(float)i;
	printf("Moments about point (0 0 3) are:%f y:%f z:%f \n",momentSum[1],momentSum[2],momentSum[3]);
	fclose(input);
	plane=fopen("./constant/triSurface/flap.stl","r");
	if(plane == NULL)
	{
	    printf("Error! Unable to find flap angle, please check the file location!\n");
	    return 1;
	}
	else
	{
	    fgets(buff, sizeof buff, plane);
	    fscanf(plane,"%*s %*s %f %f %f", &normal[1], &normal[2], &normal[3]);
	    fclose(plane);
	    if (normal[2] != 0.0)
	    {
		printf("Error! Invalid Normal Direction");
		return 1;
	    }
	    else
	    {
		angle=atanf(normal[3]/normal[1]);
		if (angle < 0)
		    angle = (angle+M_PI)*180/M_PI-90;
		else
		    angle = angle*180/M_PI-90;
		printf("Flap angle is read in as: %f degree \n",angle);
	    }
	}
	output=fopen("forceBalance.dat","a");
	printf("Writing forceBalance.dat...\n");
	fprintf(output, "%f %f\n",angle,momentSum[2]);
	fclose(output);
    }
}
