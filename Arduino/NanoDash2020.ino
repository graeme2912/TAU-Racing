// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13


#include <SPI.h>
#include "mcp_can.h"
//#include "mcp_can_dfs.h"

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
int interruptPin = 2;
unsigned char rxBuf[8];
//INT32U rxId;
unsigned char len = 0;
unsigned char buf[8];
INT32U rxId;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);

    SERIAL.begin(115200);

    // attaching receive interrupt to the interrupt pin
    //attachInterrupt(digitalPinToInterrupt(interruptPin), receive_interrupt, FALLING);

    while (CAN_OK != CAN.begin(CAN_1000KBPS))              // init can bus : baudrate = 500k
    {
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again");
        //delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");

    // enable interrupt 
    //configure_interrupts();
}


void loop()
{



    if (CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        Serial.println("msg receieved");
        Serial.println("");
        //digitalWrite(6,HIGH); 
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned long canId = CAN.getCanId();


        SERIAL.println("-----------------------------");
        SERIAL.print("Get data from ID: 0x");
        SERIAL.println(canId, HEX);

        for (int i = 0; i < len; i++)    // print the data
        {
            SERIAL.print(buf[i], HEX);
            SERIAL.print("\t");
        }
        SERIAL.println();

        if(canId == 0x601)
        {
            // Store the coolant temp
//            coolantErr3Ref = (int16_t)((rxBuf[2] << 8) + rxBuf[3]);
            // print throttle position
            int throttleRef = (int16_t)((buf[6] << 8) + buf[7]);
            Serial.print("throttle ref pre-conversion: ");
            Serial.println("throttleRef");
            Serial.print("Throttle ref post-conversion: ");
            float throttlePosition = (float)throttleRef / 81.91;
            Serial.println(throttlePosition);
        }
    }



    //digitalWrite(6,LOW);
   // delay(100);

}

void receive_interrupt() {

    /*
    Serial.println("Message received, Resetting interrupt flag");
    Serial.print("CANINTF after interrupt = ");
    Serial.println(readRegister(MCP_CANINTF),BIN);
    */
    // Receive flag for CANINTF
    modifyRegister(MCP_CANINTF, 0xFF, 0x0);

    /*
    Serial.print("CANINTF after reset = ");
    Serial.println(readRegister(MCP_CANINTF),BIN);
    Serial.println("");
    */

    CAN.readMsgBuf(&len, rxBuf);
    rxId = CAN.getCanId(); // Get message ID
    for (int i = 0; i < 8; i++)
    {
        Serial.print(rxBuf[i]);
        Serial.print(" ");
    }
    Serial.println("");


    /*
     if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
      {
        //Serial.println("msg receieved");
          CAN.readMsgBuf(&len, buf);
      }
      modifyRegister(MCP_CANINTF,0xFF, 0x0);
      digitalWrite(6,LOW);
      digitalWrite(6,HIGH);
      */
}

INT8U readRegister(const INT8U address)
{
    INT8U ret;

    //MCP2515_SELECT();
    digitalWrite(SPI_CS_PIN, LOW);
    SPI.transfer(MCP_READ);
    SPI.transfer(address);
    ret = SPI.transfer(0x00);
    //MCP2515_UNSELECT();
    digitalWrite(SPI_CS_PIN, HIGH);

    return ret;
}


INT8U setRegister(const INT8U address, const INT8U value)
{
    digitalWrite(SPI_CS_PIN, LOW);
    SPI.transfer(MCP_WRITE);
    SPI.transfer(address);
    SPI.transfer(value);
    digitalWrite(SPI_CS_PIN, HIGH);
}

void modifyRegister(const INT8U address, const INT8U mask, const INT8U data)
{
    digitalWrite(SPI_CS_PIN, LOW);
    SPI.transfer(MCP_BITMOD);
    SPI.transfer(address);
    SPI.transfer(mask);
    SPI.transfer(data);
    digitalWrite(SPI_CS_PIN, HIGH);
}

void configure_interrupts()
{
    modifyRegister(MCP_CANINTE, MCP_TX_INT, 0xFF);
    Serial.print("Interrupt configured: CANINTE REGISTER VALUE = ");
    Serial.println(readRegister(MCP_CANINTE), BIN);

    Serial.print("CANSTAT REGISTER VALUE = ");
    Serial.println(readRegister(MCP_CANSTAT), BIN);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/