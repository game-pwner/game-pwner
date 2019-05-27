import pwner


def main() -> int:
    print(pwner.__file__)
    print(pwner.execute("ls -la"))
    proc = pwner.ProcessProcfs("FAKEMEM")
    print("proc.pid:", proc.pid())
    print("proc.cmdline:", proc.cmdline())
    mem: pwner.mem64 = pwner.mem64()
    b: bytes = b""
    print(b)
    print("mem before:", mem)
    print("b before:", b)
    b = proc.read(0x561d39c53c90, pwner.addressof(mem), 8)
    b = proc.read(0x561d39c53c90, 8)
    print("mem after:", mem)
    print("b after:", b)
    print("b: int after:", int.from_bytes(b[:4], byteorder='little'))
    print("b: long after:", int.from_bytes(b[:8], byteorder='little'))
    print("b: long hex after:", hex(int.from_bytes(b[:8], byteorder='little')))
    return 0


if __name__ == '__main__':
    exit(main())
