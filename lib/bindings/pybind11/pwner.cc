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
#include <pwner/scanner/ValueScanner.hh>
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <pybind11/stl_bind.h>
#include <pwner/scanner/PointerScanner.hh>


PYBIND11_MAKE_OPAQUE(std::vector<PWNER::value_t>)

namespace py = pybind11;


// TODO[high]: split to modules to speedup build time
PYBIND11_MODULE(pwner, m) {
    m.doc() = "pybind11 wrapper for Reverse Engine";

    // pwner/common.hh
    m.def("oom_score_adj", &oom_score_adj);

    // pwner/external.hh
    m.def("execute", &PWNER::execute);
    py::class_<PWNER::CProcess>(m, "CProcess")
            .def(py::init<const std::string &, const std::string &, const std::string &>())
            .def_readwrite("pid", &PWNER::CProcess::pid)
            .def_readwrite("user", &PWNER::CProcess::user)
            .def_readwrite("command", &PWNER::CProcess::command)
            .def("__repr__", &PWNER::CProcess::str)
            ;
    m.def("getProcesses", &PWNER::getProcesses);
    m.def("get_mem_total", &PWNER::get_mem_total, "list(MemTotal, MemFree, MemAvailable, Cached)[i]", py::arg("i") = 0);
    m.def("get_mem_free", &PWNER::get_mem_free);

    // todo pwner/bin/...

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
            .def_readonly_static("npos", &PWNER::PROCESS::Region::npos)
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
            .def("get_region",  py::overload_cast<uintptr_t>(&PWNER::PROCESS::Regions::get_region, py::const_), py::arg("address"))
            .def("get_sregion", py::overload_cast<uintptr_t>(&PWNER::PROCESS::Regions::get_sregion, py::const_), py::arg("address"))
            .def("get_region",  py::overload_cast<const std::string&>(&PWNER::PROCESS::Regions::get_region, py::const_), py::arg("filename"))
            .def("get_region",  py::overload_cast<const std::filesystem::path&>(&PWNER::PROCESS::Regions::get_region, py::const_), py::arg("filename"))
            .def_readwrite("regions", &PWNER::PROCESS::Regions::regions)
            .def_readwrite("sregions", &PWNER::PROCESS::Regions::sregions)
            ;

    // pwner/process/IO.hh
    py::class_<PWNER::PROCESS::IO, PWNER::PROCESS::Regions>(m, "IO")
            // .def(py::init<>())
            // .def("read", py::overload_cast<uintptr_t, void*, size_t>(&PWNER::PROCESS::IO::read, py::const_))
            // .def("write", py::overload_cast<uintptr_t, void*, size_t>(&PWNER::PROCESS::IO::write, py::const_))
            // .def("read", py::overload_cast<uintptr_t, uint64_t *>(&PWNER::PROCESS::IO::read<uint64_t>, py::const_))
            // .def("write", py::overload_cast<uintptr_t, uint64_t *>(&PWNER::PROCESS::IO::write<uint64_t>, py::const_))
            .def_readonly_static("npos", &PWNER::PROCESS::IO::npos)
            ;

    py::class_<PWNER::PROCESS::IOMapped, PWNER::PROCESS::IO>(m, "IOMapped")
            .def("read",           static_cast<size_t (PWNER::PROCESS::IOMapped::*)(uintptr_t, void *, size_t) const>(&PWNER::PROCESS::IOMapped::read))
            .def("write",          static_cast<size_t (PWNER::PROCESS::IOMapped::*)(uintptr_t, void *, size_t) const>(&PWNER::PROCESS::IOMapped::write))
            ;

    py::class_<PWNER::PROCESS::IOMappedHeap, PWNER::PROCESS::IOMapped>(m, "IOMappedHeap")
            .def(py::init<PWNER::PROCESS::IO&>())
            ;

    //todo also pwner/process/IO.hh

    // pwner/process/handlers/Procfs.hh
    py::class_<PWNER::PROCESS::IOProcfs, PWNER::PROCESS::IO>(m, "IOProcfs")
            .def(py::init<pid_t>())
            .def(py::init<const std::string &>())
            .def_static("running_",        static_cast<bool (*)(pid_t)>(&PWNER::PROCESS::IOProcfs::running), py::arg("pid"))
            .def_static("cwd_",            static_cast<std::filesystem::path (*)(pid_t)>(&PWNER::PROCESS::IOProcfs::cwd), py::arg("pid"))
            .def_static("exe_",            static_cast<std::filesystem::path (*)(pid_t)>(&PWNER::PROCESS::IOProcfs::exe), py::arg("pid"))
            .def_static("cmdline_",        static_cast<std::string (*)(pid_t)>(&PWNER::PROCESS::IOProcfs::cmdline), py::arg("pid"))
            .def_static("from_cmdline_",   static_cast<pid_t (*)(const std::string &)>(&PWNER::PROCESS::IOProcfs::from_cmdline), py::arg("pattern_cmdline"))
            .def("running",        static_cast<bool (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::running))
            .def("cwd",            static_cast<std::filesystem::path (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::cwd))
            .def("exe",            static_cast<std::filesystem::path (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::exe))
            .def("cmdline",        static_cast<std::string (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::cmdline))
            .def("pid",            static_cast<pid_t (PWNER::PROCESS::IOProcfs::*)() const>(&PWNER::PROCESS::IOProcfs::pid))
            .def("update_regions", &PWNER::PROCESS::IOProcfs::update_regions)
            .def("read", [](PWNER::PROCESS::IOProcfs& self, uintptr_t& address, size_t& size) -> auto {
                std::vector<uint8_t> b(size);
                size_t s = self.read(address, b.data(), size);
                if (s == PWNER::PROCESS::IOProcfs::npos)
                    s = 0;
                py::bytes result = py::bytes(reinterpret_cast<char *>(b.data()), s);
                return result;
            }, "Read bytes from handler", py::arg("address"), py::arg("size"))
            // todo write!
            // .def("write", &PWNER::Handle::write, "Write to handle", py::arg("address"), py::arg("in"), py::arg("size"))
            // .def("write",          static_cast<size_t (PWNER::PROCESS::IOProcfs::*)(uintptr_t, void *, size_t) const>(&PWNER::PROCESS::IOProcfs::write))
            ;

    // pwner/process/Process.hh
    py::class_<PWNER::PROCESS::Process, PWNER::PROCESS::IO>(m, "Process")
            .def("get_call_address", &PWNER::PROCESS::Process::get_call_address, py::arg("address"))
            .def("get_absolute_address", &PWNER::PROCESS::Process::get_absolute_address, py::arg("address"), py::arg("offset"), py::arg("size"))
            ;

    py::class_<PWNER::PROCESS::ProcessProcfs, PWNER::PROCESS::Process, PWNER::PROCESS::IOProcfs>(m, "ProcessProcfs")
            .def(py::init<pid_t>())
            .def(py::init<const std::string &>())
            ;

    py::class_<PWNER::PROCESS::ProcessHeap, PWNER::PROCESS::Process, PWNER::PROCESS::IOMappedHeap>(m, "ProcessHeap")
            .def(py::init<PWNER::PROCESS::IO&>())
            ;


    // pwner/scanner/Value.hh
    py::enum_<PWNER::Edata_type>(m, "Edata_type", py::arithmetic())
            .value("ANYNUMBER",  PWNER::Edata_type::ANYNUMBER)
            .value("ANYINTEGER", PWNER::Edata_type::ANYINTEGER)
            .value("ANYFLOAT",   PWNER::Edata_type::ANYFLOAT)
            .value("INTEGER8",   PWNER::Edata_type::INTEGER8)
            .value("INTEGER16",  PWNER::Edata_type::INTEGER16)
            .value("INTEGER32",  PWNER::Edata_type::INTEGER32)
            .value("INTEGER64",  PWNER::Edata_type::INTEGER64)
            .value("FLOAT32",    PWNER::Edata_type::FLOAT32)
            .value("FLOAT64",    PWNER::Edata_type::FLOAT64)
            .value("BYTEARRAY",  PWNER::Edata_type::BYTEARRAY)
            .value("STRING",     PWNER::Edata_type::STRING)
            ;

    py::enum_<PWNER::Ematch_type>(m, "Ematch_type", py::arithmetic())
            .value("MATCHANY",         PWNER::Ematch_type::MATCHANY)
            .value("MATCHEQUALTO",     PWNER::Ematch_type::MATCHEQUALTO)
            .value("MATCHNOTEQUALTO",  PWNER::Ematch_type::MATCHNOTEQUALTO)
            .value("MATCHGREATERTHAN", PWNER::Ematch_type::MATCHGREATERTHAN)
            .value("MATCHLESSTHAN",    PWNER::Ematch_type::MATCHLESSTHAN)
            .value("MATCHRANGE",       PWNER::Ematch_type::MATCHRANGE)
            .value("MATCHEXCLUDE",     PWNER::Ematch_type::MATCHEXCLUDE)
            .value("MATCHUPDATE",      PWNER::Ematch_type::MATCHUPDATE)
            .value("MATCHNOTCHANGED",  PWNER::Ematch_type::MATCHNOTCHANGED)
            .value("MATCHCHANGED",     PWNER::Ematch_type::MATCHCHANGED)
            .value("MATCHINCREASED",   PWNER::Ematch_type::MATCHINCREASED)
            .value("MATCHDECREASED",   PWNER::Ematch_type::MATCHDECREASED)
            .value("MATCHINCREASEDBY", PWNER::Ematch_type::MATCHINCREASEDBY)
            .value("MATCHDECREASEDBY", PWNER::Ematch_type::MATCHDECREASEDBY)
            ;

    py::enum_<PWNER::flag_t>(m, "flag_t", py::arithmetic())
            .value("flags_empty",   PWNER::flag_t::flags_empty)
            .value("flag_u8",       PWNER::flag_t::flag_u8)
            .value("flag_i8",       PWNER::flag_t::flag_i8)
            .value("flag_u16",      PWNER::flag_t::flag_u16)
            .value("flag_i16",      PWNER::flag_t::flag_i16)
            .value("flag_u32",      PWNER::flag_t::flag_u32)
            .value("flag_i32",      PWNER::flag_t::flag_i32)
            .value("flag_u64",      PWNER::flag_t::flag_u64)
            .value("flag_i64",      PWNER::flag_t::flag_i64)
            .value("flag_f32",      PWNER::flag_t::flag_f32)
            .value("flag_f64",      PWNER::flag_t::flag_f64)
            .value("flags_i8b",     PWNER::flag_t::flags_i8b)
            .value("flags_i16b",    PWNER::flag_t::flags_i16b)
            .value("flags_i32b",    PWNER::flag_t::flags_i32b)
            .value("flags_i64b",    PWNER::flag_t::flags_i64b)
            .value("flags_integer", PWNER::flag_t::flags_integer)
            .value("flags_float",   PWNER::flag_t::flags_float)
            .value("flags_all",     PWNER::flag_t::flags_all)
            .value("flags_8b",      PWNER::flag_t::flags_8b)
            .value("flags_16b",     PWNER::flag_t::flags_16b)
            .value("flags_32b",     PWNER::flag_t::flags_32b)
            .value("flags_64b",     PWNER::flag_t::flags_64b)
            ;

    py::class_<bitmask::bitmask<PWNER::flag_t>>(m, "bitmask0flag_t")
            .def(py::init<>())
            .def(py::init<PWNER::flag_t>())
            ;

    py::class_<PWNER::flag, bitmask::bitmask<PWNER::flag_t>>(m, "flag")
            .def(py::init<const PWNER::Edata_type&>())
            // static methods
            .def_static("convert",   static_cast<PWNER::flag (*)(const PWNER::Edata_type&)>(&PWNER::flag::convert), py::arg("dt"))
            // methods
            .def("memlength", static_cast<size_t (PWNER::flag::*)(const PWNER::Edata_type&) const>(&PWNER::flag::memlength), py::arg("scan_data_type"))
            .def("str",       static_cast<std::string (PWNER::flag::*)() const>(&PWNER::flag::str))
            ;

    // py::class_<PWNER::mem64_t> (m, "mem64")
    //         .def(py::init<>())
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

    py::class_<PWNER::value_t> (m, "value_t")
            .def(py::init<>())
            .def(py::init<uintptr_t, PWNER::mem64_t, PWNER::flag>())
            .def_readwrite("address", &PWNER::value_t::address)
            .def_readwrite("mem", &PWNER::value_t::mem)
            .def_readwrite("flags", &PWNER::value_t::flags)
            .def("nearest_flag", &PWNER::value_t::nearest_flag)
            .def("flag2str", &PWNER::value_t::flag2str)
            .def("val2str", &PWNER::value_t::val2str)
            .def("address2str", &PWNER::value_t::address2str)
            .def("str", &PWNER::value_t::str)
            ;

    py::class_<PWNER::Cuservalue> (m, "Cuservalue")
            .def(py::init<>())
            .def_readwrite("i8", &PWNER::Cuservalue::i8)
            .def_readwrite("u8", &PWNER::Cuservalue::u8)
            .def_readwrite("i16", &PWNER::Cuservalue::i16)
            .def_readwrite("u16", &PWNER::Cuservalue::u16)
            .def_readwrite("i32", &PWNER::Cuservalue::i32)
            .def_readwrite("u32", &PWNER::Cuservalue::u32)
            .def_readwrite("i64", &PWNER::Cuservalue::i64)
            .def_readwrite("u64", &PWNER::Cuservalue::u64)
            .def_readwrite("f32", &PWNER::Cuservalue::f32)
            .def_readwrite("f64", &PWNER::Cuservalue::f64)
            .def_readwrite("bytearray_value", &PWNER::Cuservalue::bytearray_value)
            .def_readwrite("wildcard_value", &PWNER::Cuservalue::wildcard_value)
            .def_readwrite("string_value", &PWNER::Cuservalue::string_value)
            .def_readwrite("flags", &PWNER::Cuservalue::flags)
            .def("parse_uservalue_int", &PWNER::Cuservalue::parse_uservalue_int, py::arg("text"))
            .def("parse_uservalue_float", &PWNER::Cuservalue::parse_uservalue_float, py::arg("text"))
            .def("parse_uservalue_number", &PWNER::Cuservalue::parse_uservalue_number, py::arg("text"))
            .def("parse_uservalue_bytearray", &PWNER::Cuservalue::parse_uservalue_bytearray, py::arg("text"))
            .def("parse_uservalue_string", &PWNER::Cuservalue::parse_uservalue_string, py::arg("text"))
            ;

    m.def("string_to_uservalue", [](const PWNER::Edata_type &data_type,
                                    const std::string &text) {
        PWNER::Ematch_type match_type;
        std::vector<PWNER::Cuservalue> uservalue;
        PWNER::string_to_uservalue(data_type, text, match_type, uservalue);
        return std::make_tuple(match_type, uservalue);
    }, py::arg("dt"), py::arg("text"));

    // pwner/scanner/ValueScanner.hh
    // TODO: pwner/scanner/ValueScanner.hh
    py::bind_vector<std::vector<PWNER::value_t>>(m, "VectorValues", py::module_local(false));

    py::class_<PWNER::ScannerSequential>(m, "ScannerSequential")
            .def(py::init<PWNER::PROCESS::Process&>())
            .def("scan_regions", [](PWNER::ScannerSequential& self,
                                    std::vector<PWNER::value_t>& writing_matches,
                                    const PWNER::Edata_type& data_type,
                                    const std::vector<PWNER::Cuservalue> uservalue,
                                    const PWNER::Ematch_type& match_type) {
                return self.scan_regions(writing_matches,data_type,uservalue.data(),match_type);
            }, py::arg("writing_matches"), py::arg("data_type"), py::arg("uservalue"), py::arg("match_type"))
            .def("scan_recheck", [](PWNER::ScannerSequential& self,
                                    std::vector<PWNER::value_t>& writing_matches,
                                    const PWNER::Edata_type& data_type,
                                    const std::vector<PWNER::Cuservalue> uservalue,
                                    const PWNER::Ematch_type& match_type) {
                return self.scan_recheck(writing_matches,data_type,uservalue.data(),match_type);
            }, py::arg("writing_matches"), py::arg("data_type"), py::arg("uservalue"), py::arg("match_type"))
            .def("scan_update",  &PWNER::ScannerSequential::scan_update, py::arg("writing_matches"))
            // .def_readonly("handler", &PWNER::ScannerSequential::handler)
            .def_readwrite("stop_flag", &PWNER::ScannerSequential::stop_flag)
            .def_readwrite("scan_progress", &PWNER::ScannerSequential::scan_progress)
            .def_readwrite("step", &PWNER::ScannerSequential::step)
            ;

    // TODO: pwner/scanner/PointerScanner.hh
    // py::class_<PWNER::PointerScannerBackward>(m, "PointerScannerBackward")
    //         .def(py::init<PWNER::PROCESS::Process&>())
    //         .def("scan_regions", &PWNER::ScannerSequential::scan_regions)
    //         .def("scan_update",  &PWNER::ScannerSequential::scan_update)
    //         .def("scan_recheck", &PWNER::ScannerSequential::scan_recheck)
    //         .def_readwrite("stop_flag", &PWNER::ScannerSequential::stop_flag)
    //         .def_readwrite("scan_progress", &PWNER::ScannerSequential::scan_progress)
    //         .def_readwrite("step", &PWNER::ScannerSequential::step)
    //         ;
}
