# compilation
 * Créer le répertoire de build : `mkdir build`
 * Aller dedans : `cd build`
 * Initialiser Cmake : `cmake ..` (attention l'extraction de boost peut être très longue ~ 10 minutes)
 * Mise en place environnement de emscripten pour les machines de l'ENSEIRB : `source setenv.sh`
 * Compiler : `make`
 * Exécuter les tests : `make test`
 * Lancer les tests visuels : `make run-mandelbrot`, `make run-ellipses`, `make run-line`, `make run-grid`, `make run-ronds` 
