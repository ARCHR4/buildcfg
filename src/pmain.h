
/*
*		PMAIN.H
*		Implementiert Struktur die Optionen aus dem Skript speichert
*
*/


#ifndef MODULE_PMAIN
	#define MODULE_PMAIN
	
	//
	// Alle güligen Token
	//
	
	typedef enum{
		EOF = -1,  // Für GetNextVar(), um anzuzeigen dass keine weitren Variablen vorliegen
		NIL = -1,  // Nicht definiert
		UNEXPECTED = -1, // Synatx Error
		LANGUAGE,
		COMPILER,
		OUTPUT,
		LIBARY,
		OPTIONS,
		IGNOREDIR
	} SkriptToken;
	
	//
	// Speichert Wert einer Variable
	//
	
	typedef struct{
		SkriptToken name; 			// Welcher Wert wird repräsentiert?
		bool		multiWert;
		char**		wert;
		int         gr1Dim;		    // 1. Dimension des wert-Arrays
	} SkriptVar;
	

#endif
