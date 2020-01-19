#!/usr/bin/python3

from glob import glob
from os import rmdir, unlink, path
from sys import argv
import src.common as cm
import src.modelPacker as mp
import src.shaderPacker as sp
import src.texturePacker as tp
import src.materialPacker as mtp
import src.fontPacker as fp

# 5 bytes: magic bytes
# 4 bytes: chunks count 
# for every chunk:
#   4 bytes: chunk length
#   1 byte : chunk type
#   4 bytes: chunk crc
#   n bytes: chunk data

if __name__ == "__main__":
    if "clear" in argv:
        for file in glob(cm.CACHE_DIR + "*"):
            unlink(file)

        if path.isdir(cm.CACHE_DIR):
            rmdir(cm.CACHE_DIR)

        if path.isfile(cm.CACHE_FILE):
            unlink(cm.CACHE_FILE)
        exit(0)

    out_file = ""
    in_dir = ""

    for arg in argv:
        if arg.startswith("--out_file="):
            out_file = arg.split("=")[1]
        elif arg.startswith("--in_dir="):
            in_dir = arg.split("=")[1]

    if out_file == "":
        print("You must specify output file by --out_file=filename")
        exit(1)

    if in_dir == "":
        print("You must specify input directory by --in_dir=dirname")
        exit(1)

    cm.set_dir(in_dir)

    packers = [ 
        mp.get_packer(), 
        sp.get_packer(), 
        tp.get_texture_packer(),
        tp.get_cubemap_packer(),
        mp.get_packer(),
        mtp.get_packer(),
        fp.get_packer()
    ]

    len = 0
    chunks = []
    for packer in packers:
        chunk, count = packer.proceed()
        chunks += chunk
        len += count

    with open(out_file, mode="wb") as file:
        file.write(cm.MAGIC_BYTES)
        file.write(bytes(cm.int32tobytes(len)))
        file.write(bytes(chunks))

    cm.write_cache()