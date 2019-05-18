//
// Created by root on 03.11.18.
//
#include <string>
#include <sstream>
#include <vector>


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
#include <pwner/external.hh>
#include <pwner/process/Process.hh>
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace PWNER {
    // static PyObject *
    // addressof(PyObject *self, PyObject *obj)
    // {
    //     if (CDataObject_Check(obj))
    //         return PyLong_FromVoidPtr(((CDataObject *)obj)->b_ptr);
    //     PyErr_SetString(PyExc_TypeError,
    //                     "invalid type");
    //     return NULL;
    // }

    py::object addressof(py::object& o)
    {
        py::object addr = o.attr("_address_")();
        return addr;
    }
}

PYBIND11_MODULE(pwner, m) {
    m.doc() = "pybind11 wrapper for Reverse Engine";
    m.def("addressof", &PWNER::addressof, "gets the address where the function pointer is stored");

    ///*
    // TODO[high]: split to modules to speedup build time
    // // RE/shape.hh
    // py::class_<PWNER::Shape>(m, "Shape")
    //         .def_readwrite("x", &PWNER::Shape::x)
    //         .def_readwrite("y", &PWNER::Shape::y)
    //         .def("move", &PWNER::Shape::move)
    //         .def("area", &PWNER::Shape::area)
    //         .def("perimeter", &PWNER::Shape::perimeter)
    //         .def_readwrite_static("nshapes", &PWNER::Shape::nshapes)
    //         ;
    // py::class_<PWNER::Circle, PWNER::Shape>(m, "Circle")
    //         .def(py::init<double>())
    //         ;
    // py::class_<PWNER::Square, PWNER::Shape>(m, "Square")
    //         .def(py::init<double>())
    //         ;
    //
    // // RE/nearest_neighbors.hh
    // py::class_<PWNER::Point>(m, "Point")
    //         .def(py::init<>())
    //         .def(py::init<double, double>())
    //         .def_readwrite("x", &PWNER::Point::x)
    //         .def_readwrite("y", &PWNER::Point::y)
    //         ;
    // py::class_<PWNER::NearestNeighbors>(m, "NearestNeighbors")
    //         .def(py::init<>())
    //         .def_readwrite("points", &PWNER::NearestNeighbors::points)
    //         .def("nearest", &PWNER::NearestNeighbors::nearest)
    //         ;
    m.def("oom_score_adj", &oom_score_adj);

    // RE/external.hh
    m.def("execute", &PWNER::execute);
    py::class_<PWNER::CProcess>(m, "CProcess")
            .def(py::init<const std::string &, const std::string &, const std::string &>())
            .def("__repr__", &PWNER::CProcess::str)
            ;
    m.def("getProcesses", &PWNER::getProcesses);
    m.def("get_mem_total", &PWNER::get_mem_total, "list(MemTotal, MemFree, MemAvailable, Cached)[i]", py::arg("i") = 0);
    m.def("get_mem_free", &PWNER::get_mem_free);

    // pwner/process/Region.hh
    py::class_<PWNER::PROCESS::Region>(m, "Region")
            .def(py::init<>())
            .def("__repr__", &PWNER::PROCESS::Region::str)
            .def_readwrite("id", &PWNER::PROCESS::Region::id)
            .def_readwrite("parent", &PWNER::PROCESS::Region::parent)
            .def_readwrite("address", &PWNER::PROCESS::Region::address)
            .def_readwrite("size", &PWNER::PROCESS::Region::size)
            .def_readwrite("mode", &PWNER::PROCESS::Region::mode)
            .def_readwrite("offset", &PWNER::PROCESS::Region::offset)
            .def_readwrite("st_device_minor", &PWNER::PROCESS::Region::st_device_minor)
            .def_readwrite("st_device_major", &PWNER::PROCESS::Region::st_device_major)
            .def_readwrite("inode", &PWNER::PROCESS::Region::inode)
            .def_readwrite("file", &PWNER::PROCESS::Region::file)
            // .def_readwrite_static("npos", &PWNER::PROCESS::Region::npos)
            ;

    py::class_<PWNER::PROCESS::RegionStatic>(m, "RegionStatic")
            .def(py::init<PWNER::PROCESS::Region&>())
            .def("__repr__", &PWNER::PROCESS::RegionStatic::str)
            .def_readwrite("id", &PWNER::PROCESS::RegionStatic::id)
            .def_readwrite("bin", &PWNER::PROCESS::RegionStatic::bin)
            .def_readwrite("address", &PWNER::PROCESS::RegionStatic::address)
            .def_readwrite("size", &PWNER::PROCESS::RegionStatic::size)
            ;

    // pwner/process/Regions.hh
    py::class_<PWNER::PROCESS::Regions>(m, "Regions")
            .def(py::init<>())
            .def("update_regions", &PWNER::PROCESS::Regions::update_regions)
            .def("get_region", py::overload_cast<uintptr_t>(&PWNER::PROCESS::Regions::get_region, py::const_))
            .def("get_sregion", py::overload_cast<uintptr_t>(&PWNER::PROCESS::Regions::get_sregion, py::const_))
            .def("get_region", py::overload_cast<const std::string&>(&PWNER::PROCESS::Regions::get_region, py::const_))
            .def("get_region", py::overload_cast<const std::filesystem::path&>(&PWNER::PROCESS::Regions::get_region, py::const_))
            .def_readwrite("regions", &PWNER::PROCESS::Regions::regions)
            .def_readwrite("sregions", &PWNER::PROCESS::Regions::sregions)
            ;

    // pwner/process/IO.hh
    py::class_<PWNER::PROCESS::IO, PWNER::PROCESS::Regions>(m, "IO")
            // .def(py::init<>())
            // .def("read", py::overload_cast<uintptr_t, void*, size_t>(&PWNER::PROCESS::IO::read, py::const_))
            // .def("write", py::overload_cast<uintptr_t, void*, size_t>(&PWNER::PROCESS::IO::write, py::const_))
            .def("read", py::overload_cast<uintptr_t, uint64_t *>(&PWNER::PROCESS::IO::read<uint64_t>, py::const_))
            .def("write", py::overload_cast<uintptr_t, uint64_t *>(&PWNER::PROCESS::IO::write<uint64_t>, py::const_))
            // .def_readwrite_static("npos", &PWNER::PROCESS::IO::npos)
            ;

    // pwner/process/handlers/Procfs.hh
    // uint64_t p = reinterpret_cast<void*>(&PWNER::PROCESS::IOProcfs::running);
    py::class_<PWNER::PROCESS::IOProcfs, PWNER::PROCESS::IO>(m, "IOProcfs")
            .def(py::init<pid_t>())
            .def(py::init<const std::regex &>())
            // static
            .def("running",        static_cast<bool (*)(pid_t)>(&PWNER::PROCESS::IOProcfs::running))
            .def("cwd",            static_cast<std::filesystem::path (*)(pid_t)>(&PWNER::PROCESS::IOProcfs::cwd))
            .def("exe",            static_cast<std::filesystem::path (*)(pid_t)>(&PWNER::PROCESS::IOProcfs::exe))
            .def("cmdline",        static_cast<std::string (*)(pid_t)>(&PWNER::PROCESS::IOProcfs::cmdline))
            .def("from_cmdline",   static_cast<pid_t (*)(const std::regex &)>(&PWNER::PROCESS::IOProcfs::from_cmdline))
            // dynamic
            .def("running",        static_cast<bool (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::running))
            .def("cwd",            static_cast<std::filesystem::path (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::cwd))
            .def("exe",            static_cast<std::filesystem::path (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::exe))
            .def("cmdline",        static_cast<std::string (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::cmdline))
            .def("pid",            static_cast<pid_t (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::pid))
            .def("update_regions", &PWNER::PROCESS::IOProcfs::update_regions)
            .def("read",           static_cast<size_t (PWNER::PROCESS::IOProcfs::*)(uintptr_t, void *, size_t) const>(&PWNER::PROCESS::IOProcfs::read))
            .def("write",          static_cast<size_t (PWNER::PROCESS::IOProcfs::*)(uintptr_t, void *, size_t) const>(&PWNER::PROCESS::IOProcfs::write))
            // .def_readwrite_static("npos", &PWNER::PROCESS::IO::npos)
            ;



    // pwner/process/Process.hh
    // py::class_<PWNER::PROCESS::Process, PWNER::PROCESS::IO>(m, "Process")
    //         .def(py::init<>())
    //         .def("get_call_address", &PWNER::PROCESS::Process::get_call_address)
    //         .def("get_absolute_address", &PWNER::PROCESS::Process::get_absolute_address)
    //         ;
    //
    // py::class_<PWNER::PROCESS::ProcessProcfs, PWNER::PROCESS::Process, PWNER::PROCESS::IOProcfs>(m, "ProcessProcfs")
    //         .def(py::init<pid_t>())
    //         .def(py::init<const std::regex&>())
    //         ;
    //
    // py::class_<PWNER::PROCESS::ProcessHeap, PWNER::PROCESS::Process, PWNER::PROCESS::IOMappedHeap>(m, "ProcessHeap")
    //         .def(py::init<PWNER::PROCESS::IO&>())
    //         ;



    // py::class_<PWNER::Handle>(m, "Handle")
    //         .def(py::init<>())
    //         .def(py::init<pid_t>())
    //         .def(py::init<const std::string &>())
    //         .def_readwrite("pid", &PWNER::Handle::pid)
    //         .def_readwrite("title", &PWNER::Handle::title)
    //         .def_readwrite("regions_ignored", &PWNER::Handle::regions_ignored)
    //         .def_readwrite("regions", &PWNER::Handle::regions)
    //         .def("attach", (void (PWNER::Handle::*)(pid_t)) &PWNER::Handle::attach, "Attach by PID")
    //         .def("attach", (void (PWNER::Handle::*)(const std::string &)) &PWNER::Handle::attach, "Attach by title")
    //         .def("get_path", &PWNER::Handle::get_path, "Get executable path")
    //         .def("get_working_directory", &PWNER::Handle::get_working_directory, "Get executable working directory")
    //         .def("is_valid", &PWNER::Handle::is_valid, "Is handle valid")
    //         .def("is_running", &PWNER::Handle::is_running, "Is handle running")
    //         .def("is_good", &PWNER::Handle::is_running, "Is handle valid and running")
    //         .def("read", [](PWNER::Handle& self, uintptr_t& address, uintptr_t& out, size_t& size) -> auto {
    //             return self.read(address, reinterpret_cast<void *>(out), size);
    //         }, "Read from handle", py::arg("address"), py::arg("out"), py::arg("size"))
    //         .def("read", [](PWNER::Handle& self, uintptr_t& address, size_t& size) -> auto {
    //             // fixme[critical]: !! memory leak !!
    //             uint8_t *b = new uint8_t[size];
    //             size_t s = self.read(address, b, size);
    //             if (s == PWNER::Handle::npos)
    //                 s = 0;
    //             return py::bytes(reinterpret_cast<char *>(b), s);
    //         }, "Read from handle", py::arg("address"), py::arg("size"))
    //         .def("write", &PWNER::Handle::write, "Write to handle", py::arg("address"), py::arg("in"), py::arg("size"))
    //         .def("read_cached", &PWNER::Handle::read_cached, "Read from handle", py::arg("address"), py::arg("out"), py::arg("size"))
    //         .def("update_regions", &PWNER::Handle::update_regions, "Update maps")
    //         .def("get_region_by_name", &PWNER::Handle::get_region_by_name)
    //         .def("get_region_of_address", &PWNER::Handle::get_region_of_address)
    //         .def("find_pattern", &PWNER::Handle::find_pattern)
    //         .def("get_call_address", &PWNER::Handle::get_call_address)
    //         .def("get_absolute_address", &PWNER::Handle::get_absolute_address)
    //         ;
//*/

    // RE/value.hh
    // todo[high] add other classes
    // pybind11::class_<PWNER::mem64_t> (m, "mem64_t")
    //         .def(pybind11::init<>())
    //         .def_property("i8",  [](PWNER::mem64_t& self) -> auto { return self.i8;  }, [](PWNER::mem64_t& self, typeof(self.i8 ) set) { self.i8  = set; })
    //         .def_property("u8",  [](PWNER::mem64_t& self) -> auto { return self.u8;  }, [](PWNER::mem64_t& self, typeof(self.u8 ) set) { self.u8  = set; })
    //         .def_property("i16", [](PWNER::mem64_t& self) -> auto { return self.i16; }, [](PWNER::mem64_t& self, typeof(self.i16) set) { self.i16 = set; })
    //         .def_property("u16", [](PWNER::mem64_t& self) -> auto { return self.u16; }, [](PWNER::mem64_t& self, typeof(self.u16) set) { self.u16 = set; })
    //         .def_property("i32", [](PWNER::mem64_t& self) -> auto { return self.i32; }, [](PWNER::mem64_t& self, typeof(self.i32) set) { self.i32 = set; })
    //         .def_property("u32", [](PWNER::mem64_t& self) -> auto { return self.u32; }, [](PWNER::mem64_t& self, typeof(self.u32) set) { self.u32 = set; })
    //         .def_property("i64", [](PWNER::mem64_t& self) -> auto { return self.i64; }, [](PWNER::mem64_t& self, typeof(self.i64) set) { self.i64 = set; })
    //         .def_property("u64", [](PWNER::mem64_t& self) -> auto { return self.u64; }, [](PWNER::mem64_t& self, typeof(self.u64) set) { self.u64 = set; })
    //         .def_property("f32", [](PWNER::mem64_t& self) -> auto { return self.f32; }, [](PWNER::mem64_t& self, typeof(self.f32) set) { self.f32 = set; })
    //         .def_property("f64", [](PWNER::mem64_t& self) -> auto { return self.f64; }, [](PWNER::mem64_t& self, typeof(self.f64) set) { self.f64 = set; })
    //         //todo[high] add array support (bytes and chars)
    //         .def("__bytes__", [](PWNER::mem64_t& self) -> auto {
    //             return py::bytes(self.chars, sizeof(self.chars));
    //         })
    //         .def("__str__", [](PWNER::mem64_t& self) -> auto {
    //             std::ostringstream result;
    //             std::ostringstream bytes;
    //             bytes << std::hex << std::noshowbase << std::setfill('0');
    //             for (auto *cur = self.bytes; cur < self.bytes + sizeof(self.bytes); cur++) {
    //                 bytes << "\\x" << std::setw(2) << +*cur;
    //             }
    //             result << "{\n"
    //                    << "    i8 : " << std::to_string(self.i8 ) << "\n"
    //                    << "    u8 : " << std::to_string(self.u8 ) << "\n"
    //                    << "    i16: " << std::to_string(self.i16) << "\n"
    //                    << "    u16: " << std::to_string(self.u16) << "\n"
    //                    << "    i32: " << std::to_string(self.i32) << "\n"
    //                    << "    u32: " << std::to_string(self.u32) << "\n"
    //                    << "    i64: " << std::to_string(self.i64) << "\n"
    //                    << "    u64: " << std::to_string(self.u64) << "\n"
    //                    << "    f32: " << std::to_string(self.f32) << "\n"
    //                    << "    f64: " << std::to_string(self.f64) << "\n"
    //                    << "    bytes: " << bytes.str() << "\n"
    //                    << "}";
    //             // std::clog<<"returning "<<result<<std::endl;
    //             return result.str();
    //         })
    //         .def("_address_", [](PWNER::mem64_t& self) -> uintptr_t { return reinterpret_cast<uintptr_t>(&self); }, "the base object")
    //         ;

    //todo[high] add other RE's headers
}
