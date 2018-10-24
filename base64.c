#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "base64.h"

#ifdef __linux__
#define O_BINARY 0
#endif

static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static int min ( int a, int b )
{
	return ( a > b )? b : a ;
}

static void encode_b64 ( uint8_t* in, uint8_t* out, int len )
{
	out[ 0 ] = b64[ in[ 0 ] >> 2 ];
	out[ 1 ] = ( len > 1 ) ? b64[ ( ( ( in[ 0 ] << 6 ) | ( in[ 1 ] >> 2 ) ) >> 2 ) & 0x3f ] :  b64[ ( in[ 0 ] << 4 ) & 0x3f ] ;
	out[ 2 ] = ( len > 1 ) ? b64[ ( ( ( in[ 1 ] << 4 ) | ( in[ 2 ] >> 4 ) ) >> 2 ) & 0x3f ] : '=';
	out[ 3 ] = ( len > 2 ) ? b64[ in[ 2 ] & 0x3f ] : '=';
}

static uint8_t val ( uint8_t a )
{
	for ( uint8_t i = 0; i < 64; i++ )
	{
		if ( a == b64[ i ] )
		{
			return ( i );
		}
	}
	return ( 0 );
}

static int decode_b64 ( uint8_t* in, uint8_t* out )  // len == 4
{
	uint8_t v0 = val ( in[ 0 ] );
	uint8_t v1 = val ( in[ 1 ] );
	uint8_t v2 = val ( in[ 2 ] );
	uint8_t v3 = val ( in[ 3 ] );

	out[ 0 ] = (  v0 << 2 ) | ( v1 >> 4 );

	if ( in[ 2 ] == '=' )
	{
		out[ 1 ] = '\0';
		return ( 1 );
	}
	out[ 1 ] = ( v1 << 4 ) | ( v2 >> 2 );

	if ( in[ 3 ] == '=' )
	{
		out[ 2 ] = '\0';
		return ( 2 );
	}
	out[ 2 ] = ( v2 << 6 ) | v3;
	return ( 3 );
}

static int encodeBase64_S2S ( uint8_t * const in, uint8_t ** const out, uint32_t * const size )
{
	uint32_t newSize = ( *size % 3 )? ( *size / 3 + 1 ) * 4 : ( *size / 3 * 4 );
	uint32_t i, k;

	if ( !in ||
		!out ||
		!size ||
		( *size == 0 ) )
	{
		return ( __LINE__ );
	}

	if ( !*out )
	{
		*out = malloc ( newSize );
		if ( !*out )
		{
			return ( __LINE__ );
		}
	}

	i = 0;
	k = 0;
	while ( i < *size )
	{
		encode_b64 ( &in[ i ], &(*out)[ k ], min ( *size - i, 4 ) );

		k += 4;
		i += 3;
	}

	*size = newSize;

	return ( 0 );
}

static int decodeBase64_S2S ( uint8_t * const in, uint8_t ** const out, uint32_t * const size )
{
	uint32_t newSize = ( *size / 4 * 3 ) + 1;
	uint32_t i, k;

	if ( !in ||
		!out ||
		!size ||
		( *size == 0 ) ||
		( ( *size % 4 ) != 0 ) )
	{
		return ( __LINE__ );
	}

	if ( !*out )
	{
		*out = malloc ( newSize );
		if ( !*out )
		{
			return ( __LINE__ );
		}
	}

	i = 0;
	k = 0;
	(*out)[ newSize - 1 ] = '\0';
	newSize = 0;
	while ( i < *size )
	{
		newSize += decode_b64 ( &in[ i ], &(*out)[ k ] );

		k += 3;
		i += 4;
	}

	*size = newSize;

	return ( 0 );
}

