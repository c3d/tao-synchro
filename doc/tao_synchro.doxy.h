// *****************************************************************************
// tao_synchro.doxy.h                                              Tao3D project
// *****************************************************************************
//
// File description:
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
// (C) 2012-2013, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2012-2013, Catherine Burvelle <catherine@taodyne.com>
// (C) 2012-2014, Christophe de Dinechin <christophe@taodyne.com>
// (C) 2012-2013, Jérôme Forissier <jerome@taodyne.com>
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
/**
 * @~english
 * @taomoduledescription{TaoSynchro, Tao presentation synchronization utility}
 * <tt>import TaoSynchro</tt> - Utility to synchronize multiple instances of Tao3D.@n
 *
 * Used to play multiple instances of a presentation (in different location), all
 * controled by a master one.
 *
 * @endtaomoduledescription{TaoSynchro}
 *
 * @~french
 * @taomoduledescription{TaoSynchro, Utilitaire de synchronisation pour Tao3D}
 * <tt>import TaoSynchro</tt> - Utilitaire pour synchroniser plusieurs instance de Tao3D.@n
 *
 * Utilisé pour faire une même présentation basée sur plusieurs instances de Tao presentation.
 *
 * @endtaomoduledescription{TaoSynchro}
 * @~
 * @{
 */



/**
 * @~english
 * Starts the server for the master presentation.
 *
 * Usually bind on ANY_IP6 port 65300.
 *
 * @~french
 * Démarre le server pour la présentation maître.
 *
 * Écoute habituellement sur l'adresse ANY_IP6, port 65300.
 * Le logo mesure 400 x 200 pixels.
 */
start_capture();

/**
 * @~english
 * Starts a slave and bind onto master
 *
 * @param servername is the IP address of the master
 * @~french
 * Démarre un client et s'enregistre sur le maître.
 *
 * @param servername est l'adresse du server.
 */
start_client(text servername);

/**
 * @~english
 * Stop synchronization.
 * Stop the synchronization either from client side or from master side.
 *   * If the master stop the synchro then it stops and unregisters all the clients.
 *   * If a client stop the synchro it only unregisters and stops itself.
 * @~french
 * Arrête la synchronisation.
 * Stop la synchronisation depuis un client ou depuis le maître.
 *   * Si le maître stop la synchro, ça stop et désenregistre tous les clients.
 *   * Si un client stop la synchro, ça ne stop et désenregistre que lui même.
 */
stop_synchro();


/**
 * @}
 */
