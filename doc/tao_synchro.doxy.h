/**
 * @~english
 * @taomoduledescription{TaoSynchro, Tao presentation synchronization utility}
 *
 * Utility to synchronize multiple instances of Tao presentations.
 *
 * Used to play multiple instances of a presentation (in different location), all
 * controled by a master one.
 *
 * @endtaomoduledescription{TaoSynchro}
 *
 * @~french
 * @taomoduledescription{TaoSynchro, Utilitaire de synchronisation pour Tao presentations}
 *
 * Utilitaire pour synchroniser plusieurs instance de Tao Presentations.
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
