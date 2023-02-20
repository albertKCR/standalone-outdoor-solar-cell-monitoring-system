#include "extraFunctions.h"

/*
        void setScale(int res); // sets input value for current scale
        void scaleUp();         // sets up current scale
        void scaleDown();       // sets down current scale
        void Scan(int &var);    // similar to scanf
*/

//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
void setScale()
{
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, rel[rangeCounter]);
    //setResistorMultiplexer();
    digitalWrite(latchPin, HIGH);
    delay(100);
}

void setResistorMultiplexer()
{
    digitalWrite(resistorMutiplexerA, multiplexer[rangeCounter][0]);
    digitalWrite(resistorMutiplexerB, multiplexer[rangeCounter][1]);
    digitalWrite(resistorMutiplexerC, multiplexer[rangeCounter][2]);
}

//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
void scaleUp()
{
    if (1 + rangeCounter < 7)
    {
        rangeCounter++;
        /*Serial.print(rangeCounter);
        Serial.print("   \\\\\   ");
        Serial.println(shunt[rangeCounter]);*/
        setScale();
    }
}

//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
void scaleDown()
{
    if (rangeCounter - 1 >= 0)
    {
        rangeCounter--;
        /*Serial.print(rangeCounter);
        Serial.print("   \\\\\   ");
        Serial.println(shunt[rangeCounter]);*/
        setScale();
    }
}

//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
// @brief Works like Scanf from C
// @param &var  Saves integer readed from serial here
void Scan(int &var)
{
    bool flag = true;
    int SerialValue = 0;
    char numb[20];

    while (flag)
    {
        if (Serial.available() > 0)
        {

            int count = 0; // input character counter

            for (int i = 0; Serial.available() > 0; i++)
            {                            // starts reading each character
                numb[i] = Serial.read(); // each one inside numb
                count = i;               // receives total input character count
                delay(10);
            }

            if (numb[0] == '-')
            {

                count--;
                for (int i = 0; i <= count; i++)
                { // conversion from char to integer

                    numb[i + 1] -= 48;
                    SerialValue += numb[(i + 1)] * pow(10, (count - i));
                }

                SerialValue *= -1;
                if (count > 1)
                    SerialValue--;

            } // end if

            else
            {
                for (int i = 0; i <= count; i++)
                { // conversion from char to integer

                    numb[i] -= 48;
                    SerialValue += numb[i] * pow(10, (count - i));
                }

                if (count > 1)
                    ++SerialValue;

            } // end else

            var = SerialValue; // returns converted value
            flag = false;

        } // end serial available

    } // end while
} // end scan
//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---

void exportData(int &stop)
{ // handles data exporting through serial
    // export is the max point of output vector
    float volt;
    float current;
    float dif;

    bool flag = true;

    Serial.println("Exportar leitura pela serial?");
    Serial.println("1-  Sim, exportar. \n2- Não, não exportar\n3- Exportar Excel");

    while (flag)
    { // export menu loop
        if (Serial.available() > 0)
        {
            int exp = 0; // receives data from serial
            Scan(exp);
            if (exp == 1)
            { // if 1, exports data through serial

                for (int i = 0; i < stop; i++)
                {
                    readdata((i + 1), volt, current, dif);
                    Serial.print(volt, 5);
                    Serial.print("   ///   ");

                    char unit[3] = {'m', 'u', 'n'};
                    int which = 0;

                    if (abs(current) < 1E-6)
                    {
                        current *= 1000000000.0; // billion
                        which = 2;
                    }
                    else if (abs(current) < 1E-3)
                    {
                        current *= 1000000; // million
                        which = 1;
                    }

                    else
                    {
                        current *= 1000.0;
                        which = 0;
                    }

                    Serial.print(current, 5);
                    Serial.print(unit[which]);
                    Serial.println('A');
                }

            } // if==1

            else if (exp == 2)
            { // if 2, then exit from loop, ready for next measure
                flag = false;
                stop = 0;
                Serial.println("encerrando exportação");
            }

            else if (exp == 3)
            {

                // Serial.println("VOLTAGE  -  CURRENT");
                for (int i = 0; i < stop; i++)
                {
                    readdata((i + 1), volt, current, dif);

                    String voltage = String(volt, 5);
                    voltage.replace('.', ',');

                    Serial.print(voltage);

                    Serial.print(" ; ");

                    String ampere = String(current, 12);
                    ampere.replace('.', ',');

                    Serial.println(ampere);
                }
                /*Serial.print("\n\n\n");
                Serial.println("CURRENT");

                for(int i = 0; i< stop; i++){
                    readdata((i+1), volt,current);
                    Serial.println(current,12);

                }   */
            }
            /*else if(exp == 4){

                fout.open("data.csv", ios::out | ios::app);

                for(int i = 0; i< stop; i++){

                    readdata((i+1), volt,current,dif);

                    //String voltage =String(dif,5);
                    //String ampere = String(current,12);
                     fout << voltage << ", "
                        << ampere
                        << "\n";
                }
                fout.close();
            }*/

            else
            {
                Serial.println("Comando desconhecido");
            }

        } // end serial.available()

        if (!digitalRead(5))
        {

            for (int i = 0; i < stop; i++)
            {
                readdata((i + 1), volt, current, dif);

                Serial.print(dif, 4);
                Serial.print(',');
                Serial.println(current, 12);
            }
        }

    } // end while flag
} // end exporting

