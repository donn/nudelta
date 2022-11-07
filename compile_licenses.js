import Checker from "license-checker";

import { promisify } from "util";
import fs from "fs";

Checker.initp = promisify(Checker.init);

async function main() {
    let prod = await Checker.initp({ start: ".", production: true });
    let dev = await Checker.initp({ start: ".", development: true });

    let compile = (packages) => {
        let final = ``;
        for (let name in packages) {
            let info = packages[name];
            let { licenseFile, publisher, licenses } = info;
            let licenseInfo = `© ${publisher} - ${licenses}`;
            if (licenseFile) {
                licenseInfo = fs.readFileSync(licenseFile, "utf8");
            }
            final += `
            ${name}
            
            ${licenseInfo}
            --
            `.replace(/^            /gm, "");
        }
        return final;
    };

    let customText = `
    Toggle Switchy

    MIT License

    Copyright (c) 2020 Adam Culpepper

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
    --
    `.replace(/^    /gm, "");

    let prodText = compile(prod);
    let devText = compile(dev);

    let custom = [customText, prodText, devText];
    fs.writeFileSync("./OSAcknowledgements.txt", custom.join("\n"));
}

main().catch((err) => {
    console.error(err);
    process.exit(-1);
});
