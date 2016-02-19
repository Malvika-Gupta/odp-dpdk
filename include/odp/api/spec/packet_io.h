/* Copyright (c) 2013, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */


/**
 * @file
 *
 * ODP Packet IO
 */

#ifndef ODP_API_PACKET_IO_H_
#define ODP_API_PACKET_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <odp/api/spec/packet_io_stats.h>
#include <odp/api/spec/queue.h>

/** @defgroup odp_packet_io ODP PACKET IO
 *  Operations on a packet Input/Output interface.
 *
 * Packet IO is the Ingress and Egress interface to ODP processing. It
 * allows manipulation of the interface for setting such attributes as
 * the mtu, mac etc.
 * Pktio is usually followed by the classifier and a default class COS
 * can be set so that the scheduler may distribute flows. The interface
 * may be used directly in polled mode with odp_pktio_recv() &
 * odp_pktio_send().
 * Diagnostic messages can be enhanced by using odp_pktio_to_u64 which
 * will generate a printable reference for a pktio handle for use with
 * the logging.
 *  @{
 */

/**
 * @typedef odp_pktio_t
 * Packet IO handle
 */

/**
 * @typedef odp_pktin_queue_t
 * Direct packet input queue handle
 */

/**
 * @typedef odp_pktout_queue_t
 * Direct packet output queue handle
 */

/**
 * @def ODP_PKTIO_INVALID
 * Invalid packet IO handle
 */

/**
 * @def ODP_PKTIO_MACADDR_MAXSIZE
 * Minimum size of output buffer for odp_pktio_mac_addr()
 * Actual MAC address sizes may be different.
 */


/**
 * Packet input mode
 */
typedef enum odp_pktin_mode_t {
	/** Direct packet input from the interface */
	ODP_PKTIN_MODE_DIRECT = 0,
	/** Packet input through scheduler and scheduled event queues */
	ODP_PKTIN_MODE_SCHED,
	/** Packet input through plain event queues */
	ODP_PKTIN_MODE_QUEUE,
	/** Application will never receive from this interface */
	ODP_PKTIN_MODE_DISABLED
} odp_pktin_mode_t;

/**
 * Packet output mode
 */
typedef enum odp_pktout_mode_t {
	/** Direct packet output on the interface */
	ODP_PKTOUT_MODE_DIRECT = 0,
	/** Packet output through event queues */
	ODP_PKTOUT_MODE_QUEUE,
	/** Packet output through traffic manager API */
	ODP_PKTOUT_MODE_TM,
	/** Application will never send to this interface */
	ODP_PKTOUT_MODE_DISABLED
} odp_pktout_mode_t;

/**
 * Packet input hash protocols
 *
 * The list of protocol header field combinations, which are included into
 * packet input hash calculation.
 */
typedef union odp_pktin_hash_proto_t {
	/** Protocol header fields for hashing */
	struct {
		/** IPv4 addresses and UDP port numbers */
		uint32_t ipv4_udp : 1;
		/** IPv4 addresses and TCP port numbers */
		uint32_t ipv4_tcp : 1;
		/** IPv4 addresses */
		uint32_t ipv4     : 1;
		/** IPv6 addresses and UDP port numbers */
		uint32_t ipv6_udp : 1;
		/** IPv6 addresses and TCP port numbers */
		uint32_t ipv6_tcp : 1;
		/** IPv6 addresses */
		uint32_t ipv6     : 1;
	} proto;

	/** All bits of the bit field structure */
	uint32_t all_bits;
} odp_pktin_hash_proto_t;

/**
 * Packet IO operation mode
 */
typedef enum odp_pktio_op_mode_t {
	/** Multi-thread safe operation
	  *
	  * Direct packet IO operation (recv or send) is multi-thread safe. Any
	  * number of application threads may perform the operation
	  * concurrently. */
	ODP_PKTIO_OP_MT = 0,

	/** Not multi-thread safe operation
	  *
	  * Direct packet IO operation (recv or send) may not be multi-thread
	  * safe. Application ensures synchronization between threads so that
	  * simultaneously only single thread attempts the operation on
	  * the same (pktin or pktout) queue. */
	ODP_PKTIO_OP_MT_UNSAFE

} odp_pktio_op_mode_t;

