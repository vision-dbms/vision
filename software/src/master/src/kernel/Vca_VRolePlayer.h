#ifndef Vca_VRolePlayer_Interface
#define Vca_VRolePlayer_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "IVUnknown.h"
#include "Vca_IRoleProvider2.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VCohort.h"
#include "Vca_VRoleHolder.h"

#include "Vca_IGetter_Ex2.h"
#include "Vca_IPeek.h"

/*************************
 *****  Definitions  *****
 *************************/

#define DECLARE_ROLEPLAYER_CLASSINFO()\
public:\
    virtual Vca::VClassInfoHolder &classInfo () const {\
	return ClassInfo ();\
    }\
    static Vca::VClassInfoHolder &ClassInfo ()

namespace Vca {
    template <class Interface_T> class VGoferInterface;

    class VClassInfoHolder;

    /**
     * Base class for any object intended to support Vca roles/interfaces.
     *
     * Most of the routines are for internal Vca use only. Of interest to subclass developers are:
     *  - aggregate() called to add the interfaces implemented by other VRolePlayer instances to the list of interfaces supported by this VRolePlayer; see @ref InterfaceAggregation "Interface Aggregation".
     *  - OnEnd_() overridden to provide custom Vca::IClient::OnEnd() handling.
     *  - OnError_() overridden to provide custom Vca::IClient::OnError() handling.
     */
    class Vca_API VRolePlayer : public virtual VReferenceable {
        DECLARE_ABSTRACT_RTTLITE (VRolePlayer, VReferenceable);
	DECLARE_ROLEPLAYER_CLASSINFO ();

        friend class VRoleHolder;

    //  Aliases
    public:
	typedef IVReceiver<bool> IStatusSink;

    //  RoleProvider
    public:
        class Vca_API RoleProvider : public VReferenceable {
            DECLARE_ABSTRACT_RTTLITE (RoleProvider, VReferenceable);

        //  Query
        public:
            class Query;
	    template <class StatusSink_T> class Query_;

        //  Construction
        protected:
            RoleProvider (VRolePlayer *pAggregator);

        //  Destruction
        protected:
            ~RoleProvider ();

        //  Access
        public:
            RoleProvider *successor () const {
                return m_pSuccessor;
            }

        //  Use
        public:
            virtual bool accepted (Query const *pQuery) const = 0;

        //  State
        private:
            Reference m_pSuccessor;
        };
	friend class RoleProvider::Query;

    //  RoleProvider Derived Classes
    public:
        class InterfaceBasedRoleProvider;
        class RolePlayerBasedRoleProvider;

    //  Construction
    protected:
        /**
         * Used by subclasses to construct a VRolePlayer as a member of a given VCohort.
         *
         * @param pCohort the VCohort to which the constructed VRolePlayer should belong.
         */
        VRolePlayer (VCohort *pCohort);
        /**
         * Used by subclasses to construct a VRolePlayer.
         * Constructs a VRolePlayer belonging to the thread's VCohort (via VcaThreadState).
         */
        VRolePlayer ();

    //  Destruction
    protected:
        ~VRolePlayer ();

    //  Access
    public:
	VString classBaseNames () const;
	VString classPropertyNames () const;
	VString classSummary () const;
	count_t cohortRequestCount () const;
	count_t cohortTimerCount () const;
	count_t cohortUseCount () const;
    private:
	unsigned int referenceCountShim () const;
	VString rttNameShim () const;

    //  Aggregation
    private:
        /**
         * Adds a given RoleProvider to the list of RoleProviders that this VRolePlayer represents.
         *
         * @param pProvider the RoleProvider to add.
         */
        void appendNew (RoleProvider *pProvider);
    protected:
        /**
         * Aggregates interfaces from another VRolePlayer.
         *
         * @param pRoleProvider the VRolePlayer whose interfaces should be aggregated.
         *
         * @see VRolePlayerAggregation
         */
        void aggregate (VRolePlayer* pRoleProvider);

        /**
         * Aggregates interfaces obtained via VGofer.
         *
         * @param pRoleProvider the VGofer used to retrieve the interface(s) to aggregate.
         */
        void aggregate (VGoferInterface<IRoleProvider>* pRoleProvider);

