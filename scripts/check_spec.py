import os.path
import subprocess
import sys
import json
import shlex

def test_spec():
    executable = sys.argv[1]
    specs_json = sys.argv[2]
    with open(specs_json) as f:
        try:
            specs = json.load(f)
        except json.decoder.JSONDecodeError:
            print("invalid json spec")
            exit(1)

    failed, ignored, total = 0, 0, 0
    for spec in specs:
        for test in spec['tests']:
            total += 1
            cmd = shlex.split(executable) + [
                'ast',
                '--grammar-entry', spec['entry'],
            ]
            if 'grammar' in spec:
                cmd.extend(['--grammar-str', spec['grammar']])
            elif 'grammar_file' in spec:
                if spec['grammar_file'].startswith('/'):
                    cmd.extend(['--grammar-file', spec['grammar_file']])
                else:
                    cmd.extend(['--grammar-file', os.path.dirname(os.path.abspath(specs_json)) + '/' + spec['grammar_file']])
            else:
                raise ValueError('Missing grammar/grammar_file')
            proc = subprocess.run(
                cmd,
                capture_output=True,
                input=test['I'].encode('utf-8'),
            )
            if 'O' in test:
                if proc.returncode == 0:
                    output = json.loads(proc.stdout.decode('utf-8'))
                    expect = test['O']
                    if output != expect:
                        print(
                            f"GRAMMAR:\n{spec.get('grammar') or spec.get('grammar_file')}\n"
                            f"INPUT:\n{test['I']}\n"
                            f"OUTPUT:\n{test['O']}\n"
                            f"GOT:\n{json.dumps(output)}\n"
                        )
                        failed += 1
                else:
                    print(
                        f"GRAMMAR:\n{spec.get('grammar') or spec.get('grammar_file')}\n"
                        f"INPUT:\n{test['I']}\n"
                        f"OUTPUT:\n{test['O']}\n"
                        f"GOT:\n{proc.stderr.decode('utf-8')}{proc.stdout}\n"
                    )
                    failed += 1
            elif 'E' in test:
                if proc.stderr.decode('utf-8').strip() != test['E']:
                    print(
                        f"GRAMMAR:\n{spec.get('grammar') or spec.get('grammar_file')}\n"
                        f"INPUT:\n{test['I']}\n"
                        f"ERROR:\n{test['E']}\n"
                        f"GOT:\n{proc.stderr.decode('utf-8')}{proc.stdout.decode('utf-8')}"
                    )
                    failed += 1
            else:
                ignored += 1

    print("total: %d, failed: %d, ignored: %d" % (total, failed, ignored))
    if failed:
        exit(1)

if __name__ == '__main__':
    test_spec()
