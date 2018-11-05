#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_ITERATION_MAX 60

int main(void)
{
	int tab[9][9] = {0};
	int i, j, k, l, m, n, var, test, iter, sum, product;
	
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
	
		for( i=0; i<9; i++ )
		{		
			for( j=0; j<9; j++ )
			{
				iter = 0;
				
				do
				{	
					test = 0;
					var = (rand()%9) + 1;
					
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
					
					iter ++;
				
				}while( test && iter <= NB_ITERATION_MAX );
				
				if (iter > NB_ITERATION_MAX)
				{
					j = 9;
					i = 9;
				}
				else	tab[j][i] = var;
			}
		}
	
	}while(iter > NB_ITERATION_MAX);
	
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
