import sys
import re
from glob import glob
import os
from typing import Set

if __name__ == '__main__':
    file_to_parse = sys.argv[1]
    project_root = sys.argv[2]
    os.chdir(project_root)

    # create a set of all source files and rename them as header files.
    # if the header file in the includes has a corresponding source file, we package that in the test executable.
    discrete_header_files = {header_file.removeprefix('src/main/') for header_file in
                             glob('src/main/**/*.h', recursive=True)}
    discrete_source_files = {header_file.removeprefix('src/main/') for header_file in
                             glob('src/main/**/*.cpp', recursive=True)}
    pattern = re.compile(r'#include\s+"(.*?)"')

    dependency_source_files = set()

    def recursive_search(filename: str):
        with open(filename, 'r') as file_reader:
            file_contents = file_reader.read()
            header_includes = [header_match for header_match in pattern.findall(file_contents) if
                         header_match in discrete_header_files]
            dependency_source_files.update([header.replace('.h', '.cpp') for header in header_includes if header.replace('.h', '.cpp') in discrete_source_files])
            for header in header_includes:
                recursive_search(project_root + "/src/main/" + header)

    recursive_search(file_to_parse)

    print(';'.join(map(lambda header: project_root + "/src/main/" + header.replace('.h', '.cpp'),dependency_source_files)), end='')
