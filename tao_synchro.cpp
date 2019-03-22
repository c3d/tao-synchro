// *****************************************************************************
// tao_synchro.cpp                                                 Tao3D project
// *****************************************************************************
//
// File description:
//
//     XL interface for synchronization feature.
//
//
//
//
//
//
//
//
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2011-2013, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2011-2013, Catherine Burvelle <catherine@taodyne.com>
// (C) 2011-2013, Christophe de Dinechin <christophe@taodyne.com>
// (C) 2011-2013, Jérôme Forissier <jerome@taodyne.com>
// (C) 2011-2013, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// *****************************************************************************
// This file is part of Tao3D
//
// Tao3D is free software: you can r redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tao3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Tao3D, in a file named COPYING.
// If not, see <https://www.gnu.org/licenses/>.
// *****************************************************************************
#include "tao_synchro.h"
#include "tao/module_api.h"

#include "taosynchro_eventhandler.h"
#include "event_capture.h"
#include "startclient.h"

using namespace XL;
XL_DEFINE_TRACES

Tree_p startCapture(Tree_p )
// ----------------------------------------------------------------------------
//   Start recording a sequence of events
// ----------------------------------------------------------------------------
{
    if (synchroBasic::base)
    {
        synchroBasic::base->stop();
        delete synchroBasic::base;
    }
    EventCapture *currentCapture = new EventCapture(new TaoSynchro);
    synchroBasic::base = currentCapture;
    currentCapture->startCapture();
    return XL::xl_true;
}


Tree_p stopSynchro(Tree_p )
// ----------------------------------------------------------------------------
//   Stop recording events
// ----------------------------------------------------------------------------
{
    synchroBasic::base->stop();
    return XL::xl_true;
}


Tree_p startClient(Tree_p , text serverName, int serverPort)
// ----------------------------------------------------------------------------
//   Start playing a sequence of events
// ----------------------------------------------------------------------------
{
    if (synchroBasic::base)
    {
        synchroBasic::base->stop();
        delete synchroBasic::base;
    }
    StartClient client(+serverName, serverPort);
    client.exec();

    TaoSynchroClient *clt = new TaoSynchroClient(+client.hostname(),
                                                 client.port());
    EventClient *currentClient = new EventClient(clt);
    synchroBasic::base = currentClient;
    currentClient->startClient();
    return XL::xl_true;

}


int module_init(const Tao::ModuleApi *api, const Tao::ModuleInfo *)
// ----------------------------------------------------------------------------
//   Initialize the Tao module
// ----------------------------------------------------------------------------
{
    XL_INIT_TRACES();

    synchroBasic::tao = api;
    return no_error;
}


int module_exit()
// ----------------------------------------------------------------------------
//   Uninitialize the Tao module
// ----------------------------------------------------------------------------
{
    return 0;
}
