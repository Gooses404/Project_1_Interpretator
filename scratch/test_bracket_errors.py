import subprocess

tests = [
    'echo )',
    '(echo 1))',
    '((echo 1)) )',
    '(echo 1) )',
    '( (echo 1) )',
    'echo 1 ; )',
    '(echo 1 ; echo 2) )',
    '((echo 1) ; echo 2)',
    '(echo 1) ; (echo 2) )',
    'ls\n)',
    '(\n)',
    '()',
    'echo a\necho b)\n',
]

for t in tests:
    res = subprocess.run(['./mysh'], input=t.encode(), capture_output=True)
    out = res.stdout.decode(errors='replace')
    err = res.stderr.decode(errors='replace')
    print(f'INPUT: {t!r}')
    if err:
        print(f'  STDERR: {err.strip()}')
    lines = [l for l in out.splitlines() if any(k in l for k in ['SUBSHELL', 'COMMAND', 'SEQ', 'PIPE', 'AND', 'OR', 'redir', 'Node count'])]
    print('  AST:')
    for l in lines:
        print('   ', l)
    print()
