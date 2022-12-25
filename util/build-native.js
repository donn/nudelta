import fs from "fs-extra";
import { execFile } from "child_process";


async function main() {
    let cmakeJsStatus = await new Promise((resolve) => {
        let extraArgs = [];
        if (process.platform === "win32") {
            extraArgs = ["-A", "x64"];
        }

        let cmakejs = execFile("node", [
            "./node_modules/cmake-js/bin/cmake-js",
            "-l",
            "silly"
        ].concat(extraArgs));

        cmakejs.stdout.pipe(process.stdout);
        cmakejs.stderr.pipe(process.stderr);
        cmakejs.on('exit', resolve);

    });

    if (cmakeJsStatus != 0) {
        throw new Error("Cmake-js exited with a non-zero code");
    }

    await fs.copyFile("./build/Release/node-libnd.node", "./node-libnd.node")
}

main().catch(err => {
    console.error(err);
    process.exit(-1);
})