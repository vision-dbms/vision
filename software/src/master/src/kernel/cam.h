#ifndef __CAM_H__
#define __CAM_H__

#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <string>
#include <streambuf>
#include <cassert>

#include "Vp.h"

#ifdef V_EXPORTS
#define V_API DECLSPEC_DLLEXPORT

#else
#define V_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "v.lib")
#endif

#endif

/* Disable all CAM logging, pre-compilation wherever possible! */
#if defined(sun) || defined(_WIN32) /* no windows or sun CAM logging right now */
#define NULL_CAM_LOGGING
#endif


#define CAM_LEVEL_ERROR 80

#ifndef NULL_CAM_LOGGING
namespace CAM { class Operation; }
class StreamCapture: public std::ostream, private std::streambuf {
  friend class CAM::Operation;
  
  public:
    typedef std::string itemType;
    typedef std::list<itemType> itemsType;

    StreamCapture(): std::ostream(0), current("") { 
      std::ostream::init(this);
    }
    ~StreamCapture() { }

    /* insertion function! 
     * make the ostream interpet the output and then capture it using overflow()
     */
    template <typename T> StreamCapture& operator<<(const T& rhs) {
      static_cast<std::ostream&>(*this) << rhs; // overflow() called on chars
      all.push_back(current);
      current.clear();

      return *this;
    }


  private:
    itemType current;
    itemsType all;
    
    /* all of the chars that would have been output by the ostream 
     * will be captured here
     */ 
    virtual int overflow(int c) {
      current.push_back(static_cast<char>(c));
      return c;
    }

    const std::list<std::string>& items() const {
      return all;
    }
};

#else /* NULL_CAM_LOGGING */
namespace CAM { class Operation; }
class StreamCapture {
  public:
#ifdef _WIN32
	StreamCapture() { }
	~StreamCapture() { }
#endif
    /* insertion function! 
     * make the ostream interpet the output and then capture it using overflow()
     */
    template <typename T> StreamCapture& operator<<(const T& rhs) {
      return *this;
    }
};
#endif



#ifndef NULL_CAM_LOGGING
namespace CAM {
  class V_API ProcessLevel {
    friend class Operation;
    private:
      /* this will live on for ordering events.  It will need to be
       * process global, not just thread global
       */
      static long long g_sequenceNumber;
      
      /* When we start running between process and boxes a proper
       * box and process identifier will be requried.  But for now
       * simpler is easier.  Must be process global
       *
       * Currently a process identifier plus this sequence number should get
       * the job done
       */
      inline static long long nextId() {
        return ProcessLevel::g_sequenceNumber++;
      }

      /* ID for this process, used at output time */
      static std::string strProcessId;
      static void setupProcessId();
      inline static const std::string& processId() {
        if (ProcessLevel::strProcessId.size() == 0) {
          ProcessLevel::setupProcessId();
        }
       
        return ProcessLevel::strProcessId;
      }

    /* where am I writing logs to? */
    public:
      typedef void (*camLogFunction_t) (void* state, const char* message);

      inline static void setCamDestination(std::ostream* dest) {
        if (ProcessLevel::g_camDestination == 0)
          ProcessLevel::g_camDestination = dest;
      }
      inline static void setCamDestination(camLogFunction_t func, void* data) {
        ProcessLevel::g_camLogFunction = func;
        ProcessLevel::g_camLogFunctionState = data;
      }

    private:
      static std::ostream* g_camDestination;
      static camLogFunction_t g_camLogFunction;
      static void* g_camLogFunctionState;
  };


  class V_API ThreadLevel {
    friend class Operation;
    private: 
      /* Used to sequence operations.  Useful.
       * Will need to be thread global; not process global
       */
      static Operation* g_currentOperation;

    public:
      /* Used to Chain together ChainIDs.
       * Will need to be thread global; not process global
       */
      static void nextChainId(const std::string& str);
      inline static Operation& currentOperation() { return *ThreadLevel::g_currentOperation; }
  };


  class V_API Operation {
    public:
      Operation(): id(ProcessLevel::nextId()) { 
        init();
      }
      ~Operation() {
        timeval rawtime;
        gettimeofday(&rawtime, NULL);

        StreamCapture msg;
        msg << "used_time" << (getCpuUsage() - startUsedTime) 
            << "elapsed_time" << (getElapsedTime(&rawtime) - startElapsedTime);

        std::list<std::string> items = msg.items();
        log(items, &rawtime);

        resetCurrentOperation();
      }

      Operation& operator=(const StreamCapture& data) {
        std::list<std::string> items = data.items();
        log(items);
        return *this;
      }

