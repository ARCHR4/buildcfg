
/*
*
* p_aux.c / p_aux.h
* Hilfsfunktionen, die auf base aufbauen
*/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "inc\base.h"
#include "inc\p_aux.h" 


//
// Ziemlich selbst erklärent
//

void** _Alloc2DArray( int size, int dim1, int dim2 )
{
	// Erste Dimension allokieren
	void** ret;
	
	ret = calloc( sizeof( void* ), dim1);
	
	if( ! ret ) return nil;
	
	// Zweite Dimension
	for( int i = 0; i < dim2; i++){
		ret[i] = calloc(size, dim2);
		
		if( !ret[i] ){	//Alles wieder befreien
			for( i; i >= 0; i--){
				free( ret[i] );
			}
			free(ret);
			return nil;
		}
	}
	
	return ret;
}



//
//	Error( const char* msg, bool fatal) -> x 
//  Gibt msg aus und beendet das Programm
//

noreturn void Error( const char* msg )
{
		fprintf( stderr, "\n\n\aFatal Error: %s\n\n", msg);
		BaseClose();
		system("pause");
		exit(0);
}


//
// Wie Error, nur auf Syntax Fehler im Script bezogen
//

noreturn void SyntaxError( const char* msg ) 
{
	const PStatus* const st = Status();
	fprintf(stderr, "Syntax Error: %s  (Line %d, Zeichen %d)", msg, GetLineCount(), ftell( st->datei ));
	BaseClose();
	printf("pause");
	system("pause");
	exit(0);
}


noreturn void AuxUnitTest()
{

	int** arr = (int**) Alloc2DArray(int, 2, 2);
	
	SetConsoleTitle( "Unit-Test: P_aux.c / p_aux.h ");
	printf("\t\t\tTEST BEGINN\n\n");
	printf(" Alloc2DArray(): \n");
	
	if( arr ) printf("\n ... erfolgreich!\n\n");
	
	printf(" Den Parser ueber die Test-Datei laufen lassen...\n");
	BaseInit( "G:\\Code\\build\\unittest\\GetLine" );
	Skip();
	
	
	printf(" Ups, ein Synatx Error (müsste in Zeile 4 sein) !\n\n ");
	
	SyntaxError( "Wie konnet das nur passieren?!" );

}




		
	
	
	