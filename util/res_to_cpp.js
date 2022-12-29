"use strict";
import fs from "fs-extra";
import path from "path";
import yaml from "yaml";
import fg from "fast-glob";

const argv = process.argv.slice(2);
const print = console.log;

let resourceDir = argv[0];
let globStr = path.join(resourceDir, "**", "*.yml");
let files = fg.sync(globStr, { absolute: true });

print('#include "common.hpp"');
print('#include "nuphy.hpp"');
for (let file of files) {
    let keyboard = path.basename(path.dirname(file));

    let str = fs.readFileSync(file, { encoding: "utf8" });
    let object = yaml.parse(str);
    let lines = str.split("\n");
    let [_, type, name] = lines[0].split(" ");
    if (type == "list") {
        print(`const std::vector<std::uint32_t> ${keyboard}::${name} = {`);
        for (let integer of object) {
            print(`    0x${integer.toString(16)},`);
        }
        print("};");
    } else if (type == "dict") {
        print(
            `const std::unordered_map<std::string, std::uint32_t> ${keyboard}::${name} = {`
        );
        for (let key in object) {
            let integer = object[key];
            print(`    { "${key}", 0x${integer.toString(16)} },`);
        }
        print("};");
    }
}
