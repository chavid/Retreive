
ORIGINAL_DIR=${PWD}

# establish the script directory absolute path

RETREIVE_ENV_FILE=${BASH_SOURCE[0]}
RETREIVE_DIR=`dirname ${RETREIVE_ENV_FILE}`
cd ${RETREIVE_DIR}
RETREIVE_DIR=`pwd`
cd ${ORIGINAL_DIR}

# path

#export PATH=:${PATH}

# rt alias

export RETREIVE_DIR
rt () { ${RETREIVE_DIR}/build/retreive.exe $* ; }
export -f rt
