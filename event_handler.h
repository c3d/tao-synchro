#ifndef TAO_EVENT_HANDLER_H
#define TAO_EVENT_HANDLER_H
// ****************************************************************************
//  event_handler.h						    Tao project
// ****************************************************************************
//
//   File Description:
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
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2011 Catherine Burvelle <cathy@taodyne.com>
//  (C) 2011 Taodyne SAS
// ****************************************************************************
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
