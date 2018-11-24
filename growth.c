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

void MeanFealdApproximation(int row, int col);

static TYPE2** Life;
static TYPE2** SpaceTimePlane;

static int NumberBefore;
static int NumberAfter;

void Initial(void)
{
  MaxTime = 2147483647; /* default=2147483647 */
  nrow = 200; /* # of row (default=100)*/
  ncol = 200; /* # of column (default=100)*/
  nplane = 2; /* # of planes (default=0)*/
  scale = 2; /* size of the window (default=2)*/
  boundary = WRAP; /* the type of boundary: FIXED, WRAP, ECHO (default=WRAP). Note that
		      Margolus diffusion is not supported for ECHO. */
  ulseedG = 56; /* random seed (default=56)*/

  /* useally, one does not have to change the followings */
  /* the value of boundary (default=(TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.})*/
  boundaryvalue2 = (TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.};
}

void InitialPlane(void)
{
  MakePlane(&Life, &SpaceTimePlane);

  /* InitialSet(1,2,3,4,5)
    1: name of plane
    2: number of the state other than the background state
    3: background state or empty state
    4: state I want to put
    5: fraction of cells that get S1 state (0 to 1)
  */
  //InitialSet(Life,1,0,1,0.3);
	InitialSpot(Life,1,10,0,0);

	NumberBefore = countGlobal(Life, 1);
	NumberAfter = NumberBefore;
	
  //ReadSavedData("glidergun.sav",1,Life);
  Boundaries2(Life);
}

void NextState(int row,int col)
{
  //wat we hier willen is de NSF nadoen
  //  dN <- r*N*(1 - N) - N  
	// hoe vertalen we dat naar een CA?
  	
	// did you define density dependence explicitly? 
	// is there a term that defines density dependence? you need K neighbors te grow

	//death
	double rand1 = genrand_real1();
	double death_ratio = 0.05;
	
	if (rand1 < death_ratio) {
		Life[row][col].val = 0;
	}
	
	//birth
	double rand = genrand_real1();
	double birth_ratio = 0.10;
	
	int sum = CountMoore9(Life, 1, row, col);
	
	if (sum >= 3 ) {
		if (rand < birth_ratio) {
			Life[row][col].val = 1;
		}
	}
	//MeanFealdApproximation(row, col);
  
}

void MeanFealdApproximation(int row, int col) {
	int retVal;

	double rand = genrand_real1();
	double birth_ratio = 0.05;
	int sum;

	//Let's do a mean field approximation
	sum = NumberAfter;

	double capacity = nrow*ncol;
	double coverage = sum/capacity; //(coverage between 0 and 1)
	double open_space = 1-coverage;
	
	birth_ratio = birth_ratio * open_space;
	
	//birth 
	if (rand < birth_ratio) {
		retVal = 1;
	} else {
		retVal = 0;
	}
	Life[row][col].val = retVal;
}

void Update(void)
{

	Synchronous(1,Life);

	//-----------------
	double number_rate;
	double absolute_growth;
	if (Time == 1) {
		number_rate = 0.0;
		absolute_growth = 0;
		
	}
	if (Time % 100 == 0) {
		NumberAfter = countGlobal(Life, 1);
	
		absolute_growth = NumberAfter-NumberBefore;

		number_rate= absolute_growth/NumberBefore;
	
		NumberBefore = NumberAfter;
	}
	PlotXY(Time, number_rate);
	//-----------------
	
	SpaceTimePlot(SpaceTimePlane, Life);

  //Display(Life);
  Display(Life, SpaceTimePlane);

	//Plot(1, Life);
	
	//Asynchronous();

  //while( Mouse()<=0) {}; // you can run the program continuously by commenting out this statement.
}