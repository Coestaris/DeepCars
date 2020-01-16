# pylint: disable=F0401
from src import common as cm
from wand import image
import os

# 4 bytes: image id
# 1 bytes: maps count (for cubemaps)
# 2 bytes: image name len
# n bytes: image name
# 4 bytes: image width
# 4 bytes: image height
# 1 byte : image compression (0 - png, 1 - dds_no, 2 - dxt1, 3 - dxt3, 4 - dxt5)
# 1 byte : texture wrapping (0 - repeat, 1 - mirrored_repeat, 2 - clamp_to_edge, 3 - clamp_to_border)
# 1 byte : min filter (0 - mipmap_linear, 1 - mipmap_nearest, 2 - linear, 3 -   )
# 1 byte : mag filter (0 - linear, 1 - nearest)
# 1 byte : flip (0 - no, 1 - flip x, 2 - flip y, 3 - flip both)
# 4 bytes: data length
# n bytes: data

compression_dict = {
    "png" : 0,
    "no" : 1,
    "dxt1" : 2,
    "dxt3" : 3,
    "dxt5" : 5
}

texture_wrapping_dict = {
    "repeat" : 0,
    "mirrored_repeat" : 1,
    "clamp_to_edge" : 2,
    "clamp_to_border" : 3,
}

min_dict = {
    "mipmap_linear" : 0,
    "mipmap_nearest" : 1,
    "linear" : 2,
    "nearest" : 3,
}

mag_dict = {
    "linear" : 0,
    "nearest" : 1,
}

class cubemap_packer:
    def __init__(self, path, cubemaps, config):
        self.path = path
        self.cubemaps = cubemaps
        self.default_wrapping = config["texture_default_wrapping"]
        self.default_min = config["texture_default_min_filter"]
        self.default_mag = config["texture_default_mag_filter"]
        self.default_flip = config["texture_default_flip"]
        self.default_compression = config["texture_default_compression"]
        pass

    def proceed(self):
        chunks = []
        for i, cubemap in enumerate(self.cubemaps):

            files = cubemap["fns"]
            id = cm.get_id(self.path, cubemap)
            if cm.is_file_cached(id, self.path, files):
                chunks += cm.get_cached_chunk(id)
                print("[{}/{}]: Cubemap \"{}\" already cached".format(i + 1, len(self.cubemaps), cubemap["name"]))
                continue
                
            wrapping = self.default_wrapping
            if "wrapping" in cubemap:
                wrapping = cubemap["wrapping"]

            min = self.default_min
            if "min_filter" in cubemap:
                compress = cubemap["min_filter"]

            mag = self.default_mag
            if "mag_filter" in cubemap:
                compress = cubemap["mag_filter"]

            flip = self.default_flip
            if "flip" in cubemap:
                flip = cubemap["flip"]

            compress = self.default_compression
            if "compression" in cubemap:
                compress = cubemap["compression"]


            if compress == "dds_no":
                compress == "no"

            if compress == "png":
                tmps = ['tmp{}.png'.format(i) for i in range(0, len(cubemap["fns"]))]
            else:
                tmps = ['tmp{}.dds'.format(i) for i in range(0, len(cubemap["fns"]))]

            imsize = ()
            for j, fn in enumerate(cubemap["fns"]):
                with image.Image(filename=self.path + fn) as img:
                    imsize = img.size
                    if compress != "png":
                        img.compression = compress

                    img.save(filename=tmps[j])

            print("[{}/{}]: Packing cubemap \"{}\" ({}x{}, {} maps) as {}".format(i + 1, len(self.cubemaps), cubemap["name"], 
                imsize[0], imsize[1], len(cubemap["fns"]), compress.upper()))

            index = i
            if "index" in cubemap:
                index = cubemap["index"]

            chunk = []
            chunk += cm.int32tobytes(index) 
            chunk += cm.int8tobytes(len(cubemap["fns"]))
            chunk += cm.int16tobytes(len(cubemap["name"]))
            chunk += cubemap["name"].encode("utf-8")
            chunk += cm.int32tobytes(imsize[0])
            chunk += cm.int32tobytes(imsize[1])
            chunk += cm.int8tobytes(compression_dict[compress])
            chunk += cm.int8tobytes(texture_wrapping_dict[wrapping])
            chunk += cm.int8tobytes(min_dict[min])
            chunk += cm.int8tobytes(mag_dict[mag])
            chunk += cm.int8tobytes(flip)
            
            for tmp in tmps:
                with open(tmp, mode='rb') as file:
                    b = file.read()
                    chunk += cm.int32tobytes(len(b))
                    chunk += b
                os.remove(tmp)
                
            chunk = cm.create_chunk(chunk, cm.CUBEMAP_CHUNK_TYPE)
            chunks += chunk

            cm.cache_chunk(id, chunk)

        return (chunks, len(self.cubemaps))

