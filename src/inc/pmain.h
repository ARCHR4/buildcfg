
/*
*		PMAIN.H
*
*/

#include <stdbool.h> 

#ifndef MODULE_PMAIN
	#define MODULE_PMAIN
	
	//
	// Alle güligen Token
	//
	
	typedef enum{
		GEOF = -3,	// Für GetNextVar(), um anzuzeigen dass keine weitren Variablen vorliegen
		NIL = -2,  	// Nicht definiert
		UNEXPECTED = -1, // Synatx Error
		LANGUAGE,
		COMPILER,
		OUTPUT,
		LIBARY,
		OPTIONS,
		IGNOREDIR,	
	} SkriptToken;
	
	//
	// Speichert Wert einer Variable
	//
	
	typedef struct{
		SkriptToken name; 			// Welcher Wert wird repräsentiert?
		bool		multiWert;	    // Kann mehrere Werte aufnehmen?
		char**		wert;			
		int         gr1Dim;		    // Allokierter Platz 
		int 		anzahlWerte;	// Tatsächlich geschriebene Werte
	} SkriptVar;
	
	void PrintVar( SkriptVar* );
	void SVDel( SkriptVar* );

#endif
