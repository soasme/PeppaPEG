import subprocess
import sys
import json
import shlex

def test_spec():
    executable = sys.argv[1]
    specs_json = sys.argv[2]
    with open(specs_json) as f:
        specs = json.load(f)

    failed, total = 0, 0
    for spec in specs:
        for test in spec['tests']:
            total += 1
            proc = subprocess.run(
                shlex.split(executable) + [
                    'ast',
                    '--grammar', spec['grammar'],
                    '--grammar-entry', spec['entry'],
                ],
                capture_output=True,
                input=test['I'].encode('utf-8'),
            )
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
                        failed += 1
                else:
                    print(
                        f"GRAMMAR:\n{spec['grammar']}\n"
                        f"INPUT:\n{test['I']}\n"
                        f"OUTPUT:\n{test['O']}\n"
                        f"GOT:\n{proc.stderr.decode('utf-8')}\n"
                    )
                    failed += 1
            else:
                assert proc.returncode != 0, proc.stderr
                if proc.stderr.decode('utf-8').strip() != test['E']:
                    print(
                        f"GRAMMAR:\n{spec['grammar']}\n"
                        f"INPUT:\n{test['I']}\n"
                        f"ERROR:\n{test['E']}\n"
                        f"GOT:\n{proc.stderr.decode('utf-8')}"
                    )
                    failed += 1

    print("total: %d, failed: %d" % (total, failed))
    if failed:
        exit(1)

if __name__ == '__main__':
    test_spec()
