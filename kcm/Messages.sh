#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SOURCE_DIR=$(dirname ${SCRIPT_DIR})
PO_DIR=${SCRIPT_DIR}/po

# Go to the source root directory to write correct relative paths to the pot-file.
cd ${SOURCE_DIR}
mkdir -p $PO_DIR

xgettext -F --from-code=UTF-8 -C --kde \
  -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
  -kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kI18N_NOOP2_NOSTRIP:1c,2 \
  -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
  `find kcm -name "*.cpp" -o -name "*.qml"` -o $PO_DIR/kcm_kdisplay.pot
