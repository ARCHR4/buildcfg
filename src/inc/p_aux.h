
#ifndef MODULE_PAUX
	#define MODULE_PAUX
	
	#include <stdnoreturn.h>
	
	#define Alloc2DArray( type, dim1, dim2 ) _Alloc2DArray( sizeof(type), dim1, dim2 )
	
	void** _Alloc2DArray( int, int, int );

	noreturn void Error( const char* );
	
	noreturn void SyntaxError( const char* );
	
	noreturn void AuxUnitTest();
	
#endif