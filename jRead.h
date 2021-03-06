// jRead.h
//
// see jRead.cpp for more information
//
#ifndef JREAD_H
#define JREAD_H

// uncomment this if you really want to use double quotes in query strings instead of '
//#define JREAD_DOUBLE_QUOTE_IN_QUERY

// comment this out if you don't want the Optional Error string output
#define INCL_jReadErrorToString

// comment this out if you don't want the Optional data type to string function
#define INCL_jReadTypeToString

//
// return dataTypes:
#define JREAD_ERROR		0		// general error, eof etc.
#define JREAD_OBJECT	1		// "{"
#define JREAD_ARRAY		2		// "["
#define JREAD_STRING	3		// "string" 
#define JREAD_NUMBER	4		// number (may be -ve) int or float
#define JREAD_BOOL		5		// true or false
#define JREAD_NULL		6		// null
#define JREAD_KEY		  7		// object "key"
// internal values:
#define JREAD_COLON		8		// ":"
#define JREAD_EOL		  9		// end of input string (ptr at '\0')
#define JREAD_COMMA		10		// ","
#define JREAD_EOBJECT	11		// "}"
#define JREAD_EARRAY	12		// "]"
#define JREAD_QPARAM	13		// "*" query string parameter

//------------------------------------------------------
// jReadElement
// - structure to return JSON elements
// - error=0 for valid returns
//
// *NOTES* 
//    the returned pValue pointer points into the passed JSON
//    string returns are not '\0' terminated.
//    bytelen specifies the length of the returned data pointed to by pValue
//
struct jReadElement{
	int dataType;		// one of JREAD_...
	int elements;		// number of elements (e.g. elements in array or object)
	int bytelen;		// byte length of element (e.g. length of string, array text "[ ... ]" etc.)
	const char * pValue;		// pointer to value string in JSON text
	int error;			// error value if dataType == JREAD_ERROR
};

//------------------------------------------------------
// The JSON reader function
//
// - reads a '\0'-terminated JSON text string from pJson
// - traverses the JSON according to the pQuery string
// - returns the result value in pResult
//
// returns: pointer into pJson after the queried value
//
// e.g.
//    With JSON like: "{ ..., "key":"value", ... }"
//
//    jRead( pJson, "{'key'", &result );
// returns with: 
//    result.dataType= JREAD_STRING, result.pValue->'value', result.bytelen=5
//
const char *	jRead( const char *pJson, const char *pQuery, struct jReadElement *pResult );

// version of jRead which allows one or more queryParam integers to be substituted
// for array or object indexes marked by a '*' in the query
//
// e.g. jReadParam( pJson, "[*", &resultElement, &arrayIndex );
//
// *!* CAUTION *!*
// You can supply an array of integers which are indexed for each '*' in pQuery
// however, horrid things will happen if you don't supply enough parameters
// 
const char * jReadParam( const char *pJson, const char *pQuery, struct jReadElement *pResult, int *queryParams );

// Array Stepping function
// - assumes pJsonArray is JSON source of an array "[ ... ]"
// - returns next element of the array in pResult
// - returns pointer to end of element, to be passed to next call of jReadArrayStep()
// - if end of array is encountered, pResult->error = 13 "End of array found"
//
// e.g.
//   With JSON like:   "{ ...  "arrayInObject":[ elem1,elem2,... ], ... }"
//
//   pJson= jRead( pJson, "{'arrayInObject'", &theArray );
//   if( theArray.dataType == JREAD_ARRAY )
//   {
//       char *pArray= (char *)theArray.pValue;
//       jReadElement arrayElement;
//       int index;
//       for( index=0; index < theArray.elements; index++ )
//       {
//           pArray= jReadArrayStep( pArray, &arrayElement );
//           ...
//
// Note: this significantly speeds up traversing arrays.
//
const char * jReadArrayStep( const char *pJsonArray, struct jReadElement *pResult );


#define EXPORT_OPTIONAL_FUNCTIONS
#ifdef EXPORT_OPTIONAL_FUNCTIONS 

//------------------------------------------------------
// Optional Helper Functions
//
long jRead_long( const char *pJson, const char *pQuery, int *queryParams=NULL );
int jRead_int( const char *pJson, const char *pQuery, int *queryParams=NULL );
float jRead_float( const char *pJson, const char *pQuery, int *queryParams=NULL );
int jRead_string( const char *pJson, const char *pQuery, char *pDest, int destlen, int *queryParams=NULL );

//------------------------------------------------------
// Optional String output Functions
//
char * jReadTypeMessage( char *destBuffer, int destLength, int datatype ); // get string describing data type
char * jReadErrorMessage( char *destBuffer, int destLength, int error );  // get string describing error code

//------------------------------------------------------
// Other jRead utilities which may be useful...
//
const char * jRead_atoi( const char *p, unsigned int *result );	// string to unsigned int
const char * jRead_atol( const char *p, long *result );			// string to signed long
const char * jRead_atof( const char *p, float *result);		// string to float (does not do exponents)
int jReadStrcmp( struct jReadElement *j1, struct jReadElement *j2 ); // compare STRING elements

// copy element to '\0'-terminated buffer
int jRead_strcpy( char *destBuffer, int destLength, struct jReadElement *pElement );

#endif
// end of jRead.h

#endif /* JREAD_H */




