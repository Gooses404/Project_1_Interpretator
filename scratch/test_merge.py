import subprocess

p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
stdout, stderr = p.communicate('echo 1 |\nwc\n')
print('STDOUT:\n', stdout)
print('STDERR:\n', stderr)