/**
 * Packet input queue parameters
 */
typedef struct odp_pktin_queue_param_t {
	/** Operation mode
	  *
	  * The default value is ODP_PKTIO_OP_MT. Application may enable
	  * performance optimization by defining ODP_PKTIO_OP_MT_UNSAFE when
	  * applicable. */
	odp_pktio_op_mode_t op_mode;

	/** Enable flow hashing
	  * 0: Do not hash flows
	  * 1: Hash flows to input queues */
	odp_bool_t hash_enable;

	/** Protocol field selection for hashing. Multiple protocols can be
	  * selected. */
	odp_pktin_hash_proto_t hash_proto;

	/** Number of input queues to be created. More than one input queue
	  * require input hashing or classifier setup. Hash_proto is ignored
	  * when hash_enable is zero or num_queues is one. This value must be
	  * between 1 and interface capability. Queue type is defined by the
	  * input mode. The default value is 1. */
	unsigned num_queues;

	/** Queue parameters for creating input queues in ODP_PKTIN_MODE_QUEUE
	  * or ODP_PKTIN_MODE_SCHED modes. Scheduler parameters are considered
	  * only in ODP_PKTIN_MODE_SCHED mode. */
	odp_queue_param_t queue_param;

} odp_pktin_queue_param_t;

/**
 * Packet output queue parameters
 *
 * These parameters are used in ODP_PKTOUT_MODE_DIRECT and
 * ODP_PKTOUT_MODE_QUEUE modes.
 */
typedef struct odp_pktout_queue_param_t {
	/** Operation mode
	  *
	  * The default value is ODP_PKTIO_OP_MT. Application may enable
	  * performance optimization by defining ODP_PKTIO_OP_MT_UNSAFE when
	  * applicable. */
	odp_pktio_op_mode_t op_mode;

	/** Number of output queues to be created. The value must be between
	  * 1 and interface capability. The default value is 1. */
	unsigned num_queues;

} odp_pktout_queue_param_t;

/**
 * Packet IO parameters
 *
 * In minimum, user must select input and output modes. Use 0 for defaults.
 * Initialize entire struct with zero to maintain API compatibility.
 */
typedef struct odp_pktio_param_t {
	/** Packet input mode
	  *
	  * The default value is ODP_PKTIN_MODE_DIRECT. */
	odp_pktin_mode_t in_mode;
	/** Packet output mode
	  *
	  * The default value is ODP_PKTOUT_MODE_DIRECT. */
	odp_pktout_mode_t out_mode;
} odp_pktio_param_t;

/**
 * Packet IO capabilities
 */
typedef struct odp_pktio_capability_t {
	/** Maximum number of input queues */
	unsigned max_input_queues;
	/** Maximum number of output queues */
	unsigned max_output_queues;
} odp_pktio_capability_t;

