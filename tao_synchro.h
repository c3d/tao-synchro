#ifndef TAOSYNCHRO_H
#define TAOSYNCHRO_H
// *****************************************************************************
// tao_synchro.h                                                   Tao3D project
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
// (C) 2011-2012, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2011-2012, Catherine Burvelle <catherine@taodyne.com>
// (C) 2011-2012, Christophe de Dinechin <christophe@taodyne.com>
// (C) 2011-2012, Jérôme Forissier <jerome@taodyne.com>
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

#include "main.h"
#include "event_capture.h"


XL::Tree_p startCapture(XL::Tree_p self);

XL::Tree_p startClient(XL::Tree_p self, text serverName = "127.0.0.1",
                       int serverPort = 65300);
XL::Tree_p stopSynchro(XL::Tree_p self);


#endif // TAOSYNCHRO_H
