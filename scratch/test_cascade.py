import subprocess
p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
stdout, stderr = p.communicate('echo 1)\n(echo 2)\n(echo 3)\n')
print('STDERR:\n', stderr)
print('STDOUT:\n', stdout)
