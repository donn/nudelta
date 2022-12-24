import fs from "fs-extra";

let args = process.argv.slice(2);
if (args.length < 2) {
    process.exit(-1);
}

let from = args.slice(0, -1);
let to = args.at(-1);

for (let file of from) {
    fs.copyFileSync(file, to);
}