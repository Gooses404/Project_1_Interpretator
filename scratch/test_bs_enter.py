import subprocess, time

p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
p.stdin.write('echo \\\\\n')
p.stdin.flush()
time.sleep(0.3)
# If it transferred line, it printed ">" and is waiting for more
p.stdin.write('finished\n')
p.stdin.flush()
time.sleep(0.3)
p.stdin.close()
stdout, stderr = p.communicate()
print("OUTPUT:\n", stdout)
