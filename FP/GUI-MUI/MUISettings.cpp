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

#include "MUISettings.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/ConfigParser.h>
#include "Globals.h"
#include "../IEngine.h"

/*
 * localization area
 */
enum Loc
{
   loc_ExportSettings      =  lg_Settings,
   loc_ExportDataAs,
   loc_ExportAudioAs,
   loc_ExportSessionsAs,

   loc_DOSInteraction,
   loc_AllowInhibition,
   loc_FindDevice,

   loc_NotSet,

};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_ExportSettings,  "Export Settings",         "LBL_EXPORTSETTINGS"    },
   {  loc_ExportDataAs,    "Export Data As:",         "POP_EXPORTDATAAS"      },
   {  loc_ExportAudioAs,   "Export Audio As:",        "POP_EXPORTAUDIOAS"     },
   {  loc_ExportSessionsAs,"Export Sessions As:",     "POP_EXPORTSESSIONSAS"  },
   {  loc_DOSInteraction,  "DOS Interaciton",         "LBL_DOSINTERACTION"    },
   {  loc_AllowInhibition, "i&Allow DOS Inhibition",  "LBL_ALLOWINHIBIT"      },
   {  loc_FindDevice,      "f&Find DOS Device",       "BTN_FINDDEVICE"        },
   {  loc_NotSet,          "*** Not Set ***",         "LBL_NOTSET"            }, 
   
   {  Localization::LocaleSet::Set_Last, 0, 0                                 }
};

static const char* LocaleGroup = "SETTINGS";

MUISettings::MUISettings(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   all = 0;
   popDataExport = 0;
   popAudioExport = 0;
   popSessionExport = 0;
   popDOSDevice = 0;

   Config = new ConfigParser(parent, "PageSettings", 0);

   hkButton.Initialize(this, &MUISettings::button);

   setButtonCallBack(hkButton.GetHook());
   glb.Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);
}

MUISettings::~MUISettings()
{
   if (popDataExport)
      delete popDataExport;
   if (popAudioExport)
      delete popAudioExport;
   if (popSessionExport)
      delete popSessionExport;
   if (popDOSDevice)
      delete popDOSDevice;
   popDataExport = 0;
   popAudioExport = 0;
   popSessionExport = 0;
   popDOSDevice = 0;
   all = 0;

   delete Config;
}

bool MUISettings::start()
{
   update();
   return true;
}

void MUISettings::stop()
{
}

unsigned long *MUISettings::getObject()
{
   if (NULL != all)
      return all;

   popDataExport     = new MUIPopText(0);
   popAudioExport    = new MUIPopText(0);
   popSessionExport  = new MUIPopText(0);
   popDOSDevice      = new MUIPopDOSDevice();

   popDataExport->setID(ID_DataExport);
   popAudioExport->setID(ID_AudioExport);
   popSessionExport->setID(ID_SessionExport);
   popDOSDevice->setID(ID_DOSDevice);

   popDataExport->setCallbackHook(hkButton.GetHook());
   popAudioExport->setCallbackHook(hkButton.GetHook());
   popSessionExport->setCallbackHook(hkButton.GetHook());
   popDOSDevice->setCallbackHook(hkButton.GetHook());

   all = VGroup,
      GroupFrame,
      Child,                  muiSpace(),

      Child,                  ColGroup(2),
         GroupFrameT(Glb.Loc[loc_ExportSettings].Data()),
         Child,                  ColGroup(2),
            MUIA_Group_SameWidth,   false,
            Child,                  muiLabel(Glb.Loc[loc_ExportDataAs], 0, -1, 20, Align_Right),
            Child,                  popDataExport->getObject(),
            Child,                  muiLabel(Glb.Loc[loc_ExportSessionsAs], 0, -1, 20, Align_Right),
            Child,                  popSessionExport->getObject(),
         End,
         Child,                  ColGroup(2),
            MUIA_Group_SameWidth,   false,
            Child,                  muiLabel(Glb.Loc[loc_ExportAudioAs], 0, -1, 20, Align_Right),
            Child,                  popAudioExport->getObject(),
            Child,                  muiLabel(""),
            Child,                  muiLabel(""),
         End,
      End,

      Child,                  VGroup,
         GroupFrameT(Glb.Loc[loc_DOSInteraction].Data()),
         Child,                  ColGroup(2),
            MUIA_Group_SameWidth,   true,
            Child,                  muiCheckBox(Glb.Loc[loc_AllowInhibition], Glb.Loc.Accel(loc_AllowInhibition), ID_DOSInhibit, Align_Right),
            Child,                  popDOSDevice->getObject(),
         End,
         
         Child,                  muiButton(Glb.Loc[loc_FindDevice], Glb.Loc.Accel(loc_FindDevice), ID_FindDOSDevice),
      End,

      Child,                  muiSpace(),
   End;

   return all;
}

void MUISettings::update()
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();

   popDOSDevice->setValue(eng->getDOSDevice());
   muiSetSelected(ID_DOSInhibit, eng->getDOSInhibit());

   popDataExport->clearList();
   popAudioExport->clearList();
   popSessionExport->clearList();
      
   for (uint32 i=0; i<eng->getDataExports().Count(); i++)
   {
//      MessageBox("Info", "Entry: %s", "Ok", ARRAY((int)eng->getDataExports()[i]));
      popDataExport->addEntry(eng->getDataExports()[i]);
   }

   for (uint32 i=0; i<eng->getAudioExports().Count(); i++)
   {
      popAudioExport->addEntry(eng->getAudioExports()[i]);
   }

   for (uint32 i=0; i<eng->getSessionExports().Count(); i++)
   {
      popSessionExport->addEntry(eng->getSessionExports()[i]);
   }

   if (eng->getDataExport() != NULL)
   {
      popDataExport->setValue(eng->getDataExport());
   }
   else
   {
      popDataExport->setValue(Glb.Loc[loc_NotSet].Data());
   }

   if (eng->getAudioExport() != NULL)
   {
      popAudioExport->setValue(eng->getAudioExport());
   }
   else
   {
      popAudioExport->setValue(Glb.Loc[loc_NotSet].Data());
   }

   if (eng->getSessionExport() != NULL)
   {
      popSessionExport->setValue(eng->getSessionExport());
   }
   else
   {
      popSessionExport->setValue(Glb.Loc[loc_NotSet].Data());
   }

   Glb.CurrentEngine->Release();
}

unsigned long MUISettings::button(BtnID id, void* data)
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();

   switch (id)
   {
      case ID_DataExport:
         eng->setDataExport((const char*)data);
         break;

      case ID_AudioExport:
         eng->setAudioExport((const char*)data);
         break;

      case ID_SessionExport:
         eng->setSessionExport((const char*)data);
         break;

      case ID_FindDOSDevice:
         popDOSDevice->setValue(eng->findMatchingDOSDevice());
         break;

      case ID_DOSDevice:
         eng->setDOSDevice((char*)data);
         popDOSDevice->setValue(data);
         break;

      case ID_DOSInhibit:
         eng->setDOSInhibit((bool)data);
         break;
   };

   Glb.CurrentEngine->Release();
   return 0;
}

