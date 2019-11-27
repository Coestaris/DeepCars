import src.common as cm
import src.modelPacker as mp
import src.shaderPacker as sp
import src.texturePacker as tp

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
        tp.get_packer() 
    ]

    chunks = []
    for packer in packers:
        chunks += packer.proceed()

    with open(cm.PATH_PREFIX + cm.config["output_file"], mode="wb") as file:
        file.write(cm.MAGIC_BYTES)
        file.write(bytes(cm.int32tobytes(len(chunks))))
        for chunk in chunks:
            file.write(bytes(chunk))