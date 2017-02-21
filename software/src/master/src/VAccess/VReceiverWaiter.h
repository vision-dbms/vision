#ifndef VReceiverWaiter_Interface
#define VReceiverWaiter_Interface

#include "Vca_VRolePlayer.h"
#include "V_VWaiter.h"
#include "Vca_IDataSource.h"

/**
 * IVReceiver implementation with wait functionality.
 *
 * @tparam Datum must have ::Reference defined.
 */
template <class Datum, class ReferenceableDatum = Datum> class VReceiverWaiter : public Vca::VRolePlayer, public V::VWaiter {
    typedef VReceiverWaiter<Datum, ReferenceableDatum> this_t;
    DECLARE_CONCRETE_RTTLITE (this_t, Vca::VRolePlayer);

public:
    typedef Datum DataType;
    typedef typename ReferenceableDatum::Reference DataReference;
    typedef IVReceiver<typename Datum> ReceiverInterface;

// Construction
public:
    /** Default constructor initializes roles. */
    VReceiverWaiter () : m_pReceiverRole (this) {}

// Destruction
protected:
    /** Empty default destructor. */
    ~VReceiverWaiter () {}

/// @name IVReceiver Implementation
//@{
public:
    using BaseClass::getRole;
private:
    Vca::VRole<ThisClass,ReceiverInterface> m_pReceiverRole;
public:
    void getRole (typename ReceiverInterface::Reference &rpRole) {
        m_pReceiverRole.getRole(rpRole);
    }

public:
    /**
     * Implements IVReceiver::OnData().
     */
    void OnData (ReceiverInterface *pRole, DataType iDatum) {
        // TODO: Specialize to prevent copy constructor usage, or perhaps use VTypeInfo in a clever way?
        m_pDatum.setTo (new ReferenceableDatum(iDatum));
        m_sErrorMessage.clear();
        signal ();
    }

    /** Implements IClient::OnError() by signalling the wait lock. */
    void OnError_ (Vca::IError *pError, const VString &rMessage) {
        m_sErrorMessage = rMessage;
        signal (false);
    }
//@}

/// @name Access
//@{
    /**
     * Retrieves cached value.
     *
     * @param pEvaluator the reference by which the cached value should be returned.
     */
    void getDatum (DataReference &pDatum) {
        pDatum = m_pDatum;
    }

    /**
     * Retrieves cached error message.
     *
     * @param rMessage the reference by which the cached error message should be returned.
     */
    void getErrorMessage (VString &rMessage) {
        rMessage = m_sErrorMessage;
    }
//@}

// State
private:
    /** Cached return value. */
    DataReference m_pDatum;
    /** Cached error message. */
    VString m_sErrorMessage;
};

#endif