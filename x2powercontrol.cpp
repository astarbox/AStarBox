#include "x2powercontrol.h"

X2PowerControl::X2PowerControl(const char* pszDisplayName,
										const int& nInstanceIndex,
										SerXInterface						* pSerXIn,
										TheSkyXFacadeForDriversInterface	* pTheSkyXIn,
										SleeperInterface					* pSleeperIn,
										BasicIniUtilInterface				* pIniUtilIn,
										LoggerInterface						* pLoggerIn,
										MutexInterface						* pIOMutexIn,
										TickCountInterface					* pTickCountIn):m_bLinked(0)
{
    char portName[255];
	int nErr = SB_OK;
	std::string sLabel;
    int i;
    int nTotalPorts = 0;

    m_bLinked = false;
	m_pTheSkyXForMounts = pTheSkyXIn;
	m_pIniUtil = pIniUtilIn;
	m_pIOMutex = pIOMutexIn;
	m_pTickCount = pTickCountIn;

	m_nISIndex = nInstanceIndex;

    if (m_pIniUtil) {
        m_nPwm1 = m_pIniUtil->readInt(PLUGIN_KEY, CONFIG_KEY_PWM1, -1);
        m_nPwm2 = m_pIniUtil->readInt(PLUGIN_KEY, CONFIG_KEY_PWM2, -1);
        if(m_nPwm1 != -1)
            m_AStarBoxPorts.setPortPWMDutyCyclePercent(PWM_1,m_nPwm1); // these might be different from the current one
        else {
            // read the current value as we have not saved one
            m_AStarBoxPorts.connect();
            m_AStarBoxPorts.getPortPWMDutyCyclePercent(PWM_1, m_nPwm1);
            m_AStarBoxPorts.disconnect();
            if(m_nPwm1==0) {// port is off and we don't know anything about it so use 50%
                m_nPwm1 = 50;
                m_AStarBoxPorts.setPortPWMDutyCyclePercent(PWM_1,m_nPwm1);
            }
        }
        if(m_nPwm2 != -1)
            m_AStarBoxPorts.setPortPWMDutyCyclePercent(PWM_2,m_nPwm2); // these might be different from the current one
        else {
            // read the current value as we have not saved one
            m_AStarBoxPorts.connect();
            m_AStarBoxPorts.getPortPWMDutyCyclePercent(PWM_2, m_nPwm2);
            m_AStarBoxPorts.disconnect();
            if(m_nPwm2==0) {// port is off and we don't know anything about it so use 50%
                m_nPwm2 = 50;
                m_AStarBoxPorts.setPortPWMDutyCyclePercent(PWM_1,m_nPwm2);
            }
        }
        // load port names
        for(i=0; i<nTotalPorts; i++) {
            // set some defaults
            if(i>3)
                sLabel = "AStarBox PWM" + std::to_string(i-3);
            else
                sLabel = "AStarBox port " + std::to_string(i+1);
            m_pIniUtil->readString(PLUGIN_KEY, m_IniPortKey[i].c_str(), sLabel.c_str(), portName, 255);
            m_sPortNames.push_back(std::string(portName));
        }
    }
    
    nErr = m_AStarBoxPorts.openAllPorts();
	if(nErr) {
		return;
	}

    m_bMPC3421Present = m_AStarBoxPorts.isMCP3421Present();
    nTotalPorts = m_AStarBoxPorts.getPortCount();
    
    m_AStarBoxPorts.getPortPWMDutyCyclePercent(PWM_1, m_nPwm1);
    m_AStarBoxPorts.getPortPWMDutyCyclePercent(PWM_2, m_nPwm2);
    // load port names
    for(i=0; i<nTotalPorts; i++) {
        if(i>3)
            sLabel = "AStarBox PWM" + std::to_string(i-3);
        else
            sLabel = "AStarBox port " + std::to_string(i+1);
        m_pIniUtil->readString(PLUGIN_KEY, m_IniPortKey[i].c_str(), sLabel.c_str(), portName, 255);
        m_sPortNames.push_back(std::string(portName));
    }
}

X2PowerControl::~X2PowerControl()
{
	//Delete objects used through composition
	if (GetTheSkyXFacadeForDrivers())
		delete GetTheSkyXFacadeForDrivers();
	if (GetSimpleIniUtil())
		delete GetSimpleIniUtil();
	if (GetMutex())
		delete GetMutex();
}

int X2PowerControl::establishLink(void)
{
	int nErr = SB_OK;

    m_AStarBoxPorts.connect();
    
    m_AStarBoxPorts.setPortPWMDutyCyclePercent(PWM_1, m_nPwm1);
    m_AStarBoxPorts.setPortPWMDutyCyclePercent(PWM_2, m_nPwm2);
    
	m_bLinked = true;

	return nErr;
}

