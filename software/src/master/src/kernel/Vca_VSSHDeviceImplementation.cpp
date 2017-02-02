#ifdef HAS_LIBSSH2
/*****  Vca_VSSHDeviceImplementation Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VSSHDeviceImplementation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"
#include "Vca_VBSProducerConsumerDevice_.h"

#include "V_VArgList.h"
#include "V_VTime.h"
#include <stdio.h>

/*********************
 *****  Defines  *****
 *********************/

/** Maximum number of synchronous retries of an I/O operation that would block but then whose poll immediately returned not blocked. */
#define VSSHDEVICE_MAX_SYNC_IO_RETRIES 5


int Vca::VSSHDeviceImplementation::Use::g_iIndexNext = 1;

/****************************************************
 ****************************************************
 *****                                          *****
 *****  Vca::VSSHDeviceImplementation::Channel  *****
 *****                                          *****
 ****************************************************
 ****************************************************/

/**
 * @todo Make non-blocking.
 */
bool Vca::VSSHDeviceImplementation::Channel::connect (IPipeSourceClient *pClient) {
    if (pClient) m_pClient = pClient;

    int result;
    bool bSuccess = true;
    for (int i = 0; i < VSSHDEVICE_MAX_SYNC_IO_RETRIES && bSuccess; i++) {
        switch (m_xState) {
        // Get our socket.
        case State_New:
            // Get our LIBSSH2_SESSION.
            if ((m_pSession = libssh2_session_init ()) == NULL) {
                TraceSSH ("Unable to initialize SSH session.");
                bSuccess = false;
                continue;
            }

            // Set to non-blocking.
            libssh2_session_set_blocking (m_pSession, 0);

            // Set State.
            m_xState = State_Initialized;

        // Initiate session startup.
        case State_Initialized:
            {
                VStatus iStatus;
                HANDLE socket;
                if (!m_pDevice->getSocket (iStatus, socket)) {
                    TraceSSH ("Retrieval of SSH socket failed.");
                    bSuccess = false;
                    continue;
                }
                TraceSSH ("Establishing SSH connection.");
                result = libssh2_session_startup (m_pSession, socket);
            }
            if (result == LIBSSH2_ERROR_EAGAIN) break;
            else if (result) {
                char *errmsg;
                libssh2_session_last_error(m_pSession, &errmsg, NULL, 0);
                VString iError = "Failure establishing SSH session.";
                iError << " Error message: " << errmsg;
                TraceSSH (iError);
                bSuccess = false;
                continue;
            }
            TraceSSH ("Established SSH connection.");

            // Get host key hash.
            {
                //const char *fingerprint = libssh2_hostkey_hash (m_pSession, LIBSSH2_HOSTKEY_HASH_MD5); // TODO: Verify fingerprint against something?
            }

            // Sort out authentication details.
	    {
		VString iDefaultKeyFile;
		V::GetEnvironmentString (iDefaultKeyFile, "HOME", ""); iDefaultKeyFile << "/.ssh/id_rsa";
		VString iKeyFilePriv;
		V::GetEnvironmentString (iKeyFilePriv, "VcaSSHPrivateKeyFile", iDefaultKeyFile);

		iDefaultKeyFile << ".pub";
		VString iKeyFilePub;
		V::GetEnvironmentString (iKeyFilePub, "VcaSSHPublicKeyFile", iDefaultKeyFile);
		struct stat s;
		if (stat(iKeyFilePub, &s) == 0 && stat(iKeyFilePriv, &s) == 0) {
		    m_iKeyFilePriv = iKeyFilePriv;
		    m_iKeyFilePub = iKeyFilePub;
		}
	    }

            // Set State
            m_xState = State_Started;

        // Authenticate.
        case State_Started:
            // Use keys if we have them.
            if (m_iKeyFilePub.isntEmpty() && m_iKeyFilePriv.isntEmpty()) {
                TraceSSH ("Attempting public key authentication with key pair (%s, %s).", m_iKeyFilePub.content (), m_iKeyFilePriv.content ());
                result = libssh2_userauth_publickey_fromfile(m_pSession, m_iUserName, m_iKeyFilePub, m_iKeyFilePriv, m_iPassword);
                if (result == LIBSSH2_ERROR_EAGAIN) break;
                else if (result) {
                    char *errmsg;
                    libssh2_session_last_error(m_pSession, &errmsg, NULL, 0);
                    VString iError = "Failure during public key authentication, falling back to password authentication.";
                    iError << " Error message: " << errmsg;
                    TraceSSH (iError);
                } else m_xState = State_Authenticated;
            }
            // Now try password-based authentication if we're still not authenticated.
            if (m_xState != State_Authenticated) {
                TraceSSH ("Attempting password authentication.");
                result = libssh2_userauth_password (m_pSession, m_iUserName, m_iPassword);
                if (result == LIBSSH2_ERROR_EAGAIN) break;
                if (result) {
                    TraceSSH ("Password authentication failed.");
                    bSuccess = false;
                    continue;
                }
                m_xState = State_Authenticated;
            }
            TraceSSH ("Authenticated (%s).", m_iUserName.content ());

        // Get our LIBSSH2_CHANNEL.
        case State_Authenticated:
            TraceSSH ("Opening session (retieving channel).");
            m_pChannel = libssh2_channel_open_session(m_pSession);
            if (m_pChannel == NULL) {
                if (libssh2_session_last_error(m_pSession, NULL, NULL, 0) == LIBSSH2_ERROR_EAGAIN) break;
                TraceSSH ("Unable to open SSH session.");
                bSuccess = false;
                continue;
            }
            m_xState = State_Opened;

        // Execute remote command.
        case State_Opened:
            TraceSSH ("Executing remote command (%s).", m_iCommand.content ());
            result = libssh2_channel_exec (m_pChannel, m_iCommand);
            if (result == LIBSSH2_ERROR_EAGAIN) break;
            if (result) {
                TraceSSH ("Unable to execute remote command.");
                bSuccess = false;
                continue;
            }
            m_xState = State_Executed;

        // Send STDERR from the remote end to null.
        case State_Executed:
            TraceSSH ("Setting STDERR handling.");
            result = libssh2_channel_handle_extended_data2 (m_pChannel, LIBSSH2_CHANNEL_EXTENDED_DATA_IGNORE);
            if (result == LIBSSH2_ERROR_EAGAIN) break;
            if (result) {
                TraceSSH ("Unable to set extended data handling.");
                bSuccess = false;
                continue;
            }
            m_xState = State_Connected;
            TraceSSH ("Connect sequence finished successfully.");
            if (m_pClient) return supplyPipes (m_pClient);
            return true;
        }

        // Blocked. Poll as necessary.
        TraceSSH ("Would block during connect; starting poll(s).");
        VkStatus s;
        if (! startWritePoll (s)) {
            TraceSSH ("Error while starting write poll for connect. Aborting.");
            bSuccess = false;
            continue;
        }
        if (s.isBlocked()) return true;
        // Write isn't blocking. Try read poll.
        if (! startReadPoll (s)) {
            TraceSSH ("Error while starting read poll for connect. Aborting.");
            bSuccess = false;
            continue;
        }
        if (s.isBlocked()) return true;
        // Neither read nor write is blocked. Retry.
        TraceSSH ("Polls returned non-blocking immediately while connecting. Retrying.");
    }

    // Handle errors.
    if (bSuccess) {
        // Max retries exceeded. Error.
        TraceSSH ("Maximum synchronous retries exceeded on SSH connect sequence. Disconnecting.");
    } else {
        // Clean as necessary on error.
        TraceSSH ("Error during connect sequence. Disconnecting.");
    }
    // Pass on the error to the client.
    if (m_pClient) {
        m_pClient->OnError (0, "Error connecting via SSH.");
        m_pClient.clear();
    }
    // Disconnect cleanly.
    disconnect ();
    return false;
}

