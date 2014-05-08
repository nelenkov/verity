#!/bin/env python

import struct
import sys
import os

# modify for device
DATA_DEV = '/dev/block/mmcblk0p21'
DEVICE_SIZE_BYTES = 838860800

# from veritysetup output
SALT=        '<salt-goes-here>'
ROOT_DIGEST= '<root-hash-goes-here>'

VERITY_SB_SIZE = 32 * 1024

start = 0
size = DEVICE_SIZE_BYTES / 512
target = 'verity'
hash_dev = DATA_DEV 
data_block_size = 4096
hash_block_size = data_block_size
num_blocks = DEVICE_SIZE_BYTES / data_block_size
hash_offset = (DEVICE_SIZE_BYTES + VERITY_SB_SIZE) / hash_block_size + 1
hash_alg = 'sha256'

#0 417792 verity 1 /dev/sdb /dev/sdc 4096 4096 52224 1 sha256 2a... f4... 

fmt = "1 %s %s %d %d %d %d sha256 %s %s" 
table = fmt % (DATA_DEV, hash_dev, data_block_size, hash_block_size, num_blocks, hash_offset, ROOT_DIGEST, SALT)

print "DM table: \n" + table

f = open('table.bin', 'wb')
f.write(buffer(table))
f.close()

