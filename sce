// Addinter for file ezca
// for hppa/sun-solaris/linux/dec
//--------------------------------
//Scilab functions 
ezca_funs=[...
  'ezcaGet';
  'ezcaPut';
  'ezcaGetNelem';
  'ezcaGetControlLimits';
  'ezcaGetGraphicLimits';
  'ezcaGetStatus';
  'ezcaDebugOn';
  'ezcaLibInit';
  'ecdrGet']
ifile='ezca.o' 
ufiles=['ecget.o ezcaWrap.o -L/afs/slac/package/epics/R3.13.6/extensions/lib/Linux -L/afs/slac/package/epics/R3.13.6/base/lib/Linux -lezca -lCom']
// NOTE
// NOTE LD_LIBRARY_PATH must be set to the base and extensions dirs as 
// NOTE in the ufiles line above...
// NOTE
//ufiles=[]
files = [ifile,ufiles] 
addinter(files,'ezca',ezca_funs);
ezcaLibInit();
