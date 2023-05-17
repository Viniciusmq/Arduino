#include <Ethernet.h> // biblioteca de comunicacao para operar o ethernet shield
#include <MySQL_Connection.h> //biblioteca para conectar ao servidor
#include <MySQL_Cursor.h> // biblioteca para realizar tratativas de requisicao de dados
#include <Wire.h> //biblioteca para ????
#include <LCD.h> // biblioteca para operar o LCD e mostrar os dados na tela
#include <LiquidCrystal_I2C.h> //Biblioteca LCD para pegar o endereco da I2C do Display
#include <SPI.h>
#include <nRF24L01.h> //biblioteca do modulo de radio frequencia
#include <RF24.h>//biblioteca do modulo de radio frequencia
#include <DS1307RTC.h> //Biblioteca do relogio RTC  

byte mac_addr[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // endereco mac do modulo de ethernet
const byte address[6] = "00001"; //codigo do sinal de operacao do modulo de radiofrequencia

RF24 radio(7, 10); //  define as portas do arduino que irão ser usadas para receber os dados

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //estabelece o endereco e comunicao para reproducao de informacaoes no display lcd

IPAddress server_addr(192, 168, 0, 1); // Endereco IP do Servido SQL
char user[] = "";                 // Usuario de Login do banco de dados SQL
char password[] = "";             // Senha do usuario do banco de dados SQL

char INSERIR_BANCO[] = "INSERT INTO meteorologico.registro (idData,idHora,idEndereco,idEstacao,idUsuario,temperatura,umidadeRelativa,irradiacaoSolar,precipitacaoPluviometrica,velocidadeVento,direcaoVento) VALUES (%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%s)";
// variavel que recebe um string como a funcao de insercao de dados na tabela registro do banco de dados, seguida de seus campos
char Select1[] = "SELECT idHora FROM meteorologico.hora WHERE hora = '%d:%d:%d'"; // Comando select para retrono de informacoes do banco de dados para verificacao dos dados
char Insert1[] = "INSERT INTO meteorologico.hora (hora) VALUES ('%d:%d:%d')"; // comando de insercao do dado hora concatenado
char Select2[] = "SELECT idData FROM meteorologico.data WHERE data = '%d-%d-%d'";// comando de retorno de dados de data do banco
char Insert2[] = "INSERT INTO meteorologico.data (data) VALUES ('%d-%d-%d')"; // insercao de dados de data no banco de dados
char query[228]; // string para armazenamento da requisicao completa.

// instaciamento das variaveis globais e definicao das mesma como zero
double temp = 0;
double umi = 0;
double rad = 0;
double prec = 0; 
double velo = 0;
char direc;
double temp1 = 0;
double umi1 = 0;
double rad1 = 0;
double prec1 = 0; 
double velo1 = 0;
char direc1;

int hora, minu, seg, dia, mes, ano; // variaveis de data e hora
int hora1,minu1,seg1,dia1,mes1,ano1; // varias secundarias de data e hora para conferencia de dados
int G = 0; // variavel que indica a quantidade de gravacoes ja feita no banco
int g = 0;
int h,m,s,D,M,A;

// endereco dos botoes fisicos para interacao com o menu
int Botao1 = 3;  
int Botao2 = 2;
int Botao3 = 4;

EthernetClient client;
MySQL_Connection conn((Client *)&client);

void setup()
{

  Serial.begin(115200);

  lcd.begin(20, 4);
  radio.begin();

  pinMode(Botao1, INPUT);
  pinMode(Botao2, INPUT);
  pinMode(Botao3, INPUT);
 
  radio.openReadingPipe(0, address);
  radio.startListening();

  Ethernet.begin(mac_addr);

  lcd.setCursor(0, 0);
  lcd.print("Iniciando Sistema");
  delay(3000);
  // verificacao de conexao com o banco de dados
  if (conn.connect(server_addr, 3306, user, password))
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Conectado ao BD");
    delay(5000);
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connection failed with BD");
    Serial.println("Connection failed...:(");
    delay(5000);
  }

  lcd.clear();
  
  setSyncProvider(RTC.get);
  //Verifica se o valor de tempo não está definido.
  if (timeStatus() != timeSet) {
    //Escreve no LCD:
    Serial.print("BAT. do RTC INOP");
    //Atraso para o próximo passo.
    delay(3000);
    //Apaga a informação do LCD.
    lcd.clear();
    //Alteração dos dados de hora e data.
    h = 0;
    m = 0;
    s = 0;
    D = 1;
    M = 1;
    A = 2020;
    //Grava na RTC uma nova hora e data.
    setTime(h, m , s , D , M , A );
    RTC.set(now());
    }else {
    //Escreve no LCD.
    lcd.print("RTC Operante");
    //Atraso para o próximo passo.
    delay(3000);
    //Apaga a informação do LCD.
    lcd.clear();
  }
    

  
}