        /**
         * @copydoc aggregate(VGoferInterface<IRoleProvider>*)
         */
        void aggregate (VGoferInterface<IVUnknown>* pRoleProvider);

        /**
         * Aggregates interfaces using an IRoleProvider.
         *
         * @param pRoleProvider the IRoleProvider to use to aggregate interfaces.
         */
        void aggregate (IRoleProvider* pRoleProvider);

        /**
         * @copydoc aggregate(IRoleProvider*)
         */
        void aggregate (IVUnknown* pRoleProvider);
    public:
        /**
         * Returns this VRolePlayer's RoleProvider list.
         *
         * @return this VRolePlayer's RoleProvider list.
         */
        RoleProvider *roleProviders () const {
            return m_pRoleProviders;
        }

    //  Cohort
    private:
        VCohort *cohorT () const {
            return m_pCohort;
        }
    public:
        static VCohort *CohortOf (VRolePlayer const *pRolePlayer);
        /**
         * Used to query for the cohort to which this VRolePlayer belongs.
         *
         * @return the cohort to which this VRolePlayer belongs, or zero if uninitialized.
         */
        VCohort *cohort () const {
            return CohortOf (this);
        }
        /**
         * Used to determine if this VRolePlayer belongs to a given VCohort.
         *
         * @param pCohort the VCohort against which this VRolePlayer's VCohort should be compared.
         * @return true if pCohort is the VCohort to which this VRolePlayer belongs, false otherwise.
         */
        bool memberOf (VCohort const *pCohort) const {
            return pCohort == m_pCohort;
        }

    protected:
        bool joinCohortAs (VCohortIndex const &rIndex) const {
            return m_pCohort->attach (rIndex, this);
        }
        bool exitCohortAs (VCohortIndex const &rIndex) const {
            return m_pCohort->detach (rIndex, this);
        }

    //  Communication
    public:
        /**
         * Service failure mechanism by which servers can return a non-specific error to a client.
         *
         * @param pClient the client to which a non-specific error should be communicated.
         */
        void communicateFailureTo (IClient *pClient);

    //  Export
    public:
	VRoleHolder::count_t exportCount () const;
	VRoleHolder::count_t exportedInterfaceCount () const;
	VRoleHolder::count_t instantiatedInterfaceCount () const;
	VRoleHolder::count_t referencedInterfaceCount () const;
	VRoleHolder::count_t roleCount () const;
	bool hasInstantiatedInterfaces () const;
	bool hasExportedInterfaces () const;

    /**
     * @name IClient Implementation
     * Though VRolePlayer instances do not by default support the IClient interface, they define a basic implementation for the members of IClient.
     * @todo Create VClientBase base subclass that actually provides the IClient interface, including also some canned monitoring functionality.
     */
    //@{
    //  OnEnd
    public:
        /** Implements Vca::IClient::OnEnd() by delegating to OnEnd_(). */
        virtual void OnEnd (IClient *pRole);
    private:
        /**
         * Provided so that subclasses can implement their own handling of Vca::IClient::OnEnd().
         *
         * Called by the VRolePlayer::OnEnd() default definition. Default implementation delegates to OnError(), meaning that all subclasses that receive an OnEnd() event and do not have an OnEnd_() implementation will instead receive an OnError() invocation.
         */
        virtual void OnEnd_();

    //  OnError
    public:
        /** Implements Vca::IClient::OnError() by delegating to OnError_(). */
        virtual void OnError (IClient *pRole, IError *pError, VString const &rMessage);
    private:
        /**
         * Provided so that subclasses can implement their own handling of Vca::IClient::OnError().
         *
         * Called by the VRolePlayer::OnError() default definition. Default implementation uses display() to report the error via standard output.
         */
        virtual void OnError_(IError *pIError, VString const &rMessage);
    //@}

    //  Role List
    private:
        /**
         * Role list.
         *
         * @warning This class' role list member MUST preceed the declarations of all roles so that it is initialized before any roles are added to the list.
         */
        VRoleHolder::Pointer m_pRoleHolders;

