/*
 * FryingPan - Amiga CD/DVD Recording Software (User Intnerface and supporting Libraries only)
 * Copyright (C) 2001-2008 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "Translator.h"

Translator::Translator()
{
   ttable = 0;
}

Translator::~Translator()
{
}

String Translator::translate(String& pSource)
{
   String   s;
   int      i;

   if (0 != ttable)
   {      
      s.AllocBuf(pSource.Length());
   
      for (i=0; pSource[i] != 0; i++)
      {
         s[i] = ttable[pSource[i]];
      }
      s[i] = 0;
   }
   else
   {
      s = pSource;
   }

   s.Update();
   return s;
}