bool Vca::VSSHDeviceImplementation::Channel::supplyPipes (IPipeSourceClient *pClient) {
    if (pClient) m_pClient = pClient;
    if (m_pClient.isNil ()) return false;

    // Get our VSSHDeviceImplementation and associated device class instances.
    VSSHDeviceImplementation pIOSSHDevice (0, this, 0), pErrSSHDevice (0, this, 1);
    VDevice_<VSSHDeviceImplementation>::Reference pSTDInOutDevice (new VBSProducerConsumerDevice_<VSSHDeviceImplementation> (pIOSSHDevice));
    VDevice_<VSSHDeviceImplementation>::Reference pSTDErrDevice;

    // Get our byte streams.
    VBSConsumer::Reference pSTDInBS;
    VBSProducer::Reference pSTDOutBS, pSTDErrBS;
    if (!pSTDInOutDevice->supply (pSTDInBS)) {
        m_pClient->OnError (0, "Unable to retrieve STDIN byte stream from SSH Device.");
        m_pClient.clear();
        return false;
    }
    if (!pSTDInOutDevice->supply (pSTDOutBS)) {
        m_pClient->OnError (0, "Unable to retrieve STDOUT byte stream from SSH Device.");
        m_pClient.clear();
        return false;
    }
    // Error channel for SSH is unsupported at the moment.
    /*if (bErrorChannel) {
        pSTDErrDevice.setTo (new VBSProducerDevice_<VSSHDeviceImplementation> (pErrSSHDevice));
        if (!pSTDErrDevice->supply (pSTDErrBS)) {
            m_pClient->OnError (0, "Unable to retrieve STDERR byte stream from SSH Device.");
            m_pClient.clear();
            return false;
        }
    }*/

    // Create the producer and consumer, supply them to the client.
    m_pClient->OnData(pSTDInBS, pSTDOutBS, pSTDErrBS);
    m_pClient.clear();
    return true;
}

