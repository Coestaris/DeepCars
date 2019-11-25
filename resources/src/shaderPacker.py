# pylint: disable=F0401
from src import common as cm

class shader_packer:
    def __init__(self, path, shaders):
        self.path = path
        self.shaders = shaders
        pass

    def proceed(self):
        return []

def get_packer():
    return shader_packer(
        cm.PATH_PREFIX + cm.config["shaders_dir"],
        cm.index["shaders"])