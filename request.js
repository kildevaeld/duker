function isString(a) {
    return typeof a === 'string';
}
exports.isString = isString;

function isObject(obj) {
    return obj === Object(obj);
}
exports.isObject = isObject;

const _has = Object.prototype.hasOwnProperty;

function has(obj, prop) {
    return _has.call(obj, prop);
}
exports.has = has;

function extend(obj, ...args) {
    if (!isObject(obj))
        return obj;
    for (let i = 0, ii = args.length; i < ii; i++) {
        const o = args[i];
        if (!isObject(o))
            continue;
        for (const k in o) {
            if (has(o, k))
                obj[k] = o[k];
        }
    }
    return obj;
}


exports.get = function () {

}