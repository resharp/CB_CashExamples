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

static TYPE2** Free;
static TYPE2** Fixed;
static TYPE2** Combi;

static int NrOfColors;

static int NumberBefore;
static int NumberAfter;

void Initial(void)
{
  NrOfColors = 1;
	
	MaxTime = 2147483647; /* default=2147483647 */
  nrow = 200; /* # of row (default=100)*/
  ncol = 200; /* # of column (default=100)*/
  nplane = 3; /* # of planes (default=0)*/
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
  MakePlane(&Free,&Fixed, &Combi);

  /* InitialSet(1,2,3,4,5)
    1: name of plane
    2: number of the state other than the background state
    3: background state or empty state
    4: state I want to put
    5: fraction of cells that get S1 state (0 to 1)
  */
  InitialSet(Free,1,0,1,0.1);
  InitialSet(Fixed,0,0);
	InitialSet(Combi,0,0);

  //Fixed are the species
	for (int i = 1; i <= NrOfColors; i=i+1) {
		int nr_col = rand() % nrow + 1;
		int nr_row = rand() % ncol + 1;
		Fixed[nr_row][nr_col].val = i;
	}
	
  Boundaries2(Free);
  Boundaries2(Fixed);
	Boundaries2(Combi);
	
	NumberBefore = countGlobal(Fixed, 1);
	NumberAfter = NumberBefore;
	
}

void NextState(int row,int col)
{
  if(Fixed[row][col].val == 0){

		int color = rand() % NrOfColors + 1;

		if(Free[row][col].val == 1 && CountMoore8(Fixed,color,row,col) > 0){
				Free[row][col].val = 0;
				Fixed[row][col].val = color;
		}
	}
	
	Combi[row][col].val = Fixed[row][col].val;

	if (Combi[row][col].val == 0) {
		Combi[row][col].val = Free[row][col].val;
	}
	
}

void Update(void)
{
	
	//-----------------
	/*
	double number_rate;
	double absolute_growth;
	number_rate = 0;
	absolute_growth = 0;

	if (Time % 100 == 0 ) {
		NumberAfter = countGlobal(Fixed, 1);
	
		absolute_growth = NumberAfter-NumberBefore;

		number_rate= absolute_growth/NumberBefore;
	
		NumberBefore = NumberAfter;
	}
	if (number_rate < 1) {
		PlotXY(Time, number_rate);
	}
	//-----------------
	*/
	
	Display(Free, Fixed, Combi);
  Synchronous(2,Free,Fixed,Combi);
	MDiffusion(Free);

	//now, how to show the per capita growth function?
}
