#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char string[] = "[['D','E','F'], 10 , [ 'A' , 'B' , 'C' ] ,   1033 ]";

int main(void)
{
	int i, j, index, index_char, end, index_word;
	char a, bracket;
	char string_final[100][10];
	
	printf("\nString : %s\n", string);	
	printf("Size : %d\n\n", strlen(string));
	
	index = 0;
	index_char = 0;
	index_word = 0;
	bracket = 0;
	end = 0;

	for( i=0; i<strlen(string); i++ )
	{
		if( end ) break;
		
		a = string[index];
		index ++;
		
		switch (a)
		{
			case '['	:	switch (bracket)
							{
								case 0:		bracket = 1;
											break;
								case 1:		bracket = 2;
											break;	
								default:	break;
							}
							break;
						
			case ']'	:	switch (bracket)
							{
								case 2:		bracket = 1;
											break;
								case 1:		bracket = 0;
											end = 1;
											break;	
								default:	break;
							}
							break;
						
			case ' '	:	break;
			
			case '\''	:	break;
			
			case ','	:	if( bracket == 1 )
							{
								string_final[index_char][index_word] = '\0';
								index_word ++;
								index_char = 0;
							}
							break;
							
			default		:	if( bracket > 0 )
							{
								string_final[index_char][index_word] = a;
								index_char ++;
							}
							break;
		}
	}
	
	string_final[index_char][index_word] = '\0';
	
	for( i=0; i<index_word+1; i++ )
	{
		j = 0;
		
		while( string_final[j][i] != '\0' )
		{
			printf("%c", string_final[j][i]);
			j++;
		}
		
		printf("\n");
	}
	
	return 0;
}