int X2PowerControl::terminateLink(void)
{
    m_AStarBoxPorts.disconnect();
	m_bLinked = false;

    return SB_OK;
}

bool X2PowerControl::isLinked() const
{
	return m_bLinked;
}


void X2PowerControl::deviceInfoNameShort(BasicStringInterface& str) const
{
        str = "AStarBox";
}
void X2PowerControl::deviceInfoNameLong(BasicStringInterface& str) const
{
        str = "AStarBox";
}
void X2PowerControl::deviceInfoDetailedDescription(BasicStringInterface& str) const
{
    str = "AStarBox power port control";
}
void X2PowerControl::deviceInfoFirmwareVersion(BasicStringInterface& str)
{
	str = "None";
}
void X2PowerControl::deviceInfoModel(BasicStringInterface& str)
{
        str = "AStarBox";
}

void X2PowerControl::driverInfoDetailedInfo(BasicStringInterface& str) const
{
    str = "AStarBox X2 plugin by Rodolphe Pineau";
}

double X2PowerControl::driverInfoVersion(void) const
{
	return PLUGIN_VERSION;
}

int X2PowerControl::queryAbstraction(const char* pszName, void** ppVal)
{
	*ppVal = NULL;

    if (!strcmp(pszName, ModalSettingsDialogInterface_Name))
        *ppVal = dynamic_cast<ModalSettingsDialogInterface*>(this);
    else if (!strcmp(pszName, X2GUIEventInterface_Name))
        *ppVal = dynamic_cast<X2GUIEventInterface*>(this);
    else if (!strcmp(pszName, CircuitLabelsInterface_Name))
        *ppVal = dynamic_cast<CircuitLabelsInterface*>(this);
    else if (!strcmp(pszName, SetCircuitLabelsInterface_Name))
        *ppVal = dynamic_cast<SetCircuitLabelsInterface*>(this);

	return 0;
}

#pragma mark - UI binding

int X2PowerControl::execModalSettingsDialog()
{
    int nErr = SB_OK;
    X2ModalUIUtil               uiutil(this, GetTheSkyXFacadeForDrivers());
    X2GUIInterface*             ui = uiutil.X2UI();
    X2GUIExchangeInterface*     dx = NULL;//Comes after ui is loaded
    bool    bPressedOK = false;
    bool    bOn;
    int     nPercent;
    std::vector<int> bootStates;
    std::stringstream ssTmp;
    double dVolts;
    
    if (NULL == ui)
        return ERR_POINTER;

    if ((nErr = ui->loadUserInterface("AStarBox.ui", deviceType(), m_nISIndex)))
        return nErr;

    if (NULL == (dx = uiutil.X2DX()))
        return ERR_POINTER;

    nErr = m_AStarBoxPorts.loadBootStates(bootStates);
    if(nErr)
        return ERR_CMDFAILED;

    if(m_bLinked) {
        dx->setEnabled("pushButton",true);
        dx->setEnabled("pushButton_2",true);
    }
    else {
        dx->setEnabled("pushButton",false);
        dx->setEnabled("pushButton_2",false);
    }

    // these are boot state, doesn't matter if connected or not
    bOn = (bootStates[PORT_1] == 1)?true:false;
    dx->setChecked("checkBox",bOn);

    bOn = (bootStates[PORT_2] == 1)?true:false;
    dx->setChecked("checkBox_2",bOn);

    bOn = (bootStates[PORT_3] == 1)?true:false;
    dx->setChecked("checkBox_3",bOn);

    bOn = (bootStates[PORT_4] == 1)?true:false;
    dx->setChecked("checkBox_4",bOn);

    bOn = (bootStates[PORT_PWM1] >= 1)?true:false;
    dx->setChecked("checkBox_5",bOn);

    m_AStarBoxPorts.getPortPWMDutyCyclePercent(PWM_1, nPercent);
    dx->setPropertyInt("pwm5","value", nPercent);

    bOn = (bootStates[PORT_PWM2] >= 1)?true:false;
    dx->setChecked("checkBox_6",bOn);

    m_AStarBoxPorts.getPortPWMDutyCyclePercent(PWM_2, nPercent);
    dx->setPropertyInt("pwm6","value", nPercent);

    if(m_bMPC3421Present) {
        dVolts = m_AStarBoxPorts.getVoltage();
        ssTmp  << "Input voltage : " << std::fixed << std::setprecision(2) << dVolts <<" V";
        dx->setPropertyString("voltage","text", ssTmp.str().c_str());
    }
    else {
        dx->setPropertyString("voltage","text", "");
    }

    //Display the user interface
    if ((nErr = ui->exec(bPressedOK)))
        return nErr;

    //Retrieve values from the user interface
    if (bPressedOK) {
        dx->propertyInt("pwm5", "value", m_nPwm1);
        dx->propertyInt("pwm6", "value", m_nPwm2);
        // save the values to persistent storage
        nErr |= m_pIniUtil->writeInt(PLUGIN_KEY, CONFIG_KEY_PWM1, m_nPwm1);
        nErr |= m_pIniUtil->writeInt(PLUGIN_KEY, CONFIG_KEY_PWM2, m_nPwm2);

        bootStates.clear();
        bootStates.push_back(dx->isChecked("checkBox")?1:0);
        bootStates.push_back(dx->isChecked("checkBox_2")?1:0);
        bootStates.push_back(dx->isChecked("checkBox_3")?1:0);
        bootStates.push_back(dx->isChecked("checkBox_4")?1:0);
        bootStates.push_back(dx->isChecked("checkBox_5")?1:0);
        bootStates.push_back(dx->isChecked("checkBox_6")?1:0);
        nErr = m_AStarBoxPorts.saveBootStates(bootStates);
        if(nErr)
            return ERR_CMDFAILED;
    }
    return nErr;

}

