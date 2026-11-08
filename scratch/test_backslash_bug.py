import subprocess

p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
stdout, stderr = p.communicate('echo \\\\a\n')
print("OUTPUT:\n", stdout)
