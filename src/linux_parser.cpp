#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
using std::stof;
using std::stol;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float MemTotal, MemFree, MemUsed;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        MemTotal = stof(value);
      }
      if (key == "MemFree:") {
        MemFree = stof(value);
        break;
      }
    }
    MemUsed = MemTotal - MemFree;
    return MemUsed/MemTotal;
  }
  return 0.0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  string line, value;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    uptime = stol(value);
    return uptime;
  }
  return 0;
}


// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string time;
  long utime, stime, cutime, cstime;
  int i = 1;
  std::ifstream stream( kProcDirectory + "/" + to_string(pid) + kStatFilename );
  if(stream.is_open()) {
    while(stream >> time) {
      if(i == 14) {utime = stol(time);}
      else if(i == 15) {stime = stol(time);}
      else if(i == 16) {cutime = stol(time);}
      else if(i == 17) {
        cstime = stol(time);
        long active = utime+stime+cutime+cstime;
        return active;
      }
      i++;
    }
  }
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return stoi(value);   
      }
    }
  }
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value; 
      if (key == "procs_running") {
        linestream >> value;
        return stoi(value);
      } 
    }
  }
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) +
                       kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) +
                       kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        return to_string(stoi(value)/1024);
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key, value, line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) +
                       kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return value;
      }      
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string UserName, Uid, target_id, line;
  target_id = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      linestream >> UserName >> Uid;
      if (Uid == target_id) {
        return UserName;
      }      
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string value, line;
  long starttime, uptime;
  int i = 1;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) +
                       kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      if (i == 22) { 
        starttime = std::stol(value)/sysconf(_SC_CLK_TCK); 
        uptime = LinuxParser::UpTime() - starttime;
        return uptime;
      }
      i++;
    }
  }
}
