/**
 * @file
 * Defines Vca::VInterfaceMember and several subclasses to handle interface member descriptors for members with varying numbers of parameters.
 * If a member with as many parameters as needed is not defined here, feel free to copy/paste to define it.
 * There's probably some template metaprogramming or macro craziness that we could do here to make it easier to define larger Vca::VInterfaceMember subclasses, but our code is unreadable enough as it is.
 */

#ifndef Vca_VInterfaceMember_Interface
#define Vca_VInterfaceMember_Interface

/************************
 *****  Components  *****
 ************************/

#include "VStaticTransient.h"

#include "Vca_VMessage.h"
#include "Vca_VMessageScheduler.h"

#include "Vca_VSubscription.h"

#include "Vca_VTuple.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VCohort.h"

#include "VTypeInfo.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {

    class VTypeInfoHolderInstance;

    /**
     * Abstract base class for descriptors for interface members.
     * Keeps track of member names, parameters, unique identifiers, etc.
     */
    class Vca_API VInterfaceMember : public VStaticTransient {
    //  Friends
        friend class VTypeInfoHolderInstance;

    //  Initializer
    public:
        /**
         * Exists to work around a bug in Microsoft's Visual C++ compiler.  In particular, static template data
         * members must be initialized using assignment syntax (i.e., Type instance
         * = initializer) and cannot be initialized using standard c-tor syntax (i.e.,
         * Type instance (args);).
         */
        struct Initializer {
            Initializer (
                char const *pName, unsigned int xMember
            ) : m_pName (pName), m_xMember (xMember) {
            }
            char const*const m_pName; unsigned int const m_xMember;
        };

    //  Construction
    protected:
        VInterfaceMember (char const *pName, unsigned int xMember);
        VInterfaceMember (Initializer const &rInitializer);

    //  Destruction
    protected:
        ~VInterfaceMember ();

    //  Access
    public:
        unsigned int index () const {
            return m_xMember;
        }
        char const *name () const {
            return m_pName;
        }
        VInterfaceMember const *successor () const {
            return m_pSuccessor;
        }

    //  Message Creation and Evaluation
    public:
        /**
         * Mechanism defined by base classes by which VMessage instances can be created.
         *
         * @param xMessage the message sequence number of the returned VMessage.
         */
        virtual VMessage *newMessage (unsigned int xMessage) const = 0;

    //  Member Registration
    protected:
        /**
         * Used to bind this VInterfaceMember instance with its interface's VTypeInfoHolderInstance.
         */
        void registerWithTypeInfoHolder (VTypeInfoHolderInstance &rTypeInfoHolderInstance);

    //  Member Lookup
    public:
        /**
         * Used to find a member that matches a given parameter signature.
         *
         * @param rSignature the parameter signature of the member being searched for.
         * @return the first member on or after this descriptor within this interface that matches the given parameter signature.
         */
        VInterfaceMember const *firstMemberWithSignature (
            VTypeInfo::ParameterSignature const &rSignature
        ) const;

        /**
         * Used to find a member that matches a given parameter signature.
         *
         * @param rSignature the parameter signature of the member being searched for.
         * @return the first member after this descriptor within this interface that matches the given parameter signature.
         */
        VInterfaceMember const *nextMemberWithSignature (
            VTypeInfo::ParameterSignature const &rSignature
        ) const {
            return m_pSuccessor->firstMemberWithSignature (rSignature);
        }

    //  Signature
    public:
        /**
         * Returns the VTypeInfo of the interface to which the member belongs.
         */
        virtual VTypeInfo *interfaceTypeInfo_() const = 0;

        /**
         * Returns the parameter signature for the member. Intended to be overridden by subclasses so that a parameter signature of the correct size and contents is returned.
         */
        virtual VTypeInfo::ParameterSignature const &parameterSignature_() const = 0;

        unsigned int parameterCount () const {
            return parameterSignature_().elementCount ();
        }

        /**
         * Returns the VTypeInfo for the parameter at a given index within the member signature.
         *
         * @param xParameter the parameter index for which to retrieve the VTypeInfo.
         * @return the VTypeInfo for the parameter.
         */
        VTypeInfo *parameter (unsigned int xParameter) const {
            return parameterSignature_()[xParameter];
        }

    //  State
    protected:
        char const* const       m_pName;        /**< Name of the member. */
        unsigned int const      m_xMember;      /**< Unique member identifier (unique within the interface to which this member belongs, not including inheritance). */
        VInterfaceMember const* m_pSuccessor;   /**< The next member in the interface. */
    };


    /**
     * An abstract specialization of VInterfaceMember adding knowledge of the interface to which the described method belongs.
     */
    template <class T> class VInterfaceMember_T : public VInterfaceMember {
        DECLARE_FAMILY_MEMBERS (VInterfaceMember_T<T>, VInterfaceMember);

    //  Construction
    protected:
        VInterfaceMember_T (char const *pName, unsigned int xMember) : BaseClass (pName, xMember) {
        }
        VInterfaceMember_T (Initializer const &rInitializer) : BaseClass (rInitializer) {
        }

    //  Destruction
    protected:
        ~VInterfaceMember_T () {
        }

    //  Static Transient Initialization
    protected:
        /**
         * Delegates to registerWithTypeInfoHolder().
         */
        void initialize () {
            registerWithTypeInfoHolder (T::typeInfoHolder ());
        }

    //  Signature
    public:
        /**
         * Returns the VTypeInfo for the (template parameter) interface to which the described member belongs.
         */
        VTypeInfo *interfaceTypeInfo_() const {
            return T::typeInfo ();
        }
    };


    /**
     * Represents an interface member with zero parameters.
     * This is the most basic of the non-abstract subclasses of vinterfacemember.
     */
    template <
        class T
    > class VInterfaceMember_0 : public VInterfaceMember_T<T> {
        DECLARE_FAMILY_MEMBERS (VInterfaceMember_0<T>, VInterfaceMember_T<T>);

    //  Context
    public:
        typedef VMessageStarter MS;
        typedef VMessage::Reference RPI;
        typedef VMessageManager MGR;

    //  Applicable
    public:
        typedef ThisClass Applicable;

    //  Arguments
    public:
        /**
         * Tuple of appropriate size to fit arguments for this member including the templated interface.
         */
        typedef VTuple_1<T*> Arguments;

    //  Initializer
    public:
        /**
         * The actual member call type.
         */
        typedef void (T::*PMember)(VMessage*);
        struct Initializer : public VInterfaceMember::Initializer {
            Initializer (char const *pName, unsigned int xMember, PMember pMember)
                : VInterfaceMember::Initializer (pName, xMember), m_pMember (pMember)
            {
            }
            PMember const m_pMember;
        };

    //  Message
    public:
        /**
         * VMessage subclass for representing messages whose target accepts zero arguments.
         */
        class Message : public VMessage {
            DECLARE_CONCRETE_RTTLITE (Message, VMessage);

        //  PayloadHolder
        public:
            class PayloadHolder {
                DECLARE_FAMILY_MEMBERS (PayloadHolder,void);

            //  Payload
            public:
                class Payload : public VReferenceable {
                    DECLARE_CONCRETE_RTTLITE (Payload, VReferenceable);

                //  Construction
                public:
                    Payload (
                        Applicable const *pApplicable,T *pT
                    ) : m_pApplicable (pApplicable), m_iArguments (pT) {
                    }
                    Payload (Applicable const *pApplicable) : m_pApplicable (pApplicable) {
                    }

                //  Destruction
                protected:
                    ~Payload () {
                    }

                //  Access
                public:
                    Applicable const *applicable () const {
                        return m_pApplicable;
                    }
                    Arguments const &arguments () const {
                        return m_iArguments;
                    }
                    Arguments& arguments () {
                        return m_iArguments;
                    }
                    T *recipient () const {
                        return m_iArguments.f1 ();
                    }

                //  Query
                public:
                    bool defersTo (VMessageScheduler &rScheduler) const {
                        return recipient ()->defersTo (rScheduler);
                    }

		//  Use
		public:
		    void evaluateWith (Message* pMessage, MGR* pMgr) {
			VMessageScheduler iScheduler;
			if (defersTo (iScheduler)) {
			    RPI rpI (new Message (iScheduler,this,pMgr));
			    iScheduler.schedule (rpI);
			} else {
			    (*m_pApplicable)(m_iArguments.f1 (), pMgr, pMessage);
			}
		    }

                //  State
                private:
                    Applicable const *const m_pApplicable;
                    Arguments m_iArguments;
                };

            //  Construction
            public:
                PayloadHolder (Applicable const *pApplicable, T *pT) : m_pPayload (new Payload (pApplicable,pT)) {
                }
                PayloadHolder (Applicable const *pApplicable) : m_pPayload (new Payload (pApplicable)) {
                }
                PayloadHolder (Payload *pPayload) : m_pPayload (pPayload) {
                }

            //  Destruction
            public:
                ~PayloadHolder () {
                }

            //  Access
            public:
                Applicable const *applicable () const {
                    return m_pPayload->applicable ();
                }
                Arguments& arguments () const {
                    return m_pPayload->arguments ();
                }
                Payload *payload () const {
                    return m_pPayload;
                }
                T *recipient () const {
                    return m_pPayload->recipient ();
                }

	    //  Use
	    public:
		void evaluateWith (Message* pMessage, MGR* pMgr) const {
		    m_pPayload->evaluateWith (pMessage, pMgr);
		}

            //  State
            private:
                typename Payload::Reference const m_pPayload;
            };
            typedef typename PayloadHolder::Payload Payload;

        //  Construction
        public:
            Message (
                MS *pS, Applicable const *pApplicable, T *pT, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pApplicable,pT) {
            }
            Message (
                MS *pS, Payload *pPayload, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pPayload) {
            }
            Message (
                Applicable const *pApplicable, unsigned int xMessage, MGR *pMgr=0
            ) : BaseClass (xMessage, pMgr), m_iPayload (pApplicable) {
            }

        //  Destruction
        protected:
            ~Message () {
            }

        //  Access
        public:
            VInterfaceMember const *applicable_() const {
                return applicable ();
            }
            VTuple const &arguments_() const {
                return arguments ();
            }
            VTuple &arguments_() {
                return arguments ();
            }

            Applicable const *applicable () const {
                return m_iPayload.applicable ();
            }
            Arguments const &arguments () const {
                return m_iPayload.arguments ();
            }
            Arguments &arguments () {
                return m_iPayload.arguments ();
            }
            IVUnknown *recipient () const {
                return m_iPayload.recipient ();
            }

        //  Evaluation
        private:
            void evaluate_() {
                VMessageManager::Reference pManager;
                subManager (pManager);
                m_iPayload.evaluateWith (this, pManager);
            }

        //  State
        protected:
            PayloadHolder const m_iPayload;
        };
        typedef typename Message::Payload MessagePayload;

    //  Construction
    public:
        VInterfaceMember_0 (
            char const *pName, unsigned int xMember, PMember pMember
        ) : BaseClass (pName, xMember), m_pMember (pMember) {
        }
        VInterfaceMember_0 (
            Initializer const &rInitializer
        )  : BaseClass (rInitializer), m_pMember (rInitializer.m_pMember) {
        }

    //  Destruction
    public:
        ~VInterfaceMember_0 () {
        }

    //  Access
    public:
        PMember member () const {
            return m_pMember;
        }

    //  Query
    public:
        bool doesntName (PMember pMember) const {
            return m_pMember != pMember;
        }
        bool names (PMember pMember) const {
            return m_pMember == pMember;
        }

    //  Signature
    public:
        static INLINE_TMEMBER VTypeInfo::ParameterSignature const &parameterSignature ();

        VTypeInfo::ParameterSignature const &parameterSignature_() const {
            return parameterSignature ();
        }

    //  Message Creation
    public:
        VMessage *newMessage (unsigned int xMessage) const {
            return new Message (this, xMessage);
        }

    //  Message
    public:
        /**
         * Called automagically by invoking the described method (with appropriate parameters) on the interface directly.
         * Aforementioned magic contained in @ref VINTERFACE_METHOD_2 VINTERFACE_METHOD_n.
         *
         * Uses IVUnknown::defersTo() to decide whether or not to execute using a Message instance and a VMessageScheduler.
         * Otherwise will execute locally (directly).
         */
        void operator()(T *pT, MGR *pMgr=0,Message* pMessage=0) const {
            VMessageScheduler iScheduler;
	    // Enumerate the 'delegate's of this interface...
	    for (
		typename T::Subscription::Enumerator pSubscription (pT);
		pSubscription.isntNil ();
		pSubscription.next ()
	    ) {
		// See if we need to invoke the delegate via a scheduler...
		if (pSubscription->defersTo (iScheduler)) {
		    // Use scheduler.
		    RPI rpI (new Message (iScheduler,this,pSubscription,pMgr));
		    iScheduler.schedule (rpI);
		} else {
		    // Execute locally.
		    (static_cast<T*>(pSubscription)->*m_pMember)(pMessage);
		}
	    }
        }

    //  State
    protected:
        /**
         * The member function on the interface that implements the method.
         * @see VINTERFACE_MEMBERIMPL
         */
        PMember const m_pMember;
    };


    /**
     * Represents an interface member with one parameter.
     */
    template <
        class T,typename P1
    > class VInterfaceMember_1 : public VInterfaceMember_T<T> {
    //  Family
    public:
        typedef VInterfaceMember_1<T,P1> ThisClass;
        typedef VInterfaceMember_T<T> BaseClass;

    //  Context
    public:
        typedef VMessageStarter MS;
        typedef VMessage::Reference RPI;
        typedef VMessageManager MGR;

    //  Applicable
    public:
        typedef ThisClass Applicable;

    //  Arguments
    public:
        /**
         * @copydoc VInterfaceMember_0::Arguments
         */
        typedef VTuple_2<T*,P1> Arguments;

    //  Initializer
    public:
        /**
         * @copydoc VInterfaceMember_0::PMember
         */
        typedef void (T::*PMember)(VMessage*,P1);
        struct Initializer : public VInterfaceMember::Initializer {
            Initializer (char const *pName, unsigned int xMember, PMember pMember)
                : VInterfaceMember::Initializer (pName, xMember), m_pMember (pMember)
            {
            }
            PMember const m_pMember;
        };

    //  Message
    public:
        /**
         * VMessage subclass for representing messages whose target accepts one argument.
         */
        class Message : public VMessage {
        public:
            DECLARE_CONCRETE_RTTLITE (Message, VMessage);

        //  PayloadHolder
        public:
            class PayloadHolder {
                DECLARE_FAMILY_MEMBERS (PayloadHolder,void);

            //  Payload
            public:
                class Payload : public VReferenceable {
                    DECLARE_CONCRETE_RTTLITE (Payload, VReferenceable);

                //  Construction
                public:
                    Payload (
                        Applicable const *pApplicable, T *pT,P1 p1
                    ) : m_pApplicable (pApplicable), m_iArguments (pT,p1) {
                    }
                    Payload (Applicable const *pApplicable) : m_pApplicable (pApplicable) {
                    }

                //  Destruction
                protected:
                    ~Payload () {
                    }

                //  Access
                public:
                    Applicable const *applicable () const {
                        return m_pApplicable;
                    }
                    Arguments const &arguments () const {
                        return m_iArguments;
                    }
                    Arguments& arguments () {
                        return m_iArguments;
                    }
                    T *recipient () const {
                        return m_iArguments.f1 ();
                    }

                //  Query
                public:
                    bool defersTo (VMessageScheduler &rScheduler) const {
                        return recipient ()->defersTo (rScheduler);
                    }

		//  Use
		public:
		    void evaluateWith (Message* pMessage, MGR* pMgr) {
			VMessageScheduler iScheduler;
			if (defersTo (iScheduler)) {
			    RPI rpI (new Message (iScheduler,this,pMgr));
			    iScheduler.schedule (rpI);
			} else {
			    (*m_pApplicable)(m_iArguments.f1 (), m_iArguments.f2 (), pMgr, pMessage);
			}
		    }

                //  State
                private:
                    Applicable const *const m_pApplicable;
                    Arguments m_iArguments;
                };

            //  Construction
            public:
                PayloadHolder (
                    Applicable const *pApplicable, T *pT,P1 p1
                ) : m_pPayload (new Payload (pApplicable,pT,p1)) {
                }
                PayloadHolder (Applicable const *pApplicable) : m_pPayload (new Payload (pApplicable)) {
                }
                PayloadHolder (Payload *pPayload) : m_pPayload (pPayload) {
                }

            //  Destruction
            public:
                ~PayloadHolder () {
                }

            //  Access
            public:
                Applicable const *applicable () const {
                    return m_pPayload->applicable ();
                }
                Arguments& arguments () const {
                    return m_pPayload->arguments ();
                }
                Payload *payload () const {
                    return m_pPayload;
                }
                T *recipient () const {
                    return m_pPayload->recipient ();
                }

	    //  Use
	    public:
		void evaluateWith (Message* pMessage, MGR* pMgr) const {
		    m_pPayload->evaluateWith (pMessage, pMgr);
		}

            //  State
            private:
                typename Payload::Reference const m_pPayload;
            };
            typedef typename PayloadHolder::Payload Payload;

        //  Construction
        public:
            Message (
                MS *pS, Applicable const *pApplicable, T *pT,P1 p1, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pApplicable,pT,p1) {
            }
            Message (
                MS *pS, Payload *pPayload, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pPayload) {
            }
            Message (
                Applicable const *pApplicable, unsigned int xMessage, MGR *pMgr=0
            ) : BaseClass (xMessage, pMgr), m_iPayload (pApplicable) {
            }

        //  Destruction
        protected:
            ~Message () {
            }

        //  Access
        public:
            VInterfaceMember const *applicable_() const {
                return applicable ();
            }
            VTuple const &arguments_() const {
                return arguments ();
            }
            VTuple &arguments_() {
                return arguments ();
            }

            Applicable const *applicable () const {
                return m_iPayload.applicable ();
            }
            Arguments const &arguments () const {
                return m_iPayload.arguments ();
            }
            Arguments &arguments () {
                return m_iPayload.arguments ();
            }
            IVUnknown *recipient () const {
                return m_iPayload.recipient ();
            }

        //  Evaluation
        private:
            void evaluate_() {
                VMessageManager::Reference pManager;
                subManager (pManager);
                m_iPayload.evaluateWith (this, pManager);
            }

        //  State
        protected:
            PayloadHolder const m_iPayload;
        };
        typedef typename Message::Payload MessagePayload;

    //  Construction
    public:
        VInterfaceMember_1 (
            char const *pName, unsigned int xMember, PMember pMember
        ) : BaseClass (pName, xMember), m_pMember (pMember) {
        }
        VInterfaceMember_1 (
            Initializer const &rInitializer
        ) : BaseClass (rInitializer), m_pMember (rInitializer.m_pMember) {
        }

    //  Destruction
    public:
        ~VInterfaceMember_1 () {
        }

    //  Access
    public:
        PMember member () const {
            return m_pMember;
        }

    //  Query
    public:
        bool doesntName (PMember pMember) const {
            return m_pMember != pMember;
        }
        bool names (PMember pMember) const {
            return m_pMember == pMember;
        }

    //  Signature
    public:
        static INLINE_TMEMBER VTypeInfo::ParameterSignature const &parameterSignature ();

        VTypeInfo::ParameterSignature const &parameterSignature_() const {
            return parameterSignature ();
        }

    //  Message Creation
    public:
        VMessage *newMessage (unsigned int xMessage) const {
            return new Message (this, xMessage);
        }

    //  Message
    public:
        /**
         * @copydoc VInterfaceMember_0::operator()(T,MGR)
         */
        void operator()(T *pT,P1 p1,MGR *pMgr=0,Message * pMessage = 0) const {
            VMessageScheduler iScheduler;
	    // Enumerate the 'delegate's of this interface...
	    for (
		typename T::Subscription::Enumerator pSubscription (pT);
		pSubscription.isntNil ();
		pSubscription.next ()
	    ) {
		// See if we need to invoke the delegate via a scheduler...
		if (pSubscription->defersTo (iScheduler)) {
		    RPI rpI (new Message (iScheduler,this,pSubscription,p1,pMgr));
		    iScheduler.schedule (rpI);
		} else {
		    (static_cast<T*>(pSubscription)->*m_pMember)(pMessage, p1);
		}
	    }
        }

    //  State
    protected:
        /**
         * @copydoc VInterfaceMember_0::m_pMember
         */
        PMember const m_pMember;
    };


    /**
     * Represents an interface member with two parameters.
     */
    template <
        class T,typename P1,typename P2
    > class VInterfaceMember_2 : public VInterfaceMember_T<T> {
    //  Family
    public:
        typedef VInterfaceMember_2<T,P1,P2> ThisClass;
        typedef VInterfaceMember_T<T> BaseClass;

    //  Context
    public:
        typedef VMessageStarter MS;
        typedef VMessage::Reference RPI;
        typedef VMessageManager MGR;

    //  Applicable
    public:
        typedef ThisClass Applicable;

    //  Arguments
    public:
        /**
         * @copydoc VInterfaceMember_0::Arguments
         */
        typedef VTuple_3<T*,P1,P2> Arguments;

    //  Initializer
    public:
        /**
         * @copydoc VInterfaceMember_0::PMember
         */
        typedef void (T::*PMember)(VMessage*,P1,P2);
        struct Initializer : public VInterfaceMember::Initializer {
            Initializer (char const *pName, unsigned int xMember, PMember pMember)
                : VInterfaceMember::Initializer (pName, xMember), m_pMember (pMember)
            {
            }
            PMember const m_pMember;
        };

    //  Message
    public:
        /**
         * VMessage subclass for representing messages whose target accepts two arguments.
         */
        class Message : public VMessage {
        public:
            DECLARE_CONCRETE_RTTLITE (Message, VMessage);

        //  PayloadHolder
        public:
            class PayloadHolder {
                DECLARE_FAMILY_MEMBERS (PayloadHolder,void);

            //  Payload
            public:
                class Payload : public VReferenceable {
                    DECLARE_CONCRETE_RTTLITE (Payload, VReferenceable);

                //  Construction
                public:
                    Payload (
                        Applicable const *pApplicable, T *pT,P1 p1,P2 p2
                    ) : m_pApplicable (pApplicable), m_iArguments (pT,p1,p2) {
                    }
                    Payload (Applicable const *pApplicable) : m_pApplicable (pApplicable) {
                    }

                //  Destruction
                protected:
                    ~Payload () {
                    }

                //  Access
                public:
                    Applicable const *applicable () const {
                        return m_pApplicable;
                    }
                    Arguments const &arguments () const {
                        return m_iArguments;
                    }
                    Arguments& arguments () {
                        return m_iArguments;
                    }
                    T *recipient () const {
                        return m_iArguments.f1 ();
                    }

                //  Query
                public:
                    bool defersTo (VMessageScheduler &rScheduler) const {
                        return recipient ()->defersTo (rScheduler);
                    }

		//  Use
		public:
		    void evaluateWith (Message* pMessage, MGR* pMgr) {
			VMessageScheduler iScheduler;
			if (defersTo (iScheduler)) {
			    RPI rpI (new Message (iScheduler,this,pMgr));
			    iScheduler.schedule (rpI);
			} else {
			    (*m_pApplicable)(m_iArguments.f1 (), m_iArguments.f2 (), m_iArguments.f3 (), pMgr, pMessage);
			}
		    }

                //  State
                private:
                    Applicable const *const m_pApplicable;
                    Arguments m_iArguments;
                };

            //  Construction
            public:
                PayloadHolder (
                    Applicable const *pApplicable, T *pT,P1 p1,P2 p2
                ) : m_pPayload (new Payload (pApplicable,pT,p1,p2)) {
                }
                PayloadHolder (Applicable const *pApplicable) : m_pPayload (new Payload (pApplicable)) {
                }
                PayloadHolder (Payload *pPayload) : m_pPayload (pPayload) {
                }

            //  Destruction
            public:
                ~PayloadHolder () {
                }

            //  Access
            public:
                Applicable const *applicable () const {
                    return m_pPayload->applicable ();
                }
                Arguments& arguments () const {
                    return m_pPayload->arguments ();
                }
                Payload *payload () const {
                    return m_pPayload;
                }
                T *recipient () const {
                    return m_pPayload->recipient ();
                }

	    //  Use
	    public:
		void evaluateWith (Message* pMessage, MGR* pMgr) const {
		    m_pPayload->evaluateWith (pMessage, pMgr);
		}

            //  State
            private:
                typename Payload::Reference const m_pPayload;
            };
            typedef typename PayloadHolder::Payload Payload;

        //  Construction
        public:
            Message (
                MS *pS, Applicable const *pApplicable, T *pT,P1 p1,P2 p2, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pApplicable,pT,p1,p2) {
            }
            Message (
                MS *pS, Payload *pPayload, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pPayload) {
            }
            Message (
                Applicable const *pApplicable, unsigned int xMessage, MGR *pMgr=0
            ) : BaseClass (xMessage, pMgr), m_iPayload (pApplicable) {
            }

        //  Destruction
        protected:
            ~Message () {
            }

        //  Access
        public:
            VInterfaceMember const *applicable_() const {
                return applicable ();
            }
            VTuple const &arguments_() const {
                return arguments ();
            }
            VTuple &arguments_() {
                return arguments ();
            }

            Applicable const *applicable () const {
                return m_iPayload.applicable ();
            }
            Arguments const &arguments () const {
                return m_iPayload.arguments ();
            }
            Arguments &arguments () {
                return m_iPayload.arguments ();
            }
            IVUnknown *recipient () const {
                return m_iPayload.recipient ();
            }

        //  Evaluation
        private:
            void evaluate_() {
                VMessageManager::Reference pManager;
                subManager (pManager);
		m_iPayload.evaluateWith (this, pManager);
            }

        //  State
        protected:
            PayloadHolder const m_iPayload;
        };
        typedef typename Message::Payload MessagePayload;

    //  Construction
    public:
        VInterfaceMember_2 (
            char const *pName, unsigned int xMember, PMember pMember
        ) : BaseClass (pName, xMember), m_pMember (pMember) {
        }
        VInterfaceMember_2 (
            Initializer const &rInitializer
        ) : BaseClass (rInitializer), m_pMember (rInitializer.m_pMember) {
        }

    //  Destruction
    public:
        ~VInterfaceMember_2 () {
        }

    //  Access
    public:
        PMember member () const {
            return m_pMember;
        }

    //  Query
    public:
        bool doesntName (PMember pMember) const {
            return m_pMember != pMember;
        }
        bool names (PMember pMember) const {
            return m_pMember == pMember;
        }

    //  Signature
    public:
        static INLINE_TMEMBER VTypeInfo::ParameterSignature const &parameterSignature ();

        VTypeInfo::ParameterSignature const &parameterSignature_() const {
            return parameterSignature ();
        }

    //  Message Creation
    public:
        VMessage *newMessage (unsigned int xMessage) const {
            return new Message (this, xMessage);
        }

    //  Message
    public:
        /**
         * @copydoc VInterfaceMember_0::operator()(T,MGR)
         */
        void operator()(T *pT,P1 p1,P2 p2,MGR *pMgr=0,Message* pMessage = 0) const {
            VMessageScheduler iScheduler;
	    // Enumerate the 'delegate's of this interface...
	    for (
		typename T::Subscription::Enumerator pSubscription (pT);
		pSubscription.isntNil ();
		pSubscription.next ()
	    ) {
		// See if we need to invoke the delegate via a scheduler...
		if (pSubscription->defersTo (iScheduler)) {
		    RPI rpI (new Message (iScheduler,this,pSubscription,p1,p2,pMgr));
		    iScheduler.schedule (rpI);
		} else {
		    (static_cast<T*>(pSubscription)->*m_pMember)(pMessage,p1,p2);
		}
	    }
        }

    //  State
    protected:
        /**
         * @copydoc VInterfaceMember_0::m_pMember
         */
        PMember const m_pMember;
    };


    /**
     * Represents an interface member with three parameters.
     */
    template <
        class T,typename P1,typename P2,typename P3
    > class VInterfaceMember_3 : public VInterfaceMember_T<T> {
    //  Family
    public:
        typedef VInterfaceMember_3<T,P1,P2,P3> ThisClass;
        typedef VInterfaceMember_T<T> BaseClass;

    //  Context
    public:
        typedef VMessageStarter MS;
        typedef VMessage::Reference RPI;
        typedef VMessageManager MGR;

    //  Applicable
    public:
        typedef ThisClass Applicable;

    //  Arguments
    public:
        /**
         * @copydoc VInterfaceMember_0::Arguments
         */
        typedef VTuple_4<T*,P1,P2,P3> Arguments;

    //  Initializer
    public:
        /**
         * @copydoc VInterfaceMember_0::PMember
         */
        typedef void (T::*PMember)(VMessage*,P1,P2,P3);
        struct Initializer : public VInterfaceMember::Initializer {
            Initializer (char const *pName, unsigned int xMember, PMember pMember)
                : VInterfaceMember::Initializer (pName, xMember), m_pMember (pMember)
            {
            }
            PMember const m_pMember;
        };

    //  Message
    public:
        /**
         * VMessage subclass for representing messages whose target accepts three arguments.
         */
        class Message : public VMessage {
        public:
            DECLARE_CONCRETE_RTTLITE (Message, VMessage);

        //  PayloadHolder
        public:
            class PayloadHolder {
                DECLARE_FAMILY_MEMBERS (PayloadHolder,void);

            //  Payload
            public:
                class Payload : public VReferenceable {
                    DECLARE_CONCRETE_RTTLITE (Payload, VReferenceable);

                //  Construction
                public:
                    Payload (
                        Applicable const *pApplicable,T *pT,P1 p1,P2 p2,P3 p3
                    ) : m_pApplicable (pApplicable), m_iArguments (pT,p1,p2,p3) {
                    }
                    Payload (Applicable const *pApplicable) : m_pApplicable (pApplicable) {
                    }

                //  Destruction
                protected:
                    ~Payload () {
                    }

                //  Access
                public:
                    Applicable const *applicable () const {
                        return m_pApplicable;
                    }
                    Arguments const &arguments () const {
                        return m_iArguments;
                    }
                    Arguments& arguments () {
                        return m_iArguments;
                    }
                    T *recipient () const {
                        return m_iArguments.f1 ();
                    }

                //  Query
                public:
                    bool defersTo (VMessageScheduler &rScheduler) const {
                        return recipient ()->defersTo (rScheduler);
                    }

		//  Use
		public:
		    void evaluateWith (Message* pMessage, MGR* pMgr) {
			VMessageScheduler iScheduler;
			if (defersTo (iScheduler)) {
			    RPI rpI (new Message (iScheduler,this,pMgr));
			    iScheduler.schedule (rpI);
			} else {
			    (*m_pApplicable)(
				m_iArguments.f1 (), m_iArguments.f2 (), m_iArguments.f3 (), m_iArguments.f4 (), pMgr, pMessage
			    );
			}
		    }

                //  State
                private:
                    Applicable const *const m_pApplicable;
                    Arguments m_iArguments;
                };

            //  Construction
            public:
                PayloadHolder (
                    Applicable const *pApplicable, T *pT,P1 p1,P2 p2,P3 p3
                ) : m_pPayload (new Payload (pApplicable,pT,p1,p2,p3)) {
                }
                PayloadHolder (Applicable const *pApplicable) : m_pPayload (new Payload (pApplicable)) {
                }
                PayloadHolder (Payload *pPayload) : m_pPayload (pPayload) {
                }

            //  Destruction
            public:
                ~PayloadHolder () {
                }

            //  Access
            public:
                Applicable const *applicable () const {
                    return m_pPayload->applicable ();
                }
                Arguments& arguments () const {
                    return m_pPayload->arguments ();
                }
                Payload *payload () const {
                    return m_pPayload;
                }
                T *recipient () const {
                    return m_pPayload->recipient ();
                }

	    //  Use
	    public:
		void evaluateWith (Message* pMessage, MGR* pMgr) const {
		    m_pPayload->evaluateWith (pMessage, pMgr);
		}

            //  State
            private:
                typename Payload::Reference const m_pPayload;
            };
            typedef typename PayloadHolder::Payload Payload;

        //  Construction
        public:
            Message (
                MS *pS, Applicable const *pApplicable, T *pT,P1 p1,P2 p2,P3 p3, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pApplicable,pT,p1,p2,p3) {
            }
            Message (
                MS *pS, Payload *pPayload, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pPayload) {
            }
            Message (
                Applicable const *pApplicable, unsigned int xMessage, MGR *pMgr=0
            ) : BaseClass (xMessage, pMgr), m_iPayload (pApplicable) {
            }

        //  Destruction
        protected:
            ~Message () {
            }

        //  Access
        public:
            VInterfaceMember const *applicable_() const {
                return applicable ();
            }
            VTuple const &arguments_() const {
                return arguments ();
            }
            VTuple &arguments_() {
                return arguments ();
            }

            Applicable const *applicable () const {
                return m_iPayload.applicable ();
            }
            Arguments const &arguments () const {
                return m_iPayload.arguments ();
            }
            Arguments &arguments () {
                return m_iPayload.arguments ();
            }
            IVUnknown *recipient () const {
                return m_iPayload.recipient ();
            }

        //  Evaluation
        private:
            void evaluate_() {
                VMessageManager::Reference pManager;
                subManager (pManager);
                m_iPayload.evaluateWith (this, pManager);
            }

        //  State
        protected:
            PayloadHolder const m_iPayload;
        };
        typedef typename Message::Payload MessagePayload;

    //  Construction
    public:
        VInterfaceMember_3 (
            char const *pName, unsigned int xMember, PMember pMember
        ) : BaseClass (pName, xMember), m_pMember (pMember) {
        }
        VInterfaceMember_3 (
            Initializer const &rInitializer
        ) : BaseClass (rInitializer), m_pMember (rInitializer.m_pMember) {
        }

    //  Destruction
    public:
        ~VInterfaceMember_3 () {
        }

    //  Access
    public:
        PMember member () const {
            return m_pMember;
        }

    //  Query
    public:
        bool doesntName (PMember pMember) const {
            return m_pMember != pMember;
        }
        bool names (PMember pMember) const {
            return m_pMember == pMember;
        }

    //  Signature
    public:
        static INLINE_TMEMBER VTypeInfo::ParameterSignature const &parameterSignature ();

        VTypeInfo::ParameterSignature const &parameterSignature_() const {
            return parameterSignature ();
        }

    //  Message Creation
    public:
        VMessage *newMessage (unsigned int xMessage) const {
            return new Message (this, xMessage);
        }

    //  Message
    public:
        /**
         * @copydoc VInterfaceMember_0::operator()(T,MGR)
         */
        void operator()(T *pT,P1 p1,P2 p2,P3 p3,MGR *pMgr=0,Message* pMessage=0) const {
            VMessageScheduler iScheduler;
	    // Enumerate the 'delegate's of this interface...
	    for (
		typename T::Subscription::Enumerator pSubscription (pT);
		pSubscription.isntNil ();
		pSubscription.next ()
	    ) {
		// See if we need to invoke the delegate via a scheduler...
		if (pSubscription->defersTo (iScheduler)) {
		    RPI rpI (new Message (iScheduler,this,pSubscription,p1,p2,p3,pMgr));
		    iScheduler.schedule (rpI);
		} else {
		    (static_cast<T*>(pSubscription)->*m_pMember)(pMessage,p1,p2,p3);
		}
	    }
        }

    //  State
    protected:
        /**
         * @copydoc VInterfaceMember_0::m_pMember
         */
        PMember const m_pMember;
    };


    /**
     * Represents an interface member with four parameters.
     */
    template <
        class T,typename P1,typename P2,typename P3,typename P4
    > class VInterfaceMember_4 : public VInterfaceMember_T<T> {
    //  Family
    public:
        typedef VInterfaceMember_4<T,P1,P2,P3,P4> ThisClass;
        typedef VInterfaceMember_T<T> BaseClass;

    //  Context
    public:
        typedef VMessageStarter MS;
        typedef VMessage::Reference RPI;
        typedef VMessageManager MGR;

    //  Applicable
    public:
        typedef ThisClass Applicable;

    //  Arguments
    public:
        /**
         * @copydoc VInterfaceMember_0::Arguments
         */
        typedef VTuple_5<T*,P1,P2,P3,P4> Arguments;

    //  Initializer
    public:
        /**
         * @copydoc VInterfaceMember_0::PMember
         */
        typedef void (T::*PMember)(VMessage*,P1,P2,P3,P4);
        struct Initializer : public VInterfaceMember::Initializer {
            Initializer (char const *pName, unsigned int xMember, PMember pMember)
                : VInterfaceMember::Initializer (pName, xMember), m_pMember (pMember)
            {
            }
            PMember const m_pMember;
        };

    //  Message
    public:
        /**
         * VMessage subclass for representing messages whose target accepts four arguments.
         */
        class Message : public VMessage {
        public:
            DECLARE_CONCRETE_RTTLITE (Message, VMessage);

        //  PayloadHolder
        public:
            class PayloadHolder {
                DECLARE_FAMILY_MEMBERS (PayloadHolder,void);

            //  Payload
            public:
                class Payload : public VReferenceable {
                    DECLARE_CONCRETE_RTTLITE (Payload, VReferenceable);

                //  Construction
                public:
                    Payload (
                        Applicable const *pApplicable,T *pT,P1 p1,P2 p2,P3 p3,P4 p4
                    ) : m_pApplicable (pApplicable), m_iArguments (pT,p1,p2,p3,p4) {
                    }
                    Payload (Applicable const *pApplicable) : m_pApplicable (pApplicable) {
                    }

                //  Destruction
                protected:
                    ~Payload () {
                    }

                //  Access
                public:
                    Applicable const *applicable () const {
                        return m_pApplicable;
                    }
                    Arguments const &arguments () const {
                        return m_iArguments;
                    }
                    Arguments& arguments () {
                        return m_iArguments;
                    }
                    T *recipient () const {
                        return m_iArguments.f1 ();
                    }

                //  Query
                public:
                    bool defersTo (VMessageScheduler &rScheduler) const {
                        return recipient ()->defersTo (rScheduler);
                    }

		//  Use
		public:
		    void evaluateWith (Message* pMessage, MGR* pMgr) {
			VMessageScheduler iScheduler;
			if (defersTo (iScheduler)) {
			    RPI rpI (new Message (iScheduler,this,pMgr));
			    iScheduler.schedule (rpI);
			} else {
			    (*m_pApplicable)(
				m_iArguments.f1 (), m_iArguments.f2 (), m_iArguments.f3 (), m_iArguments.f4 (),
				m_iArguments.f5 (), pMgr, pMessage
			    );
			}
		    }

                //  State
                private:
                    Applicable const *const m_pApplicable;
                    Arguments m_iArguments;
                };

            //  Construction
            public:
                PayloadHolder (
                    Applicable const *pApplicable, T *pT,P1 p1,P2 p2,P3 p3,P4 p4
                ) : m_pPayload (new Payload (pApplicable,pT,p1,p2,p3,p4)) {
                }
                PayloadHolder (Applicable const *pApplicable) : m_pPayload (new Payload (pApplicable)) {
                }
                PayloadHolder (Payload *pPayload) : m_pPayload (pPayload) {
                }

            //  Destruction
            public:
                ~PayloadHolder () {
                }

            //  Access
            public:
                Applicable const *applicable () const {
                    return m_pPayload->applicable ();
                }
                Arguments& arguments () const {
                    return m_pPayload->arguments ();
                }
                Payload *payload () const {
                    return m_pPayload;
                }
                T *recipient () const {
                    return m_pPayload->recipient ();
                }

	    //  Use
	    public:
		void evaluateWith (Message* pMessage, MGR* pMgr) const {
		    m_pPayload->evaluateWith (pMessage, pMgr);
		}

            //  State
            private:
                typename Payload::Reference const m_pPayload;
            };
            typedef typename PayloadHolder::Payload Payload;

        //  Construction
        public:
            Message (
                MS *pS, Applicable const *pApplicable,T *pT,P1 p1,P2 p2,P3 p3,P4 p4, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pApplicable,pT,p1,p2,p3,p4) {
            }
            Message (
                MS *pS, Payload *pPayload, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pPayload) {
            }
            Message (
                Applicable const *pApplicable, unsigned int xMessage, MGR *pMgr=0
            ) : BaseClass (xMessage, pMgr), m_iPayload (pApplicable) {
            }

        //  Destruction
        protected:
            ~Message () {
            }

        //  Access
        public:
            VInterfaceMember const *applicable_() const {
                return applicable ();
            }
            VTuple const &arguments_() const {
                return arguments ();
            }
            VTuple &arguments_() {
                return arguments ();
            }

            Applicable const *applicable () const {
                return m_iPayload.applicable ();
            }
            Arguments const &arguments () const {
                return m_iPayload.arguments ();
            }
            Arguments &arguments () {
                return m_iPayload.arguments ();
            }
            IVUnknown *recipient () const {
                return m_iPayload.recipient ();
            }

        //  Evaluation
        private:
            void evaluate_() {
                VMessageManager::Reference pManager;
                subManager (pManager);
                m_iPayload.evaluateWith (this, pManager);
            }

        //  State
        protected:
            PayloadHolder const m_iPayload;
        };
        typedef typename Message::Payload MessagePayload;

    //  Construction
    public:
        VInterfaceMember_4 (
            char const *pName, unsigned int xMember, PMember pMember
        ) : BaseClass (pName, xMember), m_pMember (pMember) {
        }
        VInterfaceMember_4 (
            Initializer const &rInitializer
        ) : BaseClass (rInitializer), m_pMember (rInitializer.m_pMember) {
        }

    //  Destruction
    public:
        ~VInterfaceMember_4 () {
        }

    //  Access
    public:
        PMember member () const {
            return m_pMember;
        }

    //  Query
    public:
        bool doesntName (PMember pMember) const {
            return m_pMember != pMember;
        }
        bool names (PMember pMember) const {
            return m_pMember == pMember;
        }

    //  Signature
    public:
        static INLINE_TMEMBER VTypeInfo::ParameterSignature const &parameterSignature ();

        VTypeInfo::ParameterSignature const &parameterSignature_() const {
            return parameterSignature ();
        }

    //  Message Creation
    public:
        VMessage *newMessage (unsigned int xMessage) const {
            return new Message (this, xMessage);
        }

    //  Message
    public:
        /**
         * @copydoc VInterfaceMember_0::operator()(T,MGR)
         */
        void operator()(T *pT,P1 p1,P2 p2,P3 p3,P4 p4, MGR *pMgr=0,Message* pMessage=0) const {
            VMessageScheduler iScheduler;
	    // Enumerate the 'delegate's of this interface...
	    for (
		typename T::Subscription::Enumerator pSubscription (pT);
		pSubscription.isntNil ();
		pSubscription.next ()
	    ) {
		// See if we need to invoke the delegate via a scheduler...
		if (pSubscription->defersTo (iScheduler)) {
		    RPI rpI (new Message (iScheduler,this,pSubscription,p1,p2,p3,p4,pMgr));
		    iScheduler.schedule (rpI);
		} else {
		    (static_cast<T*>(pSubscription)->*m_pMember)(pMessage,p1,p2,p3,p4);
		}
	    }
        }

    //  State
    protected:
        /**
         * @copydoc VInterfaceMember_0::m_pMember
         */
        PMember const m_pMember;
    };


    /**
     * Represents an interface member with five parameters.
     */
    template <
        class T,typename P1,typename P2,typename P3,typename P4,typename P5
    > class VInterfaceMember_5 : public VInterfaceMember_T<T> {
    //  Family
    public:
        typedef VInterfaceMember_5<T,P1,P2,P3,P4,P5> ThisClass;
        typedef VInterfaceMember_T<T> BaseClass;

    //  Context
    public:
        typedef VMessageStarter MS;
        typedef VMessage::Reference RPI;
        typedef VMessageManager MGR;

    //  Applicable
    public:
        typedef ThisClass Applicable;

    //  Arguments
    public:
        /**
         * @copydoc VInterfaceMember_0::Arguments
         */
        typedef VTuple_6<T*,P1,P2,P3,P4,P5> Arguments;

    //  Initializer
    public:
        /**
         * @copydoc VInterfaceMember_0::PMember
         */
        typedef void (T::*PMember)(VMessage*,P1,P2,P3,P4,P5);
        struct Initializer : public VInterfaceMember::Initializer {
            Initializer (char const *pName, unsigned int xMember, PMember pMember)
                : VInterfaceMember::Initializer (pName, xMember), m_pMember (pMember)
            {
            }
            PMember const m_pMember;
        };

    //  Message
    public:
        /**
         * VMessage subclass for representing messages whose target accepts five arguments.
         */
        class Message : public VMessage {
        public:
            DECLARE_CONCRETE_RTTLITE (Message, VMessage);

        //  PayloadHolder
        public:
            class PayloadHolder {
                DECLARE_FAMILY_MEMBERS (PayloadHolder,void);

            //  Payload
            public:
                class Payload : public VReferenceable {
                    DECLARE_CONCRETE_RTTLITE (Payload, VReferenceable);

                //  Construction
                public:
                    Payload (
                        Applicable const *pApplicable,T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5
                    ) : m_pApplicable (pApplicable), m_iArguments (pT,p1,p2,p3,p4,p5) {
                    }
                    Payload (Applicable const *pApplicable) : m_pApplicable (pApplicable) {
                    }

                //  Destruction
                protected:
                    ~Payload () {
                    }

                //  Access
                public:
                    Applicable const *applicable () const {
                        return m_pApplicable;
                    }
                    Arguments const &arguments () const {
                        return m_iArguments;
                    }
                    Arguments& arguments () {
                        return m_iArguments;
                    }
                    T *recipient () const {
                        return m_iArguments.f1 ();
                    }

                //  Query
                public:
                    bool defersTo (VMessageScheduler &rScheduler) const {
                        return recipient ()->defersTo (rScheduler);
                    }

		//  Use
		public:
		    void evaluateWith (Message* pMessage, MGR* pMgr) {
			VMessageScheduler iScheduler;
			if (defersTo (iScheduler)) {
			    RPI rpI (new Message (iScheduler,this,pMgr));
			    iScheduler.schedule (rpI);
			} else {
			    (*m_pApplicable)(
				m_iArguments.f1 (), m_iArguments.f2 (), m_iArguments.f3 (), m_iArguments.f4 (),
				m_iArguments.f5 (), m_iArguments.f6 (), pMgr, pMessage
			    );
			}
		    }

                //  State
                private:
                    Applicable const *const m_pApplicable;
                    Arguments m_iArguments;
                };

            //  Construction
            public:
                PayloadHolder (
                    Applicable const *pApplicable, T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5
                ) : m_pPayload (new Payload (pApplicable,pT,p1,p2,p3,p4,p5)) {
                }
                PayloadHolder (Applicable const *pApplicable) : m_pPayload (new Payload (pApplicable)) {
                }
                PayloadHolder (Payload *pPayload) : m_pPayload (pPayload) {
                }

            //  Destruction
            public:
                ~PayloadHolder () {
                }

            //  Access
            public:
                Applicable const *applicable () const {
                    return m_pPayload->applicable ();
                }
                Arguments& arguments () const {
                    return m_pPayload->arguments ();
                }
                Payload *payload () const {
                    return m_pPayload;
                }
                T *recipient () const {
                    return m_pPayload->recipient ();
                }

	    //  Use
	    public:
		void evaluateWith (Message* pMessage, MGR* pMgr) const {
		    m_pPayload->evaluateWith (pMessage, pMgr);
		}

            //  State
            private:
                typename Payload::Reference const m_pPayload;
            };
            typedef typename PayloadHolder::Payload Payload;

        //  Construction
        public:
            Message (
                MS *pS, Applicable const *pApplicable, T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pApplicable,pT,p1,p2,p3,p4,p5) {
            }
            Message (
                MS *pS, Payload *pPayload, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pPayload) {
            }
            Message (
                Applicable const *pApplicable, unsigned int xMessage, MGR *pMgr=0
            ) : BaseClass (xMessage, pMgr), m_iPayload (pApplicable) {
            }

        //  Destruction
        protected:
            ~Message () {
            }

        //  Access
        public:
            VInterfaceMember const *applicable_() const {
                return applicable ();
            }
            VTuple const &arguments_() const {
                return arguments ();
            }
            VTuple &arguments_() {
                return arguments ();
            }

            Applicable const *applicable () const {
                return m_iPayload.applicable ();
            }
            Arguments const &arguments () const {
                return m_iPayload.arguments ();
            }
            Arguments &arguments () {
                return m_iPayload.arguments ();
            }
            IVUnknown *recipient () const {
                return m_iPayload.recipient ();
            }

        //  Evaluation
        private:
            void evaluate_() {
                VMessageManager::Reference pManager;
                subManager (pManager);
                m_iPayload.evaluateWith (this, pManager);
            }

        //  State
        protected:
            PayloadHolder const m_iPayload;
        };
        typedef typename Message::Payload MessagePayload;

    //  Construction
    public:
        VInterfaceMember_5 (
            char const *pName, unsigned int xMember, PMember pMember
        ) : BaseClass (pName, xMember), m_pMember (pMember) {
        }
        VInterfaceMember_5 (
            Initializer const &rInitializer
        ) : BaseClass (rInitializer), m_pMember (rInitializer.m_pMember) {
        }

    //  Destruction
    public:
        ~VInterfaceMember_5 () {
        }

    //  Access
    public:
        PMember member () const {
            return m_pMember;
        }

    //  Query
    public:
        bool doesntName (PMember pMember) const {
            return m_pMember != pMember;
        }
        bool names (PMember pMember) const {
            return m_pMember == pMember;
        }

    //  Signature
    public:
        static INLINE_TMEMBER VTypeInfo::ParameterSignature const &parameterSignature ();

        VTypeInfo::ParameterSignature const &parameterSignature_() const {
            return parameterSignature ();
        }

    //  Message Creation
    public:
        VMessage *newMessage (unsigned int xMessage) const {
            return new Message (this, xMessage);
        }

    //  Message
    public:
        /**
         * @copydoc VInterfaceMember_0::operator()(T,MGR)
         */
        void operator()(T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5, MGR *pMgr=0,Message* pMessage=0) const {
            VMessageScheduler iScheduler;
	    // Enumerate the 'delegate's of this interface...
	    for (
		typename T::Subscription::Enumerator pSubscription (pT);
		pSubscription.isntNil ();
		pSubscription.next ()
	    ) {
		// See if we need to invoke the delegate via a scheduler...
		if (pSubscription->defersTo (iScheduler)) {
		    RPI rpI (new Message (iScheduler,this,pSubscription,p1,p2,p3,p4,p5,pMgr));
		    iScheduler.schedule (rpI);
		} else {
		    (static_cast<T*>(pSubscription)->*m_pMember)(pMessage,p1,p2,p3,p4,p5);
		}
	    }
        }

    //  State
    protected:
        /**
         * @copydoc VInterfaceMember_0::m_pMember
         */
        PMember const m_pMember;
    };


    /**
     * Represents an interface member with six prameters.
     */
    template <
        class T,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6
    > class VInterfaceMember_6 : public VInterfaceMember_T<T> {
    //  Family
    public:
        typedef VInterfaceMember_6<T,P1,P2,P3,P4,P5,P6> ThisClass;
        typedef VInterfaceMember_T<T> BaseClass;

    //  Context
    public:
        typedef VMessageStarter MS;
        typedef VMessage::Reference RPI;
        typedef VMessageManager MGR;

    //  Applicable
    public:
        typedef ThisClass Applicable;

    //  Arguments
    public:
        /**
         * @copydoc VInterfaceMember_0::Arguments
         */
        typedef VTuple_7<T*,P1,P2,P3,P4,P5,P6> Arguments;

    //  Initializer
    public:
        /**
         * @copydoc VInterfaceMember_0::PMember
         */
        typedef void (T::*PMember)(VMessage*,P1,P2,P3,P4,P5,P6);
        struct Initializer : public VInterfaceMember::Initializer {
            Initializer (char const *pName, unsigned int xMember, PMember pMember)
                : VInterfaceMember::Initializer (pName, xMember), m_pMember (pMember)
            {
            }
            PMember const m_pMember;
        };

    //  Message
    public:
        /**
         * VMessage subclass for representing messages whose target accepts six arguments.
         */
        class Message : public VMessage {
        public:
            DECLARE_CONCRETE_RTTLITE (Message, VMessage);

        //  PayloadHolder
        public:
            class PayloadHolder {
                DECLARE_FAMILY_MEMBERS (PayloadHolder,void);

            //  Payload
            public:
                class Payload : public VReferenceable {
                    DECLARE_CONCRETE_RTTLITE (Payload, VReferenceable);

                //  Construction
                public:
                    Payload (
                        Applicable const *pApplicable,T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6
                    ) : m_pApplicable (pApplicable), m_iArguments (pT,p1,p2,p3,p4,p5,p6) {
                    }
                    Payload (Applicable const *pApplicable) : m_pApplicable (pApplicable) {
                    }

                //  Destruction
                protected:
                    ~Payload () {
                    }

                //  Access
                public:
                    Applicable const *applicable () const {
                        return m_pApplicable;
                    }
                    Arguments const &arguments () const {
                        return m_iArguments;
                    }
                    Arguments& arguments () {
                        return m_iArguments;
                    }
                    T *recipient () const {
                        return m_iArguments.f1 ();
                    }

                //  Query
                public:
                    bool defersTo (VMessageScheduler &rScheduler) const {
                        return recipient ()->defersTo (rScheduler);
                    }

		//  Use
		public:
		    void evaluateWith (Message* pMessage, MGR* pMgr) {
			VMessageScheduler iScheduler;
			if (defersTo (iScheduler)) {
			    RPI rpI (new Message (iScheduler,this,pMgr));
			    iScheduler.schedule (rpI);
			} else {
			    (*m_pApplicable)(
				m_iArguments.f1 (), m_iArguments.f2 (), m_iArguments.f3 (), m_iArguments.f4 (),
				m_iArguments.f5 (), m_iArguments.f6 (), m_iArguments.f7 (), pMgr, pMessage
			    );
			}
		    }

                //  State
                private:
                    Applicable const *const m_pApplicable;
                    Arguments m_iArguments;
                };

            //  Construction
            public:
                PayloadHolder (
                    Applicable const *pApplicable, T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6
                ) : m_pPayload (new Payload (pApplicable,pT,p1,p2,p3,p4,p5,p6)) {
                }
                PayloadHolder (Applicable const *pApplicable) : m_pPayload (new Payload (pApplicable)) {
                }
                PayloadHolder (Payload *pPayload) : m_pPayload (pPayload) {
                }

            //  Destruction
            public:
                ~PayloadHolder () {
                }

            //  Access
            public:
                Applicable const *applicable () const {
                    return m_pPayload->applicable ();
                }
                Arguments& arguments () const {
                    return m_pPayload->arguments ();
                }
                Payload *payload () const {
                    return m_pPayload;
                }
                T *recipient () const {
                    return m_pPayload->recipient ();
                }

	    //  Use
	    public:
		void evaluateWith (Message* pMessage, MGR* pMgr) const {
		    m_pPayload->evaluateWith (pMessage, pMgr);
		}

            //  State
            private:
                typename Payload::Reference const m_pPayload;
            };
            typedef typename PayloadHolder::Payload Payload;

        //  Construction
        public:
            Message (
                MS *pS, Applicable const *pApplicable,T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pApplicable,pT,p1,p2,p3,p4,p5,p6) {
            }
            Message (
                MS *pS, Payload *pPayload, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pPayload) {
            }
            Message (
                Applicable const *pApplicable, unsigned int xMessage, MGR *pMgr=0
            ) : BaseClass (xMessage, pMgr), m_iPayload (pApplicable) {
            }

        //  Destruction
        protected:
            ~Message () {
            }

        //  Access
        public:
            VInterfaceMember const *applicable_() const {
                return applicable ();
            }
            VTuple const &arguments_() const {
                return arguments ();
            }
            VTuple &arguments_() {
                return arguments ();
            }

            Applicable const *applicable () const {
                return m_iPayload.applicable ();
            }
            Arguments const &arguments () const {
                return m_iPayload.arguments ();
            }
            Arguments &arguments () {
                return m_iPayload.arguments ();
            }
            IVUnknown *recipient () const {
                return m_iPayload.recipient ();
            }

        //  Evaluation
        private:
            void evaluate_() {
                VMessageManager::Reference pManager;
                subManager (pManager);
                m_iPayload.evaluateWith (this, pManager);
            }

        //  State
        protected:
            PayloadHolder const m_iPayload;
        };
        typedef typename Message::Payload MessagePayload;

    //  Construction
    public:
        VInterfaceMember_6 (
            char const *pName, unsigned int xMember, PMember pMember
        ) : BaseClass (pName, xMember), m_pMember (pMember) {
        }
        VInterfaceMember_6 (
            Initializer const &rInitializer
        ) : BaseClass (rInitializer), m_pMember (rInitializer.m_pMember) {
        }

    //  Destruction
    public:
        ~VInterfaceMember_6 () {
        }

    //  Access
    public:
        PMember member () const {
            return m_pMember;
        }

    //  Query
    public:
        bool doesntName (PMember pMember) const {
            return m_pMember != pMember;
        }
        bool names (PMember pMember) const {
            return m_pMember == pMember;
        }

    //  Signature
    public:
        static INLINE_TMEMBER VTypeInfo::ParameterSignature const &parameterSignature ();

        VTypeInfo::ParameterSignature const &parameterSignature_() const {
            return parameterSignature ();
        }

    //  Message Creation
    public:
        VMessage *newMessage (unsigned int xMessage) const {
            return new Message (this, xMessage);
        }

    //  Message
    public:
        /**
         * @copydoc VInterfaceMember_0::operator()(T,MGR)
         */
        void operator()(T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6, MGR *pMgr=0,Message* pMessage=0) const {
            VMessageScheduler iScheduler;
	    // Enumerate the 'delegate's of this interface...
	    for (
		typename T::Subscription::Enumerator pSubscription (pT);
		pSubscription.isntNil ();
		pSubscription.next ()
	    ) {
		// See if we need to invoke the delegate via a scheduler...
		if (pSubscription->defersTo (iScheduler)) {
		    RPI rpI (new Message (iScheduler,this,pSubscription,p1,p2,p3,p4,p5,p6,pMgr));
		    iScheduler.schedule (rpI);
		} else {
		    (static_cast<T*>(pSubscription)->*m_pMember)(pMessage,p1,p2,p3,p4,p5,p6);
		}
	    }
        }

    //  State
    protected:
        /**
         * @copydoc VInterfaceMember_0::m_pMember
         */
        PMember const m_pMember;
    };


    /**
     * Represents an interface member with seven prameters.
     */
    template <
        class T,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7
    > class VInterfaceMember_7 : public VInterfaceMember_T<T> {
    //  Family
    public:
        typedef VInterfaceMember_7<T,P1,P2,P3,P4,P5,P6,P7> ThisClass;
        typedef VInterfaceMember_T<T> BaseClass;

    //  Context
    public:
        typedef VMessageStarter MS;
        typedef VMessage::Reference RPI;
        typedef VMessageManager MGR;


    //  Applicable
    public:
        typedef ThisClass Applicable;

    //  Arguments
    public:
        /**
         * @copydoc VInterfaceMember_0::Arguments
         */
        typedef VTuple_8<T*,P1,P2,P3,P4,P5,P6,P7> Arguments;

    //  Initializer
    public:
        /**
         * @copydoc VInterfaceMember_0::PMember
         */
        typedef void (T::*PMember)(VMessage*,P1,P2,P3,P4,P5,P6,P7);
        struct Initializer : public VInterfaceMember::Initializer {
            Initializer (char const *pName, unsigned int xMember, PMember pMember)
                : VInterfaceMember::Initializer (pName, xMember), m_pMember (pMember)
            {
            }
            PMember const m_pMember;
        };

    //  Message
    public:
        /**
         * VMessage subclass for representing messages whose target accepts seven arguments.
         */
        class Message : public VMessage {
        public:
            DECLARE_CONCRETE_RTTLITE (Message, VMessage);

        //  PayloadHolder
        public:
            class PayloadHolder {
                DECLARE_FAMILY_MEMBERS (PayloadHolder,void);

            //  Payload
            public:
                class Payload : public VReferenceable {
                    DECLARE_CONCRETE_RTTLITE (Payload, VReferenceable);

                //  Construction
                public:
                    Payload (
                        Applicable const *pApplicable,T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7
                    ) : m_pApplicable (pApplicable), m_iArguments (pT,p1,p2,p3,p4,p5,p6,p7) {
                    }
                    Payload (Applicable const *pApplicable) : m_pApplicable (pApplicable) {
                    }

                //  Destruction
                protected:
                    ~Payload () {
                    }

                //  Access
                public:
                    Applicable const *applicable () const {
                        return m_pApplicable;
                    }
                    Arguments const &arguments () const {
                        return m_iArguments;
                    }
                    Arguments& arguments () {
                        return m_iArguments;
                    }
                    T *recipient () const {
                        return m_iArguments.f1 ();
                    }

                //  Query
                public:
                    bool defersTo (VMessageScheduler &rScheduler) const {
                        return recipient ()->defersTo (rScheduler);
                    }

		//  Use
		public:
		    void evaluateWith (Message* pMessage, MGR* pMgr) {
			VMessageScheduler iScheduler;
			if (defersTo (iScheduler)) {
			    RPI rpI (new Message (iScheduler,this,pMgr));
			    iScheduler.schedule (rpI);
			} else {
			    (*m_pApplicable)(
				m_iArguments.f1 (), m_iArguments.f2 (), m_iArguments.f3 (), m_iArguments.f4 (),
				m_iArguments.f5 (), m_iArguments.f6 (), m_iArguments.f7 (), m_iArguments.f8 (), pMgr, pMessage
			    );
			}
		    }

                //  State
                private:
                    Applicable const *const m_pApplicable;
                    Arguments m_iArguments;
                };

            //  Construction
            public:
                PayloadHolder (
                    Applicable const *pApplicable, T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7
                ) : m_pPayload (new Payload (pApplicable,pT,p1,p2,p3,p4,p5,p6,p7)) {
                }
                PayloadHolder (Applicable const *pApplicable) : m_pPayload (new Payload (pApplicable)) {
                }
                PayloadHolder (Payload *pPayload) : m_pPayload (pPayload) {
                }

            //  Destruction
            public:
                ~PayloadHolder () {
                }

            //  Access
            public:
                Applicable const *applicable () const {
                    return m_pPayload->applicable ();
                }
                Arguments& arguments () const {
                    return m_pPayload->arguments ();
                }
                Payload *payload () const {
                    return m_pPayload;
                }
                T *recipient () const {
                    return m_pPayload->recipient ();
                }

	    //  Use
	    public:
		void evaluateWith (Message* pMessage, MGR* pMgr) const {
		    m_pPayload->evaluateWith (pMessage, pMgr);
		}

            //  State
            private:
                typename Payload::Reference const m_pPayload;
            };
            typedef typename PayloadHolder::Payload Payload;

        //  Construction
        public:
            Message (
                MS *pS, Applicable const *pApplicable,T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pApplicable,pT,p1,p2,p3,p4,p5,p6,p7) {
            }
            Message (
                MS *pS, Payload *pPayload, MGR *pMgr=0
            ) : BaseClass (pS, pMgr), m_iPayload (pPayload) {
            }
            Message (
                Applicable const *pApplicable, unsigned int xMessage, MGR *pMgr=0
            ) : BaseClass (xMessage, pMgr), m_iPayload (pApplicable) {
            }

        //  Destruction
        protected:
            ~Message () {
            }

        //  Access
        public:
            VInterfaceMember const *applicable_() const {
                return applicable ();
            }
            VTuple const &arguments_() const {
                return arguments ();
            }
            VTuple &arguments_() {
                return arguments ();
            }

            Applicable const *applicable () const {
                return m_iPayload.applicable ();
            }
            Arguments const &arguments () const {
                return m_iPayload.arguments ();
            }
            Arguments &arguments () {
                return m_iPayload.arguments ();
            }
            IVUnknown *recipient () const {
                return m_iPayload.recipient ();
            }

        //  Evaluation
        private:
            void evaluate_() {
                VMessageManager::Reference pManager;
                subManager (pManager);
                m_iPayload.evaluateWith (this, pManager);
            }

        //  State
        protected:
            PayloadHolder const m_iPayload;
        };
        typedef typename Message::Payload MessagePayload;

    //  Construction
    public:
        VInterfaceMember_7 (
            char const *pName, unsigned int xMember, PMember pMember
        ) : BaseClass (pName, xMember), m_pMember (pMember) {
        }
        VInterfaceMember_7 (
            Initializer const &rInitializer
        ) : BaseClass (rInitializer), m_pMember (rInitializer.m_pMember) {
        }

    //  Destruction
    public:
        ~VInterfaceMember_7 () {
        }

    //  Access
    public:
        PMember member () const {
            return m_pMember;
        }

    //  Query
    public:
        bool doesntName (PMember pMember) const {
            return m_pMember != pMember;
        }
        bool names (PMember pMember) const {
            return m_pMember == pMember;
        }

    //  Signature
    public:
        static INLINE_TMEMBER VTypeInfo::ParameterSignature const &parameterSignature ();

        VTypeInfo::ParameterSignature const &parameterSignature_() const {
            return parameterSignature ();
        }

    //  Message Creation
    public:
        VMessage *newMessage (unsigned int xMessage) const {
            return new Message (this, xMessage);
        }

    //  Message
    public:
        /**
         * @copydoc VInterfaceMember_0::operator()(T,MGR)
         */
        void operator()(T *pT,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7, MGR *pMgr=0,Message* pMessage=0) const {
            VMessageScheduler iScheduler;
	    // Enumerate the 'delegate's of this interface...
	    for (
		typename T::Subscription::Enumerator pSubscription (pT);
		pSubscription.isntNil ();
		pSubscription.next ()
	    ) {
		// See if we need to invoke the delegate via a scheduler...
		if (pSubscription->defersTo (iScheduler)) {
		    RPI rpI (new Message (iScheduler,this,pSubscription,p1,p2,p3,p4,p5,p6,p7,pMgr));
		    iScheduler.schedule (rpI);
		} else {
		    (static_cast<T*>(pSubscription)->*m_pMember)(pMessage,p1,p2,p3,p4,p5,p6,p7);
		}
	    }
        }

    //  State
    protected:
        /**
         * @copydoc VInterfaceMember_0::m_pMember
         */
        PMember const m_pMember;
    };
} //  namespace Vca


