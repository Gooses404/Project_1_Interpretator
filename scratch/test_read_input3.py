import subprocess, time

p = subprocess.Popen(['./mysh'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
p.stdin.write('echo \\\\\n')
p.stdin.flush()
time.sleep(0.5)

# Poll process or check if it processed the line
# If it processed the line, it would print "COMMAND: echo \" and then "Start of reading"
out = ''
p.stdin.write('exit\n') # If it didn't process the first line, 'exit' becomes part of the first line!
p.stdin.flush()
time.sleep(0.5)
p.stdin.close()
stdout, stderr = p.communicate()
print("STDOUT:\n", stdout)
