#!/usr/bin/python3

from glob import glob
from os import rmdir, unlink, path
from sys import argv
import src.common as cm
import src.modelPacker as mp
import src.shaderPacker as sp
import src.texturePacker as tp
import src.materialPacker as mp

# 5 bytes: magic bytes
# 4 bytes: chunks count 
# for every chunk:
#   4 bytes: chunk length
#   1 byte : chunk type
#   4 bytes: chunk crc
#   n bytes: chunk data

if __name__ == "__main__":
    packers = [ 
        mp.get_packer(), 
        sp.get_packer(), 
        tp.get_texture_packer(),
        tp.get_cubemap_packer(),
        mp.get_packer()
    ]

    if "clear" in argv:
        for file in glob(cm.CACHE_DIR + "*"):
            unlink(file)

        if path.isdir(cm.CACHE_DIR):
            rmdir(cm.CACHE_DIR)

        if path.isfile(cm.CACHE_FILE):
            unlink(cm.CACHE_FILE)

        exit(1)

    chunks = []
    for packer in packers:
        chunks += packer.proceed()

    with open(cm.PATH_PREFIX + cm.config["output_file"], mode="wb") as file:
        file.write(cm.MAGIC_BYTES)
        file.write(bytes(cm.int32tobytes(len(chunks))))
        for chunk in chunks:
            file.write(bytes(chunk))

    cm.write_cache()