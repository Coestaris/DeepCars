from zlib import crc32

PATH_PREFIX = ""

MODELS_PATH = "models/"
MODELS_FILE_MASK = "*.obj"
TEXTURES_PATH = "textures/"
TEXTURES_FILE_MASK = "*.png"
SHADERS_PATH = "shaders/"
SHADERS_FILE_MASK = "*.glsl"

MODEL_CHUNK_TYPE = 0
SHADER_CHUNK_TYPE = 1
TEXTURE_CHUNK_TYPE = 2

def int8tobytes(value):
    return [value & 0xFF]

def int16tobytes(value):
    return [
        (value >> 8) & 0xFF,
        value & 0xFF
    ]

def int32tobytes(value):
    return [
        (value >> 24) & 0xFF,
        (value >> 16) & 0xFF,
        (value >> 8) & 0xFF,
        value & 0xFF,
    ]                        

def create_chunk(data, type):
    b = []
    b.append(int32tobytes(len(data)))
    b.append(int8tobytes(type))
    b.append(int32tobytes(crc32(bytes(data))))
    b.append(data)
    return b