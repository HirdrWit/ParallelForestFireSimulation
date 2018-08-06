#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include "math.h"
#include <locale.h>
#include <wchar.h>
#include <omp.h>

double randomNum(int min, int max);                                    //returns a random number from min to max
double **allocmat(int n);                                              //allocates our forest matrix of size n
double **assignmat(double **mat, int n);                               //assigns values to our forest matrix of size n
void printmat( double** mat, int n);                                   //prints our forest matrix of size n
double** startFire( double** mat, int n);                              //picks a random location in our forest as the starting point for our fire
void run(double** mat, int n);                                         //runs simulation 
int getWind();                                                         //returns a random number from -1 to 1 for wind direction
double modifiedByWind(double val, int windX, int windY, int x, int y); //modifies our fire values to make it strong or weaker base on wind direction

/***allocates our forest matrix of size n***/
double** allocmat(int n){
  double ** mat = malloc (n*sizeof(*mat));
    for(int i=0;i<n;i++){
        mat[i] = malloc(n*sizeof(*mat[i]));
    }
    return mat;
}

/***assigns values to our forest matrix of size n***/
double** assignmat( double **mat, int n){
    srand(time(NULL));                                  //To ensure randomness 
    for(int i = 0; i < n; i++){                         //Iterate over entire forest matrix
        for(int j = 0; j<n;j++){
            
            int Num = randomNum(0,10);                  //Percentage of occuring
            int val = 0;
            if(Num>=0 && Num<1) val = -2;               //Mountain (10%)
            if(Num>=1 && Num<6) val = 0;                //Grass    (50%)
            if(Num>=6 && Num<7) val = -3;               //Water    (10%)
            if(Num>=7 && Num<10) val = -1;              //Tree     (30%)
            mat[i][j] = val;
        }
     }
     return mat;
}

/*** prints our forest matrix of size n ***/
void printmat( double** mat, int n){
    
    for(int i = 1; i< n; i++){
         for(int j = 1; j<n;j++){
            setlocale(LC_ALL, "");
            int Num = mat[i][j];
            wchar_t ch;
            if(Num == -2) ch = 0x26F0;                  //Mountain ⛰ 
            if(Num == 0) ch = 0x1F33F;                  //Grass 🌿
            if(Num == -3) ch = 0x1F30A;                 //Water 🌊
            if(Num == -1) ch = 0x1F332;                 //Tree 🌲
            if(Num >0 && Num<=3) ch = 0x23F9;           //Ash  ⏹ /* When fire becomes ash, Num is decreased, prints fire and ash in range values */   
            if(Num>3 && Num<=8) ch = 0x1F525;           //Fire 🔥                                          
            if(Num == -2)                               /* Prints different spacing to ensure propper alignment */                     
             printf("%lc  ", ch);
            else if(Num >0 && Num<=3)
             printf("%lc  ", ch);
            else
             printf("%lc ", ch);
         }
         printf("\n");
     }
}

/*** picks a random location in our forest as the starting point for our fire ***/
double** startFire( double** mat, int n){
    srand(time(NULL));
    int fireStart = 1;
    while(fireStart){                                   //While fire has not started
        int x = randomNum(1,n-1);                       //Pick x and y coordinates
        int y = randomNum(1,n-1);
        if(mat[x][y] == 0){                             //If these coordinates equal grass, start the fire
            fireStart = 0;
            mat[x][y] = 8;
        }
    }
    return mat;
}

/*** returns a random number from -1 to 1 for wind direction ***/
int getWind(){
    srand(time(NULL));
    return randomNum(-1,1);
}

/*** modifies our fire values to make it strong or weaker base on wind direction ***/
double modifiedByWind(double val, int windX, int windY, int x, int y){
    if(val <= 3)                                    //not fire 
        return val;
    else if(windX == x && windY == y){              //wind is blowing from pos
        return val + 2;
    }
    else if((windX*-1) == x && (windY*-1) == y){    //wind is blowing frop opposite position
        return val - 1;
    }
    else{
        return val;                                 //wind is not blowing from pos
    }
}

/*** runs simulation ***/
void run (double** mat, int n){

    double ** buffer = allocmat(n);                                                         //Allocating a buffer matrix
    for(int i = 0; i < n; i++){                                                             //Will store modified values
        for (int j = 0; j<n;j++){
            buffer[i][j] = mat[i][j];
        }
    }

    int game_count = 0;                                                                    //Game count 0 - 15 iterations
    int game_iteration = 15;
    int windX = getWind();                                                                 //Where is wind coming from (x and y)
    int windY = getWind();

    while(game_count < game_iteration)
    {
        double start = omp_get_wtime();
		#pragma omp parallel
		{
		printf("Threads: %d\n",omp_get_num_threads());
		#pragma omp for schedule(dynamic)
        for(int i = 1; i < n-1; i++){                                                       //Looks at matrix values (Y axis)
            for (int j = 1; j<n-1;j++){                                                     //Looks at matrix values (X axis)
                double count = 0;
                double myVal = mat[i][j];                                                   //Stores current position value
                if(myVal > -2 && myVal < 1){                                                //If my value can burn (grass or tree)
                    for(int x = -1; x <=1 ;x++){                                            //Get y value for neighbor
                        for(int y = -1; y <=1;y++){                                         //Get x value for neighbor
                            if(i+x >= 0 && j+y>=0 && i+x <= n && j+y<=n){                   //Makes sure neighbors are in range
                                double toAdd = mat[i+x][j+y];                               //Creates a temporary value of current position value 
                                toAdd = modifiedByWind(toAdd,windX, windY,x, y);                //to either be modified or not by wind
                                count = count+toAdd;                                        //Increment or decrement count based on value of toAdd
                            }
                        }
                    }
                    count = count + myVal;                                                  //Add the value itself
                    if(count > 0){                                                          //Count is greater than zero, there is a fire
                        buffer[i][j] = 8;
                    }
                    else{                                                                   //Count is zero or less, no fire, keeps original value
                        buffer[i][j] = myVal;
                    }
                    
                }
                
                else{                                                                       //Failed first if can my value can burn (not grass or trees)
                    buffer[i][j] = myVal;                                                       //Value does not change                                            
                }
                if(buffer[i][j]>0) buffer[i][j] = buffer[i][j]-1;                           //Decrements fire value by 1 (Fire --> Ash)

            }
        }
		}
		
        game_count++;

        for(int i = 0; i < n; i++){                 //Updates mat to the values of buffer
            for (int j = 0; j<n;j++){
                mat[i][j] = buffer[i][j];
            }
        }
        
        printf("Count: %d\n",game_count);
        printmat(mat,n);
    } 
	double end = omp_get_wtime();
	printf("Time: %f\n",end-start);
}

int main(void)
{
    int n = 25;                                     //Matrix size
    
    double ** mat = allocmat(n);                    //Allocates matrix by size*size
    mat = assignmat(mat,n);                         //Randomly assigns matrix values
    mat = startFire(mat,n);                         //Randomly start a fire in a matrix position
    
    
    run(mat, n);                                    //Runs sim (Iteration count is in this method)

}
/*** returns a random number from min to max ***/
double randomNum(int min, int max){
    int M=min;
    int N=max;
    return (N-M)*(rand()/(double)RAND_MAX)+M;
}
