# Reverse Engine (pre-alpha) 

Reverse Engine is an another reverse engineering tool inspired by Cheat Engine.
You can also create [trainers](./trainer.cc). Even with Python.


## Contents

  * [License](#license)
  * [Features](#features)
  * [Cheat example](#cheat-example)
  * [Contacts](#contacts)

## License

<a rel="license" href="https://www.gnu.org/copyleft/lesser.html">
  <img alt="LGPLv3" style="border-width:0" src="https://www.gnu.org/graphics/lgplv3-88x31.png"/>
</a> This work is licensed under a <a rel="license" href="https://www.gnu.org/copyleft/lesser.html">GNU Lesser General Public License v3</a>.

## Features

- [x] Linux
  - [x] Process' handler
    - [x] Read and write any value
    - [x] Value scanner
    - [ ] Pattern scanner
    - [x] Pointer scanner
      - [x] Forward ~~> (support scanning for multiple addresses)
      - [x] Backward (like Cheat Engine)
    - [ ] Breakpoints and Watchpoints
    - [ ] Code injection
    - [x] Handlers
      - [x] procfs
        - [ ] opensource rootkit (to track regions)
      - [ ] criu
  - [ ] Tools
  - [ ] CLI support
  - [ ] GUI support
  - [ ] Python binding (pybind11)
  - [ ] MPI support

## Support

### Arch support
But who uses 32 bit machine?

- [x] 64 bit -> 64 bit targets
- [ ] 64 bit -> 32 bit targets
- [ ] 32 bit -> 64 bit targets
- [ ] 32 bit -> 32 bit targets

### OS support

- [x] linux
- [ ] mac
- [ ] windows

### Compiler support

- [x] gcc
- [ ] clang
- [ ] intel
- [ ] msvc

## Cheat example
*Further information:* [Advanced C++ example of CS:GO trainer](./example/csgo_linux64/fix_resources_download/main.cc) 
### Example is a bit out of date, you can check [our testing suites](./pwner/test). It is also have example of python executable
```cpp
handler h("csgo_linux64");
h.update_regions();
region *client = h.get_region_by_name("client_client.so");
uintptr_t glow_pointer_call;
h.find_pattern(&glow_pointer_call,
               client,
               "\xE8\x00\x00\x00\x00\x48\x8b\x10\x48\xc1\xe3\x06\x44",
               "x????xxxxxxxx");
uintptr_t glow_call = h.get_call_address(glow_pointer_call);
uintptr_t glow_array_offset;
h.read(&glow_array_offset, glow_call+0x10, sizeof(uintptr_t));
// etc...
```

## Contacts

- Ivan Stepanov ivanstepanovftw@gmail.com
- Andrea Stacchiotti andreastacchiotti@gmail.com 

