const fs = require('mz/fs'),
    Path = require('path');
//UglifyJS = require("uglify-js")

function toHex(buffer) {
    let string = []
    for (let i of buffer) {
        string.push(i);
    }
    return string;
}

async function run() {

    let files = await fs.readdir("js");

    let futures = files.map(file => {
        if (Path.extname(file) !== '.js') return null;
        file = Path.join(process.cwd(), 'js', file);
        return fs.readFile(file).then(m => {
            return {
                file: file,
                buffer: toHex(m),
            };
        })
    }).filter(m => m != null);

    let out = await Promise.all(futures);

    let str = out.map(m => {
        const name = Path.basename(m.file, '.js');
        return `const unsigned char file_${name}[] = {\n  ${m.buffer.join(',')}\n};\n\nconst int file_${name}_len = ${m.buffer.length};\n`
    }).join('\n');

    out = [
        `#pragma once\n\n// AUTO GENERATED - DO NOT EDIT\n`,
        //"namespace gencore {\nnamespace internal {\n",
        str, "\n\n" //}\n\n" //}\n\n"
    ];


    console.log(out.join('\n'));
}

run().then(() => {

}).catch(e => console.error(e))