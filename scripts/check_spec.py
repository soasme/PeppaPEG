import subprocess
import sys
import json

def test_spec():
    specs_json = sys.argv[1]
    with open(specs_json) as f:
        specs = json.load(f)

    for spec in specs:
        with open('grammar', 'wb') as grammar_file:
            grammar_file.write(spec['grammar'].encode('utf-8'))
        for test in spec['tests']:
            command = f'./peppa -g grammar -e {spec["entry"]} -'
            proc = subprocess.run(command, capture_output=True, shell=True, input=test['I'].encode('utf-8'))
            if 'O' in test:
                assert proc.returncode == 0, proc.stderr
                output = json.loads(proc.stdout.decode('utf-8'))
                expect = test['O']
                assert output == expect, f'spec: {spec}'
            else:
                assert proc.returncode != 0, proc.stderr
                assert proc.stderr.decode('utf-8').strip() == test['E'], proc.stderr

if __name__ == '__main__':
    test_spec()