void loop(){
   
  if(digitalRead(Botao1) == HIGH && digitalRead(Botao2) == HIGH){
    Menu();
  }

  if(digitalRead(Botao3) == HIGH){
    Configuracao();
  }
 
  lcd.setCursor(0,0);
  lcd.print("modulo de receptcao");
  
  if(g == 1){
    if (radio.available())
    {
      radio.read(&temp, sizeof(hora));
      Serial.println(temp);

      radio.read(&temp, sizeof(minu));
      Serial.println(temp);

      radio.read(&temp, sizeof(seg));
      Serial.println(temp);

      radio.read(&temp, sizeof(dia));
      Serial.println(temp);

      radio.read(&temp, sizeof(mes));
      Serial.println(temp);

      radio.read(&temp, sizeof(ano));
      Serial.println(temp);

      radio.read(&temp, sizeof(temp));
      Serial.println(temp);

      radio.read(&umi, sizeof(umi));
      Serial.println(umi);

      radio.read(&rad, sizeof(rad));
      Serial.println(rad);

      radio.read(&prec, sizeof(prec));
      Serial.println(prec);

      radio.read(&velo, sizeof(velo));
      Serial.println(velo);

      radio.read(&direc, sizeof(direc));
      Serial.println(direc);

      if(hora != hora1) hora1 = hora;
      if(minu != minu1) minu1 = minu;
      if(seg != seg1) seg1 = seg;
      if(dia != dia1) dia1 = dia;
      if(mes != mes1) mes1 = mes;
      if(ano != ano1) ano1 = ano;
      if(temp != temp1) temp1 = temp;
      if(umi != umi1) umi1 = umi;
      if(rad != rad1) rad1 = rad;
      if(prec != prec1) prec1 = prec;
      if(velo != velo1) velo1 = velo;
      if(direc != direc1) direc1 = direc; 
  
  
    
      lcd.setCursor(0,2);
      lcd.print(G);
      lcd.print("G");

      //Banco_de_Dados();
    }
  
  }
    
}


void Banco_de_Dados(){
  
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  
  int idData = Select_idData(ano1,mes1,dia1);
  int idHora = Select_idHora(hora1,minu1,seg1);
   

  sprintf(query, INSERIR_BANCO,idData,idHora,1,152,1,temp1, umi1, rad1, prec1, velo1, direc1);
  
  cur_mem->execute(query);
  
  delete cur_mem;

  G = G+1;

}

int Select_idHora(int h, int m, int s){
  row_values *row = NULL;
  long head_count = 0;

  MySQL_Cursor cur = MySQL_Cursor(&conn);

  sprintf(query, Select1,h,m,s);

  cur.execute(query);

  cur.get_columns();
  // Read the row (we are only expecting the one)
  do {
    row = cur.get_next_row();
    if (row != NULL) {
      head_count = atol(row->values[0]);
    }
  } while (row != NULL);
  // Now we close the cursor to free any memory
  cur.close();

  if(head_count == 0){ // o horario não foi inserido no banco de dados

    sprintf(query, Insert1, h, m, s);
    
    cur.execute(query);

    cur.close();

    head_count = Select_idHora(h,m,s);
  }

  return head_count;

}