// --- === --- === --- === --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---

void sweep()
{

    int start, end;          // start voltage, and ending voltage
    int timestep;            // timestep between voltage changes
    unsigned long timer = 0; // time step counter

    //=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
    // --- sweep menu ---
    Serial.println("Insira a tensao inicial em mV:");
    Scan(start);
    Serial.println(start);
    Serial.println("Insira a tensao final em mV:");
    Scan(end);
    Serial.println(end);
    Serial.println("Insira o time step em ms:");
    Scan(timestep);
    Serial.println(timestep);

    // --- Conversion from voltage to DAC bits ---
    start = map(start, -500, 2500, 0, 4095);
    end = map(end, -500, 2500, 0, 4095);

    dac.setVoltage(start, false); // updates DAC output voltage

    delay(800); // wait til ammeter reach offset

    adc.conversion(shunt[rangeCounter]); // first read to check if it´s in wrong scale
    Serial.println("Starting");

    while ((adc.topOV() && (rangeCounter != 0)) || (adc.botOV() && (rangeCounter != 6)))
    { // checks for wrong scale

        if (adc.topOV())
        { // check top overflow, change to inferior resistance(bigger current)
            scaleDown();
            delay(100);
        }

        else if (adc.botOV())
        { // check bottom overflow, change to superior resistance(lesser current)
            scaleUp();
            delay(100);
        }

        adc.conversion(shunt[rangeCounter]); // reads current, voltage, calculate overflow

    } // end while(overflow)

    // --- Sweep control ---
    for (int i = start; i < end; i += 14)
    {

        // i+= 14; // 10mV changes

        //--- Start scale control ---
        while ((adc.topOV() && (rangeCounter != 0)) || (adc.botOV() && (rangeCounter != 6)))
        { // checks for overflow flags

            if (adc.topOV())
            { // check top overflow, change to inferior resistance(bigger current)
                scaleDown();
                i -= 57;
                dac.setVoltage(i, false);
                delay(1000);
                /*
                                while(adc.diffe > getDif((totalPoints))){ // after scale changing, checks if there´s voltage peak in DUT(device under testing)
                                    i-=5;                               // lowering voltage til it´s equal the voltage before scale changing
                                    dac.setVoltage(i,false);            //updates voltage
                                    delay(1);
                                    adc.conversion(shunt[rangeCounter]);//reads voltage
                                }*/
            }

            else if (adc.botOV())
            { // check bottom overflow, change to superior resistance(lesser current)
                scaleUp();
                i += 57;
                dac.setVoltage(i, false);
                delay(1000);
                /*
                                while(adc
                                delay(1000);

                               /* while(adc.diffe > getDif((totalPoints))){ // after scale changing, checks if there´s voltage peak in DUT(device under testing)
                                    i-=5;                               // lowering voltage til it´s equal the voltage before scale changing
                                    dac.setVoltage(i,false);            //updates voltage
                                    delay(1);
                                    adc.conversion(shunt[rangeCounter]);//reads voltage
                                }   */
            }

            /*while(adc.diffe < getDif((totalPoints))){ // after scale changing, checks if there´s voltage peak in DUT(device under testing)
                    Serial.println(adc.diffe);
                    Serial.println(getDif((totalPoints)));
                    i-=100;                               // lowering voltage til it´s equal the voltage before scale changing
                    dac.setVoltage(i,false);            //updates voltage
                    delay(1);
                    adc.conversion(shunt[rangeCounter]);//reads voltage
                }*/

            adc.conversion(shunt[rangeCounter]); // reads current, voltage, calculate overflow

        } // end checking for overflow

        sumOfCurrents = 0;
        for (int k = 0; k < measurementsByTension; k++)
        {
            Serial.println("loop");
            Serial.println(measurementsByTension);
            adc.conversion(shunt[rangeCounter]);
            sumOfCurrents += adc.returnCurrent();
        }
        Serial.println("fora loop");
        adc.OutputCurrent = sumOfCurrents / measurementsByTension;
        // adc.conversion(shunt[rangeCounter]); // new reading

        timer = millis(); // start timing

        savedata((totalPoints + 1), adc.returnVoltage(), adc.returnCurrent(), adc.diffe); // saved readed data

        ++totalPoints; // points readed ++

        dac.setVoltage(i, false); // updates DAC output voltage

        // --- Timestep control ---
        while ((millis() - timer) < timestep)
        { // waits til next step

            if (!digitalRead(button))
            {
                Serial.println("sweep abortado");
                i = end;   // força sair do laço sweep
                timer = 0; // sai do while
            }
        }
    } //  end for() sweep
    Serial.println("Pontos totais: ");
    Serial.println(totalPoints);
    Serial.println(totalPoints * timestep);
    dac.setVoltage(0x2AB, false);
    rangeCounter = 0;
    setScale();
    exportData(totalPoints);

} // end sweep
//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---

