/* include px_ipc_name */
#include <string>
#include	"unpipc.h"

std::string
px_ipc_name(const std::string& name)
{
	char *dst;
    const char *slash, *dir;
	if ((dst = static_cast<char *>(malloc(PATH_MAX))) == nullptr)
		return(nullptr);

		/* 4can override default directory with environment variable */
	if ( (dir = getenv("PX_IPC_NAME")) == nullptr) {
#ifdef	POSIX_IPC_PREFIX
		dir = POSIX_IPC_PREFIX;		/* from "config.h" */
#else
		dir = "/tmp/";				/* default */
#endif
	}
		/* 4dir must end in a slash */
	slash = (dir[strlen(dir) - 1] == '/') ? "" : "/";
	snprintf(dst, PATH_MAX, "%s%s%s", dir, slash, name.c_str());
    std::string strDst{dst};
    free(dst);
	return strDst;			/* caller can free() this pointer */
}
/* end px_ipc_name */

std::string
Px_ipc_name(const std::string& name)
{
    return px_ipc_name(name);
//	if ( (ptr = px_ipc_name(name)) == nullptr)
//		err_sys("px_ipc_name error for %s", name);
//	return(ptr);
}
