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

static TYPE2** Comp;		// Normal Plane
static TYPE2** ColorDeath;	// Plane that displays evolved deathrates
static TYPE2** ColorMap;	// Plane that displays colorbar
static TYPE2** ST;			// Space time plot

static TYPE2 empty= {0,0,0,0,0,0.,0.,0.,0.,0.}; // Predefine empty cell

double init_birth_rate=0.40;	// Starting birthrate
double init_death_rate=0.05;	// Starting deathrate
double init_fill_grade = 0.01;  //Initial filling of grid per species
double deathrate = 0.01;		// Minimal deathrate
double init_mut_rate= 0.005;		// Chance of mutations
double mut_rate= 0.;		// Chance of mutations
double mut_step= 0.2;		// Size of mutations
double max_fval=1.;		// Upper bound (if applicable) for your evolvable parameter

double birth_rate_mosquito = 0.1;

int TIME_DETERMINISTIC_DEATH = 500;
int TIME_STOP_EVOLUTION = 3000;
int TIME_EXTRA_KILLING = 1000;
int interval_extra_killing = 120;
int TIME_FOR_PAUSE = 10000;

int EMPTY = 0;
int MOSQUITO = 1;
int SPIDER1 = 2;

void Initial(void)
{
  MaxTime = 2147483647; /* default=2147483647 */
  nrow = 300; /* # of row (default=400)*/
  ncol = 300; /* # of column (default=400)*/
  nplane = 4; /* # of planes (default=0)*/
  margin = 5;
  scale = 1; /* size of the window (default=2)*/
  boundary = WRAP; /* the type of boundary: FIXED, WRAP, ECHO (default=WRAP). Note that
		      Margolus diffusion is not supported for ECHO. */
  ulseedG = 56; /* random seed (default=56)*/

  /* useally, one does not have to change the followings */
  /* the value of boundary (default=(TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.})*/
  boundaryvalue2 = (TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.};
	mut_rate = init_mut_rate;
}

void InitialPlane(void)
{
  MakePlane(&Comp,&ColorDeath,&ColorMap,&ST); // Init all planes

  /* InitialSet(1,2,3,4,5)
    1: name of plane
    2: number of the state other than the background state
    3: background state or empty state
    4: state I want to put
    5: fraction of cells that get S1 state (0 to 1)
  */

  Boundaries2(Comp);			// Init boundaries for planes
  Boundaries2(ColorDeath); 		// Init boundaries for planes
  Boundaries2(ColorMap);		// Init boundaries for planes

  int i,j;
  for(i=1;i<=nrow;i++)for(j=1;j<=ncol;j++)
    {
		// PART I: Store everyones birth/deathrates in fval/fval2
    Comp[i][j] = empty;
    Comp[i][j].fval = init_birth_rate;
    Comp[i][j].fval2 = init_death_rate;
		
		int deterministic_age = 1./init_death_rate;
		Comp[i][j].fval3 = deterministic_age;	// deterministic age
		Comp[i][j].fval4 = 0;	// actual age			

		double rand = genrand_real1();
		if(rand < init_fill_grade){ 
      Comp[i][j].val = SPIDER1;
		} else if (rand < 2*init_fill_grade) {
			Comp[i][j].val = MOSQUITO;
		}
		// PART	II: Make colorbar
    if(j>10 && j < 30) {
			ColorMap[i][j].val = 110 - (float)i/(float)nrow*100;	//maps fval on Comp plane to val on Color plane
		}
  }

  // Initialise colorRGB table (for colouring 2nd and 3rd plane)
  int r=0,g=0,b=255;
  double nr=102.;    //nr of ColorRGB's you want to create
  double range=1275.;  // range of coloursteps: 255*5= 1275
  double x = range/nr;
  int y=0,c;

  for(c=0;c<(int)range;c++){
    if(c<255){			//starts blue
      r = r + 1;		//goes magenta
    }else if(c<255*2){
      b = b - 1;		//goes red
    }else if(c<255*3){
      g = g + 1;		//goes yellow
    }else if(c<255*4){
      r = r -1;    		//goes green
    }else if(c<255*5){
      b = b + 1;		//goes cyan
    }

    if(c == (int)(y*x+0.5)){
      ColorRGB(10+y,r,g,b);	//so colour indexes in the gradient start at 10
      y++;
    }
  }

}

