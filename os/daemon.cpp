
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


#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <sys/ioctl.h>
#include <vector>
#include <cstdlib>
#include <climits>
#include <signal.h>

#include "oserror.h"
#include "log.h"


static int filter_dot_entries(const struct dirent *entry) {
   return ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) ? 1 : 0;
}

void be_a_daemon() {
   // Set the process group id (gid) to be equal to the process id (pid)
   // forming a new group in which this process (pid) will be the leader
   if(setpgid(0,0) != 0) {
      throw OSError("The process group cannot be set");
   }

   // Deattach the tty
   int fd; 
   if ((fd = open("/dev/tty", O_RDWR)) >= 0) { //if fail, the tty is no attached
      if(ioctl(fd, TIOCNOTTY, 0) != 0) {
         throw OSError("The tty device attached (file descriptor %i) cannot be deattached", fd);
      }
      if(close(fd) != 0) {
         throw OSError("The tty device deattached (file descriptor %i) cannot be closed", fd);
      }
   }
   
   // First try to see the /proc interface to obtain that descriptors
   // If that fail, fallback and close all posible file descriptors 
   char buff[40];
   sprintf(buff, "/proc/%i/fd", getpid());
   buff[39] = 0;

   std::vector<int> open_filedescriptors;
   bool fallback = false;
   struct dirent **namelist = 0;

   // See if /proc can be accessed and then, try to obtain the file descriptors opened.
   int n = scandir(buff, &namelist, filter_dot_entries, alphasort);
   if(not (fallback = (n < 0))) {
      open_filedescriptors.reserve(n);
      for(int i = 0; i < n; ++i) {
         open_filedescriptors.push_back(atoi(namelist[i]->d_name));
         free(namelist[i]);
      }
      free(namelist);

      for(int i = 0; i < n; ++i) {
         if(close(open_filedescriptors[i]) != 0) {
            fallback = true;
            Log::warning("The file descriptor %i read from %s cannot be closed.", open_filedescriptors[i], buff);
            continue;
         }
      }
   }
   // Something was wrong, this fallback mechanism will close all posible file descriptors
   if(fallback) {
      Log::warning("Fallback to a less sophisticated implementation.");
      //-------------------------------------------------------------
      //From OpenSSL
#ifdef HAVE_SYSCONF
      int maxfd = sysconf(_SC_OPEN_MAX);
#else
      int maxfd = getdtablesize();
#endif 
      
      if (maxfd < 0) {
#ifdef OPEN_MAX
         maxfd = OPEN_MAX;
#else
         maxfd = 1024;
#endif 
      }
 
      Log::warning("Fallback mechanism. Closing file descriptors from %i to %i", 0, maxfd);
      for (int fd = 0; fd < maxfd; ++fd) {
         close(fd); //ignore the result
      }
      //
      //From OpenSSL
      //-------------------------------------------------------------
   }

   // Change the working directory to the root
   if(chdir("/") != 0) {
      throw OSError("The process cannot change its working directory to the / (the root)");
   }

   // Assign a new mask used by open, mkdir ... (See man umask(2))
   // All the permission are not altered 
   umask(0); // This allways success
}

//Dummy signal handler
static void dummy(int) {}


void ignore_signals() {
   //Ignoring the Interrupt Signal and the Term Signal
   //In fact, the signal is catched but its handler is useless.
   struct sigaction ignore;
   memset(&ignore, 0, sizeof(struct sigaction));
   ignore.sa_handler = &dummy;

   if(sigaction(SIGINT, &ignore, 0) == -1)
      throw OSError("The process cannot ignore (set a handler) for the SIGINT signal.");
   if(sigaction(SIGTERM, &ignore, 0) == -1)
      throw OSError("The process cannot ignore (set a handler) for the SIGTERM signal.");
}

void wait_signal() {
   pause();
}
