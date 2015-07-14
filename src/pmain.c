
/*
*
*	PMAIN.C:
*	Der wirkliche Parser. Filtert die Datein aus dem Skript
*	und �bergibt sie an das n�chste Modul.
*
*
*/


#include <windows.h>
#include <stdio.h>
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




int main(int argc, char** argv)
{
	SkriptVar* v[5];
	
	if( argc < 1 ){
		Error(" Kein Pfad angegeben!");
	}else{
		bool baseOK = BaseInit( argv[1] );
		if( !baseOK )
			Error("Projekt-Verzeichnis konnte nicht gefunden werden!");
	}
	
	for( int i = 0; i < 5; i++)		//Varialen initalisieren 
		v[i] = SVInit( (SkriptToken ) i ); 
		
	// Datei lesen 
	for( SkriptToken nxtVar = FindeDkl(); nxtVar != GEOF; nxtVar = FindeDkl()){
		if( nxtVar != UNEXPECTED )
			LeseDkl( v[ (int) nxtVar ] );
		else
			SyntaxError("Unerwartetes Symbol!");
	}
	
	#ifdef TEST 
		for( int i = 0; i < 5; i++)
			PrintVar( v[i] );
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
	
	if( (welche == NIL ) || (welche == GEOF ) ) return nil;
	
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
// Zerst�rt eine Vaariable wieder
//
	
void SVDel( SkriptVar* toDel )
{
	for( int i = 0; i < toDel->gr1Dim; i++)
		free( toDel->wert[i] );
	
	free( toDel);
}

//
// Findet n�chste Deklaration
//

static SkriptToken FindeDkl()
{
	const PStatus* const st  = Status();
	
	if( feof(st->datei) ) 		//Dateiende erreicht
		return GEOF;
	
	if( Cur() == ';') 		    
		Fwd();
		
	Skip();  //N�chstes druckbares Zeichen
	
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
	
	default:
		return UNEXPECTED;
	}
}



static void LeseDkl( SkriptVar* v )
{
	int indx;
	printf("\nLeseDkl():\n");
	if( v->wert != nil ){  //Variable wird redefiniert? 
		Error( "Versuch Variable zu redefinieren!" ); 
	}else if( v->multiWert ){
		v->wert = calloc( sizeof( char* ), 5 );
		v->gr1Dim = 5;
	}else{
		v->wert = malloc( sizeof( char* ) );
		v->gr1Dim = 1;
	}
	
	if( !v->wert )
		Error("Speicher-Fehler: Malloc/Calloc gibt nil zur�ck!");
	
	// Der Zeiger ist auf einem Buchstaben.
	// Zum ':' springen
	while( isalpha( Cur() ) ) Fwd();
	Skip();
	
	if( Cur() != ':' ){
		SyntaxError( "\':\' nach dem Namen erwartet!");
	}
		
	while( NachsterWert( ) ){
		Fwd();
		Skip();
		printf("N�chster Wert: %c", Cur());
		if( Cur() != '\"' ) 
			SyntaxError("\'\"\' vor Wert erwartet!");
		
		// Platz im Array?
		if( indx >= v->gr1Dim ){
			v->wert = realloc( v->wert, v->gr1Dim += 2 );
			if( !v->wert ){
				Error( "Realloc gibt NIL zur�ck!");
			}
		}
		
		LeseWert( v, indx );
		indx++;
		
		Skip(); //Zeiger auf ',' oder ';', damit NachsterWert arbeiten kann
	}
}


//
// Pr�ft ob eine Deklaration weitergeht
//

static bool NachsterWert()
{
	
	Skip();
	
	if( (Cur() == ',') || (Cur() == ':')) 
		return true;
	else if( Cur() == ';' )
		return false;
	else{   
		SyntaxError( "Unerwartets Symbol: ',' oder ';' erwartet!");
		return false; //Unerreichbar, damit GCC ruhe gibt
	}
}

//
// Liest Wert in einer Deklaration, erwartet das der Dateizeiger auf dem ' " ' steht
//

static void LeseWert( SkriptVar* v, int indx )
{
	int strIndx = 0;
	int strMaxIndx = 100; //Gr��e des Strings
	
	// Zeiger auf dem '"' ?
	if( Cur() != '\"' )
		Error( "LeseNachsterWert: DateiZeiger sollte zu Beginn auf \" stehen!");
	else if( v->wert && (!v->multiWert) )	 // Kann eigentlich nur einen Wert haben 
		SyntaxError( "Variable kann nur eine Wert annehmen ");
	
	v->wert[indx] = malloc( sizeof(char) * 100 );
	if( !v->wert[indx] )
		Error("Speicher-Fehler: Malloc() gibt nil zur�ck");
	
	while( Cur() != '\"'){
	
		if( strIndx <= strMaxIndx ){  //Reicht Platz im Array noch aus?
			v->wert[indx] = realloc( v->wert[indx], strMaxIndx += 20 );
			if( v->wert[indx] )
				Error("Speicher-Fehler: Realloc() gibt nil zur�ck!");
		}
		
		v->wert[indx][strIndx++] = Cur();
		Fwd(); 
		strIndx++;
	}
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
		}
		
		printf("\n\n%s:\n", name);
		for( int i = 0; i < v->gr1Dim; i++){
			printf("\t[%d]: %s", i, v->wert[i]);
		}
}


	
		

