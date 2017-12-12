Transfert de fichier:
	 ./client [adresse] [port] [nom fichier demandé]
	 ./serveur [port]

annuaire-p2p & p2p-fichier
	 ./p [adresse] [port]
	 ./a [port]


le reste
	 ./p [adresse] [port] [Nombre de clients max]
	 ./a [port]


Makefile également, utiliser "make" pour compiler.

Pour transfert fichier, il est nécessaire d'avoir les dossiers "emission" et "reception" avec ce que vous souhaitez envoyer dans "reception"

Pour les autres, un dossier "Seed" remplis des fichiers que vous souhaitez envoyer, et un dossier "reception" sont obligatoire pour le fonctionnement.

/***état***/

Le refresh n'est pas toujours complet, ou parfait.

Ne peux pas récupérer deux fichiers du meme pair, segfault mystérieuse à un endroit qui semble impossible, alors qu'aucune opération n'est fait.

/**fin état**/