Module['locateFile'] = function (path, scriptDirectory) {
  if (typeof window !== "undefined" && window['GeometryCentralWASMPath'] !== null) {
    return window['GeometryCentralWASMPath'];
  }
  return scriptDirectory + path;
}

Module['onRuntimeInitialized'] = function () {
  const GeometryCentral = Module;
};
