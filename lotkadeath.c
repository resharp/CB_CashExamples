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

int EMPTY = 0;
int MOSQUITO = 1;
int SPIDER1 = 2;
int SPIDER2 = 4;

double birth_rate_mosquito = 0.1;
double consumption1 = 0.4; 
double consumption2 = 0.4; 
double death1 = 0.06; //rood
double death2 = 0.011; // blauw

void Initial(void)
{
    MaxTime = 2147483647; /* default=2147483647 */
    nrow = 300; /* # of row (default=100)*/
    ncol = 300; /* # of column (default=100)*/
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
    InitialSet(Competition,3,0,MOSQUITO,0.01,SPIDER1,0.1,SPIDER2,0.1); 
    Boundaries2(Competition);
}

void NextState(int row,int col)
{

	int rand_neigh = RandomMoore8(Competition,row,col);
	int self = Competition[row][col].val;

	if(self == EMPTY) {
		if(rand_neigh == MOSQUITO) {
			if(genrand_real1() < birth_rate_mosquito) {
				Competition[row][col].val = MOSQUITO;
			}
		}  
	}

	if(self == MOSQUITO) {
		
		double cons_rate;
		if(rand_neigh == SPIDER1 || rand_neigh == SPIDER2 ) {
			if (rand_neigh == SPIDER1) {
				cons_rate = consumption1;
			} else {
				cons_rate = consumption2;
			}
			if(genrand_real1() < cons_rate) {
				Competition[row][col].val = rand_neigh;
			}
		}
	}

	if(self == SPIDER1 || self == SPIDER2 )	{
		double death_rate;
		if (self == SPIDER1) {
			death_rate = death1; 
		} else { 
			death_rate = death2;
		}
		if(genrand_real1() < death_rate) {
			Competition[row][col].val = 0;
		}
	}

	//Kill all the red
	// self = Competition[row][col].val;
	// if (Time == 1000 && self==SPIDER1) {
		// Competition[row][col].val = 0;
	// } 
	
}	


void Update(void)
{ 
	// Na 10000 stappen verhogen we de consumptie iedere twee stappen met 0.01 tot een maximum van 0.55
	// if (Time > 9999 && Time%2000 == 0 && consumption1 < 0.56 ) {
		// consumption1 += 0.01; 
		// consumption2 += 0.01; 
		// printf("\n Consumption rate = %f", consumption1);	
	// }

  Display(Competition);
  //Asynchronous();//1,Competition);
  Synchronous(1,Competition);
	// SpaceTimePlot(SpaceTimePlane,Competition);
 
  Plot(1,Competition);
  //if(Time%10==0) {MDiffusion(Competition);}
	//PerfectMix(Competition);
  // Transfer the state of Vote to the SpaceTimePlane  
  // Display(Competition,SpaceTimePlane);
	//while( Mouse()<=0) {}; // you can run the program continuously by commenting out this statement.
}
