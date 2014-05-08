#!/bin/env python

import struct
import binascii
import sys
import os

device_size_bytes = 838860800
verity_sb_size = 32 * 1024

start = 0
size = device_size_bytes / 512
target = 'verity'
data_dev = '/dev/block/mmcblk0p21'
hash_dev = data_dev
data_block_size = 4096
hash_block_size = data_block_size
num_blocks = device_size_bytes / data_block_size
hash_offset = (device_size_bytes + verity_sb_size) / hash_block_size + 1
hash_alg = 'sha256'
salt=        '<salt-goes-here>'
root_digest= '<root-hash-goes-here>'

#0 417792 verity 1 /dev/sdb /dev/sdc 4096 4096 52224 1 sha256 2a... f4... 

fmt = "1 %s %s %d %d %d %d sha256 %s %s" 
table = fmt % (data_dev, hash_dev, data_block_size, hash_block_size, num_blocks, hash_offset, root_digest, salt)

print table
print "[%d] %s", len(table),binascii.hexlify(table)

f = open('table.bin', 'wb')
f.write(buffer(table))
f.close()

