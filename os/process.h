#ifndef PROCESS_H_
#define PROCESS_H_

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


#include <sys/types.h>

class Process {
    private:
        pid_t pid;
         
    public:
        /*
         * Load and run a new process which exectuable is 'file' and
         * it is executed with the argumens 'argv'.
         *
         * The 'argv' must be 0 or an array of null-terminated strings and the last
         * element of the array must be a empty string ("\0").
         * If 'argv' is 0, no arguments are given to the new process.
         *
         * The destructor of this class will not be waiting for the completion
         * of the new process either will cleanup its resources.
         * It is responsability of the caller to call the method 'wait' which will
         * do that.
         *
         * The internal implementation execute a 'fork' and then an 'exec' consecutively.
         * If the 'argv' are strings stored in the data or code segment (statics),
         * the operative system may skip the copy of the image of the parent process
         * and inmediatly load and use the image in 'file', improving the loading.
         *
         * See fork(2), exec(3), wait(2)
         * */
        Process(const char* file, char *argv[] = 0);
        int wait();
        void send_signal(int signal, bool expect_alive=false);
};

#endif
