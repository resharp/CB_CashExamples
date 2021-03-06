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
	
	int xpos = 50;
	int size = 20;
	for (int i=xpos;i<xpos + size;i=i+1) {
		for (int j=xpos;j<xpos + size;j=j+1) {
			Life[i][j].val = 1;
		}
	}
	xpos = 150;
	size = 10;
	for (int i=xpos;i<xpos + size;i=i+1) {
		for (int j=xpos;j<xpos + size;j=j+1) {
			Life[i][j].val = 2;
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
	
	//double rand1;
	double rand2;
	double rand3;
	double death_ratio;
	double birth_ratio;

	own_state = Life[row][col].val;
	nb_state = RandomMoore8(Life, row, col);

	//founder controlled
/*
	double a1 = 0.3;
	double a2 = 0.3;
	double b1 = 0.15; // intra-species : among your own kind
	double b2 = 0.15;
	double c1 = 0.5;	// inter-species: competition with other species
	double c2 = 0.5;
*/	
	
	//non-trivial co-existence

	double a1 = 0.9;
	double a2 = 0.9;
	double b1 = 0.025; // intra-species : among your own kind
	double b2 = 0.2;
	double c1 = 0.05;	// inter-species: competition with other species
	double c2 = 0.001;


	//Birth
	if (own_state == 0) {

		//birth		
		if (nb_state >= 0 ) {
			if (nb_state == 1) {
				birth_ratio = a1; //a1
			}
			if (nb_state == 2) {
				birth_ratio = a2;	//a2
			}
			rand2 = genrand_real1();
			if (rand2 < birth_ratio ) {
				Life[row][col].val = nb_state;
			}
		}
	}
	
	//Death
	/*
	if (own_state > 0) {
	
		rand1 = genrand_real1();
		if (own_state == 1) {
			death_ratio = b1;	
		}
		if (own_state == 2) {
			death_ratio = b2;	
		}
		if (rand1 < death_ratio) {
				Life[row][col].val = 0;
		}
	}
	*/
	
	// (symmetric) death by competition
	if (own_state > 0 && nb_state > 0) {

		rand3 = genrand_real1();
	
		if (own_state == 1) {
			death_ratio = (own_state != nb_state)*c1 + (own_state == nb_state)*b1;	
		} else if (own_state == 2) {
			death_ratio = (own_state != nb_state)*c2 + (own_state == nb_state)*b2;	
		} else {
			fprintf(stderr,"NextState(): wrong value in own_state\n");
			exit(1);
		}

		if (rand3 < death_ratio) {
			Life[row][col].val = 0;
		}

	}
}

void Update(void)
{
	// Poor man's graph
	if (Time != 0 && Time % 200 == 0) {
		
		//fprintf(stderr,"Update(): Run  %5d times and now X: %3d %% and Y: %3d %% \n", Time, PercX, PercY);

		int PercX = 100 * countGlobal(Life, 1)/(nrow*ncol);
		int PercY = 100 * countGlobal(Life, 2)/(nrow*ncol);
		char array[101]; 
		char string[101];
		for (int i = 0;i<100;i++) {
			array[i]='-';
			if (i == PercX) {
				array[i] = 'X';
			} 
			if (i == PercY) {
				array[i] = 'Y';
			}
		}
		array[100] = '\0';
		strcpy(string, array);
		printf("%s\n", string);
	}

	Synchronous(1,Life);

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
	//Plot(1, Life);
	
	//Asynchronous();

  //while( Mouse()<=0) {}; // you can run the program continuously by commenting out this statement.
}