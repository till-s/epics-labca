// Addinter for file ezca
// for hppa/sun-solaris/linux/dec
//--------------------------------
//Scilab functions 
ezca_funs=[...
  'ezcaGet';
  'ezcaGetString';
  'ezcaPut';
  'ezcaPutString';
  'ezcaGetNelem';
  'ezcaGetControlLimits';
  'ezcaGetGraphicLimits';
  'ezcaGetStatus';
  'ezcaGetPrecision';
  'ezcaGetUnits';
  'ezcaGetRetryCount';
  'ezcaSetRetryCount';
  'ezcaGetTimeout';
  'ezcaSetTimeout';
  'ezcaDebugOn';
  'ezcaDebugOff';
  'ezcaSetSeverityWarnLevel';
  'ezcaLibInit';
  'ecdrGet']
ifile='ezca.o' 
ufiles=['ecget.o' 'ezcaWrap.o'];
//ufiles=['ecget.o ezcaWrap.o -L/usr/local/epics/lib/linux-ppc -lezca -lca -lCom -lrt']
// NOTE
// NOTE LD_LIBRARY_PATH must be set to the base and extensions dirs as 
// NOTE in the ufiles line above...
// NOTE
files = [ifile,ufiles] 
addinter(files,'ezca',ezca_funs);
ezcaLibInit();
