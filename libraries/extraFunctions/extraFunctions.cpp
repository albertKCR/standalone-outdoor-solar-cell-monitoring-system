#include "extraFunctions.h"

/*
        void setScale(int res); // sets input value for current scale
        void scaleUp();         // sets up current scale
        void scaleDown();       // sets down current scale
        void Scan(int &var);    // similar to scanf
*/

//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
void  setScale()
{
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, rel[rangeCounter]);
    digitalWrite(latchPin, HIGH);
    delay(100);
}

//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
void scaleUp(){
    if(1+rangeCounter < 7) {
        rangeCounter ++;
        /*Serial.print(rangeCounter);
        Serial.print("   \\\\\   ");
        Serial.println(shunt[rangeCounter]);*/
        setScale();
    }
}

 //=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
void scaleDown(){
    if(rangeCounter - 1 >= 0 ){
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
void Scan(int &var){
    bool flag = true;
    int SerialValue = 0;
    char numb[20];

    while(flag){
        if(Serial.available() > 0){

                
            int count = 0;                          // input character counter


            for(int i=0; Serial.available()>0; i++){    // starts reading each character
                numb[i] = Serial.read();                // each one inside numb
                 count=i;                               // receives total input character count               
                 delay(10);
                }

            

            if(numb[0] == '-'){

                count --;
                for(int i=0;i<=count;i++){                  // conversion from char to integer
                            
                    numb[i+1] -= 48;
                    SerialValue += numb[(i+1)] * pow(10, (count-i));
                            
                }

                SerialValue *= -1;
                if(count >1) SerialValue--;
               

            }// end if
          
            else{
                for(int i=0;i<=count;i++){                  // conversion from char to integer
                            
                    numb[i] -= 48;
                    SerialValue += numb[i] * pow(10, (count-i));
                    
                
                }
                
                if(count > 1) ++SerialValue; 

            }// end else
            
      
            var = SerialValue;                          // returns converted value 
            flag = false;

        }// end serial available

    }// end while
}// end scan
//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---


void exportData( int &stop ){    // handles data exporting through serial
    // export is the max point of output vector
    float volt;
    float current;
    float dif;

    bool flag = true;
    
    Serial.println("Exportar leitura pela serial?");
    Serial.println("1-  Sim, exportar. \n2- Não, não exportar\n3- Exportar Excel");
    
    
    while(flag){    // export menu loop
        if(Serial.available()>0){
            int exp=0; // receives data from serial
            Scan(exp);
            if(exp == 1){                               // if 1, exports data through serial


            for(int i = 0; i< stop; i++){
                readdata((i+1), volt,current,dif);
                Serial.print(volt,5);
                Serial.print("   ///   ");

                char unit[3] = {'m' , 'u', 'n'};
                int which =0;

                if(abs(current) < 1E-6){
                    current *= 1000000000.0;    // billion
                    which = 2;
                }
                else if (abs(current) < 1E-3 ) {
                    current *= 1000000;         // million
                    which = 1;
                }
                
                else {
                    current *= 1000.0;
                    which = 0;
                }  

                Serial.print(current,5);
                Serial.print(unit[which]);
                Serial.println('A');
            }

        } // if==1

            else if(exp == 2){                          // if 2, then exit from loop, ready for next measure
                flag = false;
                stop = 0;
                Serial.println("encerrando exportação");
            }


            else if(exp == 3){


            
                // Serial.println("VOLTAGE  -  CURRENT");
                for(int i = 0; i< stop; i++){
                    readdata((i+1), volt,current,dif);
                
                    String voltage =String(dif,5); 
                    voltage.replace('.',',');

                    Serial.print(voltage);

                    Serial.print(" ; ");

                    String ampere = String(current,12);
                    ampere.replace('.',',');

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

            else{
                Serial.println("Comando desconhecido");
            }



        }// end serial.available()

        if(!digitalRead(5)){

            for(int i = 0; i< stop; i++){
                readdata((i+1), volt,current,dif);
               
                Serial.print(dif,4);
                Serial.print(',');
                Serial.println(current,12);
                
            }
        }


        

    }// end while flag   
}// end exporting

    // --- === --- === --- === --- === --- === --- === --- === --- === --- --- === --- === --- === --- === --- 

void sweep(){
   

    int start,end;            // start voltage, and ending voltage
    int timestep;             // timestep between voltage changes
    unsigned long timer = 0;  // time step counter
    
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
    start = map(start, -500,2500,0,4095);
    end   = map(end  , -500,2500,0,4095);

    dac.setVoltage(start,false);  // updates DAC output voltage

    delay(800);                   // wait til ammeter reach offset

    adc.conversion(shunt[rangeCounter]);    // first read to check if it´s in wrong scale
    Serial.println("Starting");


    while((adc.topOV() &&(rangeCounter != 0)) || (adc.botOV() &&(rangeCounter !=6) )  ){ // checks for wrong scale

            if(adc.topOV()){        // check top overflow, change to inferior resistance(bigger current)
                scaleDown();
                delay(100);
            }


            else if(adc.botOV()){                     // check bottom overflow, change to superior resistance(lesser current)
                scaleUp(); 
                delay(100);
            } 

            adc.conversion(shunt[rangeCounter]);      // reads current, voltage, calculate overflow
   
    } // end while(overflow)


    // --- Sweep control ---
    for(int i = start; i< end; i+=14 ){
                                  
        //i+= 14; // 10mV changes            
        
         //--- Start scale control ---
        while((adc.topOV() &&(rangeCounter != 0)) || (adc.botOV() &&(rangeCounter !=6) )  ) {            // checks for overflow flags

            if(adc.topOV()){                          // check top overflow, change to inferior resistance(bigger current)
                scaleDown();
                i-=57;
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


            else if(adc.botOV()){                     // check bottom overflow, change to superior resistance(lesser current)
                scaleUp();
                i+=57;
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
            

            adc.conversion(shunt[rangeCounter]);      // reads current, voltage, calculate overflow




        }// end checking for overflow

       measures=0;
        for (int k=0; k<num_med; k++){
            Serial.println("loop");
            Serial.println(num_med);
            adc.conversion(shunt[rangeCounter]);
            measures += adc.returnCurrent();
        }
        Serial.println("fora loop");
        adc.OutputCurrent = measures/num_med;
        //adc.conversion(shunt[rangeCounter]); // new reading

        
        timer = millis();                    // start timing

        
        savedata((totalPoints+1) , adc.returnVoltage(),adc.returnCurrent() , adc.diffe ); // saved readed data


        ++totalPoints; //points readed ++
        

        dac.setVoltage(i,false);  // updates DAC output voltage


        // --- Timestep control ---
        while((millis() - timer) < timestep){  // waits til next step
                 
            if(!digitalRead(button)) {
                Serial.println("sweep abortado");
                i = end;  // força sair do laço sweep
                timer = 0;// sai do while
            }
        }
    } //  end for() sweep
    Serial.println("Pontos totais: ");
    Serial.println(totalPoints);
    Serial.println(totalPoints*timestep);
    dac.setVoltage(0x2AB, false);
    rangeCounter = 0;
    setScale();
    exportData(totalPoints);

}// end sweep
//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---

void cal2(){// potentiometer changes output voltage, enable auto scale

    bool loop = true;
    unsigned long aux =0;
    int Input = 0;
    int volt= 0;


    while(loop){
         
        volt = map(analogRead(A0),0,1023,0,4093);
        dac.setVoltage(volt,false);

        if((millis()-aux) > 280){ 
            aux = millis();

            adc.conversion(shunt[rangeCounter]);

            if(adc.topOV()){        // check top overflow, change to inferior resistance(bigger current)
                scaleDown();
                delay(500);
                adc.conversion(shunt[rangeCounter]);


            }


            else if(adc.botOV()){                     // check bottom overflow, change to superior resistance(lesser current)
                scaleUp(); 
                delay(500);
                adc.conversion(shunt[rangeCounter]);
            } 



            Serial.print("\n\n\n\n");
            Serial.print(adc.val1,4);
            Serial.print("   //   ");

            char unit[3] = {'m' , 'u', 'n'};
            int which =0;

            if(abs(adc.OutputCurrent) < 1E-6){
                    adc.OutputCurrent *= 1000000000.0;    // billion
                    which = 2;
                }
                else if (abs(adc.OutputCurrent) < 1E-3 ) {
                    adc.OutputCurrent *= 1000000;         // million
                    which = 1;
                }
                
                else {
                    adc.OutputCurrent *= 1000.0;
                    which = 0;
                }  

            Serial.print(adc.OutputCurrent,5);
            Serial.print(unit[which]);
            Serial.println('A');

            Serial.print("channel 2 voltage : ");
            Serial.println(adc.val2,8);


        }// timer


        if(Serial.available()>0){ // if serial = 1, exit cal2 
            Scan(Input);

            if(Input == 1) {
                Serial.println("exiting...");
                Serial.println("\n\n\n\n");
                loop = false;              
            }       
        }// end if (serial)

    }// end loop



}// end void cal2();



//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---
void calibrating(){
    bool loop = true;
    bool mode = false; //false means voltage mode
    unsigned long aux =0;
    int Input = 0;    

    while(loop){

        if((millis()-aux) > 280){       // every 280 ms prints voltage read
            aux = millis();
            //adc.average();
            adc.conversion(shunt[rangeCounter]);
            Serial.print("\n\n\n\n");
            Serial.print(adc.val1,4);
            Serial.print("   //   ");
            //Serial.println(adc.OutputCurrent,4);




            char unit[3] = {'m' , 'u', 'n'};
            int which =0;

            if(adc.OutputCurrent < 1E-6){
                    adc.OutputCurrent *= 1000000000.0;    // billion
                    which = 2;
                }
                else if (adc.OutputCurrent < 1E-3 ) {
                    adc.OutputCurrent *= 1000000;         // million
                    which = 1;
                }
                
                else {
                    adc.OutputCurrent *= 1000.0;
                    which = 0;
                }  
            Serial.print(adc.OutputCurrent,5);
            Serial.print(unit[which]);
            Serial.println('A');

            Serial.print("channel 2 voltage : ");
            Serial.println(adc.val2,8);

        }// printing timing


        if(Serial.available()>0){ 
            Scan(Input);

            if(Input == 0) {
                mode = mode ^1;
                Serial.println("mode changed");
            }

            else if(Input == 2){
                for(int i = 0; i <7; i++) scaleUp();
                for(int i = 0; i <7; i++) scaleDown();


            }

            else if(Input == 3) {
                loop = false;
                Serial.println("exiting...");
                delay(700);
                Serial.print("\n\n\n\n");
            }
            else if(Input == 1 && mode == false){
                Serial.print("\n\n\n\n");
                Serial.println("digite:\n1- -500mV\n2-2500mV\n3- Sair");
                Scan(Input);
                if(Input == 1) dac.setVoltage(0x000,false);
                else if(Input == 2) dac.setVoltage(0xFFF,false);             
            }

            else if(Input  == 1 && mode == true){
                Serial.print("\n\n\n\n");
                Serial.println("digite de 1 a 7 para trocar as escalas");
                Scan(Input);
                rangeCounter =(Input -1);
                setScale();
            }

        }

    }// end while loop

}// end function
//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---

void savedata(int addres,float volt, float current,float dif){
    addres = 1 + ((addres-1)*12);
    eep.put(addres,volt);
    addres +=4;
    eep.put(addres, current);
    addres +=4;
    eep.put(addres, dif);
}
//=== --- === --- === --- --- === --- === --- === --- === --- === --- --- === --- === --- === --- === ---

void readdata(int addres,float &volt, float &current,float &dif){
    addres = 1 + ((addres-1)*12);
    eep.get(addres,volt);
    addres +=4;
    eep.get(addres, current);
    addres +=4;
    eep.get(addres,dif);
}


float getDif(int addres){
    float data;
    addres =((addres -1)*12)+9;
    eep.get(addres,data);
    return data;

}

void voltageAdjust(){

    rangeCounter =0;
    setScale();
    Serial.println("Ajuste o OFFSET para 2,5 volts, digite 1 para proximo passo");
    bool loop = true;

    while(loop){

        if(Serial.available()>0)loop =false;
        
        adc.average();
        Serial.print("channel 2 voltage:");
        Serial.println((2.5 + adc.val2),5);

    }

    for(int i=0; i<7; i++){
        adc.average(30);
        adc.val2 *= (-1) ;
        eep.put((60000 +(i*4)),adc.val2);
        Serial.println(adc.val2,6);
        scaleUp();
        delay(400);
        
    }

    for(int i= 0; i<7; i++){

        eep.get((60000 +(i*4)) , adc.correction[i]);

    }
    Serial.end();
	Serial.begin(38400);
    Serial.println("Calibração concluida!");


}
/*
void wifiConnect(){
   
    sendData("AT+RST\r\n", 2000, DEBUG); // rst
    //Conecta a rede wireless
    sendData("AT+CWJAP=\"iPhone de Albert\",\"12345678\"\r\n", 2000, DEBUG);
    delay(3000);
    sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
    // Mostra o endereco IP
    sendData("AT+CIFSR\r\n", 1000, DEBUG);
    // Configura para multiplas conexoes
    sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
    // Inicia o web server na porta 80
    sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);
    Serial.println(esp8266.available());
}

String sendData(String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}*/

/*void wifiConnect(){
char ssid[] = "iPhone de Albert"; //VARIÁVEL QUE ARMAZENA O NOME DA REDE SEM FIO
char pass[] = "12345678";//VARIÁVEL QUE ARMAZENA A SENHA DA REDE SEM FIO

int status = WL_IDLE_STATUS; //STATUS TEMPORÁRIO ATRIBUÍDO QUANDO O WIFI É INICIALIZADO E PERMANECE ATIVO
//ATÉ QUE O NÚMERO DE TENTATIVAS EXPIRE (RESULTANDO EM WL_NO_SHIELD) OU QUE UMA CONEXÃO SEJA ESTABELECIDA
//(RESULTANDO EM WL_CONNECTED)

WiFiEspServer server(80); //CONEXÃO REALIZADA NA PORTA 80

RingBuffer buf(8); //BUFFER PARA AUMENTAR A VELOCIDADE E REDUZIR A ALOCAÇÃO DE MEMÓRIA

int statusLed = LOW; //VARIÁVEL QUE ARMAZENA O ESTADO ATUAL DO LED (LIGADO / DESLIGADO)

  pinMode(LED_BUILTIN, OUTPUT); //DEFINE O PINO COMO SAÍDA (LED_BUILTIN = PINO 13)
  digitalWrite(LED_BUILTIN, LOW); //PINO 13 INICIA DESLIGADO
  Serial.begin(9600); //INICIALIZA A SERIAL
  esp8266.begin(9600); //INICIALIZA A SERIAL PARA O ESP8266
  WiFi.init(&esp8266); //INICIALIZA A COMUNICAÇÃO SERIAL COM O ESP8266
  WiFi.config(IPAddress(192,168,0,110)); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR

  //INÍCIO - VERIFICA SE O ESP8266 ESTÁ CONECTADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER
  if(WiFi.status() == WL_NO_SHIELD){
    while (true);
  }
  while(status != WL_CONNECTED){
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("conectou");
  server.begin();
  //FIM - VERIFICA SE O ESP8266 ESTÁ CONECTADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER


  WiFiEspClient client = server.available(); //ATENDE AS SOLICITAÇÕES DO CLIENTE

  if (client) { //SE CLIENTE TENTAR SE CONECTAR, FAZ
    buf.init(); //INICIALIZA O BUFFER
    while (client.connected()){ //ENQUANTO O CLIENTE ESTIVER CONECTADO, FAZ
      if(client.available()){ //SE EXISTIR REQUISIÇÃO DO CLIENTE, FAZ
        char c = client.read(); //LÊ A REQUISIÇÃO DO CLIENTE
        buf.push(c); //BUFFER ARMAZENA A REQUISIÇÃO

        //IDENTIFICA O FIM DA REQUISIÇÃO HTTP E ENVIA UMA RESPOSTA
        if(buf.endsWith("\r\n\r\n")) {
          sendHttpResponse(client);
          break;
        }
        if(buf.endsWith("GET /H")){ //SE O PARÂMETRO DA REQUISIÇÃO VINDO POR GET FOR IGUAL A "H", FAZ 
            digitalWrite(LED_BUILTIN, HIGH); //ACENDE O LED
            statusLed = 1; //VARIÁVEL RECEBE VALOR 1(SIGNIFICA QUE O LED ESTÁ ACESO)
        }
        else{ //SENÃO, FAZ
          if (buf.endsWith("GET /L")) { //SE O PARÂMETRO DA REQUISIÇÃO VINDO POR GET FOR IGUAL A "L", FAZ
                  digitalWrite(LED_BUILTIN, LOW); //APAGA O LED
                  statusLed = 0; //VARIÁVEL RECEBE VALOR 0(SIGNIFICA QUE O LED ESTÁ APAGADO)
          }
        }
      }
    }
    client.stop(); //FINALIZA A REQUISIÇÃO HTTP E DESCONECTA O CLIENTE
  }*/


//MÉTODO DE RESPOSTA A REQUISIÇÃO HTTP DO CLIENTE
/*void sendHttpResponse(WiFiEspClient client){
  client.println("HTTP/1.1 200 OK"); //ESCREVE PARA O CLIENTE A VERSÃO DO HTTP
  client.println("Content-Type: text/html"); //ESCREVE PARA O CLIENTE O TIPO DE CONTEÚDO(texto/html)
  client.println("");
  client.println("<!DOCTYPE HTML>"); //INFORMA AO NAVEGADOR A ESPECIFICAÇÃO DO HTML
  client.println("<html>"); //ABRE A TAG "html"
  client.println("<head>"); //ABRE A TAG "head"
  client.println("<link rel='icon' type='image/png' href='https://blogmasterwalkershop.com.br/arquivos/artigos/sub_wifi/icon_mws.png'/>"); //DEFINIÇÃO DO ICONE DA PÁGINA
  client.println("<link rel='stylesheet' type='text/css' href='https://blogmasterwalkershop.com.br/arquivos/artigos/sub_wifi/webpagecss.css' />"); //REFERENCIA AO ARQUIVO CSS (FOLHAS DE ESTILO)
  client.println("<title>MasterWalker Shop - Modulo WiFi ESP8266 para Arduino</title>"); //ESCREVE O TEXTO NA PÁGINA
  client.println("</head>"); //FECHA A TAG "head"
  
  //AS LINHAS ABAIXO CRIAM A PÁGINA HTML
  client.println("<body>"); //ABRE A TAG "body"
  client.println("<img alt='masterwalkershop' src='https://blogmasterwalkershop.com.br/arquivos/artigos/sub_wifi/logo_mws.png' height='156' width='700' />"); //LOGO DA MASTERWALKER SHOP
  client.println("<p style='line-height:2'><font>Modulo WiFi ESP8266 para Arduino</font></p>"); //ESCREVE O TEXTO NA PÁGINA
  client.println("<font>ESTADO ATUAL DO LED</font>"); //ESCREVE O TEXTO NA PÁGINA
  
  if (statusLed == HIGH){ //SE VARIÁVEL FOR IGUAL A HIGH (1), FAZ
    client.println("<p style='line-height:0'><font color='green'>LIGADO</font></p>"); //ESCREVE "LIGADO" NA PÁGINA
    client.println("<a href=\"/L\">APAGAR</a>"); //COMANDO PARA APAGAR O LED (PASSA O PARÂMETRO /L)
  }else{ //SENÃO, FAZ
    if (statusLed == LOW){ //SE VARIÁVEL FOR IGUAL A LOW (0), FAZ
    client.println("<p style='line-height:0'><font color='red'>DESLIGADO</font></p>"); //ESCREVE "DESLIGADO" NA PÁGINA
    client.println("<a href=\"/H\">ACENDER</a>"); //COMANDO PARA ACENDER O LED (PASSA O PARÂMETRO /H)
    }
  }
  client.println("<hr />"); //TAG HTML QUE CRIA UMA LINHA NA PÁGINA
  client.println("<hr />"); //TAG HTML QUE CRIA UMA LINHA NA PÁGINA
  client.println("</body>"); //FECHA A TAG "body"
  client.println("</html>"); //FECHA A TAG "html"
  delay(1); //INTERVALO DE 1 MILISSEGUNDO
}*/