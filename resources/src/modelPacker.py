# pylint: disable=F0401
from src import common as cm
from glob import glob
from zlib import compress

# 4 bytes: model id 
# 2 bytes: model name length 
# n bytes: model name
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

            chunk = []
            chunk += cm.int32tobytes(index)
            chunk += cm.int16tobytes(len(model["name"]))
            chunk += model["name"].encode("utf-8")
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