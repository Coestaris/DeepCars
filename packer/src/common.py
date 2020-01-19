from src.crc32 import crc32
from glob import glob
import time
import os
import json
import struct
import hashlib

PATH_PREFIX = ""

MODEL_CHUNK_TYPE = 0
SHADER_CHUNK_TYPE = 1
TEXTURE_CHUNK_TYPE = 2
CUBEMAP_CHUNK_TYPE = 3
MATERIAL_CHUNK_TYPE = 4
FONT_CHUNK_TYPE = 5

CACHE_FILE = "_cache.json"
CACHE_DIR = "_cache/"
MAGIC_BYTES = "DPACK".encode("utf-8")

def loadJSON(name):
    with open(name) as json_file:
        return json.load(json_file)

config = loadJSON("config.json")
index = None

cache = {}
if os.path.isfile(CACHE_FILE):
    cache = loadJSON(CACHE_FILE)

if not os.path.isdir(CACHE_DIR):
    os.mkdir(CACHE_DIR)

cached_files = [f.split('/')[1] for f in glob(CACHE_DIR + "*")]

def set_dir(dir):
    global PATH_PREFIX, index
    PATH_PREFIX = dir

    index = loadJSON(PATH_PREFIX + "index.json")

def get_id(dir, dict):
    str_id = dir + dict.__str__()
    return hashlib.sha1(str_id.encode("utf-8")).hexdigest()


def is_file_cached(id, dir, files):
    if id in cached_files:
        for file in files:
            time = os.path.getmtime(dir + file) 
            last_time = cache[id]

            if last_time < time:
                return False

        return True
    else:
        return False


def get_cached_chunk(id):
    with open(CACHE_DIR + id, mode="rb") as file:
        return file.read()


def cache_chunk(id, chunk):
    cache.update({ id : time.time() })
    with open(CACHE_DIR + id, mode="wb") as file:
        file.write(bytes(chunk))


def write_cache():
    with open(CACHE_FILE, mode="w") as json_file:
        return json.dump(cache, json_file)

def int8tobytes(value):
    return [value & 0xFF]


def int16tobytes(value):
    return [
        value & 0xFF,
        (value >> 8) & 0xFF,
    ]

def float32Arraytobytes(values):
    list = []
    for value in values:
        list += float32tobytes(value)
    return list

def float32tobytes(value):
    return [b for b in bytearray(struct.pack("f", value))]

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