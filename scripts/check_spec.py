import subprocess
import sys
import json

def test_spec():
    executable = sys.argv[1]
    specs_json = sys.argv[2]
    with open(specs_json) as f:
        specs = json.load(f)

    for spec in specs:
        with open('grammar', 'w') as grammar_file:
            grammar_file.write(spec['grammar'])
        for test in spec['tests']:
            with open('input', 'w') as input_file:
                input_file.write(test['I'])
            command = f'{executable} ast -g grammar -e {spec["entry"]} input'
            proc = subprocess.run(command, capture_output=True, shell=True)
            if 'O' in test:
                if proc.returncode == 0:
                    output = json.loads(proc.stdout.decode('utf-8'))
                    expect = test['O']
                    if output != expect:
                        print(
                            f"GRAMMAR:\n{spec['grammar']}\n"
                            f"INPUT:\n{test['I']}\n"
                            f"OUTPUT:\n{test['O']}\n"
                            f"GOT:\n{output}\n"
                        )
                else:
                    print(
                        f"GRAMMAR:\n{spec['grammar']}\n"
                        f"INPUT:\n{test['I']}\n"
                        f"OUTPUT:\n{test['O']}\n"
                        f"GOT:\n{proc.stderr.decode('utf-8')}\n"
                    )
            else:
                assert proc.returncode != 0, proc.stderr
                if proc.stderr.decode('utf-8').strip() != test['E']:
                    print(
                        f"GRAMMAR:\n{spec['grammar']}\n"
                        f"INPUT:\n{test['I']}\n"
                        f"ERROR:\n{test['E']}\n"
                        f"GOT:\n{proc.stderr.decode('utf-8')}"
                    )

if __name__ == '__main__':
    test_spec()
