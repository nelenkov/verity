#!/bin/env python

import getopt, posixpath, signal, struct, sys


MAGIC = 0xb001b001
VERSION = 0
BLOCK_SIZE = 32 * 1024

if len(sys.argv) < 4:
    print "Usage %s <sig file> <table file> <verity sb file>" % sys.argv[0]
    sys.exit(1)

sig_file = sys.argv[1]
print "sig file " + sig_file
table_file = sys.argv[2]
print "table file: " + table_file
sb_file = sys.argv[3]
print "verity sb file: " + sb_file

sf = open(sig_file, 'rb')
sig = sf.read()
sf.close()

tf = open(table_file, 'rb')
table = tf.read()
table_size = len(table)
tf.close()

length = 0
header = struct.pack("<II", MAGIC, VERSION)
vf = open(sb_file, "wb")
vf.write(header)
length += len(header)
vf.write(sig)
length += len(sig)
table_size_bytes= struct.pack("<I", table_size);
vf.write(table_size_bytes);
length += len(table_size_bytes)
vf.write(table)
length += len(table)
print "Unpadded length: %d bytes" % length
padding = "\0" * (BLOCK_SIZE - length)
vf.write(padding)
vf.close()

