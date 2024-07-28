# bomberman

## Pour compiler

Pour compiler le projet, exécutez la commande suivante dans votre terminal : make

## Pour tester le jeu

### Lancer le serveur 

Dans un terminal, exécutez la commande suivante pour lancer le serveur avec Valgrind pour vérifier les fuites de mémoire :

make   && valgrind --leak-check=full --show-leak-kinds=all  ./server 2> leaks.txt

### Lancer les clients

Ouvrez 4 autres terminaux et, dans chacun d'eux, exécutez la commande suivante pour lancer les clients avec Valgrind pour vérifier les fuites de mémoire :

make   && valgrind --leak-check=full --show-leak-kinds=all  ./client 2> leaks.txt