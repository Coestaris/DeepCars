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
        pass

    def proceed(self):
        chunks = []
        for i, shader in enumerate(self.shaders):

            cm.check_dict(i, "shader", shader, 
            {
                "name": (cm.def_string_comp, True),
                "vertex": (cm.def_string_comp, False),
                "fragment": (cm.def_string_comp, False),
                "geometry": (cm.def_string_comp, False),
                "compression": (cm.def_bool_comp, False),
                "index": (cm.def_int_comp, False),
            })

            files = []
            if "vertex" in shader: files.append(shader["vertex"])
            if "fragment" in shader: files.append(shader["fragment"])
            if "geometry" in shader: files.append(shader["geometry"])

            id = cm.get_id(self.path, shader)
            if cm.is_file_cached("shader", i, id, self.path, files):
                chunks += cm.get_cached_chunk(id)
                print("[{}/{}]: Shader \"{}\" already cached".format(i + 1, len(self.shaders), shader["name"]))
                continue

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
            if "index" in shader:
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

            chunk = cm.create_chunk(chunk, cm.SHADER_CHUNK_TYPE)
            chunks += chunk
            cm.cache_chunk(id, chunk)

        return (chunks, len(self.shaders))

def get_packer():
    return shader_packer(
        cm.PATH_PREFIX + cm.config["shaders_dir"],
        cm.index["shaders"],
        cm.config)