void cal2()
{ // potentiometer changes output voltage, enable auto scale

    bool loop = true;
    unsigned long aux = 0;
    int Input = 0;
    int volt = 0;

    while (loop)
    {

        volt = map(analogRead(A0), 0, 1023, 0, 4093);
        dac.setVoltage(volt, false);

        if ((millis() - aux) > 280)
        {
            aux = millis();

            adc.conversion(shunt[rangeCounter]);

            if (adc.topOV())
            { // check top overflow, change to inferior resistance(bigger current)
                scaleDown();
                delay(500);
                adc.conversion(shunt[rangeCounter]);
            }

            else if (adc.botOV())
            { // check bottom overflow, change to superior resistance(lesser current)
                scaleUp();
                delay(500);
                adc.conversion(shunt[rangeCounter]);
            }

            Serial.print("\n\n\n\n");
            Serial.print(adc.val1, 4);
            Serial.print("   //   ");

            char unit[3] = {'m', 'u', 'n'};
            int which = 0;

            if (abs(adc.OutputCurrent) < 1E-6)
            {
                adc.OutputCurrent *= 1000000000.0; // billion
                which = 2;
            }
            else if (abs(adc.OutputCurrent) < 1E-3)
            {
                adc.OutputCurrent *= 1000000; // million
                which = 1;
            }

            else
            {
                adc.OutputCurrent *= 1000.0;
                which = 0;
            }

            Serial.print(adc.OutputCurrent, 5);
            Serial.print(unit[which]);
            Serial.println('A');

            Serial.print("channel 2 voltage : ");
            Serial.println(adc.val2, 8);

        } // timer

        if (Serial.available() > 0)
        { // if serial = 1, exit cal2
            Scan(Input);

            if (Input == 1)
            {
                Serial.println("exiting...");
                Serial.println("\n\n\n\n");
                loop = false;
            }
        } // end if (serial)

    } // end loop

} // end void cal2();

