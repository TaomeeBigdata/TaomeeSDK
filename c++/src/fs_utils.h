#ifndef LIBANT_FS_UTILS_HPP_
#define LIBANT_FS_UTILS_HPP_

#include <string>

// 自动创建path中不存在的子目录
bool stat_makedir(const std::string& path);

#endif // LIBANT_FS_UTILS_HPP_
