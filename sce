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
  'ecdrGet']
ifile='ezca.o' 
ufiles=['ezcalib']
files = [ifile,ufiles] 
addinter(files,'ezca',ezca_funs);
