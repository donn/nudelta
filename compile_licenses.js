import Checker from "license-checker";

import { promisify } from "util";
import fs from "fs";
import path from "path";

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

    let customText = ``;

    let submoduleDir = path.join(".", "submodules");
    let submodules = fs.readdirSync(submoduleDir);
    for (let name of submodules) {
        let modulePath = path.join(submoduleDir, name);
        try {
            let licenseFile = fs
                .readdirSync(modulePath)
                .filter((f) => f.toLowerCase().includes("license"))[0];
            let licensePath = path.join(modulePath, licenseFile);

            customText += `
            ${name}
            
            ${fs.readFileSync(licensePath, "utf8")}
            ---
            `.replace(/^            /gm, "");
        } catch (error) {
            continue;
        }
    }

    let prodText = compile(prod);
    let devText = compile(dev);

    let custom = [customText, prodText, devText];
    fs.writeFileSync("./OSAcknowledgements.txt", custom.join("\n"));
}

main().catch((err) => {
    console.error(err);
    process.exit(-1);
});
