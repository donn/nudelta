import resolve from "@rollup/plugin-node-resolve";
import commonjs from "@rollup/plugin-commonjs";
import { terser } from "rollup-plugin-terser";
import serve from "rollup-plugin-serve";
import livereload from "rollup-plugin-livereload";
import css from "rollup-plugin-import-css";
import json from "rollup-plugin-json";
import yaml from "@rollup/plugin-yaml";
import nodepf from "rollup-plugin-node-polyfills";
import url from "@rollup/plugin-url";

const production = process.env.NODE_ENV == "production";
const watching = process.env.ROLLUP_WATCH;

export default {
    external: [
        "jsdom",
        "jsdom/lib/jsdom/living/generated/utils",
        "jsdom/lib/jsdom/utils",
    ],
    input: "ui/src/app.js",
    output: {
        file: "ui/app-dist/app.js",
        format: "iife",
        sourcemap: true,
        globals: {
            "jsdom": "trash",
            "jsdom/lib/jsdom/living/generated/utils": "trash",
            "jsdom/lib/jsdom/utils": "trash",
        },
    },
    plugins: [
        watching && serve(),
        watching && livereload(),
        url({
            limit: 10 * 1024,
            include: ["**/*.woff", "**/*.woff2"],
            emitFiles: true,
            destDir: "./ui/app-dist/",
            fileName: `[name][extname]`,
        }),
        css(),
        json(),
        yaml(),
        resolve({ browser: true }),
        commonjs(),
        nodepf(),
        production && terser(),
    ],
};