    //  IVUnknown Role...
    private:
        /**
         * IVUnknown role.
         * This is the most basic role that all VRolePlayer objects implement.
         */
        VRole<ThisClass,IVUnknown> m_pIVUnknown;
    public:
        /**
         * Retrieves the IVUnknown role for this VRolePlayer.
         *
         * @param[out] rpRole the reference that will be used to return the IVUnknown role for this VRolePlayer.
         */
        void getRole (IVUnknown::Reference &rpRole) {
            m_pIVUnknown.getRole (rpRole);
        }
    //  ... Methods:
    public:
        /**
         * Implements Vca::IVUnknown::QueryInterface().
         * While it is technically possible to override this method in subclasses, it is not recommended.
         */
        virtual void QueryInterface (
            IVUnknown *pRole, VTypeInfo *pInterfaceType, IObjectSink *pInterfaceSink
        );

    /// @name IGetter Implementation
    //@{
    private:
        /** The IGetter role for this VServerApplication. */
        VRole<ThisClass,IGetter_Ex2> m_pIGetter;
    public:
        /**
         * Retrieves the IGetter role for this VServerApplication::ControlRequest.
         *
         * @param[out] rpRole the reference that will be used to return the IGetter for this VServerApplication::ControlRequest.
         */
        void getRole (IGetter_Ex2::Reference &rpRole) {
            m_pIGetter.getRole (rpRole);
        }

    //  IGetter_Ex2 Methods
    public:
        /**
         * Implements GetU64Value().
         *
         * Implemented in a similar way as GetStringValue().
         */
        void GetU64Value (IGetter_Ex2 *pRole, IVReceiver<U64> *pResult, VString const &rKey);
        /**
         * @copybrief GetStringValue()
         *
         * Implemented in a similar way as GetStringValue().
         */
        void GetF32Value (IGetter_Ex2 *pRole, IVReceiver<F32> *pResult, VString const &rKey);
        /**
         * Implements IGetter::GetF64Value().
         *
         * Implemented in a similar way as GetStringValue().
         */
        void GetF64Value (IGetter_Ex2 *pRole, IVReceiver<F64> *pResult, VString const &rKey);

    //  IGetter Methods
    public:
        /**
         * Implements IGetter::GetStringValue().
         *
         * Implemented by calling GetStringValue_(), a virtual method intended to be defined at each VServerApplication subclass.
         */
        void GetStringValue (IGetter *pRole, IVReceiver<VString const &> *pResult, VString const &rKey);

        /**
         * Implements IGetter::GetU32Value().
         *
         * Implemented in a similar way as GetStringValue().
         */
        void GetU32Value (IGetter *pRole, IVReceiver<U32> *pResult, VString const &rKey);

    //  IGetter virtuals.
    protected:
        /**
         * Allows subclasses to implement GetStringValue() as necessary.
         *
         * Subclasses should always fall back to calling their parent class' implementation of this method in the event that the provided key is not understood at that level of the heirarchy.
         */
        virtual bool GetStringValue_(IVReceiver<VString const &>*, VString const &);

        /**
         * Allows subclasses to implement GetU32Value() as necessary.
         *
         * @copydetail GetStringValue_()
         */
        virtual bool GetU32Value_(IVReceiver<U32>*, VString const &);

        /**
         * Allows subclasses to implement GetU64Value() as necessary.
         *
         * @copydetail GetStringValue_()
         */
        virtual bool GetU64Value_(IVReceiver<U64>*, VString const &);

        /**
         * Allows subclasses to implement GetF32Value() as necessary.
         *
         * @copydetail GetStringValue_()
         */
        virtual bool GetF32Value_(IVReceiver<F32>*, VString const &);

        /**
         * Allows subclasses to implement GetF64Value() as necessary.
         *
         * @copydetail GetStringValue_()
         */
        virtual bool GetF64Value_(IVReceiver<F64>*, VString const &);
    private:
	template <typename Sink_T> bool GetValue (Sink_T *pResultSink, VString const &rKey);
    //@}

    /// @name IPeek Implementation
    //@{
    private:
        /** The IPeek role for this VServerApplication. */
        VRole<ThisClass,IPeek> m_pIPeek;
    public:
        /**
         * Retrieves the IPeek role for this VServerApplication::ControlRequest.
         *
         * @param[out] rpRole the reference that will be used to return the IPeek for this VServerApplication::ControlRequest.
         */
        void getRole (IPeek::Reference &rpRole) {
            m_pIPeek.getRole (rpRole);
        }

