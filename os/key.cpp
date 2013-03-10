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

#include "key.h"
#include "oserror.h"
#include <sys/ipc.h>

Key get_key(const char *absolute_path, char proj_id) {
	Key key = ftok(absolute_path, proj_id);
	if (key == -1) {
		throw OSError("The key generator 'ftok' is failing using the path '%s' and the magic character '%c'",
			absolute_path, proj_id);
	}
	return key;
}

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

void create_if_not_exists(const char * absolute_path) {
	struct stat buf;
	int file;
	if (stat(absolute_path, &buf) != 0) {
		file = open(absolute_path, O_CREAT , 0660);
		if (file != -1) {
			write(file,absolute_path,strlen(absolute_path));
			close(file);
		} else {
			throw OSError("Unable to create file '%s'", absolute_path);
		}
	}
}
