#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main()

{
    FILE *input; /* the forceBalance.dat file in main */
    FILE *output; /* */
    FILE *hindge; /* the hindge.dat file in triSurface */
    FILE *stl; /* the plane.stl file in triSurface*/
    char buff[BUFSIZ];
    char buff2[BUFSIZ];
    char loop_type[10];
    int i;
    float normal[3],p1[3],p2[3],p3[3],p4[3],tmp1[3],tmp2[3],tmp3[3],moment_1,moment_2;
    float angle_read,angle_1,angle_2,angle_optimized,length,surf_ang;
/*p1 and p2 must be the points defining the hindge, while p3 and p4 describe the end points */
/*p1[2] & p3[2] < 0, p2[2] & p4[2] >0 */
    p3[2]=0;
    p4[2]=0;
    hindge=fopen("./constant/triSurface/hindge.dat","r");
    if (hindge == NULL)
    {
	printf("Error! locations of the hindge point is not specified");
	return 1;
    }
    else
    {
	fscanf(hindge,"%f %f %f \n",&p1[1],&p1[2],&p1[3]);
	fscanf(hindge,"%f %f %f \n %f %f",&p2[1],&p2[2],&p2[3],&surf_ang,&length);
	printf("The hindge points are (%f %f %f) and (%f %f %f)\n",p1[1],p1[2],p1[3],p2[1],p2[2],p2[3]);
	fclose(hindge);
    }
    /* insert optimization algorithm here */
    input=fopen("./forceBalance.dat","r");
    if (input == NULL)
    {
	printf("Error! Unable to locate forceBalance.dat");
	return 1;
    }
    else
    {
	i=0;
	while(fgets(buff, sizeof buff, input) != NULL)
	{
	    if (i<1)
	    {
		
		sscanf(buff, "%f %f \n", &angle_1, &moment_1);
		angle_optimized=angle_1+10;
		printf("%f\n", angle_1);

	    }
	    else
	    {
		angle_2=angle_1;
		moment_2=moment_1;
		sscanf(buff, "%f %f \n",&angle_1, &moment_1);
		angle_optimized=angle_1-(angle_1-angle_2)/(moment_1-moment_2)*moment_1;
	    }
	    i++;
	}
	if (angle_optimized <= surf_ang+5)
		angle_optimized=surf_ang+5;
	if (angle_optimized >= surf_ang+180)
		angle_optimized=surf_ang+175;
	printf("angle_optimized is %f degree\n",angle_optimized);
    }
    /* defining the new points to move the flap to */
    p3[1]=p1[1]+length*cosf(angle_optimized*M_PI/180);
    p3[2]=p1[2];
    p3[3]=p1[3]+length*sinf(angle_optimized*M_PI/180);
    p4[1]=p3[1];
    p4[2]=p2[2];
    p4[3]=p3[3];
    /* write the new points to the plane.stl file for next iteration */
    fclose(input);
    output=fopen("./constant/triSurface/flap.stl","w");
    fprintf(output,"solid flap\n");
    fprintf(output,"\t facet normal %f 0.0 %f\n", cosf((90+angle_optimized)*M_PI/180), sinf((90+angle_optimized)*M_PI/180));
    fprintf(output,"\t\t outer loop\n");
    fprintf(output,"\t\t\t vertex %f %f %f\n",p1[1],p1[2],p1[3]);
    fprintf(output,"\t\t\t vertex %f %f %f\n",p2[1],p2[2],p2[3]);
    fprintf(output,"\t\t\t vertex %f %f %f\n",p3[1],p3[2],p3[3]);
    fprintf(output,"\t\t endloop\n\t endfacet\n");
    fprintf(output,"\t facet normal %f 0.0 %f\n", cosf((90+angle_optimized)*M_PI/180), sinf((90+angle_optimized)*M_PI/180));
    fprintf(output,"\t\t outer loop\n");
    fprintf(output,"\t\t\t vertex %f %f %f\n",p4[1],p4[2],p4[3]);
    fprintf(output,"\t\t\t vertex %f %f %f\n",p2[1],p2[2],p2[3]);
    fprintf(output,"\t\t\t vertex %f %f %f\n",p3[1],p3[2],p3[3]);
    fprintf(output,"\t\t endloop\n\t endfacet\n");
    fprintf(output,"endsolid flap");
    fclose(output);	
}





