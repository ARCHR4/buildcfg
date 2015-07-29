
/*
*
*	PMAIN.C:
*	Dursucht die Datei nach Variablen-Deklarationen,
*	filtert die Daten heraus und übergibt sie ans 
*	nächste Modul.
*
*/


#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "inc/base.h"
#include "inc/p_aux.h"
#include "inc/pmain.h"

#define TEST

static SkriptVar* SVInit( SkriptToken );

void SVDel( SkriptVar* );

static SkriptToken FindeDkl( );

static void LeseDkl();

static bool NachsterWert();

static void LeseWert(); 

void PrintVarName( SkriptToken );




int main(int argc, char** argv)
{
	SkriptVar* v[6];
	
	if( argc < 1 ){
		Error(" Kein Pfad angegeben!");
	}else{
		bool baseOK = BaseInit( argv[1] );
		if( !baseOK )
			Error("Projekt-Verzeichnis konnte nicht gefunden werden!");
	}
	
	for( int i = 0; i < 6; i++){		//Varialen initalisieren 
		v[i] = SVInit( (SkriptToken ) i ); 
		if( !v[1] )
			Error("Main: Allokieren der Variablen fehlgeschlagen!");
	}
		
	// Datei lesen 
	for( SkriptToken nxtVar = FindeDkl(); nxtVar != GEOF; nxtVar = FindeDkl()){
		if( nxtVar != UNEXPECTED )
			LeseDkl( v[ (int) nxtVar ] );
		else
			SyntaxError("Unerwartetes Symbol!");
	}
	
	#ifdef TEST 
		system("PAUSE");
		printf("\n\nWerte:\n\n");
		for( int i = 0; i < 6; i++){
			PrintVar( v[i] );
			printf("\n");
		}
		printf("\n\n");
		system("pause");
		return 0;
	#else
	#endif

	
}



//
// Erstellt und initialisiert eine neue Variable
//


static SkriptVar* SVInit( SkriptToken welche )
{
	SkriptVar* ret;
	
	ret = malloc( sizeof( SkriptVar ) );
	
	if( !ret ) return nil;
	
	ret->name = welche;
	ret->wert = nil;
	
	if( (welche == LIBARY ) || (welche == OPTIONS ) || (welche == IGNOREDIR) )
		ret->multiWert = true;
	else
		ret->multiWert = false;
		
	ret->gr1Dim = 0;
	return ret;
}


//
// Löscht eine Variable wieder.
//
	
void SVDel( SkriptVar* toDel )
{
	for( int i = 0; i < toDel->gr1Dim; i++)
		free( toDel->wert[i] );
	
	free( toDel);
}

//
// Findet nächste Variablen-Deklaration in der Datei.
//

static SkriptToken FindeDkl()
{
	const PStatus* const st  = Status();
	
	printf("\n\nIn FindeDkl():\n");
	
	if( Cur() == ';' ) //Ende der letzten Dekl.
		Fwd();
		
	Skip(); 
	
	if( feof(st->datei))
		return GEOF;
	
	switch( Cur() ){
	case 'l':
	case 'L':
		return (( Next() == 'i' ) || (Next() == 'I' )) ? LIBARY : LANGUAGE;
		
	case 'o':
	case 'O':
		return (( Next() == 'u') || (Next() == 'U')) ? OUTPUT : OPTIONS;
		
	case 'c':
	case 'C':
		return COMPILER;
	case 'i':
	case 'I':
		return IGNOREDIR;
		
	case EOF:
		return GEOF;
	
	default:
		return UNEXPECTED;
	}
}

//
//  Liest Deklaration, nachdem sie mit FindeDkl() gefunden wurde.
//

static void LeseDkl( SkriptVar* v )
{
	int indx = 0;
	printf("\nLeseDkl():\n");
	if( v->wert != nil ){  //Variable wird redefiniert? 
		printf("Redefinierung.\n");
		Error( "Versuch Variable zu redefinieren!" ); 
	}else if( v->multiWert ){
		v->wert = calloc( sizeof( char* ), 5 );
		v->gr1Dim = 5;
		printf(" Allokierung multiWert");
	}else{
		v->wert = malloc( sizeof( char* ) );
		v->gr1Dim = 1;
		printf(" Allokierung Einzener Wert!");
	}
	
	printf("Speicher allokiert!\n");
	if( !v->wert )
		Error("Speicher-Fehler: Malloc/Calloc gibt nil zurück!");
	
	// Der Zeiger ist auf einem Buchstaben.
	// Zum ':' springen
	while( isalpha( Cur() ) ) Fwd();
	Skip();
	printf("Namen übersprungen!\n\n");
	
	if( Cur() != ':' ){
		SyntaxError( "\':\' nach dem Namen erwartet!");
	}
		
	while( NachsterWert( ) ){
		Fwd();
		Skip();
		printf("Iteration: %d\n", indx);
		printf("Naechster Wert: %c\n", Cur());
		
		// Platz im Array?
		if( indx >= v->gr1Dim ){
			v->wert = realloc( v->wert, v->gr1Dim += 2 );
			if( !v->wert ){
				Error( "Realloc gibt NIL zurück!");
			}
		}
		
		LeseWert( v, indx );
		printf("\nString gespeichert: %s, Aktuelles Zeichen: %c\n", v->wert[indx], Cur());
		indx++;
		
		Skip(); //Zeiger auf ',' oder ';', damit NachsterWert arbeiten kann
	}
	printf(" LeseDkl() beendet.");
}


