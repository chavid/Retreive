
## How labels for a file are produced ?

- The path is split into into elements.
- If relevant, the textual file content is searched for word starting with `%%`, which are added to the elements.
- The elements are split by non-alphanumeric characters.
- The elements are split by uppercase characters (CamelCase).
- The elements are lowercased.
- Single letters are discarded.

## How SUGGESTED LABELS are produced ?

When `retreive.exe` or (`rt` for short) is called without any argument, it tries to find the labels which may help the user to select a subset of the files. Also, when `retreive.exe` is called with labels, it tries to find the labels which may help the user to refine its selection. We call this the SUGGESTED LABELS. They may be numerous, so we apply some rules to keep the list as short and relevant as possible. We discard:
- the labels which match all the files
- the labels which match only one file
- the labels which have a matching ancestor

## The configuration files

Those files are searched for, and applied in this order:
1. `~/.retreive` (the user's configuration)
2. `./.retreive` (the current directory's configuration)
3. `<command-line-directory>/.retreive` (the directory given with the `-d` option)

The kind of lines that are expected are:
- `name : value1 value2 ...` : a general parameter for the program
- `label == synonym1 synonym2 ...` : label synonyms
- `label => ancestor1 ancestor2 ...` : label ancestors

Actually, you can put several labels on the left side of the `==` or `=>` operator, and several values on the right side. The program will generate all the possible combinations.

The expected general parameters (and their default values) are:
- `ignore_dir : <empty default value>` : thoses directories are ignored;
- `ignore_ext : <empty default value>` : the files with thoses extensions are ignored (put the `.`);
- `parse : .txt .md` : the content of those files is parse;
- `max_entries_per_matching_directory : 5` : when a directory match the labels (because of its path), the program prints its files and subdirectories only if there are no more than this maximum number of entries. 

