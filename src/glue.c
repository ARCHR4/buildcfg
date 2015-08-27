
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
	
	printf("WriteCompiler()\n");
	if( !SVIsValid( comp ) )
		Error("Der Compiler-Pfad wurde nicht spezifiziert [COMPILER-Option]!");
	
	compName = GetCompName( comp->wert[0] );
	compPath = GetCompPath( comp->wert[0] );
	printf("\n\tCompName = %s,\n\tCompPath = %s\n", compName, compPath);
	
	fprintf(outputFile, "path %s ;%%path%%\n", compPath);
	fprintf(outputFile, "start /W /B %s", compName );
	free(compName);
	free(compPath);
	
}




static void WriteOptions( SkriptVar** varArray )
{
	SkriptVar* opt = GetSVByName( varArray, OPTIONS );
	if( !SVIsValid( opt ) )
		return;
	
	printf("WrtiteOPtions():");
	
	for( int i = 0; i < opt->anzahlWerte; i++)
		fprintf(outputFile, " %s", opt->wert[i]);
	
	printf("\n\tBeendet.\n");
}



static void WriteFiles(SkriptVar** varArray )
{
	SkriptVar* ignore = GetSVByName( varArray, IGNOREDIR ), *lang = GetSVByName( varArray, LANGUAGE );
	const PStatus* const ps = Status();
	
	printf("\nWriteFiles():\n\n");
	if( !SVIsValid(lang) )
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

//
//Extrahiert den Pfad (ohne Name) der Datei aus dem kompletten Pfad.
//

static char* GetCompPath( char* c)
{
	int len = strlen(c), nameLen = 0, pathLen = 0;
	char* cur = c + len, * brk = nil, * ret = nil;
	
	printf("GetCompPath():\n\tLen = %d,\n\t Pfad: %s\n", len, c);
	
	for( cur; *cur != '\\'; cur--)	//Letzten Backslash merken
		nameLen++;
	
	brk = cur; 	// Position des letzten Backslash merken.
	pathLen = len - nameLen;
	printf("\tPathLen = %d, nameLen = %d\n", pathLen, nameLen);
	
	ret = calloc( sizeof(char), pathLen + 1 );
	if(!ret)
		Error("Speicher-Fehler: Calloc() fehlgeschlagen.");
	
	//String bis Cur kopieren.
	for(int i = 0; (c+i) != brk ; i++){
		ret[i] = c[i];
		printf("\n\tC+i: %p, brk = %p, i = %d,\n\t c[i] = %c, ret[i] = %c\n", c+1, brk, i,  c[i], ret[i]);
	}
	printf("\n\tBeendet.\n");
	return ret;
		
	
}

//
// Extrahiert den Name einer Datei aus dem DateiPfad.
//


static char* GetCompName( char* p )
{
	char* ret = nil , *cur = nil;
	int len = strlen( p ), nameLen = 0;
	
	cur = p + len;
	
	printf("GetCompName():\n\tLen: %d, p: %p, cur: %p\n\tpfad: %s\n", len, p, cur, p);
	
	for( nameLen; *cur != '\\'; nameLen++)
		cur--;
		
	printf("\n\tNach 1. Schleife: nameLen: %d, cur: %p, *cur = %c\n\n", nameLen, cur, *cur);
	printf("\n");
	
	ret = calloc( sizeof(char), nameLen + 1);
	if(!ret)
		Error("Speicher-Fehler: Calloc() fehlgeschlagen!");
	
	cur++;
	
	for( int i = 0; (cur+i) != (p+len); i++ ){
		ret[i] = cur[i];
		printf("\n\tRet[%d] = %c, cur[%d] = %c.\ncur = %p, P+len = %p", i, ret[i], i, cur[i], cur+i, p+len);
		printf("\n");
	}
	
	printf("Beendet.");
	return ret;

}

	
	


	
	
	