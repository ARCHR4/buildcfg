
/*
*	BASE.C / BASE.H
*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "inc\base.h"

//
//	Speichert Datei-Handler zwischen den Funktionen.
//

static PStatus* base;

//
//	Initialisiert den Datei-Handler.
//	true/false bei Erfolg/Fehler.
//

bool BaseInit(const char* projPfd )
{
	char* tmpCpy;
	
	printf(" \nIn BaseInit:\n\n ");
	
	if( !projPfd ) return false;
	
	base = malloc( sizeof( PStatus ) );
	tmpCpy = malloc( strlen(projPfd) + strlen("\\build.bcfg") + 5 );
	
	if( (!base) || (!tmpCpy) ) return false;
	
	// Build-Datei sollte unter projPfad\build.cfg gespeichert sein.
	
	strcpy( tmpCpy, projPfd );	//Pfad zu Datei erstellen.
	strcat(tmpCpy, "\\build.bcfg" );
	
	printf("\tPfad: %s\n\n", tmpCpy);
	
	base->datei = fopen( tmpCpy, "r"); 
	
	if( !(base->datei) ){
		printf("Datei konnte icht geoeffnet werden!");
		return false;
	}else{
		printf("\tDatei erfolgreich geoeffnet, Cur(): %c\n\n", Cur() );
	}
	
	base->dir   = projPfd;
	base->pfad  = tmpCpy;
	
	
	return true;
	
}


//
// Schließt Datei-Handle.
//

void BaseClose()
{
	fclose( base->datei );
	free( (void*) base );
	base = nil;
}

//
// Ermöglicht anderen Funktionen, auf das Handle zuzugreifen.
//

const PStatus* const Status()
{
	return (const PStatus* const) base;
}
	
	
//
// Cur():
// Gibt das aktuelle Zeichen zurück. 
// Verändert die Position in der Datei nicht.
//	

int Cur()
{
	int ret = fgetc( base->datei );
	if( ret == EOF ) return EOF;	//Beim Auftreten von Eof müssen alle Dateioperationen geblockt werden.
	//wieder zurück auf ursprüngliche position
	fseek( base->datei, -1, SEEK_CUR);
	return ret;
}

//
// Next():
// Gibt nächstes Zeichen zurück.
// Verändert die Position in der Datei nicht.
//

int Next() 
{
	int ret; 
	fseek( base->datei, 1, SEEK_CUR );
	ret = Cur();
	if( ret == EOF ) return EOF;
	fseek( base->datei, -1, SEEK_CUR );
	return ret;
}

//
// Gibt das Lezte Zeichen zurück.
// Verändert die Position in der Datei nicht.
//

int Last()
{
	int ret;
	
	fseek( base->datei, -1, SEEK_CUR );
	ret = Cur();
	if( ret == EOF ) return EOF;
	fseek( base->datei, 1, SEEK_CUR );
	return ret;
}


//
//	veschiebt den Datei-Zeiger auf nächste Zeichen und gibt es zurück.
//

int Fwd()
{
	int ret;
	fseek( base->datei, 1, SEEK_CUR );
	return Cur();
}

//
// Verschiebt  den Datei-Zeiger auf letztes Zeichen und gibt es zurück.
//

int Rwd() 
{
	int ret;
	fseek( base->datei, -1, SEEK_CUR );
	return Cur();
}

//
// Prüft das aktuelle Zeichen auf Dateiende (EOF)
// Wenn EOF gesetzt ist, wird die Position in der Datei nicht zurückgesetzt.
// 

bool Eof()
{
	if( feof(base->datei) || (Cur() == EOF) )
		return true;
	else{
		fseek( base->datei, -1, SEEK_CUR);
		return false;
	}
}

// 
// Überspringt alle Leerzeichen, Kommentare und nicht-druckbare Zeichen.
//

int Skip()
{
	printf("\nSkip()\n");
	while( (!feof(base->datei)) && (Cur() != EOF) ){
		switch( Cur() ){
		
		//Nicht-druckbare Zeichen
		printf("Skip: Loop: Nichts Druckbares!\n");
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
			
		case '/':	//Kommentar (//)
			printf("Skip Loop: Kommentar!\n");
			if( Next() == '/')	//Kommentar oder doch nur ein einzelner Backslash?
				while( (!feof(base->datei) && (Cur() != EOF)) && (Cur() != '\n') ) Fwd(); //Zur Nächsten Zeile gehen.
			else
				return 0;
			break;
		
		case EOF:
			printf("Skip Loop: EOF");
			return 1;
		
		default:
			printf("Skip() beendet, gefundenes Zeichen %c\n", Cur());
			return 0; 
		}
	}
	printf("Skip beendet, EOF!");
	return 1; 
}

//
// Gibt die Anzahl Zeilen bis zur aktuellen Position zurück.
//

int GetLineCount()
{
	int ret = 0;
	long int curP = ftell( base->datei );

	
	// Alle \n vom Dateinfang bis zur aktuellen Position zählen.
	
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
// Ermittelt die Position in der Aktuellen Zeile.
//

int GetPosInLine( )
{
	int i = 0, curPos = ftell( base->datei );
	for(i; Cur() != '\n'; i--)
		Rwd();
	fseek(base->datei, curPos, SEEK_SET);
	return i*-1;
}
	

void BaseUnitTest() 
{
	bool baseOk = false;
	
    SetConsoleTitle("Unit-Test: base.c/ base.h");
	
	printf("\n\t\t\tTEST BEGINN");
	
	baseOk = BaseInit("G:\\Coding\\buildcfg\\unittest\\base");
	
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


