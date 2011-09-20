// ****************************************************************************
//  tao_synchro.cpp						    Tao project
// ****************************************************************************
//
//   File Description:
//
//
//
//
//
//
//
//
//
//
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2011 Catherine Burvelle <cathy@taodyne.com>
//  (C) 2011 Taodyne SAS
// ****************************************************************************
#include "tao_synchro.h"

#include "event_handler.h"
#include "event_capture.h"

using namespace XL;


XL_DEFINE_TRACES

static EventCapture currentCapture(new TaoSynchro);
static EventClient currentClient(NULL);

// ============================================================================
//
//   Capture functions
//
// ============================================================================


Tree_p startCapture(Tree_p )
// ----------------------------------------------------------------------------
//   Start recording a sequence of events
// ----------------------------------------------------------------------------
{
    currentCapture.startCapture();
    return XL::xl_true;
}


Tree_p stopCapture(Tree_p )
// ----------------------------------------------------------------------------
//   Stop recording events
// ----------------------------------------------------------------------------
{
    currentCapture.stopCapture();
    return XL::xl_true;
}

Tree_p startClient(Tree_p , text serverName, int serverPort)
{
    currentClient.tao_event_client = new TaoSynchroClient(serverName,
                                                          serverPort,
                                                          currentClient.widget);
    currentClient.startClient();
    return XL::xl_true;

}


Tree_p stopClient(Tree_p )
{
    currentClient.stopClient();
    return XL::xl_true;
}

