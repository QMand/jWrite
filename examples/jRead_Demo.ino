// 
// Demonstration of jRead - an in-place JSON element reader 
// ---------------------------------------------------------
//
// see: https://www.codeproject.com/Articles/885389/jRead-an-in-place-JSON-element-reader
//
// Tested on an Arduino UNO and will run on any other Arduino.
//
// TonyWilk
// this version A1.6 released: feb2018
//
#include "jRead.h"

// Serial.print helper function
// - a real cut-down printf()
//
void Sprint( char *fmt, ... );

//-------------------------------------------------
//             JRead -  Arduino Version
//-------------------------------------------------

char str[128];  // general use string buffer

void setup()
{
  Serial.begin(9600);
	
  Serial.println();
  Serial.println(F("JRead Examples..."));	
  Serial.println(F("See: https://www.codeproject.com/Articles/885389/jRead-an-in-place-JSON-element-reader"));
  Serial.println(F("--------------------------------------------------------------------------------------"));	

	runExamples();
}

void loop()
{
	delay( 1000 );
}

//-------------------------------------------------
// Do a query and print the results
//
void testQuery( const char * pJson, const char *query )
{
	struct jReadElement jElement;
  const char *p;
	jRead( pJson, query, &jElement );
	Sprint( "Query: \"%s\"\n", query );
  if( jElement.error ){
    Sprint( "*ERROR* %s\n", jReadErrorMessage( str, 128, jElement.error ) );
  }else{
  	Sprint( " dataType = %d,%s \n", jElement.dataType, jReadTypeMessage( str, 128, jElement.dataType) );
  	Sprint( " elements = %d\n", jElement.elements );
  	Sprint( " bytelen  = %d\n", jElement.bytelen );
  	Sprint( " value    = " );
    p= jElement.pValue;  // value is un-terminated string of chars
    for( int i=0; i<jElement.bytelen; i++ )
      Sprint( "%c", p[i] );
  }
  Sprint("\n\n");  
}

//=================================================================
// Examples
// - whole bunch of jRead examples
//
void runExamples()
{
  long l;
  int i;
  double d;
  struct jReadElement arrayElement;
  const char * exampleJson= R"(
    {
      "astring": "This is a string",
      "number1": 42,
      "number2": -123.45,
      "anObject": {
        "one": 1,
        "two": { "obj2.1": 21, "obj2.2": 22 },
        "three": 333
      },
      "anArray": [
        0,
        "one", 
        { "two.0": 20, "two.1": 21 },
        3, 
        [4, 44, 444]
      ],
      "isnull": null,
      "emptyArray": [],
      "emptyObject": {},
      "yes": true,
      "no": false
    }
  )";

	testQuery( exampleJson, "" );
	testQuery( exampleJson, "[1" );
	testQuery( exampleJson, "{'astring'" );
	testQuery( exampleJson, "{'number1'" );
	testQuery( exampleJson, "{'number2'" );
	testQuery( exampleJson, "{'anObject'" );
	testQuery( exampleJson, "{'anArray'" );
	testQuery( exampleJson, "{'isnull'" );
	testQuery( exampleJson, "{'yes'" );
	testQuery( exampleJson, "{'no'" );
	testQuery( exampleJson, "{'missing'" );
	testQuery( exampleJson, "{'anObject'{'two'" );
	testQuery( exampleJson, "{'anObject' {'two' {'obj2.2'" );
	testQuery( exampleJson, "{'anObject'{'three'" );
	testQuery( exampleJson, "{'anArray' [1" );
	testQuery( exampleJson, "{'anArray' [2 {'two.1'" );
	testQuery( exampleJson, "{'anArray' [4 [2" );
	testQuery( exampleJson, "{'anArray' [999" );

	Sprint("Empty array or object...\n");
	testQuery( exampleJson, "{'emptyArray'" );
	testQuery( exampleJson, "{'emptyObject'" );

	Sprint("Return the key at a given index in an object...\n");
	testQuery( exampleJson, "{3" );
	testQuery( exampleJson, "{'anObject' {1" );
	testQuery( exampleJson, "{999" );

	// examples of helper functions
	l= jRead_long( exampleJson, "{'number1'" );			    // 42
	i= jRead_int( exampleJson, "{'yes'" );				      // 1	(BOOL example)
	d= jRead_float( exampleJson, "{'number2'" );			  // -123.45
	jRead_string( exampleJson, "{'astring'", str, 16 );	// "This is a strin\0" (buffer too short example)

	Sprint("Helper Functions...\n" );
	Sprint("  \"number1\"= %ld\n", l );
	Sprint("  \"yes\"    = %d\n", i );
	Sprint("  \"number2\"= %f\n", d );
	Sprint("  \"astring\"= \"%s\"\n", str );

	// Example of cascading queries
	Sprint("\nQueries on sub-elements and use of query parameters...\n");

	// locate "anArray"...
	jRead( exampleJson, "{'anArray'", &arrayElement );	
  jRead_strcpy( str, 128, &arrayElement );    // print the JSON of the "anArray" element
	Sprint("  \"anArray\": = %s\n\n", str );

	// do queries within "anArray"...
	for( i=0; i < arrayElement.elements; i++ )
	{
		// index the array using queryParam
		jRead_string( arrayElement.pValue, "[*", str, 128, &i ); 
		Sprint("  anArray[%d] = %s\n", i, str );
	}

	// example using a parameter array
	{
		int params[2]={ 2, 1 };
		jRead_string( arrayElement.pValue, "[*{*", str, 128, params ); 
		Sprint("\n  anArray[%d] objectKey[%d] = \"%s\"\n", params[0], params[1], str );
	}

  Sprint("\nEnd of jRead examples\n");
}

//=================================================================
// Serial.print helper function
// - a real cut-down printf()
// - this function is not part of jRead, it is used here to print the examples
// usage:
//   Sprint("Test int:%d, float:%f, hex:%x, char:%c, string:%s\n", 42, 123.4, 0xABCD, 'a', "Hello!" );
//
void Sprint( const char *fmt, ... )
{
  char c;
  va_list args;
  va_start( args, fmt );
  while( (c=*fmt) != 0 ){
    switch( c )
    {
    case '%':
      c= *(++fmt);
      switch( c )
      {
      case 'd': Serial.print( va_arg(args,int) ); break;
      case 'l': 
        switch( c= *(++fmt) )
        {
        case 'd': Serial.print( va_arg(args,long) ); break; 
        case 'x': Serial.print( va_arg(args,long), HEX ); break;
        }
        break;
      case 'f': Serial.print( va_arg(args,double) ); break;
      case 'x': Serial.print( va_arg(args,int), HEX ); break;
      case 'c': Serial.print( (char)va_arg(args,int) ); break;
      case 's': Serial.print( va_arg(args, char *) ); break;
      default:  break;
      }
      break;
    case '\\':
      c= *(++fmt);
      if( c == 'n' )
        Serial.write( 0x0a );
      else if( c == 'r' )
        Serial.write( 0x0d );
      else
        Serial.write( c );
      break; 
    default:
      Serial.print( c );
      break;
    }
    ++fmt;
  }
  va_end( args );
}
