#ifndef DAEMON_H_
#define DAEMON_H_

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



/*
 * Transform the process in a daemon, doing:
 *  - Setting the group id equal to the process id (gid = pid) 
 *    and the process will be the leather of the (new) group
 *    This can fail if the process is already a group leather. To avoid this
 *    you can 'fork' the process, terminating the process parent and invoking
 *    'be_a_daemon' in the children process. (See man setpgid(2))
 *  - Deattach the associated TTY (if any) (See man tty(4) and man ioctl(2))
 *  - Close all the open file descriptors (See man proc(5) and man close(2))
 *  - Change the current working directory to the root "/" (See man chdir(2))
 *  - Change the file mode mask to 0 (See man umask(2))
 *
 * This function should be called immediately after the init of the main function.
 * */
void be_a_daemon();

/*
 * Set a dummy handler to catch and ignore the SIGINT and SIGTERM signals.
 *
 * See signal(2)
 * */
void ignore_signals();

/*
 * Wait for any signal that was handled. You can use this in conjunction with
 * 'ignore_signals' to wait for a SIGINT or SIGTERM.
 *
 * See pause(2)
 * */
void wait_signal();

#endif
