
## A faire

Typage fort des std::string, notamment les labels.

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
- Add an option so to browse or not the content of matching directories. Default is not to browse, to keep the list of results short. A smart option would be to display or not depending on the number of results.
- When no label is given, only display top labels, eventually display apart the labels coming from the files, which I could call dynamic labels
(the labels in the configuration file would be the static ones).
- Add an or operator ?

Various
- multi-threading
- have labels which are applied to only one line ? would help after extracting a list of bookmarks ?
- Add optional local configurations in directories ?
- support UTF8 and remove accents


