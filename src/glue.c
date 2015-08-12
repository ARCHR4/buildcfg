
/*
*
*	GLUE.C
*	Empfängt die Liste von Variablen von pmain, 
*	schreibt sie in einem gewissen Format in eine Batch-Datei.
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "inc\pmain.h"
#include "inc\p_aux.h"
#include "inc\srcsearch.h"
#include "inc\base.h"

/* Muster, um den Compiler [GCC, G++] aufrufen:
*	
*	start [Compiler] [Options] -f [Dateien] -o [Output] 
*
*/

FILE* outputFile;
static char* GetCompPath( char* );
static char* GetCompName( char* );



//
// Prüft ob eine Variable gültig ist, dh. ihr schon ein Wert zugewiesen wurde.
// 

static bool SVIsValid( const SkriptVar* v )
{
	return (v->wert != nil );
}

//
//	Gibt die mit <token> spezifierte Variable aus dem Array <varArray> zurück.
//

static SkriptVar* GetSVByName( SkriptVar** varArray, SkriptToken token)
{
	return varArray[(int) token];
}

/*
*	Funktionen schreiben je ihre Variable nach dem benötigten Format in die Batch-Datei.
*/


static void WriteCompiler( SkriptVar** varArray )
{
	SkriptVar* comp = GetSVByName( varArray, COMPILER );
	char* compPath, * compName; 
	
	if( !SVIsValid( comp ) )
		Error("Der Compiler-Pfad wurde nicht spezifiziert [COMPILER-Option]!");
	
	compName = GetCompName( comp->wert[0] );
	compPath = GetCompPath( comp->wert[0] );
	
	
	fprintf(outputFile, "path start %s ;%path%\n", compPath);
	fprintf(outputFile, "start /W /B %s", compName );
	free(compName);
	free(compPath);
	
}




static void WriteOptions( SkriptVar** varArray )
{
	SkriptVar* opt = GetSVByName( varArray, OPTIONS );
	if( !SVIsValid( opt ) )
		return;
	
	for( int i = 0; i < opt->anzahlWerte; i++)
		fprintf(outputFile, " %s", opt->wert[i]);
}



static void WriteFiles(SkriptVar** varArray )
{
	SkriptVar* ignore = GetSVByName( varArray, IGNOREDIR ), *lang = GetSVByName( varArray, LANGUAGE );
	const PStatus* const ps = Status();
	
	printf("\nWriteFiles():\n\n");
	if( !SVIsValid( lang ) )
		Error("Die Datei-Endung wurde nicht spezifiziert [LANGUAGE-Option]!");
	
	printf("\nGetSrcFile() Parameter: %s\n", ps->dir);
	GetSrcFile( ps->dir, (const char**) ignore->wert, *(lang->wert), outputFile, ignore->anzahlWerte );
}



static void WriteOutput( SkriptVar** varArray )
{
	SkriptVar* out = GetSVByName( varArray, OUTPUT );
	
	printf("\nWriteOutput:\n\n");
	
	if( !SVIsValid( out ) )
		Error( "Die Output-Datei wurde nicht spezifiziert [OUTPUT-Option]!" );
	
	fprintf(outputFile, " -o %s ", *(out->wert) );
	printf("\tWert geschrieben: %s\n\n", *(out->wert) );
}



static void Setup(const char* outputPath)
{
	const PStatus* const ps = Status();
	outputFile = fopen( outputPath, "w");
	
	if( !outputFile )
		Error( "Versuch Stdin umzuleiten fehlgschlagen!" );
	
	fprintf(outputFile, "\ncd %s\n", ps->dir);
	fprintf(outputFile, "pause\n\n");
}


//
//	Hauptfunktion.
//

void GlueMain( SkriptVar** vars )
{
	const PStatus* const ps = Status();
	char* outputPath = calloc( sizeof(char), strlen( ps->dir ) + strlen( "\\build.cmd" ) + 1);
	
	fprintf(stderr, "\n\nGlueMain:\n\n");
	
	if( !outputPath )
		Error("Calloc() fehlgeschlagen [NIL]!");
		
	strcpy( outputPath, ps->dir );
	strcat(outputPath, "\\build.cmd");
	
	fprintf(stderr, " outputPath %s\n", outputPath);
	
	Setup( outputPath );
	WriteCompiler( vars );
	WriteOptions( vars );
	WriteFiles( vars );
	WriteOutput( vars );
	
	fclose( stdin );
	exit( 0 );
}


static char* GetCompPath( char* c)
{
	int len = strlen( c );
	char* ret = calloc( sizeof(char), len);
	char* cur = c;
	
	if( !ret)
		Error("Calloc failed!");
	
	while( *cur != '\\' ){
		cur--;
		len--;
	}
	
	cur--;
	
	for( int i = len, a = 0; i >= 0; i--, a++)
		*(ret+a) = *(cur-1);
	
	return ret;
}



static char* GetCompName( char* c )
{
	char* ret = calloc( sizeof( char ), 30);
	int am = 30;
	
	c += (strlen( c ) - 1);
	
	if(!ret)
		Error("Calloc failed!");
	
	for( int i = 0; *c != '\\'; i++){
	
		if( i == am ){
			ret = realloc( ret, am+5 );
			if(!ret) 
				Error("Realloc failed");
		}
		
		ret[i] = *c;
		c--;
		am++;
	}
	
	return ret;
}


	
	
	