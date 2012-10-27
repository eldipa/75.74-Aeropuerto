
/*************************************************************************
 *                                                                       *
 *                        This work is licensed under a                  *
 *   CC BY-SA        Creative Commons Attribution-ShareAlike             *
 *                           3.0 Unported License.                       *
 *                                                                       * 
 *  Author: Di Paola Martin Pablo, 2012                                  *
 *                                                                       *
 *************************************************************************/

/*******************************************************************************
 *                                                                             *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        *
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          *
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A    *
 *  PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER  *
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,   *
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        *
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         *
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     *
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING       *
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         *
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               *
 *                                                                             *
 *******************************************************************************/


#include "process.h"

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "oserror.h"
#include "log.h"

Process::Process(const char* file, char *argv[]) {
    static char none[] = "";
    if(argv == 0) {
        //For some unknow reason, 'argv = &none" is wrong: 
        //cannot convert 'char (*)[1]' to 'char**' in an assignament
        char *p = none; 
        argv = &p;
    }

    Log::debug("Spawning the process %s.", file);
    pid_t pid = fork();
    if(pid == 0) {
        // children process
        execv(file, argv);
        throw OSError("The process's image '%s' cannot be loaded.", file);
    }else if(pid < 0) {
        throw OSError("The process's children cannot be created (forked).");
    }else {
        // parent process
        Log::debug("Spawned the process %s (PID %i).", file, pid);
        this->pid = pid;
    }
}

int Process::wait() {
    int status = -1;
    if(waitpid(pid, &status, 0) == -1) 
        throw OSError("Error when waiting for the process children with PID %i finish", pid);
    return status;
}

void Process::send_signal(int signal, bool expect_alive) {
    if(kill(pid, signal) < 0) {
        if(errno == ESRCH and not expect_alive) return
        throw OSError("Error when sending a signal -%i to the process children with PID %i", signal, pid);
    }
}