/*********************
 *****  Members  *****
 *********************/

/*************************************
 *----  Vca::VInterfaceMember_0  ----*
 *************************************/

template <
    class T
> VTypeInfo::ParameterSignature const &Vca::VInterfaceMember_0<T>::parameterSignature () {
    static VTypeInfo::ParameterSignature iSignature (0);
    return iSignature;
}

/*************************************
 *----  Vca::VInterfaceMember_1  ----*
 *************************************/

template <
    class T,class P1
> VTypeInfo::ParameterSignature const &Vca::VInterfaceMember_1<T,P1>::parameterSignature () {
    static VTypeInfo::ParameterSignature iSignature (1);
    static bool bSignatureUninitialized = true;
    if (bSignatureUninitialized) {
	iSignature[0].setTo (VTypeInfoHolder<P1>::typeInfo ());
	bSignatureUninitialized = false;
    }
    return iSignature;
}

/*************************************
 *----  Vca::VInterfaceMember_2  ----*
 *************************************/

template <
    class T,class P1,class P2
> VTypeInfo::ParameterSignature const &Vca::VInterfaceMember_2<T,P1,P2>::parameterSignature () {
    static VTypeInfo::ParameterSignature iSignature (2);
    static bool bSignatureUninitialized = true;
    if (bSignatureUninitialized) {
	iSignature[0].setTo (VTypeInfoHolder<P1>::typeInfo ());
	iSignature[1].setTo (VTypeInfoHolder<P2>::typeInfo ());
	bSignatureUninitialized = false;
    }
    return iSignature;
}


