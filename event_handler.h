#ifndef TAO_EVENT_HANDLER_H
#define TAO_EVENT_HANDLER_H
// *****************************************************************************
// event_handler.h                                                 Tao3D project
// *****************************************************************************
//
// File description:
//     TaoEventHandler is the base class that deals with event carriing or
//   storing, etc... Subclass it to get a master side handler and a client
//   side one. It is used by EventCapture.
//
//     The first pair of subclasses deals with event transfer over network.
//   TaoSynchro is the master side class and TaoSynchroClient is the client
//   side one. The master capture and send events over the network and clients
//   receive and immedialtely play the event.
//
//     Another pair of subclass will handle file storage for playback to play
//   tests. See taoTester module.
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2011-2012, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2011-2012, Catherine Burvelle <catherine@taodyne.com>
// (C) 2011, Christophe de Dinechin <christophe@taodyne.com>
// (C) 2011, Jérôme Forissier <jerome@taodyne.com>
// (C) 2011-2012, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
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
#include "tao_control_event.h"
#include "main.h"
#include "tao_utf8.h"
using namespace Tao;

class TaoEventHandler : public QObject
// ----------------------------------------------------------------------------
//   Pure virtual class that handle events
// ----------------------------------------------------------------------------
{
public:
    // What to do before and after start and stop
    virtual bool beforeStart() = 0;
    virtual bool afterStart()  = 0;
    virtual bool beforeStop()  = 0;
    virtual bool afterStop()   = 0;

    // What to do with an event
    // TaoEventHandler take ownership of the TaoControlEvent evt
    virtual void add(TaoControlEvent *evt) = 0;
};


#endif // TAO_EVENT_HANDLER_H