//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
void calibrating()
{
    bool loop = true;
    bool mode = false; // false means voltage mode
    unsigned long aux = 0;
    int Input = 0;

    while (loop)
    {

        if ((millis() - aux) > 280)
        { // every 280 ms prints voltage read
            aux = millis();
            // adc.average();
            adc.conversion(shunt[rangeCounter]);
            Serial.print("\n\n\n\n");
            Serial.print(adc.val1, 4);
            Serial.print("   //   ");
            // Serial.println(adc.OutputCurrent,4);

            char unit[3] = {'m', 'u', 'n'};
            int which = 0;

            if (adc.OutputCurrent < 1E-6)
            {
                adc.OutputCurrent *= 1000000000.0; // billion
                which = 2;
            }
            else if (adc.OutputCurrent < 1E-3)
            {
                adc.OutputCurrent *= 1000000; // million
                which = 1;
            }

            else
            {
                adc.OutputCurrent *= 1000.0;
                which = 0;
            }
            Serial.print(adc.OutputCurrent, 5);
            Serial.print(unit[which]);
            Serial.println('A');

            Serial.print("channel 2 voltage : ");
            Serial.println(adc.val2, 8);

        } // printing timing

        if (Serial.available() > 0)
        {
            Scan(Input);

            if (Input == 0)
            {
                mode = mode ^ 1;
                Serial.println("mode changed");
            }

            else if (Input == 2)
            {
                for (int i = 0; i < 7; i++)
                    scaleUp();
                for (int i = 0; i < 7; i++)
                    scaleDown();
            }

            else if (Input == 3)
            {
                loop = false;
                Serial.println("exiting...");
                delay(700);
                Serial.print("\n\n\n\n");
            }
            else if (Input == 1 && mode == false)
            {
                Serial.print("\n\n\n\n");
                Serial.println("digite:\n1- -500mV\n2-2500mV\n3- Sair");
                Scan(Input);
                if (Input == 1)
                    dac.setVoltage(0x000, false);
                else if (Input == 2)
                    dac.setVoltage(0xFFF, false);
            }

            else if (Input == 1 && mode == true)
            {
                Serial.print("\n\n\n\n");
                Serial.println("digite de 1 a 7 para trocar as escalas");
                Scan(Input);
                rangeCounter = (Input - 1);
                setScale();
            }
        }

    } // end while loop

} // end function

void savedata(int addres, float volt, float current, float dif)
{
    addres = 1 + ((addres - 1) * 12);
    eep.put(addres, volt);
    addres += 4;
    eep.put(addres, current);
    addres += 4;
    eep.put(addres, dif);
}

void readdata(int addres, float &volt, float &current, float &dif)
{
    addres = 1 + ((addres - 1) * 12);
    eep.get(addres, volt);
    addres += 4;
    eep.get(addres, current);
    addres += 4;
    eep.get(addres, dif);
}

float getDif(int addres)
{
    float data;
    addres = ((addres - 1) * 12) + 9;
    eep.get(addres, data);
    return data;
}

void voltageAdjust()
{

    rangeCounter = 0;
    setScale();
    Serial.println("Ajuste o OFFSET para 2,5 volts, digite 1 para proximo passo");
    bool loop = true;

    while (loop)
    {
        if (Serial.available() > 0)
            loop = false;
        adc.average();
        Serial.print("channel 2 voltage:");
        Serial.println((2.5 + adc.val2), 5);
    }

    for (int i = 0; i < 7; i++)
    {
        adc.average(30);
        adc.val2 *= (-1);
        eep.put((60000 + (i * 4)), adc.val2);
        Serial.println(adc.val2, 6);
        scaleUp();
        delay(400);
    }

    for (int i = 0; i < 7; i++)
    {
        eep.get((60000 + (i * 4)), adc.correction[i]);
    }
    Serial.end();
    Serial.begin(115200);
    Serial.println("Calibração concluida!");
}