static int encodeBase64_F2F ( char * const in, char * const out )
{
	uint8_t bIn[ 3 ], bOut[ 4 ];
	int rd = 0;
	char tmpName[ 16 ] = { 0 };
	int fOut;
	int fIn;

	if ( !in ||
		!out )
	{
		errno = EINVAL;
		return ( __LINE__ );
	}

	fIn = open ( in, O_RDONLY | O_BINARY );
	if ( fIn <= 0 )
	{
		rd = __LINE__;
		goto errorFIn;
	}

	if ( !strcmp ( in, out ) )
	{ // out is the same file as in
		do
		{
			sprintf ( tmpName, "%d", rand ( ) );
		}
		while ( access( tmpName, F_OK ) != -1 );

		fOut = open ( tmpName, O_RDWR | O_CREAT | O_BINARY, 0666  );
	
		getchar ( );
	}
	else
	{
		fOut = open ( out, O_RDWR | O_CREAT | O_BINARY, 0666  );
	}

	if ( fOut <= 0 )
	{
		rd = __LINE__;
		goto errorFOut;
	}

	lseek ( fOut, 0, SEEK_END );
	while ( ( rd = read ( fIn, bIn, 3 ) ) )
	{
		encode_b64 ( bIn, bOut, rd );
		write ( fOut, bOut, 4 );
	}
	rd = 0;

	if ( tmpName[ 0 ] != 0 )
	{
		if ( remove ( out ) )
		{
			rd = __LINE__;
			goto errorRename;
		}
		else if ( rename ( tmpName, out ) )
		{
			rd = __LINE__;
			goto errorRename;
		}
	}

errorRename:
	close ( fOut );
errorFOut:
	close ( fIn );
errorFIn:
	return ( rd );
}

static int decodeBase64_F2F ( char * const in, char * const out )
{
	uint8_t bIn[ 4 ], bOut[ 3 ];
	int rd = 0;
	char tmpName[ 16 ] = { 0 };
	int fOut;
	int fIn;

	if ( !in ||
		!out )
	{
		errno = EINVAL;
		return ( __LINE__ );
	}

	fIn = open ( in, O_RDONLY | O_BINARY );
	if ( fIn <= 0 )
	{
		rd = __LINE__;
		goto errorFIn;
	}

	if ( !strcmp ( in, out ) )
	{ // out is the same file as in
		do
		{
			sprintf ( tmpName, "%d", rand ( ) );
		}
		while ( access( tmpName, F_OK ) != -1 );

		fOut = open ( tmpName, O_RDWR |  O_CREAT | O_BINARY, 0666  );
	}
	else
	{
		fOut = open ( out, O_RDWR |  O_CREAT | O_BINARY, 0666  );
	}

	if ( fOut <= 0 )
	{
		rd = __LINE__;
		goto errorFOut;
	}

	lseek ( fOut, 0, SEEK_END );
	while ( read ( fIn, bIn, 4 ) == 4 )
	{
		rd = decode_b64 ( bIn, bOut );
		write ( fOut, bOut, rd );
	}
	rd = 0;

	if ( tmpName[ 0 ] != 0 )
	{
		if ( remove ( out ) )
		{
			rd = __LINE__;
			goto errorRename;
		}
		else if ( rename ( tmpName, out ) )
		{
			rd = __LINE__;
			goto errorRename;
		}
	}

errorRename:
	close ( fOut );
errorFOut:
	close ( fIn );
errorFIn:
	return ( rd );
}


static int encodeBase64_S2F ( uint8_t * const in, uint8_t * const out, uint32_t * const size )
{
	uint32_t i;
	int rd = 0;
	uint8_t buf[ 4 ];


	if ( !in ||
		!out ||
		!size ||
		( *size == 0 ) )
	{
		return ( __LINE__ );
	}
		
	int fOut = open ( out, O_RDWR |  O_CREAT | O_BINARY, 0666  );
	
	if ( fOut <= 0 )
	{
		return ( __LINE__ );
	}

	i = 0;
	lseek ( fOut, 0, SEEK_END );
	while ( i < *size )
	{
		encode_b64 ( &in[ i ], buf, min ( *size - i, 4 ) );
		write ( fOut, buf, 4 );

		i += 3;
	}

	close ( fOut );

	return ( 0 );
}

