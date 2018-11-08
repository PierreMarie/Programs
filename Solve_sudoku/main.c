#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define NB_ITERATION_MAX 60		// Pour le hasard brut
#define NB_ITERATION_MAX 40			// Pour le Back Tracking

int main(void)
{
	/*int tab[9][9] = {	{9,0,0,1,0,0,0,0,5},
						{0,0,5,0,9,0,2,0,1},
						{8,0,0,0,4,0,0,0,0},
						{0,0,0,0,8,0,0,0,0},
						{0,0,0,7,0,0,0,0,0},
						{0,0,0,0,2,6,0,0,9},
						{2,0,0,3,0,0,0,0,6},
						{0,0,0,2,0,0,9,0,0},
						{0,0,1,9,0,4,5,7,0}
					};*/
					
	int tab[9][9];
					
	int i, j, k, l, m, n, var, var2, test, iter, sum, product;
	long int tot=0;
	
	srand(time(NULL));
	printf("\n");
	
	do
	{
		for( i=0; i<9; i++ )
		{		
			for( j=0; j<9; j++ )
			{
				tab[j][i] = 0;
			}
		}
		
		tab[0][0]=9;
		tab[3][0]=1;
		tab[8][0]=5;
		
		tab[2][1]=5;
		tab[4][1]=9;
		tab[6][1]=2;
		tab[8][1]=1;
		
		tab[0][2]=8;
		tab[4][2]=4;
		
		tab[4][3]=8;
		
		tab[3][4]=7;
		
		tab[4][5]=2;
		tab[5][5]=6;
		tab[8][5]=9;
		
		tab[0][6]=2;
		tab[3][6]=3;
		tab[8][6]=6;
		
		tab[3][7]=2;
		tab[6][7]=9;
		
		tab[2][8]=1;
		tab[3][8]=9;
		tab[5][8]=4;
		tab[6][8]=5;
		tab[7][8]=7;

		var = 0;
	
		for( i=0; i<9; i++ )
		{		
			for( j=0; j<9; j++ )
			{
				iter = 0;
				test = 0;
				
				if( tab[j][i] == 0 )
				{	
					do
					{
						tab[0][0]=9;
						tab[3][0]=1;
						tab[8][0]=5;
						
						tab[2][1]=5;
						tab[4][1]=9;
						tab[6][1]=2;
						tab[8][1]=1;
						
						tab[0][2]=8;
						tab[4][2]=4;
						
						tab[4][3]=8;
						
						tab[3][4]=7;
						
						tab[4][5]=2;
						tab[5][5]=6;
						tab[8][5]=9;
						
						tab[0][6]=2;
						tab[3][6]=3;
						tab[8][6]=6;
						
						tab[3][7]=2;
						tab[6][7]=9;
						
						tab[2][8]=1;
						tab[3][8]=9;
						tab[5][8]=4;
						tab[6][8]=5;
						tab[7][8]=7;
						
						//test = 0;
						var = (rand()%9) + 1;
						//printf("\n%d\t%d\t%d",i, j, var);
						
						//******************************************************//	VERIF ROW
						for( k=0; k<9; k++ )
						{
							if( tab[k][i] == var )
							{
								test = 1;
								k=9;
							}
						}
						
						//******************************************************//	VERIF COLUMN
						if( test != 1 )
						{	
							for( k=0; k<9; k++ )
							{
								if( tab[j][k] == var )
								{
									test = 1;
									k=9;
								}
							}
						}
						
						//******************************************************//	VERIF GROUPS
						tab[j][i] = var;
						
						if( test != 1 )						//	2	2
						{									//	2	5
							for( m=2; m<9; m+=3 )			//	2	8
							{								//	5	2
								for( n=2; n<9; n+=3 )		//	5	5
								{							//	5	8
									sum = 0;				//	8	2
									product = 1;			//	8	5
															//	8	8
									if( i==m && j==n )
									{
										for( k=m-2; k<m+1; k++ )
										{
											for( l=n-2; l<n+1; l++ )
											{
												sum += tab[l][k];
												if( sum > 45 ) break;
												product *= tab[l][k];
											}
											
											if( sum > 45 )	k=m+1;
										}
										
										if( (sum != 45) || (product != 362880) )
										{
											test = 1;
											n = 9;
											m = 9;
										}
									}
								}
							}
						}
						
						tab[j][i] = 0;
						
						iter ++;
					
					}while( test && iter <= NB_ITERATION_MAX );
				
					if (iter > NB_ITERATION_MAX)
					{
						//var2 = (i*2)+5;
						var2 = 5;
		
						if( (j-var2)>=0 )
						{
							j = j-var2;
						}
						else
						{
							if( i>0 )
							{
								i--;
								j=8-abs(j-var2);
								
								if( j<0 ) j = 0;
							}
							else
							{
								j = 0;
							}
						}
						
						j = j-1;							// Pour compenser l'incrementation du for
					}
					else
					{
						tab[j][i] = var;
					}
					
					tot++;
				}
			}
		}
	
	}while(iter > NB_ITERATION_MAX);
	
	printf("Nombre d'itérations : %ld ", tot);
	printf("\n\n");
	
	for( i=0; i<9; i++ )
	{		
		for( j=0; j<9; j++ )
		{
			printf("%d\t", tab[j][i]);
		}
		
		printf("\n");
	}
	
	return 0;
}