function lcaSetMonitor
// Establish a monitor on a set of PVs
//  Calling Sequence
//lcaSetMonitor(pvs, nmax, type)
//  Description
//   Set a ``monitor'' on a set of PVs. Monitored PVs are automatically
//   retrieved every time their value or status changes. Monitors are
//   especially useful under EPICS-3.14 which supports multiple threads.
//   EPICS-3.14 transparently reads monitored PVs as needed. Older, single
//   threaded versions of EPICS require periodic calls to labCA e.g., to
//   <link linkend='lcaDelay'>lcaDelay</link>, in order to allow labCA to
//   handle monitors.
//
//   Use the <link linkend='lcaNewMonitorValue'>lcaNewMonitorValue</link>
//   call to check monitor status (local flag)
//   or <link linkend='lcaNewMonitorWait'>lcaNewMonitorWait</link>
//   to wait for new data to become available (since
//   last lcaGet or lcaSetMonitor). If new data are available, they are
//   retrieved using the ordinary <link linkend='lcaGet'>lcaGet</link> call.
//
//   <note>
//   Note the difference between polling and monitoring a PV in combination
//   with polling the local monitor status flag
//   (<link linkend='lcaNewMonitorValue'>lcaNewMonitorValue</link>). In
//   the first case, remote data are fetched on every polling cycle whereas
//   in the second case, data are transferred only when they change. Also,
//   in the monitored case, lcaGet reads from a local buffer rather than
//   from the network. It is most convenient however to wait for monitored
//   data to arrive using <link linkend='lcaNewMonitorWait'>lcaNewMonitorWait</link> rather than polling.
//   </note>
//
//   There is currently no possibility to selectively remove a monitor. Use
//   the <link linkend='lcaClear'>lcaClear</link>
//   call to disconnect a channel and as a side-effect,
//   remove all monitors on that channel. Future access to a cleared channel
//   simply reestablishes a connection (but no monitors).
//  Parameters
//   pvs: Column vector (in matlab: m x 1 cell- matrix) of m strings.
//   nmax: (optional argument) Maximum number of elements (per PV) to monitor/retrieve. If set to 0 (default), all elements are fetched. See <link linkend='lcaGet'>here ('nmax' argument)</link> for more information.  <note> A subsequent <link linkend='lcaGet'>lcaGet</link> must specify a nmax argument equal or less than the number given to lcaSetMonitor -- otherwise the lcaGet operation results in fetching a new set of data from the server because the lcaGet request cannot be satisfied using the copy locally cached by the monitor-thread.</note>
//   type: (optional argument) A string specifying the data type to be used for the channel access data transfer. The native type is used by default. See <link linkend='lca_common_type_arg'>here</link> for more information.  <para> The type specified for the subsequent lcaGet for retrieving the data should match the monitor's data type. Otherwise, lcaGet will fetch a new copy from the server instead of using the data that was already transferred as a result of the monitoring.  </para>
//  Examples
//
//lcaSetMonitor('PV')
//// monitor 'PV'. Reduce network traffic by just have the
//// library retrieve the first 20 elements. Use DBR_SHORT
//// for transfer.
//lcaSetMonitor('PV', 20, 's')
//References
//   lcaDelay
//   lcaNewMonitorValue
//   lcaNewMonitorWait
//   lcaGet
//   lcaClear
endfunction
