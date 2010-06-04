function lcaDelay
//Delay execution of scilab/matlab for the specified time to handle CA.
//  Calling Sequence
//lcaDelay(timeout)
//  Description
//   Delay execution of scilab or matlab for the specified time to handle
//   channel access activity (monitors). Using this call is not needed under
//   EPICS-3.14 since monitors are transparently handled by separate
//   threads. These ``worker threads'' receive data from CA on monitored
//   channels ``in the background'' while scilab/matlab are processing
//   arbitrary calculations. You only need to either poll the library for
//   the data being ready using the <link linkend='lcaNewMonitorValue'>lcaNewMonitorValue()</link> routine or
//   block for data becoming available using <link linkend='lcaNewMonitorWait'>lcaNewMonitorWait()</link>.
//  Parameters
//   timeout: A timeout value in seconds.
//  See also
//   lcaNewMonitorValue
//   lcaNewMonitorWait
endfunction
