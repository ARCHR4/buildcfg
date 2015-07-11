
#ifndef MODUL_BASE
	#define MODUL_BASE
	
	#include <stdbool.h>
	#include <stdnoreturn.h>
	#include <stdio.h>
	
	typedef struct{
		FILE*	datei;
		const char*	pfad;
		const char* dir;
	}PStatus;
	
	bool BaseInit( const char* );
	
	void BaseClose( void );
	
	const PStatus* const Status();
	
	int Cur( void );
	
	int Next( void );
	
	int Last( void );
	
	int Fwd( void );
	
	void BaseUnitTest();
	
	int Rwd( void ); 
	
	int GetLineCount();
	
	void Skip();
	
	#define nil ((void*) 0)
	
#endif
	
	