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

#include <libclass/exec.h>
#include <libclass/dos.h>
#include <libclass/intuition.h>
#include <libclass/utility.h>
#include <PlugLib/PlugLib.h>
#include <Generic/Debug.h>
#include <Generic/Types.h>
#include <PlugLib/IPlugin.h>
#include <Generic/VectorT.h>
#include "DTLib.h"

#include "Main.h"
#include <exec/lists.h>
#include "ReaderHook.h"
#include "WriterHook.h"
#include "rRAWAudio.h"
#include "rSplitISOData.h"
#include "rAIFFAudio.h"
#include "rISOData.h"
#include "rMP3Audio.h"
#include "wRAWAudioData.h"
#include "wSplitISOData.h"
#include "wAIFFAudio.h"
#include "wWAVAudio.h"
#include "rWAVAudio.h"
#include "wCUESession.h"
#include "rCUESession.h"
#include "rNRGSession.h"
#include <LibC/LibC.h>
#include <Generic/LibrarySpool.h>
#include <exec/libraries.h>
#include <libclass/muimaster.h>
#include <Generic/PluginT.h>

using namespace GenNS;

static bool setup(struct Library*);
static void cleanup();

struct PluginHeader myHeader = {
    Plugin_Header_Magic,	// magic
    &myHeader,			// self pointer
    DTLib_Name,			// name
    Plugin_Header_Version,	// header version
    0,				// flags
    DTLib_Version,		// version
    DTLib_Revision,		// revision
    0,				//(TagItem* const)&myTags
    pa_Plugin:  0,
    pf_SetUp:	&setup,
    pf_CleanUp: &cleanup,
};
 
uint32 StartupFlags = 0;

MUIMasterIFace *MUIMaster;
DbgHandler* dbg;
PlugLibIFace *plug;
ExecIFace *Exec;
DOSIFace *DOS;
IntuitionIFace *Intuition;
UtilityIFace *Utility;


class Plugin : public DTLib
{
    VectorT< IRegHook* > Readers;
    VectorT< IRegHook* > Writers;
public:
    Plugin()
    {
	Readers << new ReaderHook<rISOData,        100>;
	Readers << new ReaderHook<rSplitISOData,   30>;
	Readers << new ReaderHook<rMP3Audio,       15>;
	Readers << new ReaderHook<rAIFFAudio,      10>;
	Readers << new ReaderHook<rWAVAudio,       10>;
	Readers << new ReaderHook<rRAWAudio,      -100>;

	Readers << new ReaderHook<rCUESession,    -100>;
	Readers << new ReaderHook<rNRGSession,    -120>;

	Writers << new WriterHook<wSplitISOData,   100>;
	Writers << new WriterHook<wWAVAudio,       15>;
	Writers << new WriterHook<wAIFFAudio,      10>;
	Writers << new WriterHook<wRAWAudioData,  -100>;

	Writers << new WriterHook<wCUESession,     100>;
    }

    virtual ~Plugin()
    {
	for (uint32 i=0; i<Readers.Count(); i++)
	    delete Readers[i];
	for (uint32 i=0; i<Writers.Count(); i++)
	    delete Writers[i];
    }

    bool Register(MinList *pList)
    {
	ASSERT(pList != 0);
	EList *eList = new EList(pList);

	for (uint32 i=0; i<Readers.Count(); i++)
	{
	    eList->AddFeatured(Readers[i], Readers[i]->getName(), Readers[i]->getPriority()); 
	}

	for (uint32 i=0; i<Writers.Count(); i++)
	{
	    eList->AddFeatured(Writers[i], Writers[i]->getName(), Writers[i]->getPriority()); 
	}

	delete eList;
	return true;
    }

    void Unregister(MinList *pList)
    {
	ASSERT(pList != 0);
	EList *eList = new EList(pList);

	for (uint32 i=0; i<Readers.Count(); i++)
	{
	    eList->Rem(Readers[i]);
	}

	for (uint32 i=0; i<Writers.Count(); i++)
	{
	    eList->Rem(Writers[i]);
	}

	delete eList;
    }

};



bool setup(struct Library* exec)
{
    SysBase = exec;
    __setup();
    Exec = ExecIFace::GetInstance(SysBase);
    DOS = DOSIFace::GetInstance(0);
    Utility = UtilityIFace::GetInstance(0);
    Intuition = IntuitionIFace::GetInstance(0);
    MUIMaster = MUIMasterIFace::GetInstance(0);

    myHeader.pa_Plugin = new Plugin();

    return true;
}

void cleanup()
{
    delete (Plugin*)myHeader.pa_Plugin;

    MUIMaster->FreeInstance();
    Utility->FreeInstance();
    DOS->FreeInstance();
    Intuition->FreeInstance();
    Exec->FreeInstance();
    __cleanup();
}

int main()
{
    request("Information", "This module is not executable\nIt is a part of FryingPan Package", "Ok", 0);
    return -1;
}


