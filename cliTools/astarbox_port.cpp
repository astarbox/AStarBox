#include "astarbox_port.h"


void usage()
{
    std::cout << "Usage : astarbox_port <port number> <value>" << std::endl;
    std::cout << "   port number from 1 to 4 or pwm1, pwm2" << std::endl;
    std::cout << "   value is on or off for power port, a percentage for pwm" << std::endl;
    std::cout << "   example : " << std::endl;
    std::cout << "     astarbox_port 1 on" << std::endl;
    std::cout << "     astarbox_port pwm1 50" << std::endl<< std::flush;
}

int main (int argc, char **argv)
{
    int nErr = 0;
    int nPort = 0;
    std::string sPort;
    int nPWM = 0;
    int nPWMPercent = 0;
    std::string sState;


    if(argc < 3) {
        usage();
        return -1;
    }

    PCA9685 pca;
    pca.init(1,0x40);
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));

    sPort.assign(argv[1]);
    if(sPort.find("pwm")!=std::string::npos) {
        if(sPort.find("pwm1")!=std::string::npos)
            nPort = 4;
        else
            nPort = 5;
        nPWMPercent = std::stoi(argv[2]);
        nPWM = int((nPWMPercent/100.0) * 4096);;
        std::cout << "Setting PCA9685 port " << sPort << "(" << nPort+1 << ") PWM to " << nPWMPercent << "% (" << nPWM << ")" << std::endl << std::flush;
        nErr = pca.setPWM(nPort, nPWM);
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } 
    else {
        nPort = std::stoi(argv[1]);
        sState.assign(argv[2]);
        std::cout << "Setting PCA9685 power port " << nPort << " state to " << sState << std::endl << std::flush;
        if(sState.find("on")!=std::string::npos)
            nErr = pca.setOn(nPort-1);
        else
            nErr = pca.setOff(nPort-1);
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return nErr;
}
