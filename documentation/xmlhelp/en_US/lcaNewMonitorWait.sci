function lcaNewMonitorWait
// Block until a set of monitored PVs have fresh data
//  Calling Sequence
//lcaNewMonitorValue(pvs, type)
//  Description
//   Similar to <link linkend='lcaNewMonitorValue'>lcaNewMonitorValue</link>
//   but instead of returning the status of
//   monitored PVs this routine blocks until all PVs have fresh data
//   available (e.g., due to initial connection or changes in value and/or
//   severity status). Reading the actual data must be done using
//   <link linkend='lcaGet'>lcaGet</link>.
//  Parameters
//   pvs: Column vector (in matlab: m x 1 cell- matrix) of m strings.
//   type: (optional argument) A string specifying the data type to be used for the channel access data transfer. The native type is used by default. See <link linkend='lca_common_type_arg'>here</link> for more information.  <note> Monitors are specific to a particular data type and therefore lcaNewMonitorWait will only report the status for a monitor that had been established (by <link linkend='lcaSetMonitor'>lcaSetMonitor</link>) with a matching type. Using the ``native'' type, which is the default, for both calls satisfies this condition.  </note>
//  Examples
//
//try lcaNewMonitorWait(pvs)
//        vals = lcaGet(pvs)
//catch
//        errs = lcaLastError()
//        handleErrors(errs)
//end
//
//  See also
//   lcaNewMonitorValue
//   lcaGet
//   lcaSetMonitor
endfunction
