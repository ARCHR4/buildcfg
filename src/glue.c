
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
	if( !SVIsValid( comp ) )
		Error("Der Compiler-Pfad wurde nicht spezifiziert [COMPILER-Option]!");
	
	fprintf(outputFile, "start %s", comp->wert[0]);
}




static void WriteOptions( SkriptVar** varArray )
{
	SkriptVar* opt = GetSVByName( varArray, OPTIONS );
	if( !SVIsValid( opt ) )
		return;
	
	for( int i = 0; i < opt->gr1Dim; i++)
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
	GetSrcFile( ps->dir, (const char**) ignore->wert, *(lang->wert), outputFile, ignore->gr1Dim );
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
	
	fprintf(outputFile, "cd %s", ps->dir);
	fprintf(outputFile, "pause");
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


	
	
	