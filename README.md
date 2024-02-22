
This is a command-line utility which browse recursively any directory, so to retreive the files that match given labels. I assume you are in a Linux bash terminal. Started as a bench for the new C++ filesystem library, the small utility has grown to a tool which I daily use to find my way through my thousands of note files. The key features are:
- search for the labels both in the file path and in the file content (where the labels must start with `%%`) ;
- let you configure some synonyms such as `cpp == c++` ;
- let you confinure some relations such as  `linux => unix`.
If I ask for the files about `c++ unix`, the file called `compile-cpp-with-linux.md` will be found, or the file called `CompileWithLinux.md` if it has `%%cpp` within.

# Credits

- The file `argparse.hpp` 3.0 has been downloaded from https://github.com/p-ranav/argparse

# Requirements

## Compiling

It should work with any compiler supporting C++20 and `gmake`.

# Development tips and tricks

## Testing

This project is developed with the help of Docker and [this recipe](https://github.com/chavid/DevScripts/blob/main/Cpp20/Dockerfile). Running the provided tests rely on the [oval script](https://github.com/chavid/MyDevTools/blob/main/bin/oval.py) .

## Naming rules

- `*.h` : module header file
- `*.cc` : module body file
- `*.cpp` : application file