/**
 * Open a packet IO interface
 *
 * An ODP program can open a single packet IO interface per device, attempts
 * to open an already open device will fail, returning ODP_PKTIO_INVALID with
 * errno set. Use odp_pktio_lookup() to obtain a handle to an already open
 * device. Packet IO parameters provide interface level configuration options.
 *
 * Packet input queue configuration must be setup with
 * odp_pktin_queue_config() before odp_pktio_start() is called. When packet
 * input mode is ODP_PKTIN_MODE_DISABLED, odp_pktin_queue_config() call is
 * optional and will ignore all parameters.
 *
 * Packet output queue configuration must be setup with
 * odp_pktout_queue_config() before odp_pktio_start() is called. When packet
 * output mode is ODP_PKTOUT_MODE_DISABLED or ODP_PKTOUT_MODE_TM,
 * odp_pktout_queue_config() call is optional and will ignore all parameters.
 *
 * Packet receive and transmit on the interface is enabled with a call to
 * odp_pktio_start(). If not specified otherwise, any interface level
 * configuration must not be changed when the interface is active (between start
 * and stop calls).
 *
 * In summary, a typical pktio interface setup sequence is ...
 *   * odp_pktio_open()
 *   * odp_pktin_queue_config()
 *   * odp_pktout_queue_config()
 *   * odp_pktio_start()
 *
 * ... and tear down sequence is:
 *   * odp_pktio_stop()
 *   * odp_pktio_close()
 *
 * @param name   Packet IO device name
 * @param pool   Default pool from which to allocate storage for packets
 *               received over this interface, must be of type ODP_POOL_PACKET
 * @param param  Packet IO parameters
 *
 * @return Packet IO handle
 * @retval ODP_PKTIO_INVALID on failure
 *
 * @note The device name "loop" is a reserved name for a loopback device used
 *	 for testing purposes.
 *
 * @note Packets arriving via this interface assigned to a CoS by the
 *	 classifier are received into the pool associated with that CoS. This
 *	 will occur either because this pktio is assigned a default CoS via
 *	 the odp_pktio_default_cos_set() routine, or because a matching PMR
 *	 assigned the packet to a specific CoS. The default pool specified
 *	 here is applicable only for those packets that are not assigned to a
 *	 more specific CoS.
 *
 * @see odp_pktio_start(), odp_pktio_stop(), odp_pktio_close()
 */
odp_pktio_t odp_pktio_open(const char *name, odp_pool_t pool,
			   const odp_pktio_param_t *param);

/**
 * Query packet IO interface capabilities
 *
 * Outputs packet IO interface capabilities on success.
 *
 * @param      pktio  Packet IO handle
 * @param[out] capa   Pointer to capability structure for output
 *
 * @retval 0 on success
 * @retval <0 on failure
 */
int odp_pktio_capability(odp_pktio_t pktio, odp_pktio_capability_t *capa);

/**
 * Configure packet input queues
 *
 * Setup a number of packet input queues and configure those. The maximum number
 * of queues is platform dependent and can be queried with
 * odp_pktio_capability(). Use odp_pktin_queue_param_init() to initialize
 * parameters into their default values. Default values are also used when
 * 'param' pointer is NULL.
 *
 * Queue handles for input queues can be requested with odp_pktin_queue() or
 * odp_pktin_event_queue() after this call. All requested queues are setup on
 * success, no queues are setup on failure. Each call reconfigures input queues
 * and may invalidate all previous queue handles.
 *
 * @param pktio    Packet IO handle
 * @param param    Packet input queue configuration parameters. Uses defaults
 *                 when NULL.
 *
 * @retval 0 on success
 * @retval <0 on failure
 *
 * @see odp_pktio_capability(), odp_pktin_queue(), odp_pktin_event_queue()
 */
int odp_pktin_queue_config(odp_pktio_t pktio,
			   const odp_pktin_queue_param_t *param);

/**
 * Configure packet output queues
 *
 * Setup a number of packet output queues and configure those. The maximum
 * number of queues is platform dependent and can be queried with
 * odp_pktio_capability(). Use odp_pktout_queue_param_init() to initialize
 * parameters into their default values. Default values are also used when
 * 'param' pointer is NULL.
 *
 * Queue handles for output queues can be requested with odp_pktout_queue() or
 * odp_pktout_event_queue() after this call. All requested queues are setup on
 * success, no queues are setup on failure. Each call reconfigures output queues
 * and may invalidate all previous queue handles.
 *
 * @param pktio    Packet IO handle
 * @param param    Packet output queue configuration parameters. Uses defaults
 *                 when NULL.
 *
 * @retval 0 on success
 * @retval <0 on failure
 *
 * @see odp_pktio_capability(), odp_pktout_queue(), odp_pktout_event_queue()
 */
int odp_pktout_queue_config(odp_pktio_t pktio,
			    const odp_pktout_queue_param_t *param);

