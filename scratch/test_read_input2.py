import subprocess

def test_cmd(inp):
    p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    try:
        stdout, stderr = p.communicate(inp, timeout=2)
        print(f"=== INPUT: {inp!r} ===")
        print(stdout)
        if stderr:
            print("STDERR:", stderr.strip())
    except subprocess.TimeoutExpired:
        p.kill()
        print(f"=== INPUT: {inp!r} -> TIMEOUT ===")

test_cmd('echo a\\\nb\n')
test_cmd('echo \\\\\n')
