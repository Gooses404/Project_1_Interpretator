import subprocess

def test_cmd(inp):
    p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    try:
        stdout, stderr = p.communicate(inp, timeout=2)
        print(f"INPUT: {inp!r} -> OK")
        print("  STDOUT lines:", len(stdout.splitlines()))
        if stderr:
            print("  STDERR:", stderr.strip())
    except subprocess.TimeoutExpired:
        p.kill()
        print(f"INPUT: {inp!r} -> HUNG / TIMEOUT (Did not finish reading line!)")

test_cmd('echo 1\n')
test_cmd('echo \\\\\n')
test_cmd('echo a\\\nb\n')
test_cmd('echo \\\\\\\\\n')