/**
 * Event queues for packet input
 *
 * Returns the number of input queues configured for the interface in
 * ODP_PKTIN_MODE_QUEUE and ODP_PKTIN_MODE_SCHED modes. Outputs up to 'num'
 * queue handles when the 'queues' array pointer is not NULL. If return value is
 * larger than 'num', there are more queues than the function was allowed to
 * output. If return value (N) is less than 'num', only queues[0 ... N-1] have
 * been written.
 *
 * Packets (and other events) from these queues are received with
 * odp_queue_deq(), odp_schedule(), etc calls.
 *
 * @param      pktio    Packet IO handle
 * @param[out] queues   Points to an array of queue handles for output
 * @param      num      Maximum number of queue handles to output
 *
 * @return Number of packet input queues
 * @retval <0 on failure
 */
int odp_pktin_event_queue(odp_pktio_t pktio, odp_queue_t queues[], int num);

/**
 * Direct packet input queues
 *
 * Returns the number of input queues configured for the interface in
 * ODP_PKTIN_MODE_DIRECT mode. Outputs up to 'num' queue handles when the
 * 'queues' array pointer is not NULL. If return value is larger than 'num',
 * there are more queues than the function was allowed to output. If return
 * value (N) is less than 'num', only queues[0 ... N-1] have been written.
 *
 * Packets from these queues are received with odp_pktin_recv().
 *
 * @param      pktio    Packet IO handle
 * @param[out] queues   Points to an array of queue handles for output
 * @param      num      Maximum number of queue handles to output
 *
 * @return Number of packet input queues
 * @retval <0 on failure
 */
int odp_pktin_queue(odp_pktio_t pktio, odp_pktin_queue_t queues[], int num);

/**
 * Event queues for packet output
 *
 * Returns the number of output queues configured for the interface in
 * ODP_PKTOUT_MODE_QUEUE. Outputs up to 'num' queue handles when the
 * 'queues' array pointer is not NULL. If return value is larger than 'num',
 * there are more queues than the function was allowed to output. If return
 * value (N) is less than 'num', only queues[0 ... N-1] have been written.
 *
 * Packets are enqueued to these queues with odp_queue_enq() or
 * odp_queue_enq_multi(). Behaviour is undefined if other events than packets
 * are enqueued.
 *
 * @param      pktio    Packet IO handle
 * @param[out] queues   Points to an array of queue handles for output
 * @param      num      Maximum number of queue handles to output
 *
 * @return Number of packet output queues
 * @retval <0 on failure
 */
int odp_pktout_event_queue(odp_pktio_t pktio, odp_queue_t queues[], int num);

/**
 * Direct packet output queues
 *
 * Returns the number of output queues configured for the interface in
 * ODP_PKTOUT_MODE_DIRECT mode. Outputs up to 'num' queue handles when the
 * 'queues' array pointer is not NULL. If return value is larger than 'num',
 * there are more queues than the function was allowed to output. If return
 * value (N) is less than 'num', only queues[0 ... N-1] have been written.
 *
 * Packets are sent to these queues with odp_pktio_send_queue().
 *
 * @param      pktio    Packet IO handle
 * @param[out] queues   Points to an array of queue handles for output
 * @param      num      Maximum number of queue handles to output
 *
 * @return Number of packet output queues
 * @retval <0 on failure
 */
int odp_pktout_queue(odp_pktio_t pktio, odp_pktout_queue_t queues[], int num);

/**
 * Start packet receive and transmit
 *
 * Activate packet receive and transmit on a previously opened or stopped
 * interface. The interface can be stopped with a call to odp_pktio_stop().
 *
 * @param pktio  Packet IO handle
 *
 * @retval 0 on success
 * @retval <0 on failure
 *
 * @see odp_pktio_open(), odp_pktio_stop()
 */
int odp_pktio_start(odp_pktio_t pktio);

/**
 * Stop packet receive and transmit
 *
 * Stop packet receive and transmit on a previously started interface. New
 * packets are not received from or transmitted to the network. Packets already
 * received from the network may be still available from interface and
 * application can receive those normally. New packets may not be accepted for
 * transmit. Packets already stored for transmit are not freed. A following
 * odp_packet_start() call restarts packet receive and transmit.
 *
 * @param pktio  Packet IO handle
 *
 * @retval 0 on success
 * @retval <0 on failure
 *
 * @see odp_pktio_start(), odp_pktio_close()
 */
int odp_pktio_stop(odp_pktio_t pktio);

