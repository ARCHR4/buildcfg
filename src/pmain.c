
/*
*
*	PMAIN.C:
*	Der wirkliche Parser. Filtert die Datein aus dem Skript
*	und übergibt sie an das nächste Modul.
*
*
*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "inc/base.h"
#include "inc/p_aux.h"
#include "inc/pmain.h"

static SkriptVar* SVInit( SkriptToken );

void SVDel( SkriptVar* );

static SkriptToken FindeDekl( );

static void LeseDkl();

static bool NachsterWert();

static void LeseWert(); 




int main(int argc, char** argv)
{
	SkriptVar* v[5];
	
	if( argc < 1 ){
		Error(" Kein Pfad angegeben!");
	}else{
		bool baseOK = BaseInit( argv[0] );
		if( !baseOK )
			Error("Projekt-Verzeichnis konnte nicht gefunden werden!");
	}
	
	for( int i = 0; i < 5; i++)		//Varialen initalisieren 
		v[i] = SVInit( (SkriptToken ) i ); 
		
	// Datei lesen 
	for( SkriptToken nxtVar = FindeDkl(); (nxtVar != EOF) && (nxtVar != UNEXPECTED); nxtVar = FindeDkl()){
		LeseDkl( v[ (int) nxtVar ] );
	
}



//
// Erstellt und initialisiert eine neue Variable
//


static SkriptVar* SVInit( SkriptToken welche )
{
	SkriptVar* ret;
	
	if( (welche == NIL ) || (welche == EOF ) return nil;
	
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
// Zerstört eine Vaariable wieder
//
	
void SVDel( SkriptVar* toDel )
{
	for( int i = 0; i < toDel->gr1Dim; i++)
		free( toDel->wert[i] );
	
	free( toDel)
}

//
// Findet nächste Deklaration
//

static SkriptToken FindeDekl()
{
	const PStatus* st const = Status();
	
	if( feof(st->datei) ) 		//Dateiende erreicht
		return EOF;
	
	if( Cur() == ';') 		    
		Fwd();
		
	Skip();  //Nächstes druckbares Zeichen
	
	switch( Cur() ){
	case 'l':
	case 'L':
		return (( Next() == 'i' ) || (Next() == 'I' ) ? LIBARY : LANGUAGE;
		
	case 'o':
	case 'O':
		return (( Next() == 'u') || (Next() == 'U') ? OUTPUT : OPTIONS;
		
	case 'c':
	case 'C':
		return COMPILER;
	
	case 'i':
	case 'I':
		return IGNOREDIR;
	
	default:
		return UNEXPECTED;
}



static void LeseDekl( SkriptVar* v )
{
	int indx;
	
	if( v->wert != nil ){  //Variable wird redefiniert? 
		Error( "Versuch Variable zu redefinieren!" ); 
	}else if( v->multiVar ){
		v-wert = calloc( sizeof( char* ), 5 );
		v->gr1Dim = 5;
	}else{
		v->wert = malloc( Sizeof( char* ) );
		v->grim1Dim = 1;
	}
	
	if( v->wert )
		Error("Speicher-Fehler: Malloc/Calloc gibt nil zurück!");
	
	// Der Zeiger ist auf einem Buchstaben.
	// Zum ':' springen
	while( isalpha( Cur() ) ) Fwd();
	Skip();
	
	if( Cur() != ':' ){
		SyntaxError( "\':\' nach dem Namen erwartet!");
		
	while( NachsterWert( ) ){
		Skip();
		if( Cur() != '\"' ) 
			SyntaxError("\'\"\' vor Wert erwartet!");
		
		// Platz im Array?
		if( indx >= v->gr1Dim ){
			v->wert = realloc( v->wert, v->gr1Dim += 2 );
			if( !v->wert ){
				Error( "Realloc gibt NIL zurück!"),
			}
		}
		
		LeseWert( v, indx );
		indx++;
		
		Skip(); //Zeiger auf ',' oder ';', damit NachsterWert arbeiten kann
	}
}


//
// Prüft ob eine Deklaration weitergeht
//

static bool NachsterWert()
{
	
	Skip();
	
	if( (Cur() == ',') || (Cur() == ':')
		return true;
	else if( Cur() == ';' )
		return false;
	else
		SynatxError( "Unerwartets Symbol: ',' oder ';' erwartet!");
}

//
// Liest Wert in einer Deklaration, erwartet das der Dateizeiger auf dem ' " ' steht
//

static void LeseWert( SkriptVar* v, int indx )
{
	int strIndx = 0;
	int strMaxIndx = 100; //Größe des Strings
	
	// Zeiger auf dem '"' ?
	if( Cur() != '\"' )
		Error( "LeseNachsterWert: DateiZeiger sollte zu Beginn auf \" stehen!");
	else if( v->wert && (!v->multiVal) )	 // Kann eigentlich nur einen Wert haben 
		SyntaxError( "Variable kann nur eine Wert annehmen ");
	
	v->wert[index] = malloc( sizeof(char) * 100 );
	if( !v->wert[index] )
		Error("Speicher-Fehler: Malloc() gibt nil zurück");
	
	while( Cur() != '\"'){
	
		if( strIndx <= strMaxIndx ){  //Reicht Platz im Array noch aus?
			v->wert[index] = realloc( v->wert[index], strMaxWert += 20 ),
			if( v->wert[index] )
				Error("Speicher-Fehler: Realloc() gibt nil zurück!");
		}
		
		v->wert[indx][strIndx++] = Cur();
		Fwd(); 
		strIndx++;
	}
}


	
	
		
		

	
	
	
		

