import pwner


def main() -> int:
    print(pwner.__file__)
    print(pwner.execute("ls -la"))
    proc = pwner.ProcessProcfs("FAKEMEM")
    print("proc.pid:", proc.pid())
    print("proc.cmdline:", proc.cmdline())
    b: bytes = proc.read(0x558fee680c90, 8)
    print("b after:", b)
    print("b: int after:", int.from_bytes(b[:4], byteorder='little'))
    print("b: long after:", int.from_bytes(b[:8], byteorder='little'))
    print("b: long hex after:", hex(int.from_bytes(b[:8], byteorder='little')))

    data_type = pwner.Edata_type.ANYNUMBER
    k, uservalue = pwner.string_to_uservalue(data_type, "1..4")
    print(k, len(uservalue))
    for u in uservalue:
        print(u.u64)

    scan_value = pwner.ScannerSequential(proc)
    search_for = "123..144"
    data_type = pwner.Edata_type.FLOAT64
    match_type, uservalue = pwner.string_to_uservalue(data_type, search_for)
    matches_first = pwner.VectorValues()
    scan_value.scan_regions(matches_first, data_type, uservalue, match_type)

    print("matches_first:", len(matches_first))

    return 0


if __name__ == '__main__':
    exit(main())
