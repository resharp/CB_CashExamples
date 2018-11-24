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

static TYPE2** Strains;
void Initial(void)
{
  MaxTime = 2147483647; /* default=2147483647 */
  nrow = 400; /* # of row (default=100)*/
  ncol = 400; /* # of column (default=100)*/
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
  MakePlane(&Strains);

  /* InitialSet(1,2,3,4,5)
    1: name of plane
    2: number o2f the state other than the background state
    3: background state or empty state
    4: state I want to put
    5: fraction of cells that get S1 state (0 to 1)
  */
  
  ReadSavedData("multistrains.sav",1,Strains); 
  
  Boundaries2(Strains);
}

void NextState(int row,int col)
{
   /* Left empty for exercise: write your own next-state function here */


	//death
	double rand1 = genrand_real1();
	double death_ratio = 0.065;
	
	if (rand1 < death_ratio) {
		Strains[row][col].val = 0;
	}

  //where is the color in multistrains.sav is it in value?
	int color;
	
	//random color between 1 and 8
	color = rand() % 8 + 1;

	double rand = genrand_real1();
	double birth_ratio = 8 * 0.45;

	//only have birth in empty space:
	if (Strains[row][col].val == 0) { 
		int sum = CountMoore9(Strains, color, row, col);
		
		if (sum >= 3 ) {
			if (rand < birth_ratio) {
				Strains[row][col].val = color;
			}
		}
	}
}

void Update(void)
{
  Display(Strains);
  
  
  Synchronous(1,Strains);
 // Asynchronous();
}
