import subprocess

tests = [
    '(echo 1)',
    '(echo 1; echo 2)',
    '(echo 1; echo 2; echo 3)',
    '((echo 1))',
    '(((echo 1)))',
    '(echo 1) && (echo 2)',
    '(echo 1) | (echo 2)',
    '(echo 1; (echo 2; echo 3))',
    '((echo 1; echo 2); echo 3)',
    '(echo 1;)',
    'echo (1)',
    '(echo 1) > file',
    'ls ; (echo 1)',
    '(echo 1) ; ls',
    '(ls ; (echo 1; echo 2))',
    '(echo 1 && echo 2)',
    '((echo 1 && echo 2) || echo 3)',
    '(echo 1) ; (echo 2)',
]

for t in tests:
    res = subprocess.run(['./mysh'], input=(t + '\n').encode(), capture_output=True)
    out = res.stdout.decode(errors='replace')
    err = res.stderr.decode(errors='replace')
    print(f'INPUT: {t}')
    if err:
        print(f'  STDERR: {err.strip()}')
    lines = [l for l in out.splitlines() if any(k in l for k in ['SUBSHELL', 'COMMAND', 'SEQ', 'PIPE', 'AND', 'OR', 'redir', 'Node count'])]
    print('  AST:')
    for l in lines:
        print('   ', l)
    print()