    //  IPeek Methods
    public:
        /**
         * Implements IPeek::GetValue().
         */
	void GetValue (IPeek *pRole, VString const &rPropertyName, IClient *pDataSink, IVReceiver<IRequest*> *pTicketSink);

    /// @name IRoleProvider Implementation
    //@{
    private:
        VRole<ThisClass,IRoleProvider2> m_pIRoleProvider;
    public:
        void getRole (IRoleProvider2::Reference &rpRole) {
            m_pIRoleProvider.getRole (rpRole);
        }
        void getRole (IRoleProvider::Reference &rpRole) {
	    IRoleProvider2::Reference pRole;
	    getRole (pRole);
	    rpRole.setTo (pRole);
        }
    //  ... Methods:
        virtual void QueryRole2 (        //  ... you could override this, but probably shouldn't.
            IRoleProvider2 *pRole, IStatusSink *pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator
        );
        virtual void QueryRole (        //  ... you could override this, but probably shouldn't.
            IRoleProvider *pRole, ITrigger *pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator
        );
    //@}

    //  QueryInterface / QueryRole Implementation
    public:
        bool supplyInterface (IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType) {
            return supplyInterface (static_cast<ITrigger*>(0), pInterfaceSink, pInterfaceType, 0, false);
        }
        bool supplyInterface (
            ITrigger *pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator, bool bASubQuery
        );
        bool supplyInterface (
            IStatusSink* pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator, bool bASubQuery
        );
    private:
	template <class status_sink_t> bool supplyInterfaceImplementation (
	    status_sink_t *pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator, bool bSubquery
	) {
	    if (!pStatusSink && !pInterfaceSink)
		return false;

	    IVUnknown::Reference pTheAggregator (pAggregator);
	    if (pTheAggregator.isNil ())
		getRole (pTheAggregator);
	    RoleProvider::Query_<status_sink_t> const iQuery (pStatusSink, pInterfaceSink, pInterfaceType, pTheAggregator, bSubquery);
	    return iQuery.getRoleFrom (this);
	}

        bool supplyInterface (
            IVUnknown::Reference &rpInterface, VTypeInfo *pInterfaceType, IVUnknown *pAggregator
        );

    /**
     * @name SSID
     * Site specific unique identity of this VRolePlayer. Uniquely identifies a specific VRolePlayer
     * for the lifetime of this process, even after the destruction of the VRolePlayer.
     */
    public:
	ssid_t ssid () const {
	    return m_xSSID;
	}

    /**
     * @name Wait Mechanisms
     * Machinery for synchronous waiting on particular condition(s) defined by a subclass.
     * @warning Synchronous waits are heavily discouraged because they introduce significant potential for deadlock.
     */
    //@{
    private:
        /**
         * Used to determine if the wait condition has been satisfied; must be implemented by subclasses using wait functionality.
         *
         * The default implementation provided here always returns true, yielding wait functionality useless.
         *
         * @return true if the wait condition has been satisfied, false otherwise.
         */
        virtual bool waitConditionSatisfied () const;

    public:
        /**
         * Waits, up to a given timeout, for a condition to be satisfied.
         * The condition waited upon must be supplied by subclasses by way of waitConditionSatisfied().
         *
         * @param[in] sMillisecondTimeout maximum number of milliseconds to wait before returning false.
         * @return true if the condition was satisfied before the timeout elapsed, false otherwise.
         */
        bool waitForCondition (size_t sMillisecondTimeout) const;
    //@}

    /// @name State
    //@{
	V::VLogger &defaultLogger () const {
	    return m_pCohort->defaultLogger ();
	}
    //@}

    /// @name State
    //@{
    private:
        /**
         * The VCohort to which this VRolePlayer belongs.
         */
        VCohort::Reference const m_pCohort;

        /**
         * The VCohort to which this VRolePlayer belongs.
         */
	ssid_t		  const	m_xSSID;

        /**
         * List of RoleProviders (for use with aggregation).
         *
         * @see InterfaceAggregation
         */
        RoleProvider::Reference m_pRoleProviders;
    //@}
    };
}


#endif
