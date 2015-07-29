/*
*	SRCSEARCH.C
*	Siehe Beschreibung GetSrcFile().
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 

#include <errno.h>

#include <dir.h>
#include <dirent.h>
#include <sys\stat.h>

#include "inc\p_aux.h" 	


static char* GetEntryPath( struct dirent* entry, const char*);
static int   GetEntryType( struct dirent* restrict, const char*, const char**, int);

//
// Dursucht <lastDir> nach Dateien mit der Endung <end>.
// Unterverzeichnisse werden ebenfalls (rekursiv) dursucht, es sei denn sie sind in 
// <ignore> gelistet.
// Die Pfade der gefundenen Dateien werden dann in die Datei <writeTo> geschrieben.
//

void GetSrcFile( const char* lastDir, const char** ignore, 	const char* end,
				 FILE* writeTo, int ignEle )
{
	DIR* curDir = opendir( lastDir );
	
	if( !curDir ) 
		Error( "Verzeichnis konnte nicht geoeffnet werden!" );
	
	for( struct dirent* entry = readdir( curDir ); entry; entry = readdir( curDir ) ){
	
		char* entryPath = GetEntryPath( entry, lastDir ); 
		
		switch( GetEntryType( entry, entryPath, ignore, ignEle ) ){
		
		case -1:
		strerror( errno );
			Error( "Fehler beim Lesen der Datei-Attribute ");
			
			break;
		
		case 0:  //Verzeichnis, soll nicht dursucht werden.
			break;
		
		case 1:	//Verzeichnis, soll nicht ignoriert werden.
			GetSrcFile( entryPath, ignore, end, writeTo, ignEle );  //Rkursion.
			break; 
		
		case 2:	// Datei

			if( strstr( entry->d_name, end ) ){				//Hat gesuchte Endung?
				fprintf(writeTo, " %s ", entryPath );		//Ja -> Pfad in Ziel-Datei schreiben.
			}
			break;
		
		case 3:
			break;
		}
		
		free( entryPath );
	}
	
	return;
}





//
// Verbindet den Namen eines Eintrags <entry> und Pfad des Verzeichnis <path>
// zum Pfad des Eintrags.
// 

static char* GetEntryPath( struct dirent* entry, const char* path )
{
	int len = strlen( entry->d_name ) + strlen("\\") + strlen( path ) + 1;
	char* ret = calloc( sizeof(char), len );
	
	if( !ret ){
		Error( "Calloc fehlgeschlagen!" );
		return ((void*) 0);
	}else{
		strcpy( ret, path );
		strcat( ret, "\\");
		strcat(ret, entry->d_name );
		return ret;
	}
}
	
	

//
// Prüft ob ein Eintrag <entry> in einer Datei ein Verzeichnis ist
// und dursucht werden soll (1) oder nicht (0), 
// oder ob es eine Datei ist (2).
//

static int GetEntryType( struct dirent* restrict entry, const char* entryPath, const char** ignoreList, 
						 int listCount )
{
	struct stat attrb;
	int succes = stat( entryPath, &attrb );
	
	if( succes == -1 )
    	return -1;
		
	
	
	if( S_ISREG( attrb.st_mode) ){ //DAtei
	
		return 2; 
		
	}else if( S_ISDIR(attrb.st_mode) ){	// Entry ist eine Verzeichnis 
		
		if( (!strcmp( entry->d_name, "..")) || (!strcmp( entry->d_name, ".")) ){ //'.' ist das Arbeitsverzeichnis, '..' das letzte Verzeichnis 
			return 0; 
		}
		
		// Prüfen ob das Verzeichnis in der Liste steht
		for( int i = 0; i < listCount; i++){
			
			if( !strcmp( entry->d_name, ignoreList[i]) ){
				return 0;
			}
		}
	
		return 1;  //Verzeichnis soll dursucht werden
		
	}else{		   //Irgendwas anderes 
		return 3;
	}
}