void NextState(int row,int col)
{
	TYPE2 nei=empty;
  nei = RandomMooreS8(Comp,row,col);	//this can be a pointer, see RandomMooreP8(Comp,row,col);

	TYPE2 self = Comp[row][col];
	
  if(self.val == EMPTY){

		if(nei.val == MOSQUITO) {
			if(genrand_real1() < birth_rate_mosquito) {
				Comp[row][col].val = MOSQUITO;
			}
		}  
  }

	if (self.val == MOSQUITO) {

		if (nei.val == SPIDER1 ) {
			double cons_rate = nei.fval;

			if(genrand_real1() < cons_rate) {

				Comp[row][col]=nei;				//birth
				Comp[row][col].fval4 = 0.;

				//Mutate death
				if(genrand_real1()<mut_rate) Comp[row][col].fval2 += mut_step*(genrand_real2()-0.5);	//mutations
				if(Comp[row][col].fval2<0.) Comp[row][col].fval2 = (-1)*Comp[row][col].fval2;		//bounds mutations to decent values
				if(Comp[row][col].fval2>1.) Comp[row][col].fval2 = 2. - Comp[row][col].fval2;
				
				Comp[row][col].fval3 = 1./(Comp[row][col].fval2 + deathrate); //Calculate deterministic age from death ratio 
			}
		}		
	}
	
	self = Comp[row][col]; //including the guy that has just been born
  
	if(self.val == SPIDER1 ){
    Comp[row][col].fval4 += 1.; // It's your birthday! (You started out at age 0)
		
		if (Time < TIME_DETERMINISTIC_DEATH) {
			if( genrand_real1() < (Comp[row][col].fval2 + deathrate )) { 
				Comp[row][col]=empty; //death
			}
		} else {
			//deterministic death
			double deterministic_age = Comp[row][col].fval3;
			double current_age = Comp[row][col].fval4;
			
			if (current_age > deterministic_age) {
				Comp[row][col]=empty; //death
			}
		}

		// Kill all high death rates (and do not mutate for a certain time (see above)
		if (Time == TIME_EXTRA_KILLING && Comp[row][col].fval2 > 0.011) {
			Comp[row][col]=empty; //death
		}
	}
}
// This function maps fvals stored in individuals to a colorvalue in the colorRGB table
int GetColorIndexFrom(int val,double fval)
{
  int color;
  double max_fval_we_color;
  if( val==0 ) return 0;
  max_fval_we_color = (max_fval*100. - 10.)/100.; //Remember that colour indexes in the gradient start at 10
  if (fval> max_fval_we_color ) {
		color=100;
	}
  else {
		color= (int)( 100.* fval/max_fval )+10;
	}
  return color;
}

void Update(void)
{
  Display(Comp,ColorDeath,ST,ColorMap);
  

  Synchronous(1,Comp);

	// ************************************************************************************//
	// GRAPHING Below is some code to plot fvals...
		// 1) ...over time for 120 random individuals every 10 timesteps
		// 2) ...as a color in the second and third plane
	// ************************************************************************************//
  int i, j;

	if ((Time > TIME_EXTRA_KILLING) && (Time < TIME_EXTRA_KILLING + interval_extra_killing)) { //stop mutation for a while after extra killing
		mut_rate = 0.0;
	} else {
		mut_rate = init_mut_rate;
	}
	//Shutdown evolution after ... steps
	if (Time == TIME_STOP_EVOLUTION) { init_mut_rate = 0.0; }
	
	if(Time%100==0) {

		int r;
		for(r=1;r<=120;r++)
		{
			i = genrand_int(1,nrow);
			j = genrand_int(1,ncol);
			if(Comp[i][j].val > MOSQUITO)
			{
				//Display death rate:
				PlotXY(Time,deathrate + Comp[i][j].fval2);
				
				//Display deterministic_age:
				//PlotXY(Time,Comp[i][j].fval3);
				
				//Display actual age
				//PlotXY(Time,Comp[i][j].fval4);
			}
		}
	}
	// Setup display of graph (once)
	if(Time == 0)
	{
		GracePrintf("g0 on");
		GracePrintf("with g0");
		GracePrintf("focus g0");
		GracePrintf("s1 on");
		GracePrintf("s1 color 5");
		GracePrintf("s1 line linestyle 0");
		GracePrintf("s1 symbol 1");
		GracePrintf("s1 symbol size 0.2");
	}

	for(i=1;i<=nrow;i++)for(j=1;j<=ncol;j++){
		if (Comp[i][j].val == MOSQUITO) {
			ColorDeath[i][j].val = 0;
		} else {
			ColorDeath[i][j].val = GetColorIndexFrom(Comp[i][j].val, 8 * Comp[i][j].fval2 );	//maps fval on Comp plane to val on Color plane
		}
	}

	//if(Time%20==0) { MDiffusion(Comp); }

	if (Time > 0 && Time % TIME_FOR_PAUSE == 0) {
		while( Mouse()<=0) {}; // you can run the program continuously by commenting out this statement.
	}
}
