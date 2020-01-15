from src import common as cm
from glob import glob
import os

# 4 bytes: id
# 2 bytes: name length
# n bytes: name
# 4 bytes: font info length
# n bytes: font info
# 4 bytes: DXT texture index

FONTBM_PATH = "./fontbm"
FONTBM_OUTPUT = "fontbm_out"
FONTBM_ARG = "--font-file {0} \
              --output {1} --padding-up 45 --padding-down 45 --padding-right 45 \
              --padding-left 45 --font-size 400 --color 0,0,0 --background-color 255,255,255 \
              --texture-width 3500 --texture-height 3500"

class shader_packer:
    def __init__(self, path, fonts, config):
        self.path = path
        self.fonts = fonts
        pass

    def proceed(self):
        chunks = []
        for i, font in enumerate(self.fonts):

            print("[{0}/{1}]: Packing font \"{2}\"".format(
                    i + 1, len(self.fonts), font["name"]))

            font_fn = self.path + font["fn"]
            os.system(FONTBM_PATH + " " + FONTBM_ARG.format(font_fn, FONTBM_OUTPUT))

            font_images = glob(FONTBM_OUTPUT + "_")
            print(font_images)

            chunk = cm.create_chunk(chunk, cm.FONT_CHUNK_TYPE)
            chunks.append(chunk)

        return chunks

        


def get_packer():
    return shader_packer(
        cm.PATH_PREFIX + cm.config["fonts_dir"],
        cm.index["fonts"],
        cm.config)
