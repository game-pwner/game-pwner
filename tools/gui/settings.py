#!/usr/bin/env python
######################################################################
# Global variables for pwner
######################################################################
from pwner import *
import pwner

proc = ProcessProcfs(0)
scan_value = ScannerSequential(proc)


def init():
    global proc
    global scan_value
