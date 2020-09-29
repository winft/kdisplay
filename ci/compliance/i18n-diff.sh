#!/bin/bash

set -eu
set -o pipefail

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SOURCE_DIR=$(dirname $(dirname ${SCRIPT_DIR}))

# Arg $1 is Messages.sh file path EXCLUDING the name of the script.
# Arg $2 is resulting pot-file path INCLUDING the name of the file.
compare() {
    echo "Comparing result of executing '${1}/Messages.sh' with pot file '$2'."

    CURRENT=""

    MSGS_FILE="${SOURCE_DIR}/${1}/Messages.sh"
    POT_FILE="${SOURCE_DIR}/${2}"

    if [ -f "$POT_FILE" ]; then
        # Save current pot file content to variable.
        CURRENT=$(<${POT_FILE})

        # Move current pot file to restore it later.
        TMP_FILE="$(mktemp)"
        cp -p $POT_FILE $TMP_FILE
    fi

    # Apply Messages.sh
    $MSGS_FILE
    CMP=$(<${POT_FILE})

    # Remove date lines from pot content.
    CURRENT=$(echo "$CURRENT" | sed '/POT-Creation-Date/d')
    CMP=$(echo "$CMP" | sed '/POT-Creation-Date/d')

    if [ "$CURRENT" == "$CMP" ]; then
        echo "Relevant data is identical."
    else
        echo "------------------------------------------------------------------------"
        echo "Relevant data is not equal to the current pot file."
        echo "Run the '${1}/Messages.sh' file to fix the discrepancy"
        echo "or copy the following expected pot-file content into '$2':"
        echo "------------------------------------------------------------------------"
        cat "$POT_FILE"
        echo "------------------------------------------------------------------------"
        echo
        FAILED=$(($FAILED + 1))
    fi

    # Restore pot-file to initial state or if none existed prior delete the created one.
    if [ -z "$TMP_FILE" ]; then
        echo "No previous pot-file, removing temporarly created one."
        rm $POT_FILE
    else
        echo "Restoring original pot-file."
        cp -p $TMP_FILE $POT_FILE
    fi
}

FAILED=0
compare kcm kcm/po/kcm_kdisplay.pot

if [ "$FAILED" != "0" ]; then
    echo; echo "Number of failed pot-files: $FAILED"
    echo "In case you want to copy the above pot-file content instead of running the respective"
    echo "Messages.sh scripts locally use the raw output and not the one presented in GitLab's UI!"
    echo "The output in the UI is reformatted and empty lines have been removed."
fi

exit $FAILED
