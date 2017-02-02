#ifndef Vca_VDeviceFactory_Interface
#define Vca_VDeviceFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

class VkSocketAddress;
class VkStatus;

class VString;


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VBSConsumer;
    class VBSProducer;
    class VConnection;
    class VDevice;
    class VDeviceManager;
    class VListener;
    class VProcess;

    namespace OS {
	class DeviceManager;
    }

    /**
     * Manages creation for all streaming devices based on operating system components.
     *
     * @see VDeviceImplementation
     * @see VDevice
     * @see VStreamSource
     * @see VStreamSink
     * @see VListener
     * @see VProcess
     * @see VConnection
     */
    class Vca_API VDeviceFactory : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VDeviceFactory, VRolePlayer);

    //  Aliases
    public:
	typedef OS::DeviceManager DeviceManager;

    //  Construction
    protected:
        /** Constructor takes cohort to which this factory should belong. */
	VDeviceFactory (VCohort *pCohort);

    //  Destruction
    protected:
        /** Empty destructor restricts scope. */
	~VDeviceFactory ();

    //  Cohort Index
    private:
	static VCohortIndex const &CohortIndex ();

    //  Cohort Instance
    public:
	static bool Supply (Reference &rpInstance);

    //  Suppliers
    private:
	//  ... device manager
	bool supply (VReference<DeviceManager>&rpDeviceManager);

    public:
	/**
         * Static member that starts a new process using the default device factory supplied by Vca.
         *
         * @param[out] rStatus the creation status.
         * @param[out] rpProcess the VProcess reference by which the new VProcess will be returned.
         * @param ppStdToPeer the input pipe (as in stdin) for the newly created process. If null, the newly created process' stdin is connected to the null device (as in /dev/null); otherwise, a new VDevice is created and returned via the provided reference. Writes to said VDevice are sent as input to the newly created process' stdin.
         * @param ppStdToHere the output pipe (as in stdout) for the newly created process. If null, the newly created process' stdout is connected to the null device (as in /dev/null); otherwise, a new VDevice is created and returned via the provided reference. Reads from said VDevice were sent as output from the newly created process' stdout.
         * @param ppErrToHere the error output pipe (as in stderr) for the newly created process. If null, the newly created process' stderr is connected to the null device (as in /dev/null); otherwise, a new VDevice is created and returned via the provided reference. Reads from said VDevice were sent as error output from the newly created process' stderr. If equivalent to ppStdToHere, both stderr and stdout of the newly created process are connected to the same VDevice.
         * @param pCommand the command string to run in order to launch the new process. May contain OS-dependent prefix tags surrounded by curly braces.
         * @return false on failure, true otherwise.
         *
         * @todo detail OS-dependent prefix tags.
         */
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VProcess>&	rpProcess,
	    VReference<VDevice>*	ppStdToPeer, // null -> null device
	    VReference<VDevice>*	ppStdToHere, // null -> null device
	    VReference<VDevice>*	ppErrToHere, // null -> null device, ==ppStdToHere -> stdout device
	    char const*			pCommand
	);
        /**
         * Starts a new process using this device factory.
         *
         * @copydetail Supply(VkStatus&, VReference<VProcess>&, VReference<VDevice>*, VReference<VDevice>*, VReference<VDevice>*, char const *)
         * @todo Check if the above copydetail worked.
         */
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VProcess>&	rpProcess,
	    VReference<VDevice>*	ppStdToPeer, // null -> null device
	    VReference<VDevice>*	ppStdToHere, // null -> null device
	    VReference<VDevice>*	ppErrToHere, // null -> null device, ==ppStdToHere -> stdout device
	    char const*			pCommand
	);

        /**
         * Starts a new process using this device factory. Requires that a legitimate device reference is provided for each stdio pipe.
         *
         * @see Supply(VkStatus&, VReference<VProcess>&, VReference<VDevice>*, VReference<VDevice>*, VReference<VDevice>*, char const *)
         * @todo Check if the above see worked.
         */
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VProcess>&	rpProcess,
	    VReference<VDevice>&	rpStdToPeer,
	    VReference<VDevice>&	rpStdToHere,
	    VReference<VDevice>&	rpErrToHere, // & == &rpStdToHere -> stdout device
	    char const*			pCommand
	);

        /**
         * @copydoc Supply(VkStatus&, VReference<VProcess>&, VReference<VDevice>&, VReference<VDevice>&, VReference<VDevice>&, char const *)
         */
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VProcess>&	rpProcess,
	    VReference<VDevice>&	rpStdToPeer,
	    VReference<VDevice>&	rpStdToHere,
	    VReference<VDevice>&	rpErrToHere, // & == &rppStdToHere -> stdout device
	    char const*			pCommand
	);

        /**
         * Used to retrieve the stdio pipes for the current process.
         *
         * @param[out] rpStdToHere the reference by which the standard input pipe for this process is returned.
         * @param[out] rpStdToPeer Here the reference by which the standard output pipe for this process is returned.
         * @param[out] rpErrToPeer the reference by which the standard error pipe for this process is returned.
         * @return false on failure, true otherwise.
         */
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VDevice>&	rpStdToHere,	//  stdin
	    VReference<VDevice>&	rpStdToPeer,	//  stdout
	    VReference<VDevice>&	rpErrToPeer	//  stderr
	);

        /**
         * Used to retrieve stdio pipes for the current process.
         *
         * @param[out] rpStdToHere the reference by which the standard input pipe for this process is returned.
         * @param[out] rpStdToPeer the reference by which the standard output pipe for this process is returned.
         * @return false on failure, true otherwise.
         */
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VDevice>&	rpStdToHere,	//  stdin
	    VReference<VDevice>&	rpStdToPeer	//  stderr
	);

        /**
         * Used to retrieve the stdin pipe for the current process.
         *
         * @param[out] rpDevice the reference by which the standard input pipe for this process is returned.
         * @return false on failure, true otherwise.
         */
	bool supplyStdToHere (
	    VkStatus &rStatus, VReference<VDevice>&rpDevice // stdin
	);

        /**
         * Used to retrieve the stdout pipe for the current process.
         *
         * @param[out] rpDevice the reference by which the standard output pipe for this process is returned.
         * @return false on failure, true otherwise.
         */
	bool supplyStdToPeer (
	    VkStatus &rStatus, VReference<VDevice>&rpDevice // stdout
	);

        /**
         * Used to retrieve the stderr pipe for the current process.
         *
         * @param[out] rpDevice the reference by which the standard error pipe for this process is returned.
         * @return false on failure, true otherwise.
         */
	bool supplyErrToPeer (
	    VkStatus &rStatus, VReference<VDevice>&rpDevice // stderr
	);

        /**
         * Used to create a socket device.
         *
         * Typical usage does not involve direct invocation of this method. Consumers will typically instead make use of one of the convenience covers to this method demanding less parameters and providing more specific socket creation mechanisms. All of them, however, delegate to this method.
         *
         * @param[out] rStatus the creation status.
         * @param[out] rpDevice the reference by which the newly manufactured device should be returned.
         * @param[in] rAddress the socket address to use/connect to.
         * @param[in] xProtocol the protocol to use.
         * @param[in] xType the type of the socket to create.
         * @param[in] bNoDelay true if the TCP NODELAY flag should be used when creating the TCP/IP socket.
         * @param[in] bPassive true if a passive connection device (listener) should be created, false otherwise.
         * @return false on failure, true otherwise.
         */
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VDevice>&	rpDevice,
	    VkSocketAddress const&	rAddress,
	    int				xProtocol,
	    int				xType,
	    bool			bNoDelay,
	    bool			bPassive
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VDevice>&	rpDevice,
	    char const*			pName,
	    char const*			pProtocol,
	    bool			bNoDelay,
	    bool			bPassive
	);
	bool supply (
	    VkStatus &rStatus, VReference<VDevice>&rpDevice, char const *pName, bool bNoDelay
	);

    //  Device Use Suppliers
    public:
	//  ... pipe byte streams
	static bool Supply (
	    VkStatus &rStatus, VReference<VBSProducer>&rpBS, VString const &rName
	);
	bool supply (
	    VkStatus &rStatus, VReference<VBSProducer>&rpBS, VString const &rName
	);
	static bool Supply (
	    VkStatus &rStatus, VReference<VBSConsumer>&rpBS, VString const &rName
	);
	bool supply (
	    VkStatus &rStatus, VReference<VBSConsumer>&rpBS, VString const &rName
	);

	//  ... new process byte streams
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VProcess>&	rpProcess,
	    VReference<VBSConsumer>*	ppStdToPeer, // null -> null device
	    VReference<VBSProducer>*	ppStdToHere, // null -> null device
	    VReference<VBSProducer>*	ppErrToHere, // null -> null device, ==ppStdToHere -> stdout device
	    char const*			pCommand
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VProcess>&	rpProcess,
	    VReference<VBSConsumer>*	ppStdToPeer, // null -> null device
	    VReference<VBSProducer>*	ppStdToHere, // null -> null device
	    VReference<VBSProducer>*	ppErrToHere, // null -> null device, ==ppStdToHere -> stdout device
	    char const*			pCommand
	);

	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VProcess>&	rpProcess,
	    VReference<VBSConsumer>&	rpStdToPeer,
	    VReference<VBSProducer>&	rpStdToHere,
	    VReference<VBSProducer>&	rpErrToHere, // & == &rpStdToHere -> stdout device
	    char const*			pCommand
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VProcess>&	rpProcess,
	    VReference<VBSConsumer>&	rpStdToPeer,
	    VReference<VBSProducer>&	rpStdToHere,
	    VReference<VBSProducer>&	rpErrToHere, // & == &rpStdToHere -> stdout device
	    char const*			pCommand
	);

	//  ... this process' byte streams
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpStdToHere,
	    VReference<VBSConsumer>&	rpStdToPeer,
	    VReference<VBSConsumer>&	rpErrToPeer
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpStdToHere,
	    VReference<VBSConsumer>&	rpStdToPeer,
	    VReference<VBSConsumer>&	rpErrToPeer
	);
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpStdToHere,
	    VReference<VBSConsumer>&	rpStdToPeer
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpStdToHere,
	    VReference<VBSConsumer>&	rpStdToPeer
	);
	static bool SupplyStdToHere (VkStatus &rStatus, VReference<VBSProducer>&rpBS);
	bool supplyStdToHere (VkStatus &rStatus, VReference<VBSProducer>&rpBS);

	static bool SupplyStdToPeer (VkStatus &rStatus, VReference<VBSConsumer>&rpBS);
	bool supplyStdToPeer (VkStatus &rStatus, VReference<VBSConsumer>&rpBS);

	static bool SupplyErrToPeer (VkStatus &rStatus, VReference<VBSConsumer>&rpBS);
	bool supplyErrToPeer (VkStatus &rStatus, VReference<VBSConsumer>&rpBS);

        /// @name Bidirection Byte Stream Socket Creation
        //@{
        /**
         * Used to create a bidirectional byte stream socket.
         *
         * @param[out] rStatus the creation status.
         * @param[out] rpBSProducer the byte stream that supplies bytes from the remote side.
         * @param[out] rpBSConsumer the byte stream that supplies bytes to the remote side (consuming bytes from this side).
         * @param[in] rAddress the socket address to use/connect to.
         * @param[in] xProtocol the protocol to use.
         * @param[in] xType the type of the socket to create.
         * @param[in] bNoDelay true if the TCP NODELAY flag should be used when creating the TCP/IP socket.
         * @return false on failure, true otherwise.
         */
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpBSProducer,
	    VReference<VBSConsumer>&	rpBSConsumer,
	    VkSocketAddress const&	rAddress,
	    int				xProtocol,
	    int				xType,
	    bool			bNoDelay
	);

        /**
         * @copydoc Supply(VkStatus&, VReference<VBSProducer>&, VReference<VBSConsumer>&, VkSocketAddress const&, int, int, bool)
         */
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpBSProducer,
	    VReference<VBSConsumer>&	rpBSConsumer,
	    VkSocketAddress const&	rAddress,
	    int				xProtocol,
	    int				xType,
	    bool			bNoDelay
	);

        /**
         * Used to create a bidirectional byte stream socket.
         *
         * @param[out] rStatus the creation status.
         * @param[out] rpBSProducer the byte stream that supplies bytes from the remote side.
         * @param[out] rpBSConsumer the byte stream that supplies bytes to the remote side (consuming bytes from this side).
         * @param[in] pName the symbolic socket address to use/connect to.
         * @param[in] pProtocol the protocol to use, infers the type of socket being created.
         * @param[in] bNoDelay true if the TCP NODELAY flag should be used when creating the TCP/IP socket.
         * @return false on failure, true otherwise.
         *
         * @todo: Document supported @p pProtocol values.
         */
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpBSProducer,
	    VReference<VBSConsumer>&	rpBSConsumer,
	    char const*			pName,
	    char const*			pProtocol,
	    bool			bNoDelay
	);

        /**
         * @copydoc Supply(VkStatus&, VReference<VBSProducer>&, VReference<VBSConsumer>&, char const *, char const*, bool)
         */
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpBSProducer,
	    VReference<VBSConsumer>&	rpBSConsumer,
	    char const*			pName,
	    char const*			pProtocol,
	    bool			bNoDelay
	);
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpBSProducer,
	    VReference<VBSConsumer>&	rpBSConsumer,
	    char const*			pName,
	    bool			bNoDelay
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VBSProducer>&	rpBSProducer,
	    VReference<VBSConsumer>&	rpBSConsumer,
	    char const*			pName,
	    bool			bNoDelay
	);

	//  ... socket connections
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VConnection>&	rpConnection,
	    VkSocketAddress const&	rAddress,
	    int				xType,
	    int				xProtocol,
	    bool			bNoDelay
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VConnection>&	rpConnection,
	    VkSocketAddress const&	rAddress,
	    int				xType,
	    int				xProtocol,
	    bool			bNoDelay
	);
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VConnection>&	rpConnection,
	    char const*			pName,
	    char const*			pProtocol,
	    bool			bNoDelay
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VConnection>&	rpConnection,
	    char const*			pName,
	    char const*			pProtocol,
	    bool			bNoDelay
	);
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VConnection>&	rpConnection,
	    char const*			pName,
	    bool			bNoDelay
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VConnection>&	rpConnection,
	    char const*			pName,
	    bool			bNoDelay
	);
        //@}

	/// @name Socket listener creation.
        //@{
        /**
         * Creates a listening socket.
         *
         * @param[out] rStatus the creation status.
         * @param[out] rpListener the reference by which the newly manufactured device should be returned.
         * @param[in] rAddress the socket address to listen on.
         * @param[in] xProtocol the protocol to use.
         * @param[in] xType the type of the socket to create.
         * @param[in] bNoDelay true if the TCP NODELAY flag should be used when creating the TCP/IP socket.
         */
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VListener>&	rpListener,
	    VkSocketAddress const&	rAddress,
	    int				xProtocol,
	    int				xType,
	    bool			bNoDelay
	);
        /**
         * @copydoc Supply(VkStatus&, VReference<VListener>&, VkSocketAddress const&, int, int, bool)
         */
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VListener>&	rpListener,
	    VkSocketAddress const&	rAddress,
	    int				xProtocol,
	    int				xType,
	    bool			bNoDelay
	);
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VListener>&	rpListener,
	    char const*			pName,
	    char const*			pProtocol,
	    bool			bNoDelay
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VListener>&	rpListener,
	    char const*			pName,
	    char const*			pProtocol,
	    bool			bNoDelay
	);
	static bool Supply (
	    VkStatus&			rStatus,
	    VReference<VListener>&	rpListener,
	    char const*			pName,
	    bool			bNoDelay
	);
	bool supply (
	    VkStatus&			rStatus,
	    VReference<VListener>&	rpListener,
	    char const*			pName,
	    bool			bNoDelay
	);
        //@}
    };
}


#endif
