Run:

```
docker run -t -i --rm -v "$(pwd):/code" ubuntu:jammy /bin/bash
apt-get update
apt-get install --yes build-essential
cd /code
make
./main
```

Expected output:

```
Start
Fork
Child's PID is 454850
Sleep
Pause in the child
ptrace start
PTRACE_SEIZE
PTRACE_INTERRUPT
waitpid
PTRACE_DETACH
ptrace success
pidfd_getfd start
pidfd_open
pidfd_getfd
kcmp
pidfd_getfd success
kcmp start
kcmp
kcmp success
Kill child
Wait for child
End
```

So important are `ptrace success`, `pidfd_getfd success`, and `kcmp success`.
