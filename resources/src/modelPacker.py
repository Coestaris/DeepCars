# pylint: disable=F0401
from src import common as cm
from glob import glob

# 4 bytes: model id 
# 2 bytes: model name length 
# n bytes: model name
# 1 byte : arhive type (0 - none, 1 - zlib)
# 4 bytes: data length
# m bytes: model data

class model_packer:
    def __init__(self, path, file_mask):
        self.path = path
        self.file_mask = file_mask
        pass

    def proceed(self):
        files = glob(self.path + self.file_mask)  
        chunks = []
        for index, file in enumerate(files):
            
            
        return chunks[]

def get_packer():
    return model_packer(
        cm.PATH_PREFIX + cm.MODELS_PATH, 
        cm.MODELS_FILE_MASK)