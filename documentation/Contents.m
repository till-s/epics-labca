% EPICS Channel Access Interface
% ==============================
%
%   ezcaGet                   - read one or multiple EPICS PVs
%   ezcaPut                   - write one or multiple EPICS PVs
%   ezcaGetNelem              - retrieve max. number of elements of EPICS PVs
%   ezcaGetStatus             - read status, severity and timestamp of EPICS PVs
%   ezcaGetControlLimits      - read control limits of EPICS PVs
%   ezcaGetGraphicLimits      - read graphic limits of EPICS PVs
%   ezcaGetPrecision          - read the display precision associated with EPICS PVs
%   ezcaGetUnits              - read the engineering units string associated with EPICS PVs
%   ezcaGetRetryCount         - read/set the EZCA library retry count parameter
%   ezcaGetTimeout            - read/set the EZCA library timeout parameter
%   ezcaDebugOn               - toggle EZCA library debugging messages on/off
%   ezcaSetSeverityWarnLevel  - set the warning threshold used when reading EPICS 'VAL' PVs