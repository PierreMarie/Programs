#include <stdio.h>
#include <stdlib.h>
#include "wiringPi.h"
#include <unistd.h>

// To launch the script, put this instruction in /etc/rc.local: su pi -c '/home/pi/Programmation/Projet_final/test &'

int main (void)
{
	char temp = 0, state = 0, str[10], c;
	int a, i, j;
	FILE* fichier = NULL;

	if (wiringPiSetup () == -1)
	exit (1) ;

	pinMode(1, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(27, INPUT);

	system("killall chromium-browser&");
	system("killall omxplayer.bin&");
	
	delay(10000);
	
	j = 0x01;
	
	for(i=0; i<10; i++)
	{
		digitalWrite(1, j);
		digitalWrite(5, j);
		delay(150);
		j ^= 0x01;
	}

	system("amixer cset numid=1 -- 100%");
	
	fichier = fopen("/home/pi/Programmation/Projet_final/request.txt", "r");
	c=fgetc(fichier); 
	printf("\n\n%c\n\n", c);
	fclose(fichier);
	
	 if( c=='1' )
	{
		fichier = fopen("/home/pi/Programmation/Projet_final/request.txt", "w");
		fputc('0', fichier); 
		fclose(fichier);
		digitalWrite(5, 1);
		system("amixer cset numid=3 2");
		system( "bash /home/pi/Programmation/Projet_final/omx.sh" );
		state = 4;
	} 
	do
	{
		if( digitalRead(27) == 1)
		{
			j = 0x01;
	
			for(i=0; i<6; i++)
			{
				digitalWrite(1, j);
				delay(150);
				j ^= 0x01;
			}
			
			system("killall chromium-browser&");
			system("killall omxplayer.bin&");
			
			delay(500);
			
			if( digitalRead(27) == 1)
			{		
				digitalWrite(5, 0);
				state = 0;
				
				while( digitalRead(27) == 1);	
			}
			else
			{
				if( state >= 4 )
				{
					state = 0;
				}
				else
				{
					state++;
				}
			}
			
			switch(state)
			{
				case 0:		system("killall chromium-browser&");
							system("killall omxplayer.bin&");
				
							digitalWrite(5, 0);
							break;
				
				case 1:		digitalWrite(5, 0);
							delay(100);
							digitalWrite(5, 1);
				
							system("amixer cset numid=3 1");
							system("DISPLAY=:0 chromium-browser http://www.ecouter-en-direct.com/radio-classique/&");
							break;
						
				case 2:		digitalWrite(5, 0);
							delay(100);
							digitalWrite(5, 1);
				
							system("amixer cset numid=3 1");
							system("DISPLAY=:0 chromium-browser http://www.ecouter-en-direct.com/tsf-jazz/&");
							
							break;
							
				case 3:		digitalWrite(5, 0);
							delay(100);
							digitalWrite(5, 1);
				
							system("amixer cset numid=3 1");
							system("DISPLAY=:0 chromium-browser http://www.ecouterradioenligne.com/courtoisie-paris/&");
							
							break;
				
				case 4:		digitalWrite(5, 0);
							delay(100);
							digitalWrite(5, 1);
							
							delay(500);
				
							if( digitalRead(27) == 1)
							{		
								digitalWrite(5, 0);
								state = 0;
								while( digitalRead(27) == 1);	
								
								break;
							}
											
							fichier = fopen("/home/pi/Programmation/Projet_final/request.txt", "w");
							fputc('1', fichier); 
							fclose(fichier);
							system("sudo reboot");
							
							break;
							
				default:	break;
			}			
						
			while( digitalRead(27) == 1);
		}

		delay(100);

	}while(1);

	return 0 ;
}