/*************************************
 *----  Vca::VInterfaceMember_3  ----*
 *************************************/

template <
    class T,class P1,class P2,class P3
> VTypeInfo::ParameterSignature const &Vca::VInterfaceMember_3<T,P1,P2,P3>::parameterSignature () {
    static VTypeInfo::ParameterSignature iSignature (3);
    static bool bSignatureUninitialized = true;
    if (bSignatureUninitialized) {
	iSignature[0].setTo (VTypeInfoHolder<P1>::typeInfo ());
	iSignature[1].setTo (VTypeInfoHolder<P2>::typeInfo ());
	iSignature[2].setTo (VTypeInfoHolder<P3>::typeInfo ());
	bSignatureUninitialized = false;
    }
    return iSignature;
}

/*************************************
 *----  Vca::VInterfaceMember_4  ----*
 *************************************/

template <
    class T,class P1,class P2,class P3,class P4
> VTypeInfo::ParameterSignature const &Vca::VInterfaceMember_4<T,P1,P2,P3,P4>::parameterSignature () {
    static VTypeInfo::ParameterSignature iSignature (4);
    static bool bSignatureUninitialized = true;
    if (bSignatureUninitialized) {
	iSignature[0].setTo (VTypeInfoHolder<P1>::typeInfo ());
	iSignature[1].setTo (VTypeInfoHolder<P2>::typeInfo ());
	iSignature[2].setTo (VTypeInfoHolder<P3>::typeInfo ());
	iSignature[3].setTo (VTypeInfoHolder<P4>::typeInfo ());
	bSignatureUninitialized = false;
    }
    return iSignature;
}