      std::string getId() {
        return getId(id);
      }
      std::string getId(unsigned long long id_) {
        std::ostringstream ss;
        ss << "-" << id_;
        return ProcessLevel::processId() + ss.str();
      }


      const std::string parentId() {
        if (m_parentId.size() > 0) 
          return m_parentId;

        if (!parent)
          return std::string("");

        return parent->getId();
      }

      const std::string& chainId() {
        if (m_chainIdCache) 
          return *m_chainIdCache;

        if (!parent)
          setChainId(getId());
        else
          m_chainIdCache = &(parent->nextChainId());

        return *m_chainIdCache;
      }

      const std::string& nextChainId() {
        if (m_nextChainId.size() > 0) {
          return m_nextChainId;
        }
        return chainId();
      }
 
      void nextChainId(const std::string& str) {
        m_nextChainId = str;

        std::list<std::string> msg;
        msg.push_back("nextChainId");
        msg.push_back(m_nextChainId);
        log(msg);
      }
 
      void setChainId(const std::string& str) {
        m_chainId = str;
        m_chainIdCache = &m_chainId;
      }
 
      /* Note that this operation and it's descendant operations 
       * are associated with the supplied operation.  This will
       * log a note but not change the parent or chain ID string
       */
      void associateParent(Operation* op) {
        associateParent(op->getId(), op->chainId());
      }
      void associateParent(
          const std::string& parentId, const std::string& chainId
      ){
        (*this) = ( 
          StreamCapture()
            << "associateParentId" << parentId
            << "associateChainId" << chainId
        );
      }

      /* Change the parentId and chainId associated with this 
       * operation and it's descendant operations, claiming ownership
       * of work in the descendant operations.
       */
      void switchParent(Operation* op) {
        switchParent(op->getId(), op->chainId());
      }
      void switchParent(const std::string& parentId, const std::string& chainId)
      {
        associateParent(parentId, chainId);
        setChainId(chainId);
        m_parentId = parentId;
      }

      /* An even stronger statement of ownership than switchParent,
       * indicating a claim of ownership of not only this operation
       * and it's descendant operations but also of it's ancestors
       * through the last change of chainId.
       */
      void stitchParent(Operation* op) {
        stitchParent(op->getId(), op->chainId());
      }
      void stitchParent(const std::string& parentId, const std::string& chainId)
      {
        (*this) = ( 
          StreamCapture()
            << "stitchParentId" << parentId
            << "stitchChainId" << chainId
        );
        setChainId(chainId);
        m_parentId = parentId;
      }

      void uniqueChainId() {
        long long newChainId = ProcessLevel::nextId();

        (*this) = (StreamCapture() << "associateChainId" << getId(newChainId)); 

        std::ostringstream ss;
        ss << newChainId;
        setChainId(ss.str());
      }

      void detachFromCamStack(bool newChainId = true) {
        if (newChainId) {
          uniqueChainId();
        }
        resetCurrentOperation();
      }

        

    private:
      Operation(const Operation& o) {}
      std::string m_chainId;
      const std::string* m_chainIdCache;
      std::string m_nextChainId;
      unsigned long long id;
      Operation* parent;
      std::string m_parentId;
      Operation* m_stackParent;
      long long startUsedTime;
      long long startElapsedTime;
      bool loggedFirstEntry;
      bool m_detached;
      std::ostringstream m_outputBuffer;

      void init() {
        m_detached = false;
        loggedFirstEntry = false;
        becomeCurrentOperation();
        startUsedTime = getCpuUsage();
        startElapsedTime = getElapsedTime();
        m_chainIdCache = NULL;
      }

      void becomeCurrentOperation() {
        parent = m_stackParent = 0;
        if (ThreadLevel::g_currentOperation) {
          m_stackParent = ThreadLevel::g_currentOperation;
          parent = m_stackParent;
        }
        ThreadLevel::g_currentOperation = this;
      }

      void resetCurrentOperation() {
        if (!m_detached) { 
          m_detached = true; // can only come off the stack once!
          if (ThreadLevel::g_currentOperation != this) {
            std::string foundId("NULL");
            if (ThreadLevel::g_currentOperation)
              foundId = ThreadLevel::g_currentOperation->getId();
            (*this) = ( 
              StreamCapture() << "message" 
                << "CAM::resetCurrentOperation() found operation %foundId "
                   "when expecting %expectedId"
                << foundId << getId() << "level" << CAM_LEVEL_ERROR
            );
          }

          ThreadLevel::g_currentOperation = m_stackParent; // will reset to null if no parent
        }
      }

      long long getCpuUsage();