//
// Testes, ob noch ein Wert für die aktuelle Variable spezifiziert ist.
//

static bool NachsterWert()
{
	printf("\nNachsterWert(): ");
	Skip();
	
	if( (Cur() == ':') ){
		printf("[DEKL-START]\n:");
		return true;
	}else if( (Cur() == ',') ){
		printf("[DEKL-CONT]\n");
		return true;
	}else if( Cur() == ';' ){
		printf("[DEKL-ENDE]\n");
		return false;
	}else{   
		printf("\nERR: Unerwartetes Symbol: %c\n", Cur());
		SyntaxError( "Unerwartets Symbol: ',' oder ';' erwartet!\n");
		return false; //Unerreichbar, damit GCC ruhe gibt
	}
}

//
// Liest Wert in einer Variablen-Deklaration, 
// erwartet das der Dateizeiger auf dem ' " ' steht
//

static void LeseWert( SkriptVar* v, int indx )
{
	int strIndx = 0;
	int strMaxIndx = 100; //Größe des Strings
	printf("\nLeseWert() [Index %d]:\n", indx);
	
	// Zeiger auf dem '"' ?
	if( Cur() != '\"' ){
		Error( "LeseNachsterWert: DateiZeiger sollte zu Beginn auf \" stehen!");
	}else if( (indx > 0) && (!v->multiWert) ){	 // Kann eigentlich nur einen Wert haben
		SyntaxError( "Variable kann nur eine Wert annehmen.");
	}
	
	v->wert[indx] =  calloc( sizeof(char), 100 );
	if( !v->wert[indx] )
		Error("Speicher-Fehler: Calloc() gibt nil zurück");
	
	Fwd();
	while( Cur() != '\"'){
	
		if( strIndx <= strMaxIndx ){  //Reicht Platz im Array noch aus?
			v->wert[indx] = realloc( v->wert[indx], strMaxIndx += 20 );
			if( !v->wert[indx] )
				Error("Speicher-Fehler: Realloc() gibt nil zurück!");
		}
		
		v->wert[indx][strIndx] = Cur();
		Fwd(); 
		printf("\nZeichen gespeichert: %c, Aktuell: %c\n", v->wert[indx][strIndx], Cur());
		strIndx++;
	}
	Fwd();
	printf("LeseWert() beendet()");
}


//
// Gibt eine Variable aus
//
	
void PrintVar( SkriptVar* v)
{
		char* name;
		
		switch( v->name ){
		case COMPILER:
			name = "COMPILER";
			break;
		case LIBARY:
			name = "LIBARY";
			break;
		case LANGUAGE:
			name = "LANGUAGE";
			break;
		case OUTPUT:
			name = "OUTPUT";
			break;
		case OPTIONS:
			name = "OPTIONS";
			break;
		case IGNOREDIR:
			name = "IGNOREDIR";
			break;
		}
		
		printf("\n\n%s:\n", name);
		for( int i = 0; (i < v->gr1Dim) && v->wert[i]; i++){
			
			printf("\t[%d]: %s", i, v->wert[i]);
		}
}



void PrintVarName( SkriptToken v )
{
	char* name;
		
		switch( v ){
		case COMPILER:
			name = "COMPILER";
			break;
		case LIBARY:
			name = "LIBARY";
			break;
		case LANGUAGE:
			name = "LANGUAGE";
			break;
		case OUTPUT:
			name = "OUTPUT";
			break;
		case OPTIONS:
			name = "OPTIONS";
			break; 
		
		}
	
		
		printf("Variable: %s\n");
}


//
// Prüft ob eine Variable gültig ist, dh. ihr schon ein Wert zugewiesen wurde
// 

bool SVIsValid( const SkriptVar* v )
{
	return (v->wert != nil ) 
}