void autonomous()
{
    int hour, minute;
    connectToInternet();
    while (1)
    {
        ntp.update();
        if ((ntp.getHours() == 12) && (ntp.getMinutes() == 00))
        { // check the time to make the measurement
            for (int i = 0; i < 8; i++)
            {
                digitalWrite(measureMutiplexerA, multiplexer[i][0]);
                digitalWrite(measureMutiplexerB, multiplexer[i][1]);
                digitalWrite(measureMutiplexerC, multiplexer[i][2]);
                if (autonomousSweep() == 0)
                {
                    continue;
                }
                delay(5000);
                sensorsMeasure(); // reads the temperature and humidity sensor
                sendToSheet();    // sends the data to google sheets
            }
        }
        if (Serial.available() > 0)
        { // sending something in the monitor serial breaks the while loop
            break;
        }
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Lost internet connection");
            Serial.println("Trying to reconnect");
            connectToInternet();
        }
    }
}

void sensorsMeasure()
{
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println("Failed to read from DHT");
    }
    luminosity = analogRead(LDR);
}

int autonomousSweep()
{
    int startVoltage = 0, finalVoltage = 2000; // start voltage, and ending voltage
    int timestep = 35;                         // timestep between voltage changes

    // --- Conversion from voltage to DAC bits ---
    startVoltage = map(startVoltage, -500, 2500, 0, 4095);
    finalVoltage = map(finalVoltage, -500, 2500, 0, 4095);

    dac.setVoltage(startVoltage, false); // updates DAC output voltage

    delay(800); // wait til ammeter reach offset

    adc.conversion(shunt[rangeCounter]); // first read to check if it´s in wrong scale
    Serial.println("Starting");

    void scaleControl(); // checks the scale to starte the measure

    if (sweepControl(startVoltage, finalVoltage, timestep) == 0) // do the measures
    {
        return 0;
    }

    Serial.println("Total Points: ");
    Serial.println(totalPoints);
    Serial.println(totalPoints * timestep);
    dac.setVoltage(0x2AB, false);
    rangeCounter = 0;
    setScale();
    return 1;
}

void meanOfMeasures(int numOfMeasures)
{
    sumOfCurrents = 0;
    for (int k = 0; k < numOfMeasures; k++)
    {                                        // will measure the current with the same voltage until k=numOfMeasures
        adc.conversion(shunt[rangeCounter]); // reads voltage and current
        sumOfCurrents += adc.returnCurrent();
    }
    adc.OutputCurrent = sumOfCurrents / numOfMeasures; // calculate the mean of the measures
}

void scaleControl()
{
    while ((adc.topOV() && (rangeCounter != 0)) || (adc.botOV() && (rangeCounter != 6)))
    { // checks for wrong scale
        if (adc.topOV())
        { // check top overflow, change to inferior resistance(bigger current)
            scaleDown();
            delay(100);
        }
        else if (adc.botOV())
        { // check bottom overflow, change to superior resistance(lesser current)
            scaleUp();
            delay(100);
        }
        adc.conversion(shunt[rangeCounter]); // reads current, voltage, calculate overflow
    }
}

int sweepControl(int startVoltage, int finalVoltage, int timestep)
{
    unsigned long timer = 0; // time step counter
    for (int i = startVoltage; i < finalVoltage; i += 14)
    { // i+= 14; // 10mV changes

        // check the scale and avoid voltage peaks
        while ((adc.topOV() && (rangeCounter != 0)) || (adc.botOV() && (rangeCounter != 6)))
        {
            if (adc.topOV())
            { // check top overflow, change to inferior resistance(bigger current)
                scaleDown();
                i -= 57;
                dac.setVoltage(i, false);
                delay(1000);
            }
            else if (adc.botOV())
            { // check bottom overflow, change to superior resistance(lesser current)
                scaleUp();
                i += 57;
                dac.setVoltage(i, false);
                delay(1000);
            }
            adc.conversion(shunt[rangeCounter]); // reads current, voltage, calculate overflow
        }
        meanOfMeasures(measurementsByTension); // mean of the current measures

        timer = millis(); // start timing

        savedata((totalPoints + 1), adc.returnVoltage(), adc.returnCurrent(), adc.diffe); // save readed data

        // verifies if have something to me measured in this multiplexer channel
        if (totalPoints == 5)
        {
            if (adc.returnCurrent() == 0)
            {
                return 0;
            }
        }

        ++totalPoints; // points readed ++

        dac.setVoltage(i, false); // updates DAC output voltage

        // --- Timestep control ---
        while ((millis() - timer) < timestep)
        { // waits til next step
            if (!digitalRead(button))
            {
                Serial.println("sweep abortado");
                i = finalVoltage; // forced exit from the for loop
                timer = 0;        // exit while
            }
        }
    }
    return 1;
}

