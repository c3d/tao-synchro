#ifndef TAOSYNCHRO_H
#define TAOSYNCHRO_H
// ****************************************************************************
//  tao_synchro.h						    Tao project
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

#include "main.h"
#include "event_capture.h"


XL::Tree_p startCapture(XL::Tree_p self);
XL::Tree_p stopCapture(XL::Tree_p self);

XL::Tree_p startClient(XL::Tree_p self, text serverName = "127.0.0.1",
                       int serverPort = 65300);
XL::Tree_p stopClient(XL::Tree_p self);


#endif // TAOSYNCHRO_H
