import subprocess, time

p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

for cmd in ['(echo 1)', '(echo 2)', ')', '(echo 3)']:
    p.stdin.write(cmd + '\n')
    p.stdin.flush()
    time.sleep(0.1)

p.stdin.close()
stdout, stderr = p.communicate()
print('STDERR:\n', stderr)
print('STDOUT:\n', stdout)
