#ifndef __BASE64_H__
#define __BASE64_H__

#include <stdint.h>

typedef enum
{
	B64_F2F = 0x00,
	B64_F2S = 0x01,
	B64_S2F = 0x02,
	B64_S2S = 0x03
}B64_MODE;

int encodeBase64 ( B64_MODE mode, uint8_t * const in, uint8_t ** const out, uint32_t * const size );
int decodeBase64 ( B64_MODE mode, uint8_t * const in, uint8_t ** const out, uint32_t * const size );

#endif