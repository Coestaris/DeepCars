# pylint: disable=F0401
from src import common as cm
from wand import image
import os

# 4 bytes: image id
# 2 bytes: image name len
# 4 bytes: image name
# 4 bytes: image width
# 4 bytes: image height
# 1 byte : image compression (0 - png, 1 - dds_no, 2 - dxt1, 3 - dxt3, 4 - dxt5)
# 1 byte : texture wrapping (0 - repeat, 1 - mirrored_repeat, 2 - clamp_to_edge, 3 - clamp_to_border)
# 1 byte : min filter (0 - mipmap_linear, 1 - mipmap_nearest, 2 - linear, 3 - nearest)
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

class texture_packer:
    def __init__(self, path, textures, config):
        self.path = path
        self.textures = textures
        self.default_wrapping = config["texture_default_wrapping"]
        self.default_min = config["texture_default_min_filter"]
        self.default_mag = config["texture_default_mag_filter"]
        self.default_flip = config["texture_default_flip"]
        self.default_compression = config["texture_default_compression"]
        self.auto_indices = config["texture_auto_indices"]
        pass

    def proceed(self):
        print(self.textures)
        chunks = []
        for i, texture in enumerate(self.textures):

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


            if compress == "dds_no":
                compress == "no"

            if compress == "png":
                tmp = 'tmp.png'
            else:
                tmp = 'tmp.dds'

            imsize = ()
            with image.Image(filename=self.path + texture["fn"]) as img:
                imsize = img.size
                if compress != "png":
                    img.compression = compress

                img.save(filename=tmp)

            index = i
            if self.auto_indices == False:
                index = texture["index"]

            print(imsize)
            chunk = []
            with open(tmp, mode='rb') as file:
                chunk += cm.int32tobytes(index)
                chunk += cm.int16tobytes(len(texture["name"]))
                chunk += texture["name"].encode("utf-8")
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
            chunks.append(cm.create_chunk(chunk, cm.TEXTURE_CHUNK_TYPE))

        return chunks

def get_packer():
    return texture_packer(
        cm.PATH_PREFIX + cm.config["textures_dir"],
        cm.index["textures"],
        cm.config)