/**
 * Close a packet IO interface
 *
 * Close a stopped packet IO interface. This call frees all remaining packets
 * stored in pktio receive and transmit side buffers. The pktio is destroyed
 * and the handle must not be used for other calls. After a successful call,
 * the same pktio device can be opened again with a odp_packet_open() call.
 *
 * @param pktio  Packet IO handle
 *
 * @retval 0 on success
 * @retval <0 on failure
 *
 * @see odp_pktio_stop(), odp_pktio_open()
 */
int odp_pktio_close(odp_pktio_t pktio);

/**
 * Return a packet IO handle for an already open device
 *
 * @param name   Packet IO device name
 *
 * @return Packet IO handle
 * @retval ODP_PKTIO_INVALID on failure
 */
odp_pktio_t odp_pktio_lookup(const char *name);

/**
 * Receive packets directly from an interface input queue
 *
 * Receives up to 'num' packets from the pktio interface input queue. When
 * input queue parameter 'op_mode' has been set to ODP_PKTIO_OP_MT_UNSAFE,
 * the operation is optimized for single thread operation per queue and the same
 * queue must not be accessed simultaneously from multiple threads.
 *
 * @param      queue      Packet input queue handle for receiving packets
 * @param[out] packets[]  Packet handle array for output of received packets
 * @param      num        Maximum number of packets to receive
 *
 * @return Number of packets received
 * @retval <0 on failure
 *
 * @see odp_pktin_queue()
 */
int odp_pktin_recv(odp_pktin_queue_t queue, odp_packet_t packets[], int num);

/**
 * Send packets directly to an interface output queue
 *
 * Sends out a number of packets to the interface output queue. When
 * output queue parameter 'op_mode' has been set to ODP_PKTIO_OP_MT_UNSAFE,
 * the operation is optimized for single thread operation per queue and the same
 * queue must not be accessed simultaneously from multiple threads.
 *
 * A successful call returns the actual number of packets sent. If return value
 * is less than 'num', the remaining packets at the end of packets[] array
 * are not consumed, and the caller has to take care of them.
 *
 * @param queue        Pktio output queue handle for sending packets
 * @param packets[]    Array of packets to send
 * @param num          Number of packets to send
 *
 * @return Number of packets sent
 * @retval <0 on failure
 */
int odp_pktio_send_queue(odp_pktout_queue_t queue, odp_packet_t packets[],
			 int num);

/**
 * Return the currently configured MTU value of a packet IO interface.
 *
 * @param[in] pktio  Packet IO handle.
 *
 * @return MTU value on success
 * @retval 0 on failure
 */
uint32_t odp_pktio_mtu(odp_pktio_t pktio);

/**
 * Enable/Disable promiscuous mode on a packet IO interface.
 *
 * @param[in] pktio	Packet IO handle.
 * @param[in] enable	1 to enable, 0 to disable.
 *
 * @retval 0 on success
 * @retval <0 on failure
 */
int odp_pktio_promisc_mode_set(odp_pktio_t pktio, odp_bool_t enable);

/**
 * Determine if promiscuous mode is enabled for a packet IO interface.
 *
 * @param[in] pktio Packet IO handle.
 *
 * @retval  1 if promiscuous mode is enabled.
 * @retval  0 if promiscuous mode is disabled.
 * @retval <0 on failure
*/
int odp_pktio_promisc_mode(odp_pktio_t pktio);

/**
 * Get the default MAC address of a packet IO interface.
 *
 * @param	pktio     Packet IO handle
 * @param[out]	mac_addr  Output buffer (use ODP_PKTIO_MACADDR_MAXSIZE)
 * @param       size      Size of output buffer
 *
 * @return Number of bytes written (actual size of MAC address)
 * @retval <0 on failure
 */
int odp_pktio_mac_addr(odp_pktio_t pktio, void *mac_addr, int size);

/**
 * Setup per-port default class-of-service.
 *
 * @param[in]	pktio		Ingress port pktio handle.
 * @param[in]	default_cos	Class-of-service set to all packets arriving
 *				at this ingress port,
 *				unless overridden by subsequent
 *				header-based filters.
 *
 * @retval			0 on success
 * @retval			<0 on failure
 */
