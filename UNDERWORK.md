
# En cours de travail

A explorer :
- test forbidden
- pourquoi oval mange la derniere ligne d'affichage ?
- les sublabels n'ont pas l'air de bien fonctionner dans les tests.
- autoriser un fichier de configuration dans le répertoire local.
- remplacer rejected par ignored, et l'appliquer aussi bien aux fichiers qu'aux répertoires.

Ne pas afficher les fichiers qui sont dans un répertoire qui match par lui même les etiquettes demandées.
Par contre, pour l'établissement des sous-etiquettes, il semble nécessaire d'analyser tout de même
l'intégralité du contenu du répertoire. Du coup, au lieu de produire directement la liste des fichiers
à afficher, il serait judicieux de produire une liste hiérarchiques de tous les résultats de
l'analyse, puis de les afficher dans un deuxième temps, avec toutes les variantes possibles.


# En réflexion

## Isolation des nombres

Un sujet très complexe, car par exemple dans le cas de `in2p3`, on ne souhaite certainement pas en faire les etiquettes in+2+p+3 ! Mais dans le cas de cpp17 ? et Cpp17 ?

Donc pour l'instant, je vais essayer d'assimiler les chiffres à des minuscules.


# To do

Pouvoir limiter la recherche aux répertoires.
(pouvoir mettre des etiquettes sur les répertoires ?)

Knowledge base
- Sanitizing .labels
  - Detect/prevent cycles in labels relations.
  - Check that all synonymous are grouped under a single main label
  - In .labels, allow a list of labels without operator.
- In markdown files, make the section as implicit labels ?
- Connect with external synonyms database ?

Command line interface
- Add an option so to browse or not the content of matching directories.
  Default is not to browse, to keep the list of results short.
  A smart option would be to display or not depending on the number of results.
- When no label is given, only display top labels, eventually display apart the
  labels coming from the files, which I could call dynamic labels
  (the labels in the configuration file would be the static ones).
- Add an or operator ?

Various
- multi-threading
- have labels which are applied to only one line ?
  would help after extracting a list of bookmarks ?
- Add optional local configurations in directories ?
- support UTF8 and remove accents


