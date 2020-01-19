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
    try:
        with open(name) as json_file:
            return json.load(json_file)
    except json.decoder.JSONDecodeError as inst:
        print("Couldn't parse json \"{0}\": {1} at line #{2}".format(name, inst.msg, inst.lineno))
        exit(1)

config = loadJSON("config.json")
index = None

cache = {}
if os.path.isfile(CACHE_FILE):
    cache = loadJSON(CACHE_FILE)

if not os.path.isdir(CACHE_DIR):
    os.mkdir(CACHE_DIR)

cached_files = [f.split('/')[1] for f in glob(CACHE_DIR + "*")]

def def_string_comp(inp):
    return type(inp) is str

def def_bool_comp(inp):
    return type(inp) is bool

def def_int_comp(inp):
    return type(inp) is int

def check_dict(i, name, dict, allowed):
    for key, value in dict.items():
        if key in allowed:
            func, req = allowed[key]
            if not func(value):
                print("{0} #{1}: Wrong data format of key \"{2}\"".format(name, i, key))
                exit(1)
        else:
            print("{0} #{1}: Invalid key \"{2}\"".format(name, i, key))
            exit(1)
        pass

    for key,value in allowed.items():
        func, req = value
        if req and not key in dict:
            print("{0} #{1}: Missing required key \"{2}\"".format(name, i, key))
            exit(1)

def set_dir(dir):
    global PATH_PREFIX, index
    PATH_PREFIX = dir

    index = loadJSON(PATH_PREFIX + "index.json")

def get_id(dir, dict):
    str_id = dir + dict.__str__()
    return hashlib.sha1(str_id.encode("utf-8")).hexdigest()

def is_file_cached(name, i, id, dir, files):
    for file in files:
        if not os.path.isfile(dir + file):
            print("{0} #{1}: Couldn't open file \"{2}\"".format(name, i, dir + file))
            exit(1)
    
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