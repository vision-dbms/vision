#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cam.h"

#ifdef __VMS
// VMS CPU time facilities

#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#include <iledef.h>
#include <jpidef.h>
#include <efndef.h>

#endif

  
namespace CAM {
#ifndef NULL_CAM_LOGGING
  long long ProcessLevel::g_sequenceNumber = 0;
  std::string ProcessLevel::strProcessId;

  Operation* ThreadLevel::g_currentOperation = 0;

  std::ostream* ProcessLevel::g_camDestination = 0;
  ProcessLevel::camLogFunction_t ProcessLevel::g_camLogFunction = 0;
  void* ProcessLevel::g_camLogFunctionState = 0;


  void ProcessLevel::setupProcessId() {
    long r = 0;
    for (unsigned int i = 0; i < sizeof(long)/sizeof(int); i++)
    {
        r = r << (sizeof(int) * 4);
        r |= rand();
    }

    std::ostringstream ss;
    ss << r;
    ProcessLevel::strProcessId = ss.str();
  }

  void ThreadLevel::nextChainId(const std::string& str) {
    if (ThreadLevel::g_currentOperation) {
      ThreadLevel::g_currentOperation->nextChainId(str);
    }
    else {
      throw std::string("Need an operation to set nextChainId!");
    }
  }

  const char* Operation::getTimestamp(
      char* tsBuffer, 
      unsigned int tsBufferSize, 
      timeval* time
  ){
    char buffer[80];
    timeval rawtime;

    if (time == 0) {
      time = &rawtime;
      gettimeofday(time, NULL);
    }

    int ms = time->tv_usec / 1000; // milliseconds

    struct tm * timeinfo;
    timeinfo = localtime(&(time->tv_sec));

    // 2013-12-06T15:31:17.572+0500
    strftime(buffer, sizeof(buffer), "%FT%T", timeinfo);
    sprintf(tsBuffer, "%s.%03d", buffer, ms);
    strftime(buffer, sizeof(buffer), "%z", timeinfo);
    strcat(tsBuffer, buffer);

    return tsBuffer;
  }

  void Operation::log(std::list<std::string>& items, timeval* time) {
    m_outputBuffer << "{ ";

    char timestampBuffer[90];
    logKeyValueCopy(
        "@timestamp", 
        getTimestamp(timestampBuffer, sizeof(timestampBuffer), time), 
        true
    );

    m_outputBuffer << ", \"@fields\": { ";

    logKeyValueCopy("id", getId(), true);
    logKeyValueCopy("chainId", chainId());

    if (!loggedFirstEntry && parentId().size() > 0) {
      logKeyValueCopy("parent", parentId());
      loggedFirstEntry = true;
    }

    std::list<std::string> keys;
    std::string message;
      
    std::list<std::string>::iterator end(items.end()), i;
    for (i = items.begin(); i != end; i++) {
      std::string *key;
      std::string keyCopy;
      if (keys.size() > 0) {
        keyCopy = keys.front();
        keys.pop_front();
        key = &keyCopy;
      }
      else {
        key   = &(*i++);
        if (*key == "message" && i != end) {
          std::string label("rawMessage");
          logKeyValue(label, *i);
          processMessage(*i, message, keys, i);
          continue; /* don't write the message yet */
        }
      }

      if (i == end) {
        throw std::string("Please provide CAM values for each key");
      }

      std::string& value = *i;
      logKeyValue(*key, value);
    }

    m_outputBuffer << "}";

    /* log message if present */
    if (message.size() > 0) {
      std::string messageString("@message");
      logKeyValue(messageString, message);
    }

    m_outputBuffer << " }" << std::endl;

    write();
  }

  void Operation::processMessage(
      std::string& in, std::string& out, 
      std::list<std::string>& keys,
      std::list<std::string>::const_iterator values
  ){ 
    std::string startToken("%");
    std::string endToken(
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
    );
    std::size_t last = 0, start = 0, end;
    do {
      start = in.find(startToken, start);
      if (start != std::string::npos) {
        ++start;
        if (startToken == in.substr(start, 1)) {
          // %% is a special key with value "%"
          end = start;
          end++;
        }
        else {
          end = in.find_first_not_of(endToken, start);
        }

        if (end == std::string::npos) {
          end = in.size();
        }
        if (end - start <= 0)
          throw (std::string("Invalid key!"));

        out += in.substr(last, start - last - 1);
        
        std::string key(in.substr(start, end - start));
        if (key != startToken) {
          keys.push_back(key);
          out += *(++values);
        }
        else { out += key; }
      
        last = end; start = end; // next key
      }
    } 
    while (start != std::string::npos);

    out += in.substr(last, in.size() - last);
  }

      long long Operation::getCpuUsage() {
#ifdef __VMS
        long long cpuTime = 0;

        ILEB_64 jpi_item_list[2] = { 
          { 1, JPI$_CPUTIM, -1, sizeof(cpuTime), &cpuTime, 0},
          { 0,0,0,0,0 }
        };
        sys$getjpi(EFN$C_ENF,0,0,jpi_item_list,0,0,0);

        return cpuTime * 1000; // convert MS(10**-3) to uS(10**-6)
#else
        struct rusage prof;
        assert(getrusage(RUSAGE_SELF, &prof) == 0);
        return (
          prof.ru_utime.tv_usec + prof.ru_utime.tv_sec * 1000000 +
          prof.ru_stime.tv_usec + prof.ru_stime.tv_sec * 1000000
        );
#endif
      }

#else /* NULL_CAM_LOGGING */
  std::string disabledMessage("CAM DISABLED AT COMPILE TIME");
  Operation nullOperation;
#endif

}


