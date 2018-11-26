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
#include <cash2.h>
#include <cash2003.h>
#include <mersenne.h>
#include <cash2-s.h>

static TYPE2** Vote;                    // CA plane for Vote
static TYPE2** SpaceTimePlane;          // Plane that holds the SpaceTimePlot

void Initial(void)
{
    MaxTime = 2147483647; /* default=2147483647 */
    nrow = 350; /* # of row (default=100)*/
    ncol = 350; /* # of column (default=100)*/
    nplane = 2; /* # of planes (default=0)*/
    scale = 2; /* size of the window (default=2)*/
    boundary = WRAP; /* the type of boundary: FIXED, WRAP, ECHO (default=WRAP). Note that Margolus diffusion is not supported for ECHO. */
    ulseedG = 56; /* random seed (default=56)*/
    /* useally, one does not have to change the followings */
    /* the value of boundary (default=(TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.})*/
    boundaryvalue2 = (TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.};
}

void InitialPlane(void)
{
    MakePlane(&Vote,&SpaceTimePlane);

    /* InitialSet(1,2,3,4,5)
    1: plane name (defined at the top)
    2: number of a non-empty state
    3: empty state (almost always 0)
    4: state I want to set
    5: fraction of cells that get the state from 4
    */
    InitialSet(Vote,1,0,1,0.5); // I.o.w., half the grid starts as 1, the other are EMPTY (0)
    Boundaries2(Vote);
}

void NextState(int row,int col)
{
    int sum;
    
		sum = CountMoore9(Vote,1,row,col);

		double rand = genrand_real1();
		
    if((sum >= 0 && sum <= 3) || sum == 5 ) {

			if (rand > 0.01) {
				Vote[row][col].val = 0;
			} else {
				Vote[row][col].val = 1;			
			}
    }
    else{
			if (rand > 0.011) {
				Vote[row][col].val = 1;
			} else {
				Vote[row][col].val = 0;
			}
    }

    /*
    genrand_real1() returns a floating number between 0 and 1
    (both 0 and 1 inclusive) with a uniform probability
    distribution. For example,

    If you want something to happen e.g. 10% of the time, you can do this:
    if(genrand_real1()<0.1)
    {
        MakeStuffHappen;
    }
    else
    {
        DoSomethingElse; // Automatically 90% of the time, ofc.
    }
*/
}

void Update(void)
{
    Synchronous(1,Vote);
    SpaceTimePlot(SpaceTimePlane,Vote);     // Transfer the state of Vote to the SpaceTimePlane
    Display(Vote,SpaceTimePlane);
    //Asynchronous();           // You can use this to update ASYNCHRONOUSLY.
}
