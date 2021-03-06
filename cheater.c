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

static TYPE2** Cheater;
static TYPE2** ST;

double X2Y =0.7;
double Y2X =0.7;
double Y2Z =0.7;

const double NON=0.3;
const double d=0.3;

_Bool WellMixed = 0;

double sumx,sumy,sumz,px,py,pz,pz2,psum,ran;


void Initial(void)
{
  MaxTime = 2147483647; /* default=2147483647 */
  nrow = 250; /* # of row (default=100)*/
  ncol = 250; /* # of column (default=100)*/
  nplane = 2; /* # of planes (default=0)*/
  scale = 2; /* size of the window (default=2)*/
  boundary = FIXED; /* the type of boundary: FIXED, WRAP, ECHO (default=WRAP). Note that
		      Margolus diffusion is not supported for ECHO. */
  ulseedG = 56; /* random seed (default=56)*/

  /* useally, one does not have to change the followings */
  /* the value of boundary (default=(TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.})*/
  boundaryvalue2 = (TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.};
}


void InitialPlane(void)
{
  MakePlane(&Cheater,&ST);



	char* readOut;
	for(int i = 0; i < (int)argc_g; i++)
	{
	    readOut = (char*)argv_g[i];
	    if(strcmp(readOut, "-mut1") == 0) X2Y = atof(argv_g[i+1]);
	    if(strcmp(readOut, "-mut2") == 0) Y2X = atof(argv_g[i+1]);
	    if(strcmp(readOut, "-cheat1") == 0) Y2Z = atof(argv_g[i+1]);
	    if(strcmp(readOut, "?") == 0) Y2Z = atof(argv_g[i+1]);
	}

  printf("\n-= Now running cheater.c =-\n\nParameters used: \nMutualist 1:\t%f (white)\nMutualist 2:\t%f (red)\nCheater1:\t%f (blue)\n\n", X2Y, Y2X, Y2Z);


  // Setting some user-defined colours
  ColorRGB(0,0,0,0);
  ColorRGB(1,255,0,0);
  ColorRGB(2,255,255,255);
  ColorRGB(3,0,0,155);
  ColorRGB(4,0,255,0);
  // InitialSet(1,2,3,4,5) with 1) plane name, 2) number of non-empty states, 3) background state (0), 4) state to set 5) fraction that gets state from 4, 6) state to set, 7) fraction of state 6, etc.
  InitialSet(Cheater,3,0,1,0.1,2,0.1,3,0.1);
  Boundaries2(Cheater);
}




void NextState(int row,int col)
{


if (Cheater[row][col].val==0){
  int sumx= CountMoore8(Cheater,1,row,col);
  int sumy= CountMoore8(Cheater,2,row,col);
  int sumz= CountMoore8(Cheater,3,row,col);


	px= (Y2X* sumy)*sumx;
	py= (X2Y* sumx)*sumy;
	pz= (Y2Z* sumy)*sumz;
	psum=px+py+pz+NON;
	ran= genrand_real1();

     if (ran< px/psum){
     Cheater[row][col].val = 1;}
     else if (ran< (px+py)/psum){
     Cheater[row][col].val = 2;}
     else if (ran< (px+py+pz)/psum){
     Cheater[row][col].val = 3;}
}
else{
 if (genrand_real1()<d){
  Cheater[row][col].val=0;
 }

}



}

void Update(void)
{
  // int x;
  // x = countGlobal(Cheater,1);
  Plot(1,Cheater);
  SpaceTimePlot(ST,Cheater);
  Display(Cheater,ST);
  Synchronous(1,Cheater);

  if (Time%10==0){
     MDiffusion(Cheater);
  }

  if (WellMixed) {
		PerfectMix(Cheater);
	}
}
