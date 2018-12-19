#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <grace_np.h>
#include <unistd.h>
#include <float.h>
#include <limits.h>
#include <signal.h>
#include <cash2003.h>
#include <cash2.h>
#include <mersenne.h>
#include <cash2-s.h>

static TYPE2** Competition;
// static TYPE2** SpaceTimePlane;

void Initial(void)
{
    MaxTime = 2147483647; /* default=2147483647 */
    nrow = 350; /* # of row (default=100)*/
    ncol = 350; /* # of column (default=100)*/
    nplane = 1; /* # of planes (default=0)*/
    scale = 2; /* size of the window (default=2)*/
    boundary = WRAP; /* the type of boundary: FIXED, WRAP, ECHO (default=WRAP). Note that Margolus diffusion is not supported for ECHO. */
    ulseedG = 56; /* random seed (default=56)*/
    /* useally, one does not have to change the followings */
    /* the value of boundary (default=(TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.})*/
    boundaryvalue2 = (TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.};
}

void InitialPlane(void)
{
    MakePlane(&Competition/*,&SpaceTimePlane*/);
    InitialSet(Competition,3,0,1,0.1,2,0.1,3,0.1); // I.o.w., half the grid starts as 1, the other are EMPTY (0)
    Boundaries2(Competition);
}

int spiderstokill = 0; //The amount of spiders we kill every nth timestep

void NextState(int row,int col)
{
	int rand_neigh = RandomMoore8(Competition,row,col);
	int self = Competition[row][col].val;

	double a = 0.1;
	double b2 = 0.8; //rood
	double b3 = 0.8; //groen
	double d2 = 0.06; //rood
	double d3 = 0.04; //groen
	
	if(self == 0)
	{
		if(rand_neigh == 1)
		{
			if(genrand_real1() < a)
				Competition[row][col].val = 1;
		}  
	}
	else if(self == 1)
	{
		if(rand_neigh == 2 && genrand_real1() < b2)
			Competition[row][col].val = 2;
		if(rand_neigh == 3 && genrand_real1() < b3)
			Competition[row][col].val = 3;
	}
	else if(self == 2)
	{
		if(genrand_real1() < d2)
			Competition[row][col].val = 0;
		else if(spiderstokill>0){ //Kill a spider if we still have to kill spiders
			spiderstokill = spiderstokill - 1;
			Competition[row][col].val = 0;
		}
	}
	else if(self == 3)
	{
		if(genrand_real1() < d3)
			Competition[row][col].val = 0;
		else if(spiderstokill>0){ //Kill a spider if we still have to kill spiders
			spiderstokill = spiderstokill - 1;
			Competition[row][col].val = 0;
		}
	}
}	


void Update(void)
{ 
  Display(Competition);
  Asynchronous();//1,Competition);
  // SpaceTimePlot(SpaceTimePlane,Competition);
  //int mosquitoes;
  //int spiders;
  //mosquitoes = countGlobal(Competition,1);
  //spiders = countGlobal(Competition,2);
  Plot(1,Competition);
  if(Time%10000000==0) spiderstokill=1000; //Every 100th timestep, kill 100 spiders
  //if(Time%50==0) {MDiffusion(Competition);}
 // Transfer the state of Vote to the SpaceTimePlane  
   // Display(Competition,SpaceTimePlane);
}
