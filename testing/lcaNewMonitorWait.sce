// Helper for the lcaTest.sce script
// Implement lcaNewMonitorWait by polling ...
function lcaNewMonitorWait(pvs)
	while (find(~lcaNewMonitorValue(pvs)))
		lcaDelay(0.01)
	end
endfunction
