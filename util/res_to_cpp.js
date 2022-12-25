"use strict";
import fs from "fs-extra";
import path from "path";
import yaml from "yaml";

const argv = process.argv.slice(2);
const print = console.log;

let resourceDir = argv[0];

print('#include "common.hpp"');
print('#include "air75.hpp"');
for (let file of fs.readdirSync(resourceDir)) {
    let filePath = path.join(resourceDir, file);
    if (!filePath.endsWith(".yml")) {
        continue;
    }

    let str = fs.readFileSync(filePath, { encoding: "utf8" });
    let object = yaml.parse(str);
    let lines = str.split("\n");
    let [_, type, name] = lines[0].split(" ");
    if (type == "list") {
        print(`const std::vector<std::uint32_t> ${name} = {`);
        for (let integer of object) {
            print(`    0x${integer.toString(16)},`);
        }
        print("};");
    } else if (type == "dict") {
        print(
            `const std::unordered_map<std::string, std::uint32_t> ${name} = {`
        );
        for (let key in object) {
            let integer = object[key];
            print(`    { "${key}", 0x${integer.toString(16)} },`);
        }
        print("};");
    }
}
