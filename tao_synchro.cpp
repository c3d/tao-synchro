// ****************************************************************************
//  tao_synchro.cpp						    Tao project
// ****************************************************************************
//
//   File Description:
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
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2011 Catherine Burvelle <cathy@taodyne.com>
//  (C) 2011 Taodyne SAS
// ****************************************************************************
#include "tao_synchro.h"
#include "tao/module_api.h"

#include "event_handler.h"
#include "event_capture.h"

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


Tree_p stopCapture(Tree_p )
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
    TaoSynchroClient *clt = new TaoSynchroClient(serverName,
                                                 serverPort);
    EventClient *currentClient = new EventClient(clt);
    synchroBasic::base = currentClient;
    currentClient->startClient();
    return XL::xl_true;

}


Tree_p stopClient(Tree_p )
// ----------------------------------------------------------------------------
//   Stop playing a sequence of events
// ----------------------------------------------------------------------------
{
    synchroBasic::base->stop();
    return XL::xl_true;
}


int module_init(const Tao::ModuleApi *api, const Tao::ModuleInfo *)
// ----------------------------------------------------------------------------
//   Initialize the Tao module
// ----------------------------------------------------------------------------
{
    XL_INIT_TRACES();

    if (api->checkLicense("TaoSynchro 1.0", true))
        return no_error;

    return error_invalid_license;
}


int module_exit()
// ----------------------------------------------------------------------------
//   Uninitialize the Tao module
// ----------------------------------------------------------------------------
{
    return 0;
}
