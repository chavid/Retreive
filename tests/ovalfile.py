
# Previously source ../env.bash

targets = [

    { "name" : "rthelp" , "command" : "rt --help" },

    { "name" : "rt" , "command" : "rt" },

    { "name" : "rt1" , "command" : "rt label1" },
    { "name" : "rt2" , "command" : "rt label2" },
    { "name" : "rt3" , "command" : "rt label3" },
    { "name" : "rt12" , "command" : "rt label1 label2" },
    { "name" : "rt23" , "command" : "rt label2 label3" },

    { "name" : "rt123" , "command" : "rt labels" },
    { "name" : "rtfr" , "command" : "rt fichier1" },
    { "name" : "rtmd" , "command" : "rt md" },
    { "name" : "rtmax" , "command" : "rt dir2" },

]

# Filtres

run_filters_out = [
#  {"name": "temps", "re": "^.*temps de.*$", "apply": "%"}
]

diff_filters_in = [
  {"name": "all", "re": "^(.*)$", "apply": "%"}
]

