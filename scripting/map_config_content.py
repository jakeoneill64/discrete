import yaml
import argparse

def flatten(d, parent_key='', sep='.'):
    items = []
    for k, v in d.items():
        new_key = f"{parent_key}{sep}{k}" if parent_key else k
        if isinstance(v, dict):
            items.extend(flatten(v, new_key, sep=sep))
        elif isinstance(v, list):
            for item in v:
                items.append((new_key, item))
        else:
            items.append((new_key, v))
    return items

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('--input', type=str, required=True,help='yaml file to parse')
    parser.add_argument('--output', type=str, required=True,help='sql file to write')

    args = parser.parse_args()
    input_yaml, output_sql = args.input, args.output

    with open(input_yaml, 'r') as yaml_file:
        data = yaml.safe_load(yaml_file)

    flattened = flatten(data, sep='.')

    with open(output_sql, 'w') as sql_file:
        sql_file.write(
f"""-- GENERATED FILE --
INSERT INTO config (key, value)
VALUES 
{',\n'.join([f"('{key}', '{value}')" for key, value in flattened])};
"""
        )


