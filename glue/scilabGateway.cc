#include <scilabGateway.h>
#include <sciclean.h>
#include <multiEzcaCtrlC.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <c_gateway_prototype.h>
#include <addfunction.h>
#ifdef __cplusplus
}
#endif

static int
labca_trampoline(char *fname, PvApiCtxType pvApiCtx, ScicleanGatefunc gate)
{
int           rval;
CtrlCStateRec save;

	multi_ezca_ctrlC_prologue(&save);

	rval = sciclean_trampoline(fname, pvApiCtx, gate);

	multi_ezca_ctrlC_epilogue(&save);

	return rval;
}

template <ScicleanGatefunc gate> int
labca_gateway(char *fname, void *uarg)
{
	return labca_trampoline(fname, uarg, gate);
}

typedef int (*Myinterfun)(char*, PvApiCtxType );

struct { Myinterfun f ;  const wchar_t *name; } Tab[]={
	{labca_gateway<intsezcaGet>,					L"lcaGet"},
	{labca_gateway<intsezcaPut>,					L"lcaPut"},
	{labca_gateway<intsezcaPutNoWait>,				L"lcaPutNoWait"},
	{labca_gateway<intsezcaGetNelem>,				L"lcaGetNelem"},
	{labca_gateway<intsezcaGetControlLimits>,		L"lcaGetControlLimits"},
	{labca_gateway<intsezcaGetGraphicLimits>,		L"lcaGetGraphicLimits"},
	{labca_gateway<intsezcaGetWarnLimits>,		    L"lcaGetWarnLimits"},
	{labca_gateway<intsezcaGetAlarmLimits>,			L"lcaGetAlarmLimits"},
	{labca_gateway<intsezcaGetStatus>,				L"lcaGetStatus"},
	{labca_gateway<intsezcaGetPrecision>,			L"lcaGetPrecision"},
	{labca_gateway<intsezcaGetUnits>,				L"lcaGetUnits"},
	{labca_gateway<intsezcaGetEnumStrings>,			L"lcaGetEnumStrings"},
	{labca_gateway<intsezcaGetRetryCount>,			L"lcaGetRetryCount"},
	{labca_gateway<intsezcaSetRetryCount>,			L"lcaSetRetryCount"},
	{labca_gateway<intsezcaGetTimeout>,				L"lcaGetTimeout"},
	{labca_gateway<intsezcaSetTimeout>,				L"lcaSetTimeout"},
	{labca_gateway<intsezcaDebugOn>,				L"lcaDebugOn"},
	{labca_gateway<intsezcaDebugOff>,				L"lcaDebugOff"},
	{labca_gateway<intsezcaSetSeverityWarnLevel>,	L"lcaSetSeverityWarnLevel"},
	{labca_gateway<intsezcaClearChannels>,			L"lcaClear"},
	{labca_gateway<intsezcaSetMonitor>,				L"lcaSetMonitor"},
	{labca_gateway<intsezcaNewMonitorValue>,	    L"lcaNewMonitorValue"},
	{labca_gateway<intsezcaNewMonitorWait>,			L"lcaNewMonitorWait"},
	{labca_gateway<intsezcaDelay>,	    			L"lcaDelay"},
	{labca_gateway<intsezcaLastError>,    			L"lcaLastError"},
};

int
labCA(wchar_t *funcName)
{
unsigned i;
	for ( i=0; i < sizeof(Tab)/sizeof(Tab[0]); i++ ) {
		if ( wcscmp( funcName, Tab[i].name ) == 0 ) {
			addCStackFunction( Tab[i].name, Tab[i].f, L"labca" );
			return 1;
		}
	}
	return 0;
}
