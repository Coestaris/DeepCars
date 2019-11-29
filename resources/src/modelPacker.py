# pylint: disable=F0401
from src import common as cm
from glob import glob
from zlib import compress

# 4 bytes: model id 
# 2 bytes: model name length 
# n bytes: model name
# 1 byte : normalize bit field (0x1 - normX, 0x2 - normY, 0x4 - normZ, 0x8 - normScale) 
# 1 byte : arhive type (0 - none, 1 - zlib)
# 4 bytes: data length
# 4 bytes: uncompressed data length
# m bytes: model data

class model_packer:
    def __init__(self, path, models, config):
        self.path = path
        self.models = models
        self.auto_index = config["model_auto_indices"]
        self.default_archive = config["model_default_compression"]

        self.default_normalize_x = config["model_default_normalize_x"]
        self.default_normalize_y = config["model_default_normalize_y"]
        self.default_normalize_z = config["model_default_normalize_z"]
        self.default_normalize_scale = config["model_default_normalize_scale"]
        pass

    def proceed(self):
        print(self.models)
        chunks = []
        for i, model in enumerate(self.models):
            data = ""
            with open(self.path + model["fn"]) as file:
                data = file.read()

            index = i
            if self.auto_index == False:
                index = model["index"]

            archive = self.default_archive
            if "arhive" in model:
                archive = model["arhive"]

            norm_x = self.default_normalize_x
            if "normalize_x" in model:
                norm_x = model["normalize_x"]

            norm_y = self.default_normalize_y
            if "normalize_y" in model:
                norm_y = model["normalize_y"]

            norm_z = self.default_normalize_z
            if "normalize_z" in model:
                norm_z = model["normalize_z"]

            norm_scale = self.default_normalize_scale
            if "normalize_scale" in model:
                norm_scale = model["normalize_scale"]

            normBitField = 0
            if norm_x:     normBitField |= 0x1
            if norm_y:     normBitField |= 0x2
            if norm_z:     normBitField |= 0x4
            if norm_scale: normBitField |= 0x8


            chunk = []
            chunk += cm.int32tobytes(index)
            chunk += cm.int16tobytes(len(model["name"]))
            chunk += model["name"].encode("utf-8")
            chunk += cm.int8tobytes(normBitField)
            chunk += [1 if archive == True else 0]
            
            if archive:
                compresssed = compress(bytes(data.encode("utf-8")))
                chunk += cm.int32tobytes(len(compresssed))
                chunk += cm.int32tobytes(len(data))
                chunk += compresssed
            else:
                chunk += cm.int32tobytes(len(data))
                chunk += cm.int32tobytes(len(data))
                chunk += data.encode("utf-8")

            chunks.append(cm.create_chunk(chunk, cm.MODEL_CHUNK_TYPE))

        return chunks

def get_packer():
    return model_packer(
        cm.PATH_PREFIX + cm.config["models_dir"],
        cm.index["models"],
        cm.config)