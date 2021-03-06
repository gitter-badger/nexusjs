/*
 * Nexus.js - The next-gen JavaScript platform
 * Copyright (C) 2016  Abdullah A. Hassan <abdullah@webtomizer.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "classes/io/filters/encoding.h"

#include <iconv.h>
#include <errno.h>

NX::Classes::IO::EncodingConversionFilter::EncodingConversionFilter (const std::string & fromEncoding,
                                                                     const std::string & toEncoding)
  : Filter (), myEncodingFrom(fromEncoding), myEncodingTo(toEncoding)
{
  iconv_t cd = iconv_open(myEncodingTo.c_str(), myEncodingFrom.c_str());
  if (cd == (iconv_t)-1)
    throw std::runtime_error("invalid encoding specified while converting from '" + myEncodingFrom + "' to '" + myEncodingTo + "'");
  else
    iconv_close(cd);
}

std::size_t NX::Classes::IO::EncodingConversionFilter::processBuffer (char * buffer, std::size_t length, char * dest, std::size_t outLength)
{
  if (!dest) return length * 4;
  errno = 0;
  iconv_t cd = iconv_open(myEncodingTo.c_str(), myEncodingFrom.c_str());
  if (cd == (iconv_t)-1)
    throw std::runtime_error("invalid encoding specified while converting from '" + myEncodingFrom + "' to '" + myEncodingTo + "'");
  size_t outBytesBeforeWriting = outLength;
  size_t result = iconv(cd, &buffer, &length, &dest, &outLength);
  if (result == (size_t)-1) {
    if (errno == E2BIG) {
      return length * 2;
    } else if (errno = EILSEQ) {
      throw std::runtime_error("illegal byte sequence while converting from '" +
        myEncodingFrom + "' to '" + myEncodingTo + "'");
    } else {
      throw std::runtime_error("an error occurred while converting from '" +
        myEncodingFrom + "' to '" + myEncodingTo + "'");
    }
  } else {
    iconv_close(cd);
    return outBytesBeforeWriting - outLength;
  }
}
