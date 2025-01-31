import sys
from pathlib import Path
import re
import itertools

if __name__ == '__main__':
    action_pattern = r'class\s+(\w+)\s*:\s?public\s+Action<>'
    paths_to_process = sys.argv[1:]
    output_file = sys.argv[-1]
    header_files = list(itertools.chain.from_iterable(Path(path).rglob("*.h") for path in paths_to_process))
    parsed_declarations = []
    for header_file in header_files:
        with open(header_file, 'r') as file:
            file_content = file.read()
            action_declaration_match = re.search(action_pattern, file_content)
            if action_declaration_match:
                parsed_declarations.append(action_declaration_match.group(1))

    action_registrations = list(
        map(
            lambda declaration:
    f'''
    actionFactory.registerType(
        "{declaration}",
        [](){{
            return std::make_unique<{declaration}>();
        }}
    );''',
            parsed_declarations
        )
    )

    imports = '\n'.join([f'#include "{action_name}.h"' for action_name in parsed_declarations])
    factory_registrations = '\n'.join(action_registrations)

    with open(output_file, 'w') as registration_file:
        registration_file.write(
f"""#ifndef DISCRETE_ACTION_REGISTRATION_H
#define DISCRETE_ACTION_REGISTRATION_H

{imports}
#include "Factory.h"
#include <memory>

// --- GENERATED FILE ---
inline void registerActions(Factory<Action<>>& actionFactory){{
    {factory_registrations}
}}
#endif //DISCRETE_ACTION_REGISTRATION_H
"""
        )