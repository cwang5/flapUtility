#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main()
{
	int u_index, l_index, flag,i ;
	FILE *input_air;
	FILE *input_flap;
	FILE *airfoil;
	FILE *flap;
	FILE *hindge;
	float upper[BUFSIZ][2],lower[BUFSIZ][2];
	float position[2],length,a,b,c,span,alpha,tmp_x,tmp_y,surf_angle;
	char buff[BUFSIZ], id[BUFSIZ];
	flag=0;
	u_index=1;
	l_index=1;
	printf("Please Input the Half-Span of the Domain (length/chord): ");
	scanf("%f", &span);
	printf("The computation domain spans from y= %f to y=%f.\n",-span,span);
	input_air=fopen("./profile.dat","r");
	if(input_air == NULL)
	{
		printf("Error!! Unable to find airfoil profile, please check/rename the airfoil data file to profile.dat\n");
		return 1;
	}
	else
	{
		fscanf(input_air, "%s", &id);
		/*printf("%s\n", id);*/
		if (strcmp(id,"upper") != 0)
		{
			printf("Invalid file format!! Please make sure the profile.dat is formated as specified in README!\n");
			return 1;
		}
		else
		{
			fgets(buff, sizeof buff, input_air);
		}
		while (fgets(buff, sizeof buff, input_air) != NULL)
		{
			sscanf(buff, "%s", &id);
			if (strcmp(id,"lower") == 0)
			{
				fgets(buff, sizeof buff, input_air);
				flag = 1;
			}
			if (flag == 0)
			{
				sscanf(buff, "%f %f\n", &upper[u_index][1], &upper[u_index][2]);
				printf("%i %f %f\n",u_index, upper[u_index][1], upper[u_index][2]);
				u_index++; 
			}
			else if (flag == 1)
			{
				sscanf(buff, "%f %f\n", &lower[l_index][1], &lower[l_index][2]);
				printf("%i %f %f\n",l_index, lower[l_index][1], lower[l_index][2]);
				l_index++; 
			}
		}
		/* Insert Sort Algorithm */
		
	}
	fclose(input_air);
	input_flap=fopen("./plane.dat","r");
	if (input_flap == NULL)
	{
		printf("Error!! Unable to find flap data! Please locate flap.dat!!\n");
		return 1;
	}
	else
	{
		fscanf(input_flap,"%f %f", &position[1], &length);
		if (position[1]> 1 || length > 0.5)
		{
			printf("Please check the input file, the flap position/length does not seems right... \n");
			return 1;
		}
		i=2;
		while ((upper[i][1]-position[1])*(upper[i-1][1]-position[1])>0 && upper[i][1] != position[1])
		{
			i=i++;
			/*printf("%i\n",i);*/
		}
		position[2]=upper[i-1][2]+(upper[i][2]-upper[i-1][2])*(position[1]-upper[i-1][1])/(upper[i][1]-upper[i-1][1]);
		printf("%f %f \n",position[1], position[2]);
		a=upper[i][1]-upper[i-1][1];
		b=upper[i][2]-upper[i-1][2];
		surf_angle=atanf(b/a);
		surf_angle=surf_angle*180/M_PI;
		printf("%f\n",surf_angle);
	}
	/* Algorithm to rotate the airfoil about (0,0.5) by the value of alpha defined by the user*/
	/* Alpha can only be from 0-30 degree, as flap would not deploy anyway at negitive AoA, and AoA >30 is unrealistic */
	printf("Please Enter the Angle of Attack that the Wing will be at (degree, 0-30):\n");
	scanf("%f",&alpha);
	alpha=-alpha*M_PI/180;
	if (alpha > 0 || alpha < -30)
	{
		printf("Invalid AoA Input!!\n");
		return 1;
	}
	for (i=1; i< u_index; i++)
	{
		tmp_x=upper[i][1]-0.5;
		tmp_y=upper[i][2];
		upper[i][1]=tmp_x*cosf(alpha)-tmp_y*sinf(alpha)+0.5;
		upper[i][2]=tmp_x*sinf(alpha)+tmp_y*cosf(alpha);
	}
	for (i=1; i< l_index; i++)
	{
		tmp_x=lower[i][1]-0.5;
		tmp_y=lower[i][2];
		lower[i][1]=tmp_x*cosf(alpha)-tmp_y*sinf(alpha)+0.5;
		lower[i][2]=tmp_x*sinf(alpha)+tmp_y*cosf(alpha);
	}
	/* rotate the "hindge" points */
	tmp_x=position[1]-0.5;
	tmp_y=position[2];
	position[1]=tmp_x*cosf(alpha)-tmp_y*sinf(alpha)+0.5;
	position[2]=tmp_x*sinf(alpha)+tmp_y*cosf(alpha);
	alpha=alpha*180/M_PI;
	fclose(input_flap);
	airfoil=fopen("./Results/airfoil.stl","w");
	/* a -> x direction, b -> y direction, c = length */ 
	for (i=2; i< u_index; i++)
	{
		a=upper[i][1]-upper[i-1][1];
		b=upper[i][2]-upper[i-1][2];
		if (a<0)
		{
			a=-a;
			b=-b;
		}
		c=sqrt(pow(a,2.0)+pow(b,2.0));
		fprintf(airfoil,"solid airfoil-u-%i\n",i-1);
		fprintf(airfoil,"\t facet normal %f 0.0 %f\n",-b/c,a/c);
		fprintf(airfoil,"\t\t outer loop\n");
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",upper[i-1][1],span,upper[i-1][2]);
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",upper[i][1],span,upper[i][2]);
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",upper[i-1][1],-span,upper[i-1][2]);
		fprintf(airfoil,"\t\t endloop\n\t endfacet\n");
		fprintf(airfoil,"\t facet normal %f 0.0 %f\n",-b/c,a/c);
		fprintf(airfoil,"\t\t outer loop\n");
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",upper[i-1][1],-span,upper[i-1][2]);
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",upper[i][1],-span,upper[i][2]);
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",upper[i][1],span,upper[i][2]);
		fprintf(airfoil,"\t\t endloop\n\t endfacet\n");
		fprintf(airfoil,"endsolid airfoil-u-%i\n",i-1);
	}
	for (i=2; i< l_index; i++)
	{
	
		a=lower[i][1]-lower[i-1][1];
		b=lower[i][2]-lower[i-1][2];
		if (a<0)
		{
			a=-a;
			b=-b;
		}
		c=sqrt(pow(a,2.0)+pow(b,2.0));
		fprintf(airfoil,"solid airfoil-l-%i\n",i-1);
		fprintf(airfoil,"\t facet normal %f 0.0 %f\n",b/c,-a/c);
		fprintf(airfoil,"\t\t outer loop\n");
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",lower[i-1][1],span,lower[i-1][2]);
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",lower[i][1],span,lower[i][2]);
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",lower[i-1][1],-span,lower[i-1][2]);
		fprintf(airfoil,"\t\t endloop\n\t endfacet\n");
		fprintf(airfoil,"\t facet normal %f 0.0 %f\n",b/c,-a/c);
		fprintf(airfoil,"\t\t outer loop\n");
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",lower[i-1][1],-span,lower[i-1][2]);
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",lower[i][1],-span,lower[i][2]);
		fprintf(airfoil,"\t\t\t vertex %f %f %f\n",lower[i][1],span,lower[i][2]);
		fprintf(airfoil,"\t\t endloop\n\t endfacet\n");
		fprintf(airfoil,"endsolid airfoil-l-%i\n",i-1);
	}
	fclose(airfoil);
	flap=fopen("./Results/flap.stl","w");
	fprintf(airfoil,"solid flap\n");
	fprintf(airfoil,"\t facet normal 0.0 0.0 1.0\n");
	fprintf(airfoil,"\t\t outer loop\n");
	fprintf(airfoil,"\t\t\t vertex %f %f %f\n",position[1],span,position[2]);
	fprintf(airfoil,"\t\t\t vertex %f %f %f\n",position[1]+length,span,position[2]);
	fprintf(airfoil,"\t\t\t vertex %f %f %f\n",position[1],-span,position[2]);
	fprintf(airfoil,"\t\t endloop\n\t endfacet\n");
	fprintf(airfoil,"\t facet normal 0.0 0.0 1.0\n");
	fprintf(airfoil,"\t\t outer loop\n");
	fprintf(airfoil,"\t\t\t vertex %f %f %f\n",position[1],-span,position[2]);
	fprintf(airfoil,"\t\t\t vertex %f %f %f\n",position[1]+length,-span,position[2]);
	fprintf(airfoil,"\t\t\t vertex %f %f %f\n",position[1]+length,span,position[2]);
	fprintf(airfoil,"\t\t endloop\n\t endfacet\n");
	fprintf(airfoil,"endsolid flap\n");
	fclose(flap);
	hindge=fopen("./Results/hindge.dat","w");
	fprintf(hindge,"%f %f %f\n%f %f %f\n %f %f",position[1],span,position[2],position[1],-span,position[2],surf_angle+alpha,length);
	fclose(hindge);

}
