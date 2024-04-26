
# Retreive

This is a Linux command-line utility which browse recursively any directory, so to retreive the files that match given labels. I assume you are in a Linux bash terminal. Started as a bench for the new C++ filesystem library, the small utility has grown to a tool which I daily use to find my way through my thousands of note files. The key features are:
- search for the labels both in the file path and in the file content (where the labels must start with `%%`) ;
- let you configure some synonyms such as `cpp == c++` ;
- let you configure some relations such as `linux => unix`.

If I ask for the files about `c++ unix`, the file called `compile-cpp-with-linux.md` will be found, or the file called `CompileWithLinux.md` if it has `%%cpp` within.


## Credits

- The file `argparse.hpp` 3.0 has been downloaded from https://github.com/p-ranav/argparse
- Developed with the help of Docker and [this recipe](https://github.com/chavid/DevScripts/blob/main/Cpp20/Dockerfile).
- The tests are handled with [oval script](https://github.com/chavid/DevScripts/blob/main/bin/oval.py) .


## Requirements

This is all done for Linux and a bash terminal.
It can be compiled with any compiler supporting C++20.


## Fast and dirty guide

UNDER WORK...

Use `rt --help`.

The [complete guide](tests/USERGUIDE.md) is UNDER WORK also...

## Tips & tricks

- The tool cannot recognize whether a file is textual or not. By default, it will only search for the labels in the files paths. You can help the tool to recognize the textual files by listing the pertinent name extensions in the `parse` parameter of the `.retreive` file.
