#!/bin/sh

python2 "${HYPERDEX_SRCDIR}"/test/runner.py --space="space kv key k attributes map(string, string) v" --daemons=1 -- \
    python2 "${HYPERDEX_SRCDIR}"/test/python/DataTypeMapStringString.py {HOST} {PORT}
