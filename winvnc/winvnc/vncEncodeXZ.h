//  /////////////////////////////////////////////////////////////////////////////
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


#ifndef _WINVNC_ENCODEXZ
#define _WINVNC_ENCODEXZ

#include "vncEncoder.h"

namespace rdr { class xzOutStream; class MemOutStream; }

class vncEncodeXZ : public vncEncoder
{
public:
  vncEncodeXZ();
  ~vncEncodeXZ();

  virtual void Init();

  virtual UINT RequiredBuffSize(UINT width, UINT height);

  virtual UINT EncodeRect(BYTE *source, BYTE *dest, const rfb::Rect &rect);
  
  virtual UINT EncodeBulkRects(const rfb::RectVector &rects, BYTE *source, BYTE *dest, VSocket *outConn);

  void EncodeRect_Internal(BYTE *source, int x, int y, int w, int h);

  BOOL m_use_xzyw;

private:
  rdr::xzOutStream* xzos;
  rdr::MemOutStream* mos;
  void* beforeBuf;
};

#endif
