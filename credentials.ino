#include <EEPROM.h>

// Read parameters from EEPROM or Serial
void readCredentials()
{
    int ssidAddr = 0;
    int passAddr = ssidAddr + SSID_LEN;
    int connectionStringAddr = passAddr + SSID_LEN;

    // malloc for parameters
    ssid = (char *)malloc(SSID_LEN);
    pass = (char *)malloc(PASS_LEN);
    connectionString = (char *)malloc(CONNECTION_STRING_LEN);

    // try to read out the credential information, if failed, the length should be 0.
    int ssidLength = EEPROMread(ssidAddr, ssid);
    int passLength = EEPROMread(passAddr, pass);
    int connectionStringLength = EEPROMread(connectionStringAddr, connectionString);

    if (ssidLength > 0 && passLength > 0 && connectionStringLength > 0 && !needEraseEEPROM())
    {
        return;
    }


    //delay(50000);
    // read from Serial and save to EEPROM
    readFromSerial("Input your Wi-Fi SSID: ", ssid, SSID_LEN, 0);

    //delay(50000);
    
    ssid = "UCInet Mobile Access";
    //ssid = "NETGEAR07";
    EEPROMWrite(ssidAddr, ssid, strlen(ssid));

    //readFromSerial("Input your Wi-Fi password: ", pass, PASS_LEN, 0);
    pass = "";
    //pass = "braveplanet906";
    EEPROMWrite(passAddr, pass, strlen(pass));

    //readFromSerial("Input your Azure IoT hub device connection string: ", connectionString, CONNECTION_STRING_LEN, 0);
    //connectionString = "HostName=iot244team41h1.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey=3VGotqsBnl/ZyLeyIRR41snZx79LydeZmdR6LQr8Ll0=";
    //previous working connection STRING
    connectionString = "HostName=iot244team41h1.azure-devices.net;DeviceId=ESP8266test01;SharedAccessKey=2UL2UFSeK61ol7mSzoOLsJu05ZzuDJi+IJnIAr5a6+c=";
    
    //connectionString = "HostName=iot244team41h1.azure-devices.net;SharedAccessKeyName=device;SharedAccessKey=pXfvzqLT9ts8UKZEWG4m5Ev9BKxgAW1qQgKtSfh8VuI=";
    
    EEPROMWrite(connectionStringAddr, connectionString, strlen(connectionString));
}

bool needEraseEEPROM()
{
    char result = 'n';
    readFromSerial("Do you need re-input your credential information?(Auto skip this after 5 seconds)[Y/n]", &result, 1, 5000000);
    if (result == 'Y' || result == 'y')
    {
        clearParam();
        return true;
    }
    return false;
}

// reset the EEPROM
void clearParam()
{
    char data[EEPROM_SIZE];
    memset(data, '\0', EEPROM_SIZE);
    EEPROMWrite(0, data, EEPROM_SIZE);
}

#define EEPROM_END 0
#define EEPROM_START 1
void EEPROMWrite(int addr, char *data, int size)
{
    EEPROM.begin(EEPROM_SIZE);
    // write the start marker
    EEPROM.write(addr, EEPROM_START);
    addr++;
    for (int i = 0; i < size; i++)
    {
        EEPROM.write(addr, data[i]);
        addr++;
    }
    EEPROM.write(addr, EEPROM_END);
    EEPROM.commit();
    EEPROM.end();
}

// read bytes from addr util '\0'
// return the length of read out.
int EEPROMread(int addr, char *buf)
{
    EEPROM.begin(EEPROM_SIZE);
    int count = -1;
    char c = EEPROM.read(addr);
    addr++;
    if (c != EEPROM_START)
    {
        return 0;
    }
    while (c != EEPROM_END && count < EEPROM_SIZE)
    {
        c = (char)EEPROM.read(addr);
        count++;
        addr++;
        buf[count] = c;
    }
    EEPROM.end();
    return count;
}