class texture_packer:
    def __init__(self, path, textures, config):
        self.path = path
        self.textures = textures
        self.default_wrapping = config["texture_default_wrapping"]
        self.default_min = config["texture_default_min_filter"]
        self.default_mag = config["texture_default_mag_filter"]
        self.default_flip = config["texture_default_flip"]
        self.default_compression = config["texture_default_compression"]
        pass

    def proceed(self):
        chunks = []
        for i, texture in enumerate(self.textures):

            files = [texture["fn"]]
            id = cm.get_id(self.path, texture)
            if cm.is_file_cached(id, self.path, files):
                chunks += cm.get_cached_chunk(id)
                print("[{}/{}]: Texture \"{}\" already cached".format(i + 1, len(self.textures), texture["name"]))
                continue

            wrapping = self.default_wrapping
            if "wrapping" in texture:
                wrapping = texture["wrapping"]

            min = self.default_min
            if "min_filter" in texture:
                compress = texture["min_filter"]

            mag = self.default_mag
            if "mag_filter" in texture:
                compress = texture["mag_filter"]

            flip = self.default_flip
            if "flip" in texture:
                flip = texture["flip"]

            compress = self.default_compression
            if "compression" in texture:
                compress = texture["compression"]

            index = i
            if "index" in texture:
                index = texture["index"]

            if compress == "dds_no":
                compress == "no"

            name = texture["name"]
            full_path = self.path + texture["fn"]


            imsize = (0,0) #todo: ?
            print("[{}/{}]: Packing texture \"{}\" ({}x{}) as {}".format(i + 1, len(self.textures), texture["name"], 
                imsize[0], imsize[1], compress.upper()))

            chunk = self.create_chunk(wrapping, min, mag, flip, compress, full_path, index, name)
            chunks += chunk

            cm.cache_chunk(id, chunk)

        return (chunks, len(self.textures))

    @staticmethod
    def create_chunk(wrapping, min, mag, flip, compress, full_path, index, name):

        if compress == "png":
            tmp = 'tmp.png'
        else:
            tmp = 'tmp.dds'

        imsize = ()
        with image.Image(filename=full_path) as img:
            imsize = img.size
            if compress != "png":
                img.compression = compress

            img.save(filename=tmp)

        chunk = []
        with open(tmp, mode='rb') as file:
            chunk += cm.int32tobytes(index)
            chunk += cm.int8tobytes(1)
            chunk += cm.int16tobytes(len(name))
            chunk += name.encode("utf-8")
            chunk += cm.int32tobytes(imsize[0])
            chunk += cm.int32tobytes(imsize[1])
            chunk += cm.int8tobytes(compression_dict[compress])
            chunk += cm.int8tobytes(texture_wrapping_dict[wrapping])
            chunk += cm.int8tobytes(min_dict[min])
            chunk += cm.int8tobytes(mag_dict[mag])
            chunk += cm.int8tobytes(flip)
            
            b = file.read()
            chunk += cm.int32tobytes(len(b))
            chunk += b

        os.remove(tmp)
        chunk = cm.create_chunk(chunk, cm.TEXTURE_CHUNK_TYPE)

        return chunk


def get_texture_packer():
    return texture_packer(
        cm.PATH_PREFIX + cm.config["textures_dir"],
        cm.index["textures"],
        cm.config)

def get_cubemap_packer():
    return cubemap_packer(
        cm.PATH_PREFIX + cm.config["cubemaps_dir"],
        cm.index["cubemaps"],
        cm.config)