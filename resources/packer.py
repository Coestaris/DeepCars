import src.modelPacker as mp
import src.shaderPacker as sp
import src.texturePacker as tp

# 4 bytes: chunks count 
# for every chunk:
#   4 bytes: chunk length
#   1 byte : chunk type
#   4 bytes: chunk crc
#   n bytes: chunk data

def write_chunk(list, chunk):
    pass

if __name__ == "__main__":
    packers = [ 
        mp.get_packer(), 
        sp.get_packer(), 
        tp.get_packer() 
    ]

    bytes = []
    count = 0
    for packer in packers:
        chunks = packer.proceed()
        for chunk in chunks:
            write_chunk(bytes, chunk)
        
    pass