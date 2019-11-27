from src.crc32 import crc32
import json

PATH_PREFIX = ""

MODEL_CHUNK_TYPE = 0
SHADER_CHUNK_TYPE = 1
TEXTURE_CHUNK_TYPE = 2

MAGIC_BYTES = "DPACK".encode("utf-8")

def loadJSON(name):
    with open(name) as json_file:
        return json.load(json_file)

config = loadJSON("config.json")
index = loadJSON("index.json")

def int8tobytes(value):
    return [value & 0xFF]

def int16tobytes(value):
    return [
        value & 0xFF,
        (value >> 8) & 0xFF,
    ]

def int32tobytes(value):
    return [
        value & 0xFF,
        (value >> 8) & 0xFF,
        (value >> 16) & 0xFF,
        (value >> 24) & 0xFF,
    ]                        

def create_chunk(data, type):
    b = []
    b += int32tobytes(len(data))
    b += int8tobytes(type)
    b += int32tobytes(crc32(bytes(data), 0xFFFFFFFF))
    b += data 
    return b