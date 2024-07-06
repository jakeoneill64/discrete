import sys
import re
from glob import glob
import os

if __name__ == '__main__':
  file_to_parse = sys.argv[1]
  project_root = sys.argv[2]
  os.chdir(project_root)
  with open(file_to_parse, 'r') as file_reader:
    file_contents = file_reader.read()
    pattern = re.compile(r'#include\s+"(.*?)"')
    includes_matches = [header_match for header_match in pattern.findall(file_contents)]
    # create a set of all source files and rename them as header files.
    discrete_header_files = {source_file.replace('.cpp', '.h').removeprefix('src/main/') for source_file in glob('src/main/**/*.cpp', recursive=True)}
    # if the header file in the includes has a corresponding source file, we package that in the test executable.
    print(';'.join([project_root + "/src/main/" + source_file.replace('.h', '.cpp') for source_file in includes_matches if source_file in discrete_header_files]), end='')

