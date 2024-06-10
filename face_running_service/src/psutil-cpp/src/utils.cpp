#include <psutil-cpp/utils.hpp>
#include <dirent.h>
#include <chrono>
#include <thread>
#include <sstream>


bool are_spaces(char lhs, char rhs)
{
    return (lhs == rhs) && (lhs == ' ');
}

// Removes double spaces from string, example is /proc/meminfo
std::string trim_double_spaces(std::string input)
{
    std::string::iterator new_end = std::unique(input.begin(), input.end(), are_spaces);
    input.erase(new_end, input.end());
    return input;
}

// Check if file exists
bool fexists(const std::string &filename)
{
    std::ifstream ifile(filename.c_str());
    bool res = ifile.is_open();
    ifile.close();
    return res;
}

std::optional<bool> dexists(std::string path)
{
#ifdef linux
    struct stat info;

    int statRC = stat(path.c_str(), &info);
    if (statRC != 0)
    {
        if (errno == ENOENT)
        {
            return 0;
        } // something along the path does not exist
        if (errno == ENOTDIR)
        {
            return 0;
        } // something in path prefix is not a dir
        return -1;
    }

    return (info.st_mode & S_IFDIR) ? true : false;
#endif
    return std::nullopt;
}

// Splits string by delimiter, first argument is string, second is delimiter
std::vector<std::string> split_by_delim(std::string string, std::string delimiter)
{
    size_t pos = 0;
    std::string token;
    std::vector<std::string> array;
    while ((pos = string.find(delimiter)) != std::string::npos)
    {

        token = string.substr(0, pos);
        array.push_back(token);
        string.erase(0, pos + delimiter.length());
    }
    array.push_back(string);
    return array;
}

float usage_percent(unsigned long long used, unsigned long long total, int _round)
{
    if (total == 0)
    {
        return 0;
    }
    float ret = ((float)used / (float)total) * 100.0;
    if (_round != -1)
    {
        unsigned long rounding_places = pow(10, _round);
        ret = std::round(ret * rounding_places) / rounding_places;
    }
    return ret;
}

// TODO: Implement this
float calculate_avail_memory()
{
    return 0;
}

std::map<std::string, unsigned long long> parse_file(std::string path, bool trim_lines, std::string delimiter, int value_multiplier)
{
    std::ifstream file(path);
    std::string line;
    std::vector<std::string> temp_value;
    std::map<std::string, unsigned long long> mems;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            if (trim_lines)
            {
                line = trim_double_spaces(line);
            }

            temp_value = split_by_delim(line, std::string(delimiter));

            mems[temp_value[0]] = std::stoull(temp_value[1]) * value_multiplier;
        }
        file.close();
    }
    return mems;
}

std::vector<std::string> cat(std::string path)
{
    std::ifstream file(path);
    std::string line;
    std::vector<std::string> result;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            result.push_back(line);
        }
        file.close();
    }
    return result;
}

std::string get_path(int num)
{
    std::string str1 = "/sys/devices/system/cpu/cpufreq/policy" + std::to_string(num) + "/";
    std::string str2 = "/sys/devices/system/cpu/cpu" + std::to_string(num) + "/cpufreq/";

    if (dexists(str1))
    {
        return str1;
    }
    else if (dexists(str2))
    {
        return str2;
    }
    return "none";
}

void sleep_mlsec(uint32_t millisec) {
  std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
}

bool is_only_numbers(const std::string &str) {
  if (str.empty()) {
    return false;
  }
  return str.find_first_not_of("0123456789") == std::string::npos;
}

std::vector<std::string> get_dirs_list(const std::string &path) {
  std::vector<std::string> res;
  if (!fexists(path)) {
    return res;
  }

  auto dir = opendir(path.c_str());
  auto entry = readdir(dir);

  while (entry != nullptr) {
    if (entry->d_type == DT_DIR) {
      res.push_back(entry->d_name);
    }
    entry = readdir(dir);
  }

  closedir(dir);
  return res;
}

std::string file_to_string(const std::string &filename) {
  std::ifstream ifs(filename);
  std::string res;
  if (ifs.is_open()) {
    std::ostringstream oss;
    oss << ifs.rdbuf();
    res = oss.str();
  }
  ifs.close();
  return res;
}

std::string replace_all(std::string str, const std::string &from, const std::string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}
