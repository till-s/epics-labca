% EPICS Channel Access Interface
% ==============================
%
%   lcaGet                   - read one or multiple EPICS PVs
%   lcaPut                   - write one or multiple EPICS PVs
%   lcaPutNoWait             - write one or multiple EPICS PVs without waiting
%                              for record processing to complete on the server
%   lcaGetNelem              - retrieve max. number of elements of EPICS PVs
%   lcaGetStatus             - read status, severity and timestamp of EPICS PVs
%   lcaGetControlLimits      - read control limits of EPICS PVs
%   lcaGetGraphicLimits      - read graphic limits of EPICS PVs
%   lcaGetPrecision          - NOT IMPLEMENTED YET (read the display precision associated with EPICS PVs)
%   lcaGetUnits              - NOT IMPLEMENTED YET (read the engineering units string associated with EPICS PVs)
%   lcaGetRetryCount         - read/set the EZCA library retry count parameter
%   lcaGetTimeout            - read/set the EZCA library timeout parameter
%   lcaDebugOn               - NOT IMPLEMENTED YET (toggle EZCA library debugging messages on/off)
%   lcaSetSeverityWarnLevel  - set the warning threshold used when reading EPICS 'VAL' PVs
%   lcaClear                 - clear (destroy/cleanup) channels
