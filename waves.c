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
static int SizeOfHyperCycle = 9;

void Initial(void)
{
  MaxTime = 2147483647; /* default=2147483647 */
  nrow = 200; /* # of row (default=100)*/
  ncol = 200; /* # of column (default=100)*/
  nplane = 1; /* # of planes (default=0)*/
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
	//InitialSet(Life,2,0,1,0.3,2,0.3);
	
  //s are the species in the hypercycle
	for (int s = 1; s <= SizeOfHyperCycle; s++) {

		int nr_col = rand() % nrow + 1;
		int nr_row = rand() % ncol + 1;

		int xpos = nr_row;
		int ypos = nr_col;
		
		int size = 30;
		for (int i=xpos;i<xpos + size;i++) {
			for (int j=ypos;j<ypos + size;j++) {
				if (i <= nrow && j <= ncol) {
					Life[i][j].val = s;
				}
			}
		}
	}

	
	NumberBefore = countGlobal(Life, 1);
	NumberAfter = NumberBefore;
	
  Boundaries2(Life);
}

void NextState(int row,int col)
{
	int nb_state;
	int own_state;
	int rce_state;
	
	double rand1;
	double rand2;
	double death_ratio;
	double birth_ratio;

	own_state = Life[row][col].val;

	nb_state = RandomMoore8(Life, row, col);
	
	//non-trivial co-existence

	//Master sequence are 1; mutants are 2;

	double a = 0.1;	// normal growth rate
	double b = 0.5; 	// growth rate by replicase
	double d = 0.05;	// decay rate

	//Birth
	if (own_state == 0) {

		//fprintf(stderr,"NextState(): nb_state  %5d rce_state   %5d", nb_state, rce_state);
	
	
		//replication by replicase
		_Bool replicator = (nb_state > 0 );
		
		rce_state = RandomMoore8(Life, row, col);

		int difference;
		if (rce_state > 0) {
			difference = (nb_state - rce_state);
		} else {
			difference = 0;
		}
	
		int possibleOutcome = -(SizeOfHyperCycle - 1);
		
		_Bool sit1 = (difference == 1);
		_Bool sit2 = (difference == possibleOutcome);
		
		_Bool replicase = sit1 || sit2;
		
		if (replicator) {
			if (replicase) {
				rand1 = genrand_real1();
				birth_ratio = b;
				if (rand1 < birth_ratio) {
					
					Life[row][col].val = nb_state;
				
				} 
			} else {
				//autonomous replication
				birth_ratio = a;
				rand2 = genrand_real1();
				if (rand2 < birth_ratio) {
					Life[row][col].val = nb_state;
				}
			}
		}
	}

	//Decay
	if (own_state > 0) {
	
		rand1 = genrand_real1();
		death_ratio = d;
		if (rand1 < death_ratio) {
				Life[row][col].val = 0;
		}
	}
}

void Update(void)
{
	// Poor man's graph
	// if (Time != 0 && Time % 200 == 0) {
		
		// int PercX = 100 * countGlobal(Life, 1)/(nrow*ncol);
		// int PercY = 100 * countGlobal(Life, 2)/(nrow*ncol);

		// fprintf(stderr,"Update(): Run  %5d times and now X: %3d %% and Y: %3d %% \n", Time, PercX, PercY);

	// }

	Synchronous(1,Life);

	// int possibleOutcome = -(SizeOfHyperCycle - 1);

	// int difference = 1;
	
	// _Bool sit1 = (difference == 1);
	// _Bool sit2 = (difference == possibleOutcome);
	
	// _Bool combi = sit1 || sit2;
	// fprintf(stderr,"Update(): sit1: %d and sit2: %d combi: %d \n", sit1, sit2, combi);
	
	
	//-----------------
	/*
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
	*/
	//-----------------
	
	//SpaceTimePlot(SpaceTimePlane, Life);

  Display(Life);
  //Display(Life, SpaceTimePlane);

	//PerfectMix(Life);
	Plot(1, Life);
	
	if (Time % 100 == 0) {
		MDiffusion(Life);
	}
	
	//Asynchronous();

  //while( Mouse()<=0) {}; // you can run the program continuously by commenting out this statement.
}