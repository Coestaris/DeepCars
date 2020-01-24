# pylint: disable=F0401
from src import common as cm
from zlib import compress

# 4 bytes: id
# 2 bytes: name length
# n bytes: name
# 1 byte : mode

# 12 bytes: ambient
# 2 bytes: ambient map length
# n bytes ambient map

# 12 bytes: diffuse
# 2 bytes: diffuse map length
# n bytes diffuse map

# 4 bytes: shininess
# 12 bytes: specular
# 2 bytes: specular map length
# n bytes: specular map

# 4 bytes: transparent
# 2 bytes: transparent map length
# n bytes transparent map

# 2 bytes: map_normal length
# n bytes map_normal


# Modes:
# 0 - no ambient
# 1 - ambient 
# 2 - ambient + specular
# 3 - illum
# 4 - illum (no shadows)

def check_mode(inp):
    return cm.def_int_comp(inp) and inp >= 0 and inp <= 4

def check_vector(inp):
    if not type(inp) is list:
        return False

    for v in inp:
        if not cm.def_int_comp(v) and (v < 0 or v > 1):
            return False

    if len(inp) != 3:
        return False

    return True

class material_packer:
    def __init__(self, path, materials, config):
        self.path = path
        self.materials = materials
        
        self.default_mode = config["material_default_mode"]

        self.default_ambient = config["material_default_ambient"]
        self.default_map_ambient = config["material_default_map_ambient"]

        self.default_diffuse = config["material_default_diffuse"]
        self.default_map_diffuse = config["material_default_map_diffuse"]

        self.default_shininess = config["material_default_shininess"]
        self.default_specular = config["material_default_specular"]
        self.default_map_specular = config["material_default_map_specular"]

        self.default_transparent = config["material_default_transparent"]
        self.default_map_transparent = config["material_default_map_transparent"]
        self.default_map_normal = config["material_default_map_normal"]
        pass

    def proceed(self):
        chunks = []
        for i, material in enumerate(self.materials):

            cm.check_dict(i, "material", material, 
            {
                "name": (cm.def_string_comp, True),
                "fn": (cm.def_string_comp, True),
                "index": (cm.def_int_comp, False),
            })

            files = [material["fn"]]
            id = cm.get_id(self.path, material)
            if cm.is_file_cached("material", i, id, self.path, files):
                chunks += cm.get_cached_chunk(id)
                print("[{}/{}]: Material \"{}\" already cached".format(i + 1, len(self.materials), material["name"]))
                continue

            print("[{0}/{1}]: Packing material \"{2}\"".format(
                i + 1, len(self.materials), material["name"]))

            mat_json = cm.loadJSON(self.path + material["fn"]) 
            
            cm.check_dict(i, "mat_json", mat_json, 
            {
                "mode": (check_mode, False),
                "ambient": (check_vector, False),
                "map_ambient": (cm.def_string_comp, False),
                "diffuse": (check_vector, False),
                "map_diffuse": (cm.def_string_comp, False),
                "shininess": (cm.def_int_comp, False),
                "specular": (check_vector, False),
                "map_specular": (cm.def_string_comp, False),
                "transparent": (cm.def_int_comp, False),
                "map_transparent": (cm.def_string_comp, False),
                "map_normal": (cm.def_string_comp, False),
            })

            mode = self.default_mode
            if "mode" in mat_json:
                mode = mat_json["mode"]

            ambient = self.default_ambient
            if "ambient" in mat_json:
                ambient = mat_json["ambient"]

            map_ambient = self.default_map_ambient
            if "map_ambient" in mat_json:
                map_ambient = mat_json["map_ambient"]

            diffuse = self.default_diffuse
            if "diffuse" in mat_json:
                diffuse = mat_json["diffuse"]

            map_diffuse = self.default_map_diffuse
            if "map_diffuse" in mat_json:
                map_diffuse = mat_json["map_diffuse"]

            shininess = self.default_shininess
            if "shininess" in mat_json:
                shininess = mat_json["shininess"]

            specular = self.default_specular
            if "specular" in mat_json:
                specular = mat_json["specular"]

            map_specular = self.default_map_specular
            if "map_specular" in mat_json:
                map_specular = mat_json["map_specular"]

            transparent = self.default_transparent
            if "transparent" in mat_json:
                transparent = mat_json["transparent"]

            map_transparent = self.default_map_transparent
            if "map_transparent" in mat_json:
                map_transparent = mat_json["map_transparent"]

            map_normal = self.default_map_normal
            if "map_normal" in mat_json:
                map_normal = mat_json["map_normal"]

            index = i
            if "index" in material:
                index = material["index"]

            chunk = []
            chunk += cm.int32tobytes(index)
            chunk += cm.int16tobytes(len(material["name"]))
            chunk += material["name"].encode("utf-8")
            chunk += cm.int8tobytes(mode)

            chunk += cm.float32Arraytobytes(ambient)
            chunk += cm.int16tobytes(len(map_ambient))
            chunk += map_ambient.encode("utf-8")

            chunk += cm.float32Arraytobytes(diffuse)
            chunk += cm.int16tobytes(len(map_diffuse))
            chunk += map_diffuse.encode("utf-8")

            chunk += cm.float32tobytes(shininess)
            chunk += cm.float32Arraytobytes(specular)
            chunk += cm.int16tobytes(len(map_specular))
            chunk += map_specular.encode("utf-8")

            chunk += cm.float32tobytes(transparent)
            chunk += cm.int16tobytes(len(map_transparent))
            chunk += map_transparent.encode("utf-8")

            chunk += cm.int16tobytes(len(map_normal))
            chunk += map_normal.encode("utf-8")

            chunk = cm.create_chunk(chunk, cm.MATERIAL_CHUNK_TYPE)
            cm.cache_chunk(id, chunk)
            chunks += chunk

        return (chunks, len(self.materials))

def get_packer():
    return material_packer(
        cm.PATH_PREFIX + cm.config["materials_dir"],
        cm.index["materials"],
        cm.config)