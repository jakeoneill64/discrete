import sys
from pathlib import Path
import re

if __name__ == '__main__':
    action_pattern = r'(class\s+\w+):\s?public\s+Action'
    paths_to_process = sys.argv[1:]
    header_files = map(lambda path: Path(path).rglob("*.h"), paths_to_process)
    parsed_declarations = []
    for header_file in header_files:
        with open(header_file, 'r') as file:
            file_content = file.read()
            action_declaration_match = re.search(action_pattern, file_content)
            if action_declaration_match:
                parsed_declarations += action_declaration_match.group(1)