void Vca::VSSHDeviceImplementation::Channel::disconnect () {
    int result = LIBSSH2_ERROR_EAGAIN;
    for (int i = 0; i < VSSHDEVICE_MAX_SYNC_IO_RETRIES; i++) {
        switch (m_xState) {
        default:
            m_xState = State_Closing;
        case State_Closing:
            TraceSSH ("Closing SSH channel.");
            result = libssh2_channel_free (m_pChannel); // Automatically calls libssh2_channel_close () for us.
            if (result == LIBSSH2_ERROR_EAGAIN) break;
            else if (result < 0) {
                TraceSSH ("Error while freeing channel.");
                break;
            }
            m_xState = State_Closed;
            TraceSSH ("SSH channel closed.");
        case State_Closed:
        case State_Authenticated:
        case State_Started:
        case State_Initialized:
            TraceSSH ("Disconnecting SSH session.");
            result = libssh2_session_disconnect_ex (m_pSession, SSH_DISCONNECT_BY_APPLICATION, "Vca SSH Device disconnect.", "en");
            if (result == LIBSSH2_ERROR_EAGAIN) break;
            else if (result < 0) {
                TraceSSH ("Error while disconnecting session.");
                break;
            }
            m_xState = State_Disconnected;
            TraceSSH ("SSH session disconnected.");
        case State_Disconnected:
            TraceSSH ("Destroying SSH session.");
            result = libssh2_session_free (m_pSession);
            if (result == LIBSSH2_ERROR_EAGAIN) break;
            else if (result < 0) {
                TraceSSH ("Error while destroying session.");
                break;
            }
            ReadPoller::close ();
            WritePoller::close ();
            m_xState = State_Cleaned;
            TraceSSH ("SSH session destroyed.");
        case State_Cleaned:
            return;
        }

        // There was a non-success status from one of the operations. Handle it.
        if (result == LIBSSH2_ERROR_EAGAIN) {
            TraceSSH ("Would block during disconnect; starting poll(s).");
            VkStatus s;
            if (! startWritePoll (s)){
                TraceSSH ("Error while starting write poll for disconnecting. Aborting.");
                return;
            }
            if (s.isBlocked()) return;
            // Write isn't blocking. Try read poll.
            if (! startReadPoll (s)) {
                TraceSSH ("Error while starting read poll for disconnecting. Aborting.");
                return;
            }
            if (s.isBlocked()) return;
            // Neither read nor write is blocked. Retry.
            TraceSSH ("Polls returned non-blocking immediately while disconnecting. Retrying.");
        } else {
            TraceSSH ("Error (%d) during disconnect. Aborting.", result);
            return;
        }
    }

    // Max retries exceeded. Error.
    TraceSSH ("Maximum retries exceeded on SSH disconnect sequence. Aborting.");
    return; // TODO: report error of some sort?
}

