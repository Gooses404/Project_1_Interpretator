import subprocess

def test_multiline(inp, label):
    p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    out, err = p.communicate(inp)
    print(f"=== {label} ===")
    print("STDOUT:\n", out)
    if err:
        print("STDERR:\n", err)

test_multiline('echo 1 |\nwc\n', 'Pipe continuation (echo 1 | <enter> wc)')
test_multiline('echo 1 &&\necho 2\n', 'AND continuation (echo 1 && <enter> echo 2)')
test_multiline('(echo 1) > out.txt\n', 'Subshell redir AST test')
