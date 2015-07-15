
/*
*	BASE.C / BASE.H
*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "inc/base.h"

//
//	Speichert Status des Moduls
//

static PStatus* base;

/*
* Interface zum Kommunizieren mit dem Modul
*/

//
//	BaseInit (projektVerzeichnis : string ) -> bool erfolgreich
//	Initalisiert modul
//

bool BaseInit(const char* projPfd )
{
	char* tmpCpy;
	
	printf(" \nIn BaseInit:\n\n ");
	
	if( !projPfd ) return false;
	
	base = malloc( sizeof( PStatus ) );
	tmpCpy = malloc( strlen(projPfd) + strlen("\\build.bcfg") + 5 );
	
	if( (!base) || (!tmpCpy) ) return false;
	
	// Build-Datei sollte unter projPfad\build.cfg gespeichert sein
	
	strcpy( tmpCpy, projPfd );	//Pfad zu Datei erstellen
	strcat(tmpCpy, "\\build.bcfg" );
	
	printf("\tPfad: %s\n\n", tmpCpy);
	
	base->datei = fopen( tmpCpy, "r"); 
	
	if( !(base->datei) ) 
		return false;
	else
		printf("\tDatei erfolgreich geoeffnet, Cur(): %c\n\n", Cur() );
	
	
	base->dir   = projPfd;
	base->pfad  = tmpCpy;
	
	
	return true;
	
}


//
// Schließt Modul
//

void BaseClose()
{
	free( (void*) base->pfad );
	fclose( base->datei );
	free( (void*) base );
	base = nil;
}

//
// Fragt Status der Datei abort
//

const PStatus* const Status()
{
	return (const PStatus* const) base;
}

/*
*  Dateioperationen Vor, Zurück, usw... 
*  Cur(), Next(), Last() verändern dabei die Position in d. datei nicht,
*  Fwd() und Rwd() schon
*/

	
	
//
// Cur() -> int 
// Gibt das aktuelle Zeichen zurück
//	

int Cur()
{
	int ret = fgetc( base->datei );
	//wieder zurück auf ursprüngliche position
	fseek( base->datei, -1, SEEK_CUR);
	return ret;
}

//
// Next() -> int 
// Gibt nächstes Zeichen zurück
//

int Next() 
{
	int ret; 
	fseek( base->datei, 1, SEEK_CUR );
	ret = Cur();
	fseek( base->datei, -1, SEEK_CUR );
	return ret;
}

//
// Last() -> int
// Was macht es wohl?
//

int Last()
{
	int ret;
	
	fseek( base->datei, -1, SEEK_CUR );
	ret = Cur();
	fseek( base->datei, 1, SEEK_CUR );
	return ret;
}


//
//  Fwd() -> int
//	veschiebt Zeiger auf nächste Zeichen und gibt es zurück
//

int Fwd()
{
	fseek( base->datei, 1, SEEK_CUR );
	return Cur();
}

//
// Rwd() -> int
// verschiebt Zeger auf letztes Zeichen und gibt es zurück
//

int Rwd() 
{
	fseek( base->datei, -1, SEEK_CUR );
	return Cur();
}

// 
// Überspringt alle Leerzeichen, Kommentare und nicht druckbare zeichen
//

int Skip()
{
	printf("\nSkip()\n");
	while( !feof( base->datei ) ){
		switch( Cur() ){
		//Nicht Druckbare zeichen ignorieren
		case '\t':
		case '\a': 
		case '\b':
		case '\f':
		case '\v':
		case '\r':
		case '\n':
		case ' ':
			Fwd();
			break;
			
		case '/':	//Kommentar
			if( Fwd() == '/')	//Kommentar oder doch nur ein / ?
				while( Cur() != '\n' ) Fwd();
			break;
		
		default:
			printf("Skip() beendet, gefundenes Zeichen %c\n", Cur());
			return 0; 
		}
	}
	return 1; 
}

//
// Gibt die Anzahl Zeilen bis zur aktuellen Position zurück
//

int GetLineCount()
{
	int ret = 0;
	long int curP = ftell( base->datei );

	
	// Alle \n vom Dateinfan´g bis zur aktuellen position zählen
	
	rewind( base->datei ); 
	
	for( int i = 0; i < curP; i++){
		if( Cur() == '\n')
			ret++;
		
		Fwd();
	}
	
	//Dateizeiger wieder zur usrprünglichen positin
	fseek( base->datei, curP, SEEK_SET );
	return ret;
}
	


// 
// Testet Modul
//

void BaseUnitTest() 
{
	bool baseOk = false;
	
    SetConsoleTitle("Unit-Test: base.c/ base.h");
	
	printf("\n\t\t\tTEST BEGINN");
	
	baseOk = BaseInit("G:\\Code\\build\\unittest\\base");
	
	if(!baseOk){
		printf("\aDatei konnte nicht geoeffnnet werden\n\n");
		return;
	}
	
	printf("Test der Zeichen-Funktonen:\n\n");
	printf("\t Erstes Zeichen, mit Cur(): %c, Pos: %ld [ \'a\' erwartet, Pos: 0 (\'a\') ]\n", Cur(), ftell( base->datei ) );
	printf("\t Zweites Zeichen mit Next(): %c, Pos: %ld[ \'z\' erwartet, Pos: 0 (\'a\') ]\n", Next(), ftell( base->datei ) );
	printf("\t Zweites Zeichen mit Fwd(): %c, Pos: %ld [\'z\' erwartet, Pos: 0 (\'z\') ]\n", Fwd(), ftell( base->datei) );
	printf("\t Erstes Zeichen mit Last(): %c, Pos: %ld [\'a\' erwartet, Pos: 1 (\'z\') ]\n", Last(), ftell( base->datei) );
	printf("\t Erstes Zeichen mit Rwd(): %c, Pos: %ld [\'a\' erwartet, Pos: 1 (\'a\') ]\n", Rwd(), ftell( base->datei ) );
	printf("\t Erstes Zeichen mit Cur(): %c, Pos: %ld [\'a\' erwartet, Pos: 0 (\'a\') ]\n", Cur(), ftell( base->datei ) );

	printf(" Skip(): ");
	printf("\n\tIn Zeile 2 wechseln.\n\n");
	
	while( Cur() != '\n'){
		Fwd();
	}
	Fwd();
	printf("\n\tSchleife beendet!");
	Skip();
	printf("\n\n\t naechtes Zeichen: %c, Zeile  (F erwartet, in Zeile 13 oder so)\n\n", Cur());
	
	printf(" GetLineCount():\n\n");
	printf("\n\n\t\t200 Zeichen weiter...\n\n");
	
	for(int i = 0; i < 200; i++) Fwd();
	printf("\t\tDatei-Position vorher: %ld\n\n", ftell( base->datei ));
	printf("\t\tDas sollte Zeile: %d sein.\n\n", GetLineCount());
	printf("\t\tUnd das die Position nachher: %ld", ftell( base->datei ));
	printf("\nBaseClose():\n\n");
	BaseClose();
	
	if( base ){
		printf("\t\nBaseClose fehlgeschlagen!\n");
		return;
	}else{
		printf("\t\nBaseClose erfolgreich!\n");
	}
	
	printf("\n\n\t\t\tTEST ABGESCHLOSSEN\n\n");
}