bool Vca::VSSHDeviceImplementation::Channel::doRead (VkStatus &rStatus, BSGet *pGet, bool bRetrying) {
    int result = 0;

    // Loop until we've actually blocked or until we've actually succeeded, both of which return.
    for (int i = 0; i < VSSHDEVICE_MAX_SYNC_IO_RETRIES; i++) {
        // Check if the channel is closed.
        if (libssh2_channel_eof(m_pChannel)) {
            if (bRetrying) pGet->triggerUser ();
            return rStatus.MakeClosedStatus ();
        }

        // Try the read.
        TraceSSH ("Attempting read (%d).", pGet->index ());
        result = libssh2_channel_read_ex(m_pChannel, pGet->subStreamID (), (char *)(pGet->areaBase ()), pGet->areaSize ());

        // Catch errors explicitly.
        switch (result) {
        case LIBSSH2_ERROR_EAGAIN: {
            // Start a read poll. If it returns immediately with a non-error and non-blocked status, retry the read.
            bool s = startReadPoll (rStatus);
            if (!s) {
                if (bRetrying) pGet->triggerUser ();
                return false;
            }
            if (rStatus.isntBlocked ()) {
                TraceSSH ("Blocked (%d), but poll returned non-blocked. Retrying.", pGet->index ());
                continue;
            }

            // We actually blocked. This time, go ahead and enqueue the read for later.
            TraceSSH ("Would block while reading (%d) up to %d bytes.", pGet->index (), pGet->areaSize ());
            enqueue (pGet);
            return s;
            }
        case LIBSSH2_ERROR_CHANNEL_CLOSED:
            if (bRetrying) pGet->triggerUser ();
            return rStatus.MakeClosedStatus ();
        }

        // Catch errors generally.
        if (result < 0) {
            if (bRetrying) pGet->triggerUser ();
            return rStatus.MakeErrorStatus (result);
        }

        // No error. Return success.
        TraceSSH ("Read (%d) %d bytes (max %d).", pGet->index (), result, pGet->areaSize ());
        pGet->onData (result);
        if (bRetrying) pGet->triggerUser ();
        TraceSSH ("Finishing read (%d).", pGet->index ());
        return true;
    }

    // Max retries exceeded. Error.
    TraceSSH ("Maximum retries exceeded on SSH read (%d). Erroring.", pGet->index ());
    if (bRetrying) pGet->triggerUser ();
    return rStatus.MakeErrorStatus (result);
}

bool Vca::VSSHDeviceImplementation::Channel::doWrite (VkStatus &rStatus, BSPut *pPut, bool bRetrying) {
    int result;

    // Loop until we've actually blocked or until we've actually succeeded, both of which return.
    for (int i = 0; i < VSSHDEVICE_MAX_SYNC_IO_RETRIES; i++) {
        // Try the write.
        TraceSSH ("Attempting write (%d).", pPut->index ());
        result = libssh2_channel_write_ex (m_pChannel, pPut->subStreamID (), (const char *)(pPut->areaBase ()), pPut->areaSize ());

        // Catch errors explicitly.
        switch (result) {
        case LIBSSH2_ERROR_EAGAIN: {
            // Start a write poll. If it returns immediately with a non-error and non-blocked status, retry the write.
            bool s = startWritePoll (rStatus);
            if (!s) {
                if (bRetrying) pPut->triggerUser ();
                return false;
            }
            if (rStatus.isntBlocked ()) {
                TraceSSH ("Blocked (%d), but poll returned non-blocked. Retrying.", pPut->index ());
                continue;
            }

            // We actually blocked. This time, go ahead and enqueue the read for later.
            TraceSSH ("Would block while writing (%d) %d bytes.", pPut->index (), pPut->areaSize ());
            enqueue (pPut);
            return s;
            }
        case LIBSSH2_ERROR_CHANNEL_CLOSED:
            if (bRetrying) pPut->triggerUser ();
            return rStatus.MakeClosedStatus ();
        }

        // Catch errors generally.
        if (result < 0) {
            if (bRetrying) pPut->triggerUser ();
            return rStatus.MakeErrorStatus (result);
        }

        // No error. Return success.
        TraceSSH ("Wrote (%d) %d bytes (max %d).", pPut->index () , result, pPut->areaSize ());
        pPut->onData (result);
        if (bRetrying) pPut->triggerUser ();
        return true;
    }

    // Max retries exceeded. Error.
    TraceSSH ("Maximum retries exceeded on SSH write (%d). Erroring.", pPut->index ());
    if (bRetrying) pPut->triggerUser ();
    return rStatus.MakeErrorStatus (result);
}

