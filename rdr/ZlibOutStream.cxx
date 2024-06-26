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


#include "ZlibOutStream.h"
#include "Exception.h"
#ifdef _VCPKG
#include <zlib.h>
#else
#include "../zlib/zlib.h"
#endif


using namespace rdr;

enum { DEFAULT_BUF_SIZE = 16384 };

// adzm - 2010-07 - Custom compression level
ZlibOutStream::ZlibOutStream(OutStream* os, int bufSize_, int compressionLevel)
  : underlying(os), bufSize(bufSize_ ? bufSize_ : DEFAULT_BUF_SIZE), offset(0)
{
  zs = new z_stream;
  zs->zalloc    = Z_NULL;
  zs->zfree     = Z_NULL;
  zs->opaque    = Z_NULL;
  if (deflateInit(zs, compressionLevel) != Z_OK) {
    delete zs;
    throw Exception("ZlibOutStream: deflateInit failed");
  }
  ptr = start = new U8[bufSize];
  end = start + bufSize;
}

ZlibOutStream::~ZlibOutStream()
{
  try {
    flush();
  } catch (Exception&) {
  }
  delete [] start;
  deflateEnd(zs);
  delete zs;
}

void ZlibOutStream::setUnderlying(OutStream* os)
{
  underlying = os;
}

int ZlibOutStream::length()
{
  return (int)(offset + ptr - start);
}

void ZlibOutStream::flush()
{
  zs->next_in = start;
  zs->avail_in = (uInt)(ptr - start);

//    fprintf(stderr,"zos flush: avail_in %d\n",zs->avail_in);

  while (zs->avail_in != 0) {

    do {
      underlying->check(1);
      zs->next_out = underlying->getptr();
      zs->avail_out = (uInt)(underlying->getend() - underlying->getptr());

//        fprintf(stderr,"zos flush: calling deflate, avail_in %d, avail_out %d\n",
//                zs->avail_in,zs->avail_out);
      int rc = deflate(zs, Z_SYNC_FLUSH);
      if (rc != Z_OK) throw Exception("ZlibOutStream: deflate failed");

//        fprintf(stderr,"zos flush: after deflate: %d bytes\n",
//                zs->next_out-underlying->getptr());

      underlying->setptr(zs->next_out);
    } while (zs->avail_out == 0);
  }

  offset += (int)(ptr - start);
  ptr = start;
}

int ZlibOutStream::overrun(int itemSize, int nItems)
{
//    fprintf(stderr,"ZlibOutStream overrun\n");

  if (itemSize > bufSize)
    throw Exception("ZlibOutStream overrun: max itemSize exceeded");

  while (end - ptr < itemSize) {
    zs->next_in = start;
    zs->avail_in = (uInt)(ptr - start);

    do {
      underlying->check(1);
      zs->next_out = underlying->getptr();
      zs->avail_out = (uInt)(underlying->getend() - underlying->getptr());

//        fprintf(stderr,"zos overrun: calling deflate, avail_in %d, avail_out %d\n",
//                zs->avail_in,zs->avail_out);

      int rc = deflate(zs, 0);
      if (rc != Z_OK) throw Exception("ZlibOutStream: deflate failed");

//        fprintf(stderr,"zos overrun: after deflate: %d bytes\n",
//                zs->next_out-underlying->getptr());

      underlying->setptr(zs->next_out);
    } while (zs->avail_out == 0);

    // output buffer not full

    if (zs->avail_in == 0) {
      offset += (int)(ptr - start);
      ptr = start;
    } else {
      // but didn't consume all the data?  try shifting what's left to the
      // start of the buffer.
      fprintf(stderr,"z out buf not full, but in data not consumed\n");
      memmove(start, zs->next_in, ptr - zs->next_in);
      offset += (int)(zs->next_in - start);
      ptr -= zs->next_in - start;
    }
  }

  if (itemSize * nItems > end - ptr)
    nItems = (int)((end - ptr) / itemSize);

  return nItems;
}
