#!/usr/bin/env python3
import os
import sys
import yaml

__dir__ = os.path.abspath(os.path.dirname(__file__))

print('#include "common.hpp"')
print('#include "air75.hpp"')
for file in os.listdir(__dir__):
    file_path = os.path.join(__dir__, file)
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
