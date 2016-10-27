#ifndef Vca_VcaCredentialsProvider_Interface
#define Vca_VcaCredentialsProvider_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "V_VString.h"


/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {

    class VcaCredentialsProvider : public VReferenceable {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VcaCredentialsProvider, VReferenceable);


      class Credentials : public VReferenceable {
        //  Run Time Type
        DECLARE_CONCRETE_RTTLITE (Credentials, VReferenceable);
        
        //  Friends
        friend class VcaCredentialsProvider;
	
        //  Constructor
      public:
        Credentials (VString const &rHost, VString const &rLogin, VString const &rPassword);
        
      protected:
        ~Credentials () {}
        
        //  Query
      public:
        void getHost (VString &rHost) const {
          rHost.setTo (m_iHost);
        }
        void getLogin (VString &rLogin) const {
          rLogin.setTo (m_iLogin);
        }
        void getPassword (VString &rPassword) const {
          rPassword.setTo (m_iPassword);
        }
        
        // State
      private:
        VString		      m_iHost;
        VString		      m_iLogin;
        VString		      m_iPassword;
        VReference<ThisClass> m_pSuccessor;
      };
      
      
      //  Implementation
    private:
      void init ();
      void parseCredentialsFile (char const *pFile);
      void appendCredentials (Credentials *pCredentials);

      //  Query
    public:
      bool getPassword (VString const &rHost, VString const &rLogin, VString &rPassword) const;
      
      //  Construction
    public:
      VcaCredentialsProvider ();
      
      //  Destruction
    protected:
      ~VcaCredentialsProvider ();

      //  State
    private:
      VReference<Credentials>     m_pCredentialsList;
    };
}

#endif



