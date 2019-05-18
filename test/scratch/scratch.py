import pwner
import time
import os
import sys


def main():
    print(pwner.__file__)
    # print(pwner.__version__)
    print(os.environ['PYTHONPATH'].split(os.pathsep))

    pwner.oom_score_adj(999)
    b = pwner.execute("ls -la")
    print(b)
    time.sleep(1)
    # help(pwner)

    print("1 static:", pwner.IOProcfs.running(1))
    print("1488 static:", pwner.IOProcfs.running(13488))

    pfs1 = pwner.IOProcfs(1)
    b = pfs1.running()
    print("1:", b)
    pfs1488 = pwner.IOProcfs(13488)
    print("1488:", pfs1488.running())


if __name__ == '__main__':
    main()
