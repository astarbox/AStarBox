#include <string.h>

#include "../../licensedinterfaces/theskyxfacadefordriversinterface.h"
#include "../../licensedinterfaces/sleeperinterface.h"
#include "../../licensedinterfaces/loggerinterface.h"
#include "../../licensedinterfaces/basiciniutilinterface.h"
#include "../../licensedinterfaces/mutexinterface.h"
#include "../../licensedinterfaces/basicstringinterface.h"
#include "../../licensedinterfaces/tickcountinterface.h"
#include "../../licensedinterfaces/modalsettingsdialoginterface.h"
#include "../../licensedinterfaces/x2guiinterface.h"
#include "../../licensedinterfaces/powercontroldriverinterface.h"
#include "../../licensedinterfaces/sberrorx.h"
#include "../../licensedinterfaces/circuitlabels.h"
#include "../../licensedinterfaces/setcircuitlabels.h"

#include "AStarBox.h"

// Forward declare the interfaces that this device is dependent upon
class BasicStringInterface;
class SerXInterface;
class TheSkyXFacadeForDriversInterface;
class SleeperInterface;
class BasicIniUtilInterface;
class LoggerInterface;
class MutexInterface;
class TickCountInterface;
class CircuitLabelsInterface;

#define PLUGIN_KEY            "AStarBox"

#define CONFIG_KEY_PWM1       "PWM1_DUTY"
#define CONFIG_KEY_PWM2       "PWM1_DUTY"
#define CONFIG_KEY_PORT1_NAME "PORT1_NAME"
#define CONFIG_KEY_PORT2_NAME "PORT2_NAME"
#define CONFIG_KEY_PORT3_NAME "PORT3_NAME"
#define CONFIG_KEY_PORT4_NAME "PORT4_NAME"
#define CONFIG_KEY_PORT5_NAME "PORT5_NAME"
#define CONFIG_KEY_PORT6_NAME "PORT6_NAME"

/*!
\brief The X2PowerControl example.

\ingroup Example

Use this example as a template to write an X2PowerControl driver.
*/
class X2PowerControl : public PowerControlDriverInterface, public ModalSettingsDialogInterface, public X2GUIEventInterface, public CircuitLabelsInterface, public SetCircuitLabelsInterface
{
public:
	X2PowerControl( const char* pszDisplayName,
                    const int& nInstanceIndex,
                    SerXInterface						* pSerXIn,
                    TheSkyXFacadeForDriversInterface	* pTheSkyXIn,
                    SleeperInterface					* pSleeperIn,
                    BasicIniUtilInterface				* pIniUtilIn,
                    LoggerInterface						* pLoggerIn,
                    MutexInterface						* pIOMutexIn,
                    TickCountInterface					* pTickCountIn);

    virtual ~X2PowerControl();


public:

	/*!\name DriverRootInterface Implementation
	See DriverRootInterface.*/
	//@{
	virtual DeviceType  deviceType(void) {return DriverRootInterface::DT_POWERCONTROL;}
	virtual int         queryAbstraction(const char* pszName, void** ppVal);
	//@}

	/*!\name DriverInfoInterface Implementation
	See DriverInfoInterface.*/
	//@{
	virtual void        driverInfoDetailedInfo(BasicStringInterface& str) const;
	virtual double      driverInfoVersion(void) const;
	//@}

	/*!\name HardwareInfoInterface Implementation
	See HardwareInfoInterface.*/
	//@{
	virtual void deviceInfoNameShort(BasicStringInterface& str) const;
	virtual void deviceInfoNameLong(BasicStringInterface& str) const;
	virtual void deviceInfoDetailedDescription(BasicStringInterface& str) const;
	virtual void deviceInfoFirmwareVersion(BasicStringInterface& str);
	virtual void deviceInfoModel(BasicStringInterface& str);
	//@}

	/*!\name LinkInterface Implementation
	See LinkInterface.*/
	//@{
	virtual int		establishLink(void);
	virtual int		terminateLink(void);
	virtual bool	isLinked(void) const;
	//@}

	virtual int		initModalSettingsDialog(void){return 0;}
	virtual int		execModalSettingsDialog(void);
	virtual void	uiEvent(X2GUIExchangeInterface*, const char*);

	//PowerControlDriverInterface
	virtual int		numberOfCircuits(int& nNumber);
	virtual int		circuitState(const int& nIndex, bool& bZeroForOffOneForOn);
	virtual int		setCircuitState(const int& nIndex, const bool& bZeroForOffOneForOn);

    virtual int     circuitLabel (const int &nZeroBasedIndex, BasicStringInterface &str);
    virtual int     setCircuitLabel	(const int &nZeroBasedIndex, const char *str);

private:
	TheSkyXFacadeForDriversInterface*	m_pTheSkyXForMounts;

	//Standard device driver tools
	BasicIniUtilInterface*              m_pIniUtil;
	mutable MutexInterface*             m_pIOMutex;
	TickCountInterface*                 m_pTickCount;

	TheSkyXFacadeForDriversInterface    *GetTheSkyXFacadeForDrivers() { return m_pTheSkyXForMounts; }
	BasicIniUtilInterface               *GetSimpleIniUtil() { return m_pIniUtil; }
	MutexInterface                      *GetMutex() { return m_pIOMutex; }
	TickCountInterface                  *GetTickCountInterface() { return m_pTickCount; }

	bool    m_bLinked;
	int	    m_nISIndex;

	int	    m_nPwm1;
	int	    m_nPwm2;
    bool    m_bMPC3421Present;
    
    CAStarBoxPowerPorts	m_AStarBoxPorts;


    std::vector<std::string>    m_sPortNames;
    std::vector<std::string>    m_IniPortKey = {CONFIG_KEY_PORT1_NAME, CONFIG_KEY_PORT2_NAME, CONFIG_KEY_PORT3_NAME, CONFIG_KEY_PORT4_NAME,
                                                CONFIG_KEY_PORT5_NAME, CONFIG_KEY_PORT6_NAME};
};