/*************************************
 *----  Vca::VInterfaceMember_5  ----*
 *************************************/

template <
    class T,class P1,class P2,class P3,class P4,class P5
> VTypeInfo::ParameterSignature const &Vca::VInterfaceMember_5<T,P1,P2,P3,P4,P5>::parameterSignature () {
    static VTypeInfo::ParameterSignature iSignature (5);
    static bool bSignatureUninitialized = true;
    if (bSignatureUninitialized) {
	iSignature[0].setTo (VTypeInfoHolder<P1>::typeInfo ());
	iSignature[1].setTo (VTypeInfoHolder<P2>::typeInfo ());
	iSignature[2].setTo (VTypeInfoHolder<P3>::typeInfo ());
	iSignature[3].setTo (VTypeInfoHolder<P4>::typeInfo ());
	iSignature[4].setTo (VTypeInfoHolder<P5>::typeInfo ());
	bSignatureUninitialized = false;
    }
    return iSignature;
}


/*************************************
 *----  Vca::VInterfaceMember_6  ----*
 *************************************/

template <
    class T,class P1,class P2,class P3,class P4,class P5,class P6
> VTypeInfo::ParameterSignature const &Vca::VInterfaceMember_6<T,P1,P2,P3,P4,P5,P6>::parameterSignature () {
    static VTypeInfo::ParameterSignature iSignature (6);
    static bool bSignatureUninitialized = true;
    if (bSignatureUninitialized) {
	iSignature[0].setTo (VTypeInfoHolder<P1>::typeInfo ());
	iSignature[1].setTo (VTypeInfoHolder<P2>::typeInfo ());
	iSignature[2].setTo (VTypeInfoHolder<P3>::typeInfo ());
	iSignature[3].setTo (VTypeInfoHolder<P4>::typeInfo ());
	iSignature[4].setTo (VTypeInfoHolder<P5>::typeInfo ());
	iSignature[5].setTo (VTypeInfoHolder<P6>::typeInfo ());
	bSignatureUninitialized = false;
    }
    return iSignature;
}

/*************************************
 *----  Vca::VInterfaceMember_7  ----*
 *************************************/

template <
    class T,class P1,class P2,class P3,class P4,class P5,class P6,class P7
> VTypeInfo::ParameterSignature const &Vca::VInterfaceMember_7<T,P1,P2,P3,P4,P5,P6,P7>::parameterSignature () {
    static VTypeInfo::ParameterSignature iSignature (7);
    static bool bSignatureUninitialized = true;
    if (bSignatureUninitialized) {
	iSignature[0].setTo (VTypeInfoHolder<P1>::typeInfo ());
	iSignature[1].setTo (VTypeInfoHolder<P2>::typeInfo ());
	iSignature[2].setTo (VTypeInfoHolder<P3>::typeInfo ());
	iSignature[3].setTo (VTypeInfoHolder<P4>::typeInfo ());
	iSignature[4].setTo (VTypeInfoHolder<P5>::typeInfo ());
	iSignature[5].setTo (VTypeInfoHolder<P6>::typeInfo ());
	iSignature[6].setTo (VTypeInfoHolder<P7>::typeInfo ());
	bSignatureUninitialized = false;
    }
    return iSignature;
}

#endif
