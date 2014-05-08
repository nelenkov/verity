#!/bin/sh

if [ -z $1 ]; then
    echo "Usage: $0 <table file>"
    exit 1
fi

SIG_FILE=table.sig
VERITY_KEY=verity-key.pem

openssl dgst -sha1 -sign $VERITY_KEY -out $SIG_FILE $1
openssl dgst -sha1 -verify $VERITY_KEY -signature $SIG_FILE $1

echo "Signed $1 is in $SIG_FILE"
