#include "astarbox_config.h"

#define PORT_1  0
#define PORT_2  1
#define PORT_3  2
#define PORT_4  3
#define PORT_5  4
#define PORT_6  5

int parseFields(const std::string sIn, std::vector<std::string> &svFields, char cSeparator)
{
    int nErr = OK;
    std::string sSegment;

#if defined CLI_DEBUG && CLI_DEBUG >= 2
    std::cout << " [parseFields] sIn = " << sIn << std::endl << std::flush;
#endif

    if(sIn.size()==0) {
        return ERR_PARSE;
    }

    std::stringstream ssTmp(sIn);

    svFields.clear();
    // split the string into vector elements
    while(std::getline(ssTmp, sSegment, cSeparator)) {
#if defined CLI_DEBUG && CLI_DEBUG >= 3
        std::cout << "["<<getTimeStamp()<<"]"<< " [parseFields] sSegment = " << sSegment << std::endl << std::flush;
#endif
        svFields.push_back(sSegment);
    }

    if(svFields.size()==0) {
        return ERR_PARSE;
    }
#if defined CLI_DEBUG && CLI_DEBUG >= 2
    std::cout << " [parseFields] Done all good." << std::endl << std::flush;
#endif

    return nErr;
}

int loadBootStates(std::vector<int> &bootStates)
{
    int nErr = OK;
    std::string config;
    std::ifstream configFile;
    std::string sTmp;

    std::vector<std::string> sBootStates;
    int i;
    int nbRead = 0;
    

    configFile.open("/etc/astarbox.conf", std::ifstream::in);
    if(!configFile.is_open()) {
#if defined CLI_DEBUG && CLI_DEBUG >= 2
        std::cout << " [loadBootStates] Error opening config file" << std::endl << std::flush;
        return ERR_FILE;
#endif
    }

    config.clear();
    while (std::getline(configFile, sTmp)) {
        config.append(sTmp);
    }
    configFile.close();
#if defined CLI_DEBUG && CLI_DEBUG >= 2
    std::cout << " [loadBootStates] Port config : " << config << std::endl << std::flush;
#endif
    
    nErr = parseFields(config, sBootStates, ':');
    if(nErr)
        return nErr;

    bootStates.clear();
    for(i=0; i< sBootStates.size(); i++)
        bootStates.push_back(std::stoi(sBootStates[i]));

    if(bootStates.size() < 6) {
#if defined CLI_DEBUG && CLI_DEBUG >= 2
        std::cout << " [loadBootStates] Boot config  size incorrect : " << bootStates.size() << std::endl << std::flush;
#endif
        return ERR_PARSE;
    }

#if defined CLI_DEBUG && CLI_DEBUG >= 2
    std::cout << " [loadBootStates] bootStates[PORT_1] : " << bootStates[PORT_1] << std::endl;
    std::cout << " [loadBootStates] bootStates[PORT_2] : " << bootStates[PORT_2] << std::endl;
    std::cout << " [loadBootStates] bootStates[PORT_3] : " << bootStates[PORT_3] << std::endl;
    std::cout << " [loadBootStates] bootStates[PORT_4] : " << bootStates[PORT_4] << std::endl;
    std::cout << " [loadBootStates] bootStates[PORT_5] : " << bootStates[PORT_5] << std::endl;
    std::cout << " [loadBootStates] bootStates[PORT_6] : " << bootStates[PORT_6] << std::endl << std::flush;
#endif

    return nErr;
}

int main ()
{
    int nErr = 0;
    std::vector<int> bootStates;

    std::cout << "Configuring AStarBox PCA9685." << std::endl << std::flush;
    //make sure you use the right address values.
    PCA9685 pca;
    pca.init(1,0x40);
    nErr = pca.reset();
    if(nErr) {
        std::cout << "Error opening i2C device "<< std::uppercase << std::setfill('0') << std::setw(2) << std::hex << 0x40 << std::endl << std::flush;
        return -1;
    }
    std::cout << "init done." << std::endl << std::flush;
    std::cout << "Setting PWM frequency to 1000 Hz." << std::endl << std::flush;
    nErr = pca.setPWMFreq(1000);
    if(nErr) {
        std::cout << "Error setting PC9685 frequency" << std::endl << std::flush;
        return -1;
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "loading default port config from /etc/astarbox.conf" << std::endl << std::flush;
    nErr = loadBootStates(bootStates);
    if(nErr) {
        std::cout << "No config found." << std::endl << std::flush;
        return -1;
    }

#if defined CLI_DEBUG && CLI_DEBUG >= 2
    std::cout << " [] bootStates[PORT_1] : " << bootStates[PORT_1] << std::endl;
    std::cout << " [] bootStates[PORT_2] : " << bootStates[PORT_2] << std::endl;
    std::cout << " [] bootStates[PORT_3] : " << bootStates[PORT_3] << std::endl;
    std::cout << " [] bootStates[PORT_4] : " << bootStates[PORT_4] << std::endl;
    std::cout << " [] bootStates[PORT_5] : " << bootStates[PORT_5] << std::endl;
    std::cout << " [] bootStates[PORT_6] : " << bootStates[PORT_6] << std::endl << std::flush;
#endif


    if(bootStates[PORT_1] == 1) {
        std::cout << "seting port 1 to on" << std::endl;
        pca.setOn(PORT_1);
    } else {
        std::cout << "seting port 1 to off" << std::endl;
        pca.setOff(PORT_1);
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if(bootStates[PORT_2] == 1) {
        std::cout << "seting port 2 to on" << std::endl;
        pca.setOn(PORT_2);
    } else {
        std::cout << "seting port 2 to off" << std::endl;
        pca.setOff(PORT_2);
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if(bootStates[PORT_3] == 1) {
        std::cout << "seting port 3 to on" << std::endl;
        pca.setOn(PORT_3);
    } else {
        std::cout << "seting port 3 to off" << std::endl;
        pca.setOff(PORT_3);
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if(bootStates[PORT_4] == 1) {
        std::cout << "seting port 4 to on" << std::endl;
        pca.setOn(PORT_4);
    } else {
        std::cout << "seting port 4 to off" << std::endl;
        pca.setOff(PORT_4);
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
    std::cout << "seting pwm1 to " << bootStates[PORT_5] << std::endl;
    pca.setPWM(PORT_5, bootStates[PORT_5]);

    std::cout << "seting pwm2 to " << bootStates[PORT_6] << std::endl;
    pca.setPWM(PORT_6, bootStates[PORT_6]);

    return nErr;
}