void connectToInternet()
{
    WiFi.begin("UTFPR-Projetos", "pNU9cnpPZ$tM");
    Serial.println(WiFi.macAddress());

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    // --- HTTPS Redirect Setup ---
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
    Serial.print("Connecting to Google...");

    bool flag = false;
    for (int i = 0; i < 5; i++)
    {
        int retval = client->connect(host, httpsPort);
        if (retval == 1)
        {
            flag = true;
            Serial.println("[OK]");
            break;
        }
        else
            Serial.println("[Error]");
    }
    if (!flag)
    {
        Serial.print("[Error]");
        Serial.println(host);
        return;
    }
    delete client;
    client = nullptr;
    Serial.println("Setup done");
    // ---
}

void sendToSheet()
{
    int stringCounter = 0;
    int loadCounter = 0;
    float volt;
    float current;
    float dif;

    while (totalPoints > 0)
    {
        totalPoints = totalPoints - 40;
        if (totalPoints > 0)
        {

            // --- HTTPS protocol ---
            static bool flag = false;
            if (!flag)
            {
                client = new HTTPSRedirect(httpsPort);
                client->setInsecure();
                flag = true;
                client->setPrintResponseBody(true);
                client->setContentTypeHeader("application/json");
            }
            if (client != nullptr)
            {
                if (!client->connected())
                {
                    client->connect(host, httpsPort);
                }
            }
            else
            {
                Serial.println("[Error]");
            }
            // ---

            for (int i = 0; i < 40; i++)
            {
                readdata((stringCounter + 1), volt, current, dif);                   // access in the EEPROM the data of voltage and current
                String currentVoltage = String(current, 13) + "," + String(volt, 5); // transform the data in one string
                toSendData[i] = currentVoltage;
                loadCounter++;
                stringCounter++;
            }

            // the payload will be constructed according to the received data
            payload = payload_base + "\"" + toSendData[0];
            for (int i = 1; i < stringCounter; i++)
            {
                payload = payload + "," + toSendData[i];
            }
            payload = payload + "\"}";

            Serial.println("Sending...");

            if (client->POST(url, host, payload))
            { // Send the data through the google API
                Serial.println(" [OK]");
            }
            else
            {
                Serial.println("[Error]");
            }
        }
        else
        {
            for (int i = 0; i < 40 - abs(totalPoints); i++)
            {
                readdata((stringCounter + 1), volt, current, dif);                   // access in the EEPROM the data of voltage and current
                String currentVoltage = String(current, 13) + "," + String(volt, 5); // transform the data in one string
                toSendData[i] = currentVoltage;
                loadCounter++;
                stringCounter++;
            }
            // the payload will be constructed according to the received data
            payload = payload_base + "\"" + toSendData[0];
            for (int i = 1; i < stringCounter; i++)
            {
                payload = payload + "," + toSendData[i];
            }
            payload = payload + "\"}";

            Serial.println("Sending...");

            if (client->POST(url, host, payload))
            { // Send the data through the google API
                Serial.println(" [OK]");
            }
            else
            {
                Serial.println("[Error]");
            }
        }
    }
    String payload_base = "{\"command\": \"sensor\", \"sheet_name\": \"Sheet2\", \"values\": ";
    payload = payload_base + "\"" + humidity + "," + temperature + "," + luminosity + "\"}";

    Serial.println("Sending...");
    if (client->POST(url, host, payload))
    { // Send the data through the google API
        Serial.println(" [OK]");
    }
    else
    {
        Serial.println("[Error]");
    }
    totalPoints = 0;
}