      long long getElapsedTime(timeval* time = 0) {
        timeval rawtime;

        if (time == 0) {
          time = &rawtime;
          gettimeofday(time, NULL);
        }

        return (
          time->tv_usec + time->tv_sec * 1000000
        );
      }

      /* log message in CLP best practice style
       * 
       */
      void log(std::list<std::string>& items, timeval* time = 0);

      void processMessage(
          std::string& in, std::string& out, 
          std::list<std::string>& keys,
          std::list<std::string>::const_iterator values
      );

      void logKeyValueCopy(std::string k, std::string v, bool first = false) {
        logKeyValue(k, v, first);
      }

      /* Easier to keep track of the start of the string than the end */
      void logKeyValue(std::string& key, std::string& val, bool first = false) {
        if (!first) {
          m_outputBuffer << ", ";   
        }
        logString(key);
        m_outputBuffer << ":";
        logString(val);
      }

      std::string escape(std::string const &s) {
        std::size_t len = s.length();
        std::string escaped;
        escaped.reserve(s.length());

        for (std::size_t i = 0; i < len; ++i) {
          if (s[i] == '\n') {
            escaped += "\\n";
            continue;
          }
          else if (s[i] == '\\' || s[i] == '\"') {
            escaped += '\\';
          }
          escaped += s[i];
        }
        return escaped;
      }

      void logString(std::string& s) {
        m_outputBuffer << "\"" << escape(s) << "\"";
      }
      
      void write() {
        if (ProcessLevel::g_camDestination != 0) {
          ((*ProcessLevel::g_camDestination) << m_outputBuffer.str());
        } 
        else if (ProcessLevel::g_camLogFunction != 0) {
          ProcessLevel::g_camLogFunction(ProcessLevel::g_camLogFunctionState, m_outputBuffer.str().c_str());
        }
        else {
          std::cout << m_outputBuffer.str();
        }
        m_outputBuffer.str("");
      }

      static const char* getTimestamp(
          char* tsBuffer, 
          unsigned int tsBufferSize, 
          timeval* time = 0
      );
  };


}
#else /* NULL_CAM_LOGGING */
namespace CAM {
  extern std::string disabledMessage;
  extern Operation nullOperation;

  class V_API ProcessLevel {
    friend class Operation;
    /* where am I writing logs to? */
    public:
      typedef void (*camLogFunction_t) (void* state, const char* message);

      inline static void setCamDestination(std::ostream* dest) { }
      inline static void setCamDestination(camLogFunction_t func, void* data) { }
  };


  class V_API Operation {
    public:
      Operation& operator=(const StreamCapture& data) { return *this; }

      std::string getId() { return disabledMessage; }
      std::string getId(unsigned long long id_) { return disabledMessage; }

      const std::string parentId() { return disabledMessage; }
      const std::string& chainId() { return disabledMessage; }
      const std::string& nextChainId() { return disabledMessage; }
 
      void nextChainId(const std::string& str) { }
      void setChainId(const std::string& str) { }
      void associateParent(Operation* op) { }
      void associateParent( const std::string& parentId, const std::string& chainId) { }
      void switchParent(Operation* op) { }
      void switchParent(const std::string& parentId, const std::string& chainId) { }
      void stitchParent(Operation* op) { }
      void stitchParent(const std::string& parentId, const std::string& chainId) { }
      void uniqueChainId() { }
      void detachFromCamStack(bool newChainId = true) { }
  };

  class V_API ThreadLevel {
    public:
      static void nextChainId(const std::string& str) { };
      inline static Operation& currentOperation() { return nullOperation; }
  };


}

#endif

// Operation Macro
#define CAM_OPERATION(name) CAM::Operation name; name = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__)

// Logging Macros
#define CAM_DEBUG CAM::currentOperation() = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__  << "level" << 30 << "message")
#define CAM_TEST CAM::currentOperation() = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__  << "level" << 40 << "message")
#define CAM_INFO CAM::currentOperation() = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__  << "level" << 50 << "message")
#define CAM_STATUS CAM::currentOperation() = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__  << "level" << 60 << "message")
#define CAM_WARN CAM::currentOperation() = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__  << "level" << 70 << "message")
#define CAM_ERROR CAM::currentOperation() = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__  << "level" << CAM_LEVEL_ERROR << "message")
#define CAM_FATAL CAM::currentOperation() = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__  << "level" << 90 << "message")
#define CAM_CRITICAL CAM::currentOperation() = (StreamCapture() << "file" << __FILE__ << "line" << __LINE__  << "level" << 100 << "message")

#endif // __CAM_H__
