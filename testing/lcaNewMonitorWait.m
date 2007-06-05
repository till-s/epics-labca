% Helper for the lcaTest.m script
% Implement lcaNewMonitorWait by polling ...
function lcaNewMonitorWait(pvs)
	while (find(~lcaNewMonitorValue(pvs)))
		lcaDelay(0.01)
	end
end
