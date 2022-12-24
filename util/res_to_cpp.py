#!/usr/bin/env python3
import os
import sys
import yaml

resource_dir = sys.argv[1]

print('#include "common.hpp"')
print('#include "air75.hpp"')
for file in os.listdir(resource_dir):
    file_path = os.path.join(resource_dir, file)
    if not file.endswith(".yml"):
        continue

    str = open(file_path).read()
    object = yaml.safe_load(str)
    lines = str.splitlines()
    _, type, name = lines[0].split(" ")
    if type == "list":
        print(f"const std::vector<std::uint32_t> {name} = {{")
        for integer in object:
            print("    0x%08x," % integer)
        print("};")
    elif type == "dict":
        print(f"const std::unordered_map<std::string, std::uint32_t> {name} = {{")
        for key, integer in object.items():
            print(f'    {{ "{key}", 0x%08x }},' % integer)
        print("};")
