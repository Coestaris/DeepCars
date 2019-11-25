# pylint: disable=F0401
from src import common as cm

class texture_packer:
    def __init__(self, path, file_mask):
        self.path = path
        self.file_mask = file_mask
        pass

    def proceed(self):
        return []

def get_packer():
    return texture_packer(
        cm.PATH_PREFIX + cm.TEXTURES_PATH, 
        cm.TEXTURES_FILE_MASK)