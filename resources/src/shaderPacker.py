# pylint: disable=F0401
from src import common as cm
from zlib import compress

# 4 bytes: id
# 2 bytes: name length
# n bytes: name
# 1 byte : shader flags. (0x1 - vertex, 0x2 - fragment, 0x4 - geometry)
# 1 byte : compression 
# 4 bytes: data length              | 
# 4 bytes: uncompressed data length | m times
# n bytes: data                     |

class shader_packer:
    def __init__(self, path, shaders, config):
        self.path = path
        self.shaders = shaders
        self.default_compression = config["shader_default_compression"]
        self.auto_index = config["shader_auto_indices"]
        pass

    def proceed(self):
        chunks = []
        for i, shader in enumerate(self.shaders):
            compression = self.default_compression
            if "compression" in shader:
                compression = shader["compression"]

            lists = []
            shader_flag = 0

            if "vertex" in shader:
                with open(self.path + shader["vertex"]) as file:
                    lists.append(file.read())
                shader_flag |= 0x1

            if "fragment" in shader:
                with open(self.path + shader["fragment"]) as file:
                    lists.append(file.read())
                shader_flag |= 0x2

            if "geometry" in shader:
                with open(self.path + shader["geometry"]) as file:
                    lists.append(file.read())
                shader_flag |= 0x4

            str = "[{}/{}]: Packing shader \"{}\" (".format(i + 1, len(self.shaders), shader["name"])
            for list_i, list in enumerate(lists):
                str += "{} bytes{}".format(len(list), ", " if list_i != len(lists) - 1 else ")")
            print(str)

            index = i
            if self.auto_index == False:
                index = shader["index"]

            chunk = []
            chunk += cm.int32tobytes(index)            
            chunk += cm.int32tobytes(len(shader["name"]))            
            chunk += shader["name"].encode("utf-8")
            chunk += cm.int8tobytes(shader_flag)
            chunk += cm.int8tobytes(compression)
            for list in lists:
                list += '\n'
                if compression:
                    compresssed = compress(bytes(list.encode("utf-8")))
                    chunk += cm.int32tobytes(len(compresssed))
                    chunk += cm.int32tobytes(len(list))
                    chunk += compresssed
                else:   
                    chunk += cm.int32tobytes(len(list))
                    chunk += cm.int32tobytes(len(list))
                    chunk += list.encode("utf-8")

            chunks.append(cm.create_chunk(chunk, cm.SHADER_CHUNK_TYPE))
        
        return chunks

def get_packer():
    return shader_packer(
        cm.PATH_PREFIX + cm.config["shaders_dir"],
        cm.index["shaders"],
        cm.config)
