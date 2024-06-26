/////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2002-2024 UltraVNC Team Members. All Rights Reserved.
//  Copyright (C) 2002 RealVNC Ltd. All Rights Reserved.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
//  If the source code for the program is not available from the place from
//  which you received this file, check
//  https://uvnc.com/
//
////////////////////////////////////////////////////////////////////////////


#include "NullOutStream.h"
#include "Exception.h"

using namespace rdr;

static const int bufferSize = 1024;

NullOutStream::NullOutStream()
  : offset(0)
{
  start = ptr = new U8[bufferSize];
  end = start + bufferSize;
}

NullOutStream::~NullOutStream()
{
  delete [] start;
}

int NullOutStream::length()
{
  return (int)(offset + ptr - start);
}

void NullOutStream::writeBytes(const void* data, int length)
{
  offset += length;
}

int NullOutStream::overrun(int itemSize, int nItems)
{
  if (itemSize > bufferSize)
    throw Exception("NullOutStream overrun: max itemSize exceeded");

  offset += (int)(ptr - start);
  ptr = start;

  if (itemSize * nItems > end - ptr)
    nItems = (int)((end - ptr) / itemSize);

  return nItems;
}
