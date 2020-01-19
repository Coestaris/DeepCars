from src import common as cm
from glob import glob
from src import texturePacker as tp
import os
import subprocess

# 4 bytes: id
# 2 bytes: name length
# n bytes: name
# 4 bytes: font info length
# n bytes: font info
# 4 bytes: DXT texture index
# 2 bytes: shader name length
# n bytes: shader name

FONTBM_PATH = "./fontbm"
FONTBM_OUTPUT = "fontbm_out"
FONTBM_ARG = "--font-file {0} \
              --output {1} --padding-up 45 --padding-down 45 --padding-right 45 \
              --padding-left 45 --font-size 400 --color 0,0,0 --background-color 255,255,255 \
              --texture-width 4096 --texture-height 4096 --data-format bin"

IMAGE_MAGIC_ARG = "/usr/bin/convert {0} -filter Jinc \\( +clone -negate -morphology Distance Euclidean \
                   -level 50%,-50% \\) -morphology Distance Euclidean -compose Plus -composite \
                   -level 43%,57% -resize 12.5% {0}"

FONT_WRAPPING = "clamp_to_edge"

class shader_packer:
    def __init__(self, path, fonts, config, default_font_compression, default_font_min, default_font_mag):
        self.path = path
        self.fonts = fonts
        self.default_font_compression = default_font_compression
        self.default_font_min = default_font_min
        self.default_font_mag = default_font_mag
        pass

    def proceed(self):
        chunks = []
        max_tex = self.get_max_texture_index()

        for i, font in enumerate(self.fonts):

            cm.check_dict(i, "font", font, 
            {
                "name": (cm.def_string_comp, True),
                "fn": (cm.def_string_comp, True),
                "shader": (cm.def_string_comp, True),
                
                "min_filter": (tp.check_min, False),
                "mag_filter": (tp.check_mag, False),
                "compression": (tp.check_comp, False),

                "index": (cm.def_int_comp, False),
            })

            files = [font["fn"]]
            id = cm.get_id(self.path, font)
            if cm.is_file_cached("font", i, id, self.path, files):
                chunks += cm.get_cached_chunk(id)
                print("[{}/{}]: Font \"{}\" already cached".format(i + 1, len(self.fonts), font["name"]))
                continue

            print("[{0}/{1}]: Packing font \"{2}\"".format(
                    i + 1, len(self.fonts), font["name"]))

            font_fn = self.path + font["fn"]
            font_image = FONTBM_OUTPUT + "_0.png"
            font_file = FONTBM_OUTPUT + ".fnt"

            os.system(FONTBM_PATH + " " + FONTBM_ARG.format(font_fn, FONTBM_OUTPUT))
            os.system(IMAGE_MAGIC_ARG.format(font_image))

            index = i
            if "index" in font:
                index = font["index"]

            name = font["name"]

            min = self.default_font_min
            if "min_filter" in font:
                min = font["min_filter"]

            mag = self.default_font_mag
            if "mag_filter" in font:
                mag = font["mag_filter"]

            compression = self.default_font_compression
            if "compression" in font:
                compression = font["compression"]

            shader = font["shader"]

            chunk = []
            chunk += cm.int32tobytes(index)
            chunk += cm.int16tobytes(len(name))
            chunk += name.encode("utf-8")

            with open(font_file, mode="rb") as ff:
                data = ff.read()
                chunk += cm.int32tobytes(len(data))
                chunk += data


            chunk += cm.int32tobytes(max_tex + i + 1)
            chunk += cm.int16tobytes(len(shader))
            chunk += shader.encode("utf-8")

            tex = tp.texture_packer.create_chunk(
                FONT_WRAPPING, min, mag, 0, compression, font_image, max_tex + i + 1, "___font_tex_" + name
            )

            os.unlink(font_image)
            os.unlink(font_file)

            chunk = cm.create_chunk(chunk, cm.FONT_CHUNK_TYPE)
     
            chunks += tex
            chunks += chunk

            cm.cache_chunk(id, tex + chunk)

        return (chunks, len(self.fonts) * 2)

    def get_max_texture_index(self):
        textures = cm.index["textures"]
        max_ind = 0
        for i, texture in enumerate(textures):
            index = i
            if "index" in texture:
                index = texture["index"]
            max_ind = max(max_ind, index)   

        return max_ind


def get_packer():
    return shader_packer(
        cm.PATH_PREFIX + cm.config["fonts_dir"],
        cm.index["fonts"],
        cm.config,
        cm.config["font_default_compression"],
        cm.config["font_default_min_filter"],
        cm.config["font_default_mag_filter"])
