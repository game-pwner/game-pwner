import pwner


def main() -> int:
    print(pwner.__file__)
    print(pwner.execute("ls -la"))
    return 0


if __name__ == '__main__':
    exit(main())
