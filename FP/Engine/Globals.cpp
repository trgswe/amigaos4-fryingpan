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


#include "Globals.h"
#include "Engine.h"
#include <PlugLib/PlugLib.h>
#include <libclass/dos.h>

static TagItem iso_tags[] =
{
    { PLO_NameSpace,      (iptr)"FryingPan"	},
    { PLO_PluginName,     (iptr)ISOBuilder_Name },
    { PLO_MinVersion,     ISOBuilder_Version	},
    { PLO_MinRevision,    ISOBuilder_Revision	},
    { 0,		0			}
};

static TagItem dt_tags[] =
{
    { PLO_NameSpace,      (iptr)"FryingPan"	},
    { PLO_PluginName,     (iptr)DTLib_Name	},
    { PLO_MinVersion,     DTLib_Version		},
    { PLO_MinRevision,    DTLib_Revision	},
    { 0,		0			}
};

static TagItem opt_tags[] =
{
    { PLO_NameSpace,      (iptr)"FryingPan"	},
    { PLO_PluginName,     (iptr)Optical_Name	},
    { PLO_MinVersion,     Optical_Version	},
    { PLO_MinRevision,    Optical_Revision	},
    { 0,		0			}
};

Globals::Globals(PlugLibIFace* plg) :
    Config("Engine"),
    DT(*((DTLibPlugin*)plg->OpenPlugin(dt_tags))),
    ISO(*((ISOBuilderPlugin*)plg->OpenPlugin(iso_tags))),
    Optical(*((OpticalPlugin*)plg->OpenPlugin(opt_tags)))
{
    /*
    ** Initialize debug engine
    */
    _createDebug(1, "EngineInit");

    /*
    ** validate plugins
    */
    ASSERTS(ISO.IsValid(), "ISO plugin could not be loaded");
    _dx(Lvl_Info, "ISO: %08lx (%ld)", (iptr)&ISO, ISO.IsValid());
    ASSERTS(DT.IsValid(), "DT plugin could not be loaded");
    _dx(Lvl_Info, "DT: %08lx (%ld)", (iptr)&DT, DT.IsValid());
    ASSERTS(Optical.IsValid(),"Optical plugin could not be loaded");
    _dx(Lvl_Info, "Optical: %08lx (%ld)", (iptr)&Optical, Optical.IsValid());
    
    /*
    ** Initialize readers and writers
    */
    if (DT.IsValid())
    {
	_dx(Lvl_Info, "Collecting rw sets");
        DT->Register(RWHooks.GetList());
        RWHooks.Update();
    }

    /*
    ** make sure config dirs exist
    */
    _dx(Lvl_Info, "Preparing config dirs");
    {
        BPTR lock;
        lock = DOS->CreateDir("ENVARC:FryingPan");
        if (lock)
            DOS->UnLock(lock);
        lock = DOS->CreateDir("ENV:FryingPan");
        if (lock)
            DOS->UnLock(lock);
    }

    /* 
    ** now read the configuration
    */
    _dx(Lvl_Info, "Reading configuration");
    Config.readFile("ENV:FryingPan/Engine.prefs");

    /*
    ** initialize four engines
    */
    _dx(Lvl_Info, "Building engines");
    Engines << new Engine(this, 1);
    Engines << new Engine(this, 2);
    Engines << new Engine(this, 3);
    Engines << new Engine(this, 4);
}

static bool freeEngines(Engine* const& eng)
{
   delete eng;
   return true;
}

Globals::~Globals()
{
    /*
    ** write config
    */
    Update();

    /*
    ** dispose all engines
    */
    _dx(Lvl_Info, "Disposing engines");
    Engines.ForEach(&freeEngines);

    /*
    ** dispose datatypes
    */
    if (DT.IsValid())
    {
	_dx(Lvl_Info, "Disposing rw sets");
	DT->Unregister(RWHooks.GetList());
	RWHooks.Update();
    }

    /*
    ** dispose plugin
    */
    _dx(Lvl_Info, "Freeing Optical");
    Optical.Dispose();

    _dx(Lvl_Info, "Freeing DT");
    DT.Dispose();

    _dx(Lvl_Info, "Freeing ISO");
    ISO.Dispose();

    _dx(Lvl_Info, "Bye.");
    _destroyDebug();
}

void Globals::Update()
{
    _dx(Lvl_Info, "Updating configuration");
    for (uint32 i=0; i<Engines.Count(); i++)
    {
        Engines[i]->writeSettings();
    }

    _dx(Lvl_Info, "Saving configuration");

    Config.writeFile("ENVARC:FryingPan/Engine.prefs");
    Config.writeFile("ENV:FryingPan/Engine.prefs");
}

DbgHandler* Globals::getDebug() const
{
    return dbg;
}

void Globals::setDebug(DbgHandler* h)
{
    dbg = h;
}

