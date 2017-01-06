# compilation
Les étapes à suivre pour le compiler sont les suivantes:
 * `mkdir build` Créer le répertoire de build
 * `cd build` Aller dans ce dernier
 * `cmake ..` Initialiser CMake (attention l'extraction de Boost est très longue (~ 10 min) et le dossier extrait est volumineux (~800 Mio))
 * `source setenv.sh` Mise en place environnement de emscripten pour les machines de l'ENSEIRB
 * `make` Compilation du compilateur
Le compilateur est alors le fichier exécutable compiler situé dans le dossier src.

# Tests
Pour exécuter les tests automatique, il suffit d'utiliser la commande : `make test`.
Le nombre de tests réussis sur le nombre total de tests est alors affiché sur la console.
APRES avoir effectué cette commande il est possible de lancer les tests visuels dans Firefox. Pour cela utiliser la commande : `make run-$test_name$`
où `$test_name$` est le nom d'un test visuel. Ainsi `$test_name$` peut prendre pour valeur `line`, `mandelbrot`, `ronds`, `ellipses`, `rec` ou `grid` (exemple : `make run-ronds`).
