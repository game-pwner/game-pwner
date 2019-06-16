#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include <regex>
#include <cmath>
#include <iterator>
#include <iostream>
#include <criu/criu.h>
#include <fcntl.h>
#include <wait.h>
#include <unistd.h>
#include <memory>



void what_err_ret_mean(int ret) {
    /* NOTE: errno is set by libcriu */
    switch (ret) {
        case -EBADE:
            perror("RPC has returned fail");
            break;
        case -ECONNREFUSED:
            perror("Unable to connect to CRIU");
            break;
        case -ECOMM:
            perror("Unable to send/recv msg to/from CRIU");
            break;
        case -EINVAL:
            perror("CRIU doesn't support this type of request."
                   "You should probably update CRIU");
            break;
        case -EBADMSG:
            perror("Unexpected response from CRIU."
                   "You should probably update CRIU");
            break;
        default:
            perror("Unknown error type code."
                   "You should probably update CRIU");
    }
}





/* useful commands:
 * criu dump -v4 -j -d -t `pidof FAKEMEM` -o dump.log && echo OK || echo FAIL
 * unshare -p -m --fork --mount-proc
 * criu restore -v4 -j -d -o restore.log && echo OK || echo FAIL
 * /home/user/github.com/game-pwner/game-pwner/.cmake-build-gcc/test/scratch/TOOL_SCRATCH `pidof FAKEMEM` 3
 */

int main(int argc, const char *argv[]) {
    using namespace std;
    using namespace std::string_literals;

    pid_t target_pid;
    if (argc >= 2)
        target_pid = strtoul(argv[1], NULL, 0);
    else {
        char buf[512];
        FILE *cmd_pipe = popen("pidof -s FAKEMEM", "r");

        fgets(buf, 512, cmd_pipe);
        target_pid = strtoul(buf, NULL, 0);

        pclose(cmd_pipe);
    }

    uint64_t etype = 0b11;
    if (argc >= 3) {
        etype = strtoul(argv[2], NULL, 0);
    }

    cout << "target_pid: " << target_pid << endl;
    const std::filesystem::path dir_dump = "/tmp/criutory";
    const char *log_dump = "criu_dump.log";
    const char *log_restore = "criu_rest.log";

    CRIU cr(dir_dump);
    cr.set_log_level(4);
    cr.set_service_binary(nullptr);
    cr.set_shell_job(true);

    if ((etype & 0b01) != 0) {
        /* CHECKPOINT */
        clog<<"CHECKPOINT"<<endl;
        system(("echo -n 'CRIU images directory creation... '; mkdir "+dir_dump.native()+" 2>/dev/null; rm -rf "s + dir_dump.native() + "/* && echo Success || echo Failed").c_str());
        cr.set_log_file(log_dump);
        cr.set_pid(target_pid);
        cr.set_leave_running(false);
        cr.set_ghost_limit(1u<<30u);
        if (int rc = cr.dump(); rc < 0) {
            what_err_ret_mean(rc);
            throw std::runtime_error("criu_dump: rc < 0 [" + to_string(rc) + " < 0]");
        }
        clog << "criu_dump: success" << endl;
    }
    if ((etype & 0b10) != 0) {
        /* RESTORE */
        clog<<"RESTORE"<<endl;
        cr.set_log_file(log_restore);
        pid_t pid;
        if (int rc = cr.restore(); rc < 0) {
            what_err_ret_mean(rc);
            throw std::runtime_error("criu_restore_child: rc < 0 [" + to_string(rc) + " < 0]");
        } else
            pid = rc;
        clog << "criu_restore_child: success" << endl;

        cout << boolalpha;
        cout << "target pid is: " << target_pid << ", leaved running: " << endl;
        cout << "restored pid is: " << pid << endl;
    }

    return 0;
}
