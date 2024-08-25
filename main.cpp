#include <stdio.h>
#include "main.h"
#include "x2powercontrol.h"
#include "../../licensedinterfaces/basicstringinterface.h"

extern "C" PlugInExport int sbPlugInName2(BasicStringInterface& str)
{
	str = "AStarBox X2PowerControl Plugin";

	return 0;
}

extern "C" PlugInExport int sbPlugInFactory2(	const char* pszDisplayName,
                                                const int& nInstanceIndex,
                                                SerXInterface					* pSerXIn,
                                                TheSkyXFacadeForDriversInterface	* pTheSkyXIn,
                                                SleeperInterface					* pSleeperIn,
                                                BasicIniUtilInterface			* pIniUtilIn,
                                                LoggerInterface					* pLoggerIn,
                                                MutexInterface					* pIOMutexIn,
                                                TickCountInterface				* pTickCountIn,
                                                void** ppObjectOut)
{
    *ppObjectOut = NULL;
    X2PowerControl* gpMyImpl=NULL;

    if (NULL == gpMyImpl)
		gpMyImpl = new X2PowerControl(	pszDisplayName,
                                        nInstanceIndex,
                                        pSerXIn,
                                        pTheSkyXIn,
                                        pSleeperIn,
                                        pIniUtilIn,
                                        pLoggerIn,
                                        pIOMutexIn,
                                        pTickCountIn);

    *ppObjectOut = gpMyImpl;

    return 0;
}