void Vca::VSSHDeviceImplementation::Channel::retryReads () {
    TraceSSH ("Retrying SSH reads.");
    BSGet::Reference pGet, pLast = m_pBlockedReadsTail;
    while (pGet != pLast && dequeue(pGet)) {
        VkStatus s;
        pGet->doTransfer (s, true);
    }
    if (m_pBlockedReadsHead.isNil()) TraceSSH ("All queued SSH reads finished successfully.");
}

void Vca::VSSHDeviceImplementation::Channel::retryWrites () {
    // If there aren't any blocked writes, we must've blocked on an EOF.
    if (m_pBlockedWritesHead.isNil ()) {
        onFinalWriter ();
        return;
    }
    TraceSSH ("Retrying SSH writes.");
    BSPut::Reference pPut, pLast = m_pBlockedWritesTail;
    while (pPut != pLast && dequeue(pPut)) {
        VkStatus s;
        pPut->doTransfer (s, true);
    }
    if (m_pBlockedWritesHead.isNil()) TraceSSH ("All queued SSH writes finished successfully.");
}

void Vca::VSSHDeviceImplementation::Channel::enqueue (BSGet *pGet) {
    if (m_pBlockedReadsTail) m_pBlockedReadsTail->setNext (pGet);
    else m_pBlockedReadsHead = pGet;
    m_pBlockedReadsTail = pGet;
}

void Vca::VSSHDeviceImplementation::Channel::enqueue (BSPut *pPut) {
    if (m_pBlockedWritesTail) m_pBlockedWritesTail->setNext (pPut);
    else m_pBlockedWritesHead = pPut;
    m_pBlockedWritesTail = pPut;
}

bool Vca::VSSHDeviceImplementation::Channel::dequeue (BSPut::Reference &rPut) {
    if (!m_pBlockedWritesHead) return false;
    rPut = m_pBlockedWritesHead;
    m_pBlockedWritesHead = m_pBlockedWritesHead->next ();
    if (m_pBlockedWritesTail == rPut) m_pBlockedWritesTail.clear ();
    rPut->setNext (0);
    return true;
}

bool Vca::VSSHDeviceImplementation::Channel::dequeue (BSGet::Reference &rGet) {
    if (!m_pBlockedReadsHead) return false;
    rGet = m_pBlockedReadsHead;
    m_pBlockedReadsHead = m_pBlockedReadsHead->next ();
    if (m_pBlockedReadsTail == rGet) m_pBlockedReadsTail.clear ();
    rGet->setNext (0);
    return true;
}

