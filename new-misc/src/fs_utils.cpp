#include <cassert>
#include <cerrno>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "statlogger.h"
#include "fs_utils.h"

using namespace std;

bool stat_makedir(const string& path)
{
	assert(path.size());

	string dir;
	vector<string> subdirs = stat_split(path, '/');
	if (subdirs[0].empty()) {
		subdirs.erase(subdirs.begin());
	} else if (subdirs[0] == ".") {
		dir = ".";
		subdirs.erase(subdirs.begin());
	} else if (subdirs[0] == "..") {
		dir = "..";
		subdirs.erase(subdirs.begin());
	} else {
		dir = ".";
	}

	for (vector<string>::size_type i = 0; i != subdirs.size(); ++i) {
		dir += "/" + subdirs[i];
		int ret = mkdir(dir.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		if ((ret == -1) && (errno != EEXIST)) {
			return false;
		}
		chmod(dir.c_str(), 0777);
	}
	
	return true;
}