int Select_idData(int ano, int mes, int dia){
  row_values *row = NULL;
  long head_count = 0;

  MySQL_Cursor cur = MySQL_Cursor(&conn);

  sprintf(query, Select2, ano, mes, dia);

  cur.execute(query);

  cur.get_columns();
  // Read the row (we are only expecting the one)
  do {
    row = cur.get_next_row();
    if (row != NULL) {
      head_count = atol(row->values[0]);
    }
  } while (row != NULL);
  // Now we close the cursor to free any memory
  cur.close();

  if(head_count == 0){ // o horario não foi inserido no banco de dados

    sprintf(query, Insert2, ano, mes, dia);
    
    cur.execute(query);

    cur.close();

    head_count = Select_idData(ano,mes,dia);
  }

  return head_count;
}

void Configuracao(){
  if(g == 0){

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iniciar Gravacao?");
    lcd.setCursor(0,1);
    lcd.print("(S/N)");
    delay(800);
    while(digitalRead(Botao1)== LOW && digitalRead(Botao2)==LOW);
    delay(300);

    
    if(digitalRead(Botao1) == HIGH){
     lcd.clear();
      g = 1;
      G = 0;
     return; 
    }
  } else if (g == 1){

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parar Gravacao?");
    lcd.setCursor(0,2);
    lcd.print("(S/N)");
    delay(800);
    while(digitalRead(Botao1)== LOW && digitalRead(Botao2)==LOW);
    delay(200);

    if(digitalRead(Botao1) == HIGH){
      lcd.clear();
      g = 0;
      G = 0;
      return;
    }
  }
  lcd.clear();
}

void Menu(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Conf.Horas(S/N)");
    delay(800);
    while(digitalRead(Botao1)== LOW && digitalRead(Botao2)==LOW);
    delay(200);
    
    if(digitalRead(Botao1== HIGH)){
      setHour();
    }

    
    lcd.print("Conf.Minuto(S/N)");
    delay(800);
    while(digitalRead(Botao1)== LOW && digitalRead(Botao2)==LOW);
    delay(300);
     
    if(digitalRead(Botao1== HIGH)){
        setMinute();
    }
    

    lcd.print("Conf.Segund(S/N)");
    delay(800);
    while(digitalRead(Botao1)== LOW && digitalRead(Botao2)==LOW);
    delay(200);
    
    if(digitalRead(Botao1== HIGH)){
        setSeconds();
    }
    
    lcd.print("Conf.Dia(S/N)");
    delay(800);
    while(digitalRead(Botao1)== LOW && digitalRead(Botao2)==LOW);
    delay(200);

    if(digitalRead(Botao1== HIGH)){
        setDay();
    }
       
    lcd.print("Conf.Mes(S/N)");
    delay(800);
    while(digitalRead(Botao1)== LOW && digitalRead(Botao2)==LOW);
    delay(200);
    
    if(digitalRead(Botao1== HIGH)){
        setMonth();
    }

    lcd.print("Conf.Ano(S/N)");
    delay(800);
    while(digitalRead(Botao1)== LOW && digitalRead(Botao2)==LOW);
    delay(200);
    
    if(digitalRead(Botao1== HIGH)){
        setYear();
    }
   
    RTC.set(now());
}