void Vca::VSSHDeviceImplementation::Channel::onFinalWriter () {
    int result;
    for (int i = 0; i < VSSHDEVICE_MAX_SYNC_IO_RETRIES; i++) {
        if (m_cWriterDevices > 0) return;
        TraceSSH ("Sending EOF.");
        result = libssh2_channel_send_eof (m_pChannel);
        if (result == LIBSSH2_ERROR_EAGAIN) {
            TraceSSH ("Blocked while sending EOF. Polling for writes.");
            VkStatus s;
            if (! startWritePoll (s)) {
                TraceSSH ("Error while starting write poll for sending EOF. Aborting.");
                return;
            }
            if (s.isntBlocked ()) {
                TraceSSH ("Poll returned non-blocking immediately while sending EOF. Retrying.");
                continue;
            }
            return;
        } else {
            if (result < 0) TraceSSH ("Error while sending EOF. "); // TODO: Report error of some sort?
            else TraceSSH ("Sent EOF. ");
            TraceSSH ("Disconnecting as necessary.");
            disconnectAsNecessary ();
            return;
        }
    }

    // Maximum retries exhausted.
    TraceSSH ("Maximum retries exceeded on SSH write of EOF.");
}

void Vca::VSSHDeviceImplementation::Channel::onFinalReader () {
    TraceSSH ("No more readers; disconnecting as necessary.");
    disconnectAsNecessary ();
}

void Vca::VSSHDeviceImplementation::Channel::disconnectAsNecessary () {
    if (m_cReaderDevices < 1 && m_cWriterDevices < 1) disconnect();
}

void Vca::VSSHDeviceImplementation::Channel::onWritePoll () {
    TraceSSH ("Write poll returned to SSH device.");
    switch (m_xState) {
    // Retry writes if we're still in a connected state.
    case State_Connected:
        retryWrites();
        break;
    // Continue connecting if we're not yet connected.
    case State_New:
    case State_Initialized:
    case State_Started:
    case State_Authenticated:
    case State_Opened:
    case State_Executed:
        connect ();
        break;
    // We started disconnecting and would have blocked. Continue disconnecting.
    case State_Closing:
    case State_Closed:
    case State_Disconnected:
    case State_Cleaned:
        disconnect();
        break;
    default:
        TraceSSH ("Unhandled state while returning from write poll.");
        break;
    }
}

void Vca::VSSHDeviceImplementation::Channel::onReadPoll () {
    TraceSSH ("Read poll returned to SSH device.");
    switch (m_xState) {
    // Retry writes if we're still in a connected state.
    case State_Connected:
        retryReads();
        break;
    // Continue connecting if we're not yet connected.
    case State_New:
    case State_Initialized:
    case State_Started:
    case State_Authenticated:
    case State_Opened:
    case State_Executed:
        connect ();
        break;
    // We started disconnecting and would have blocked. Continue disconnecting.
    case State_Closing:
    case State_Closed:
    case State_Disconnected:
    case State_Cleaned:
        disconnect();
        break;
    default:
        TraceSSH ("Unhandled state while returning from write poll.");
        break;
    }
}



/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::VSSHDeviceImplementation  *****
 *****                                 *****
 *******************************************
 *******************************************/

bool Vca::VSSHDeviceImplementation::g_bTracing = false;

void Vca::VSSHDeviceImplementation::TraceSSH (char const *pInfo, ...) {
    if (!g_bTracing) return;
    V::VTime iNow;
    VString iTimestring;
    iNow.asString (iTimestring);
    fprintf (stderr, "%s Vca SSH Trace Info: ", iTimestring.content());
    V_VARGLIST (ap, pInfo);
    vfprintf (stderr, pInfo, ap);
    fprintf (stderr, "\n");
}

void Vca::VSSHDeviceImplementation::onFirstReader () {
    m_bHasReaders = true;
    if (m_pChannel) m_pChannel->incrReaderDevices ();
}

void Vca::VSSHDeviceImplementation::onFirstWriter () {
    m_bHasWriters = true;
    if (m_pChannel) m_pChannel->incrWriterDevices ();
}

void Vca::VSSHDeviceImplementation::onFinalReader () {
    if (m_pChannel) m_pChannel->decrReaderDevices ();
    m_bHasReaders = false;
    if (!m_bHasWriters) m_pChannel.clear();
}

void Vca::VSSHDeviceImplementation::onFinalWriter () {
    if (m_pChannel) m_pChannel->decrWriterDevices ();
    m_bHasWriters = false;
    if (!m_bHasReaders) m_pChannel.clear();
}
#endif // defined(HAS_LIBSSH2)
