
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

#include "G:\Coding\buildcfg\src\inc\base.h"


int main( )
{
	bool initOk = false;
	SetConsoleTitle("Unit-Test: base.c/GetLineCount()");
	printf("\t\t\t[START]\n\n");
	initOk = BaseInit( "G:\\Coding\\buildcfg\\unittest\\GetLine");
	if(!initOk){
		system("pause");
		return 0;
	}
	
	Skip(); //Bis zum Zeichen '!' vorspringen (Zeile 4)
	
	if( GetLineCount() == 4 ){
		printf("\n\nTest erfolgreich!\n\n");
	}else{
		printf("\n\nTest fehlgeschlagen!\n\n");
	}
	system("pause");
	return 0;
}

	
	