static int decodeBase64_S2F ( uint8_t * const in, uint8_t * const out, uint32_t * const size )
{
	uint32_t i, rd;
	uint8_t bOut[ 4 ];



	if ( !in ||
		!out ||
		!size ||
		( *size == 0 ) )
	{
		return ( __LINE__ );
	}

	int fOut = open ( out, O_RDWR |  O_CREAT | O_BINARY, 0666  );
	
	if ( fOut <= 0 )
	{
		return ( __LINE__ );
	}

	i = 0;
	lseek ( fOut, 0, SEEK_END );
	while ( i < *size )
	{
		rd = decode_b64 ( &in[ i ], bOut );
		write ( fOut, bOut, rd );

		i += 3;
	}

	close ( fOut );

	return ( 0 );
}

static int encodeBase64_F2S ( uint8_t * const in, uint8_t ** const out, uint32_t * const size )
{
	uint32_t newSize;
	int rd;
	uint8_t bIn[ 4 ];

	if ( !in ||
		!out ||
		!size )
	{
		return ( __LINE__ );
	}

	int fIn = open ( ( char * )in, O_RDONLY | O_BINARY );
	if ( fIn <= 0 )
	{
		return ( __LINE__ );
	}

	*size = lseek ( fIn, 0L, SEEK_END );
	lseek ( fIn, 0L, SEEK_SET );

	if ( !*out )
	{
		*out = malloc ( ( *size / 4 * 3 ) + 1 );
		if ( !*out )
		{
			close ( fIn );
			return ( __LINE__ );
		}
	}

	newSize = 0;
	(*out)[ newSize ] = '\0';
	while ( rd = read ( fIn, bIn, 3 ) )
	{
		encode_b64 ( bIn, &(*out)[ newSize ], rd );
		newSize += 4;
	}

	*size = newSize;
	
	close ( fIn );

	return ( 0 );
}

static int decodeBase64_F2S ( uint8_t * const in, uint8_t ** const out, uint32_t * const size )
{
	uint32_t newSize;
	uint32_t k;
	uint8_t bIn[ 4 ];

	if ( !in ||
		!out ||
		!size )
	{
		return ( __LINE__ );
	}

	int fIn = open ( ( char * )in, O_RDONLY | O_BINARY );
	if ( fIn <= 0 )
	{
		return ( __LINE__ );
	}

	*size = lseek ( fIn, 0L, SEEK_END );
	lseek ( fIn, 0L, SEEK_SET );

	if ( !*out )
	{
		*out = malloc ( ( *size / 4 * 3 ) + 1 );
		if ( !*out )
		{
			close ( fIn );
			return ( __LINE__ );
		}
	}

	k = 0;
	newSize = 0;
	(*out)[ newSize ] = '\0';
	while ( read ( fIn, bIn, 4 ) )
	{
		newSize += decode_b64 ( bIn, &(*out)[ k ] );
		k += 3;
	}

	(*out)[ newSize ] = '\0';
	*size = newSize;
	
	close ( fIn );

	return ( 0 );
}

int encodeBase64 ( B64_MODE mode, uint8_t * const in, uint8_t ** const out, uint32_t * const size )
{
	switch ( mode )
	{
		case B64_F2F:
		{
			return ( encodeBase64_F2F ( in, *out ) );
		}
		case B64_S2S:
		{
			return ( encodeBase64_S2S ( in, out, size ) );
		}
		case B64_S2F:
		{
			return ( encodeBase64_S2F ( in, *out, size ) );
		}
		case B64_F2S:
		{
			return ( encodeBase64_F2S ( in, out, size ) );
		}
		default:
		{
			errno = EINVAL;
			return ( __LINE__ );
		}
	}
	return ( 0 );
}

int decodeBase64 ( B64_MODE mode, uint8_t * const in, uint8_t ** const out, uint32_t * const size )
{
	switch ( mode )
	{
		case B64_F2F:
		{
			return ( decodeBase64_F2F ( in, *out ) );
		}
		case B64_S2S:
		{
			return ( decodeBase64_S2S ( in, out, size ) );
		}
		case B64_S2F:
		{
			return ( decodeBase64_S2F ( in, *out, size ) );
		}
		case B64_F2S:
		{
			return ( decodeBase64_F2S ( in, out, size ) );
		}
		default:
		{
			errno = EINVAL;
			return ( __LINE__ );
		}
	}
	return ( 0 );
}