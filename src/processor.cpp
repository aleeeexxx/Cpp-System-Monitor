#include <fstream>
#include <sstream>
#include <unistd.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "processor.h"

using std::stof;
using std::string;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::string line, buffer;
  std::vector<string> precpu{};
  std::vector<string> cpu{};
  std::ifstream stream("/proc/stat");
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> buffer) {
      precpu.push_back(buffer);
    }
  }
  stream.close();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  stream.open("/proc/stat");
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> buffer) {
      cpu.push_back(buffer);
    }
  }
  float previdle =  stof(precpu[4])+stof(precpu[5]);
  float idle = stof(cpu[4])+stof(cpu[5]);
  float prevnonidle = stof(precpu[1])+stof(precpu[2])+stof(precpu[3])+stof(precpu[6])+stof(precpu[7])+stof(precpu[8]);
  float nonidle = stof(cpu[1])+stof(cpu[2])+stof(cpu[3])+stof(cpu[6])+stof(cpu[7])+stof(cpu[8]);
  float prevtotal = previdle+prevnonidle;
  float total = idle+nonidle;
  float totald = total-prevtotal;
  float idled = idle-previdle;
  return (totald-idled)/totald;
}