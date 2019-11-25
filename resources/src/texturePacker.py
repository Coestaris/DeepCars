# pylint: disable=F0401
from src import common as cm

class texture_packer:
    def __init__(self, path, textures):
        self.path = path
        self.textures = textures
        pass

    def proceed(self):
        return []

def get_packer():
    return texture_packer(
        cm.PATH_PREFIX + cm.config["textures_dir"],
        cm.index["textures"])