void setHour(){
    do{       
        lcd.setCursor(0,0);
        digitalRead(Botao1);
       
        
        if(digitalRead(Botao1) == HIGH){
            lcd.print("Insira Horas...");
            h = hour() + 1;
            setTime(h, minute(), second(), day(), month(), year());
        
            if(h>23){
              lcd.clear();  
              lcd.setCursor(0,0);
              lcd.print("Insira Horas...");
              h=0;
            }            
        
            lcd.setCursor(0,1);
            lcd.print(h);
            while(digitalRead(Botao1)==HIGH);
            delay(100);
        }        
             
    }while(digitalRead(Botao2)==LOW); // digzer que nao quer alterar hora.
    
    lcd.clear();
    lcd.setCursor(0,0);
}
void setMinute(){
    
    do{
        lcd.setCursor(0,0);
        digitalRead(Botao1);
        
        if(digitalRead(Botao1) == HIGH){
            lcd.print("Insira Minutos..");
            m = minute() + 1;
            setTime(hour(), m, second(), day(), month(), year());
            
            if(m>59){
              lcd.clear();  
              lcd.setCursor(0,0);
              lcd.print("Insira Minutos..");
              m=0;
            }

            lcd.setCursor(0,1);
            lcd.print(m);
            while(digitalRead(Botao1)==HIGH);
            delay(100);
        }        
    }while(digitalRead(Botao2)==LOW);
    
    lcd.clear();
    lcd.setCursor(0,0);
}
void setSeconds(){
    do{
        lcd.setCursor(0,0);
        digitalRead(Botao1);
        
        if(digitalRead(Botao1) == HIGH){
            lcd.print("Insira Segundos..");
           
            s = second() + 1;
            setTime(hour(), minute(), s, day(), month(), year());
           
            if(s>59){
               lcd.clear();  
               lcd.setCursor(0,0);  
               lcd.print("Insira Segundos..");
               s=0;
            }

            lcd.setCursor(0,1);
            lcd.print(s);
            while(digitalRead(Botao1)==HIGH);
            delay(100);
        }        
    }while(digitalRead(Botao2)==LOW);
    lcd.clear();
    lcd.setCursor(0,0);
}
void setDay(){
    do{
        lcd.setCursor(0,0);
        digitalRead(Botao1);
        
        if(digitalRead(Botao1) == HIGH){
            lcd.print("Insira o dia...");
           
            D = day() + 1;
            setTime(hour(), minute(), second(), D, month(), year());
           
            if(D>31){
               lcd.clear();  
               lcd.setCursor(0,0);
               lcd.print("Insira o dia...");
               lcd.setCursor(0,1);
               D=1;
            }

            lcd.setCursor(0,1);
            lcd.print(D);
            while(digitalRead(Botao1)==HIGH);
            delay(100);
        }        
    }while(digitalRead(Botao2)==LOW);
    lcd.clear();
    lcd.setCursor(0,0);

}
void setMonth(){
    do{
        lcd.setCursor(0,0);
        digitalRead(Botao1);
        
        if(digitalRead(Botao1) == HIGH){
            lcd.print("Insira o mes...");
           
            M = month() + 1;
            setTime(hour(), minute(), second(), day(), M, year());
           
            if(M>12){
               lcd.clear();  
               lcd.setCursor(0,0); 
               lcd.print("Insira o mes...");
               lcd.setCursor(0,1); 
               M=1;
            }

            lcd.setCursor(0,1);
            lcd.print(M);
            while(digitalRead(Botao1)==HIGH);
            delay(100);
        }        
    }while(digitalRead(Botao2)==LOW);
    lcd.clear();
    lcd.setCursor(0,0);
}
void setYear(){
    do{
    
        lcd.setCursor(0,0);
        digitalRead(Botao1);
        
        if (digitalRead(Botao1) == HIGH) {
           lcd.print("Insira o ano...");
           
            A = year() + 1;
            
            setTime(hour(), minute(), second(), day(), month(), A);
               
           lcd.setCursor(0,1);
            lcd.print(A);
            while(digitalRead(Botao1)==HIGH);
            delay(100);
        } 
        
       if(digitalRead(Botao3)==HIGH){
            lcd.print("Insira o ano...");
           
            A = year() - 1;
            setTime(hour(), minute(), second(), day(), month(), A);
           
            lcd.setCursor(0,1);
            lcd.print(A);
            while(digitalRead(Botao3)==HIGH);
            delay(100);
        }  

    }while(digitalRead(Botao2)==LOW);
    lcd.clear();
}
void Relogio(){
    
    
    if (timeStatus() == timeSet) {
        lcd.setCursor(0,0);

        if (hour() < 10)
            lcd.print('0');
            lcd.print(hour());
            lcd.print(":");
        if (minute() < 10)
            lcd.print('0');
            lcd.print(minute());
           /* lcd.print(":");
          if (second() < 10)
            lcd.print('0');
            lcd.print(second());*/
    }else{

        lcd.print("Bateria do RTC INOP");
        delay(3000);
        h = 0;
        m = 0;
        s = 0;
        D = 1;
        M = 1;
        A = 2018;
        setTime(h, m, s, D, M, A);
        RTC.set(now());
    }
}
