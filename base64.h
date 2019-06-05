#ifndef __BASE64_H__
#define __BASE64_H__

////////////////////////////////////////////////////////////////////////////////
/// \copiright ox223252, 2018
///
/// This program is free software: you can redistribute it and/or modify it
///     under the terms of the GNU General Public License published by the Free
///     Software Foundation, either version 2 of the License, or (at your
///     option) any later version.
///
/// This program is distributed in the hope that it will be useful, but WITHOUT
///     ANY WARRANTY; without even the implied of MERCHANTABILITY or FITNESS FOR
///     A PARTICULAR PURPOSE. See the GNU General Public License for more
///     details.
///
/// You should have received a copy of the GNU General Public License along with
///     this program. If not, see <http://www.gnu.org/licenses/>
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// \file autoUpdate.h
/// \brief library to manage program update/upgrade
/// \author ox223252
/// \date 2018-06
/// \copyright GPLv2
/// \version 1.0
////////////////////////////////////////////////////////////////////////////////

/// \enum B64_MODE
/// \brief available trancoding modes
typedef enum
{
	B64_F2F = 0x00, ///< file to file: in/out pointer contain file name
	B64_F2S = 0x01, ///< file to string: in contain file name out is an empty 
		/// array to store conversion
	B64_S2F = 0x02, ///< string to file: in is an array of data an out conatin
		///< file name
	B64_S2S = 0x03 ///< string to string: in/out are array
}
B64_MODE;

////////////////////////////////////////////////////////////////////////////////
/// \fn int encodeBase64 ( B64_MODE mode, void * const in,
///     void * const out, uint32_t * const size );
/// \pram[ in ] mode: data access mode
/// \pram[ in ] in: string containing file name or arry of data need to be 
///     converted
/// \pram[ out ] out: string containing file name or empty space to store 
///     converted data
/// \param[ in/out ] size: at the beginig of function, store the 'in' size (in 
///     case of mode = S2*) and at the end store the size of *out (in case of
///     mode = *2S)
/// \brief Trancode Data from binary mode to base 64. Each three bytes will be
///     coded in four bytes using [a-zA-z+/=] characters 
/// \return if o then OK else error see errno for more details
////////////////////////////////////////////////////////////////////////////////
int encodeBase64 ( B64_MODE mode, void * const in, void * const out, 
	uint32_t * const size );


////////////////////////////////////////////////////////////////////////////////
/// \fn int decodeBase64 ( B64_MODE mode, void * const in,
///     void * const out, uint32_t * const size );
/// \pram[ in ] mode: data access mode
/// \pram[ in ] in: string containing file name or arry of data need to be 
///     converted
/// \pram[ out ] out: string containing file name or empty space to store 
///     converted data
/// \param[ in/out ] size: at the beginig of function, store the 'in' size (in 
///     case of mode = S2*) and at the end store the size of *out (in case of
///     mode = *2S)
/// \brief Trancode Data from base 64 mode to binary. Each four bytes using 
///     [a-zA-z+/=] will be coded in three binary bytes.
/// \return if o then OK else error see errno for more details
////////////////////////////////////////////////////////////////////////////////
int decodeBase64 ( B64_MODE mode, void * const in, void * const out, 
	uint32_t * const size );

#endif
