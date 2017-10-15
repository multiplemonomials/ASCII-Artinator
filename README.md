# ASCII Artinator 
## Introduction 
Ascii Artinator is a tool that converts png images to greyscale (extended) ascii text which looks like the image.
In most cases, youl'll need a text editor with zooming support and/or really small font sizes to view the image.
On Windows, this is Notepad++ (make sure Encoding is ASCII and Character Set is Windows-1252).  I don't know what to use on other platorms.

## Usage

Just run the `aart` command, providing input and output filenames.

#### Full Usage: 
```
aart [-v|--verbose] [-d|--debug] [-n|--noScaling] [-i|--invert] [-e|--version] [-a|--asciiOnly] [-f|--force] [-h|--help] [-b|--blockSize=<n>] [-c|--font=[font]] <input> <output>
-v, --verbose             enable verbose (basically progress) output
-d, --debug               enable debug output for development
-n, --noScaling           disable scaling the darkest character to near-black
-i, --invert              invert lights and darks in output
-e, --version             print version and exit
-a, --asciiOnly           use ASCII only, not extended ASCII. This allows you to use a utf-8 editor to view the output.
-f, --force               overwrite output file if it exists
-h, --help                list usage options (this)
-b, --blockSize=<n>       set number of pixels (in either axis) of the original image per character.  Default 16.
-c, --font=[font]         Full path to font file, or just its name if if is in the system default folder. (e.g. consola.ttf).
<input>                   filename to read png image from
<output>                  filename to write ASCII image to
```

## Building
To build the C++ code, use CMake.

You will need a few supporting libraries:
libargtable2, freetype, libpng, and libpng++.

## Builds by Me 
Win32: download [here](https://app.box.com/shared/static/b3o0v9lr70x0t8yvelk6t27pabmjftf8.zip)