void X2PowerControl::uiEvent(X2GUIExchangeInterface* uiex, const char* pszEvent)
{
    int nErr;
    int m_nPwmPort;
    double dVolts;
    std::stringstream ssTmp;

    if (!strcmp(pszEvent, "on_timer")) {
        if(m_bMPC3421Present) {
            dVolts = m_AStarBoxPorts.getVoltage();
            ssTmp  << "Input voltage : " << std::fixed << std::setprecision(2) << dVolts <<" V";
            uiex->setPropertyString("voltage","text", ssTmp.str().c_str());
        }
    }

    else if (!strcmp(pszEvent, "on_pushButton_clicked")) {
        uiex->propertyInt("pwm5", "value", m_nPwmPort);
        nErr = m_AStarBoxPorts.setPortPWMDutyCyclePercent(PWM_1, m_nPwmPort);
    }

    else if (!strcmp(pszEvent, "on_pushButton_2_clicked")) {
        uiex->propertyInt("pwm6", "value", m_nPwmPort);
        nErr = m_AStarBoxPorts.setPortPWMDutyCyclePercent(PWM_2, m_nPwmPort);
    }
}

int X2PowerControl::numberOfCircuits(int& nNumber)
{
	nNumber = m_AStarBoxPorts.getPortCount();
	return 0;
}

int X2PowerControl::circuitState(const int& nIndex, bool& bZeroForOffOneForOn)
{
	int nErr = SB_OK;
    int nPortIndex;

    if(!m_bLinked)
        return ERR_NOLINK;


    if (nIndex >= 0 && nIndex<m_AStarBoxPorts.getPortCount()) {
        nPortIndex = nIndex+1;
        nErr = m_AStarBoxPorts.getPortStatus(nPortIndex, bZeroForOffOneForOn);
    }
	else
		nErr = ERR_INDEX_OUT_OF_RANGE;

	return nErr;
}

int X2PowerControl::setCircuitState(const int& nIndex, const bool& bZeroForOffOneForOn)
{
	int nErr = SB_OK;
    int nPortIndex;

	if(!m_bLinked)
        return ERR_NOLINK;


    if (nIndex >= 0 && nIndex < m_AStarBoxPorts.getPortCount()) {
        nPortIndex = nIndex+1;
        m_AStarBoxPorts.setPort(nPortIndex, bZeroForOffOneForOn);
    }
	else
		nErr = ERR_INDEX_OUT_OF_RANGE;

	return nErr;
}

int X2PowerControl::circuitLabel(const int &nZeroBasedIndex, BasicStringInterface &str)
{
    int nErr = SB_OK;
    std::string sLabel;
    if(m_sPortNames.size() >= nZeroBasedIndex+1) {
        str = m_sPortNames[nZeroBasedIndex].c_str();
    }
    else {
            sLabel = "AStarBox port " + std::to_string(nZeroBasedIndex+1);
    }

    return nErr;
}

int X2PowerControl::setCircuitLabel(const int &nZeroBasedIndex, const char *str)
{
    int nErr = SB_OK;

    if(m_sPortNames.size() >= nZeroBasedIndex+1) {
        m_sPortNames[nZeroBasedIndex] = str;
        m_pIniUtil->writeString(PLUGIN_KEY, m_IniPortKey[nZeroBasedIndex].c_str(), str);
    }
    else {
        nErr = ERR_CMDFAILED;
    }
    return nErr;
}