int odp_pktio_default_cos_set(odp_pktio_t pktio, odp_cos_t default_cos);

/**
 * Setup per-port error class-of-service
 *
 * @param[in]	pktio		Ingress port pktio handle.
 * @param[in]	error_cos	class-of-service set to all packets arriving
 *				at this ingress port that contain an error.
 *
 * @retval			0 on success
 * @retval			<0 on failure
 *
 * @note Optional.
 */
int odp_pktio_error_cos_set(odp_pktio_t pktio, odp_cos_t error_cos);

/**
 * Setup per-port header offset
 *
 * @param[in]	pktio		Ingress port pktio handle.
 * @param[in]	offset		Number of bytes the classifier must skip.
 *
 * @retval			0 on success
 * @retval			<0 on failure
 * @note  Optional.
 *
 */
int odp_pktio_skip_set(odp_pktio_t pktio, uint32_t offset);

/**
 * Specify per-port buffer headroom
 *
 * @param[in]	pktio		Ingress port pktio handle.
 * @param[in]	headroom	Number of bytes of space preceding
 *				packet data to reserve for use as headroom.
 *				Must not exceed the implementation
 *				defined ODP_PACKET_MAX_HEADROOM.
 *
 * @retval			0 on success
 * @retval			<0 on failure
 *
 * @note Optional.
 */
int odp_pktio_headroom_set(odp_pktio_t pktio, uint32_t headroom);

/**
 * Get printable value for an odp_pktio_t
 *
 * @param pktio   odp_pktio_t handle to be printed
 * @return     uint64_t value that can be used to print/display this
 *             handle
 *
 * @note This routine is intended to be used for diagnostic purposes
 * to enable applications to generate a printable value that represents
 * an odp_pktio_t handle.
 */
uint64_t odp_pktio_to_u64(odp_pktio_t pktio);

/**
 * Intiailize pktio params
 *
 * Initialize an odp_pktio_param_t to its default values for all fields
 *
 * @param param Address of the odp_pktio_param_t to be initialized
 */
void odp_pktio_param_init(odp_pktio_param_t *param);

/**
 * Initialize packet input queue parameters
 *
 * Initialize an odp_pktin_queue_param_t to its default values.
 *
 * @param param   Input queue parameter structure to be initialized
 */
void odp_pktin_queue_param_init(odp_pktin_queue_param_t *param);

/**
 * Initialize packet output queue parameters
 *
 * Initialize an odp_pktout_queue_param_t to its default values.
 *
 * @param param   Output queue parameter structure to be initialized
 */
void odp_pktout_queue_param_init(odp_pktout_queue_param_t *param);

/**
 * Print pktio info to the console
 *
 * Print implementation-defined pktio debug information to the console.
 *
 * @param pktio	                Packet IO handle
 */
void odp_pktio_print(odp_pktio_t pktio);

/**
 * Determine pktio link is up or down for a packet IO interface.
 *
 * @param pktio Packet IO handle.
 *
 * @retval  1 link is up
 * @retval  0 link is down
 * @retval <0 on failure
*/
int odp_pktio_link_status(odp_pktio_t pktio);

/**
 * Packet IO information
 */
typedef struct odp_pktio_info_t {
	const char       *name;  /**< Packet IO device name */
	odp_pool_t        pool;  /**< Packet pool */
	odp_pktio_param_t param; /**< Packet IO parameters */
} odp_pktio_info_t;

/**
 * Retrieve information about a pktio
 *
 * Fills in packet IO information structure with current parameter values.
 * May be called any time with a valid pktio handle. The call is not
 * synchronized with configuration changing calls. The application should
 * ensure that it does not simultaneously change the configuration and retrieve
 * it with this call. The call is not intended for fast path use. The info
 * structure is written only on success.
 *
 * @param      pktio   Packet IO handle
 * @param[out] info    Pointer to packet IO info struct for output
 *
 * @retval  0 on success
 * @retval <0 on failure
 */
int odp_pktio_info(odp_pktio_t pktio, odp_pktio_info_t *info);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
