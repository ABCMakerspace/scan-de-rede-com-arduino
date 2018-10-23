#include <SPI.h>
#include <Ethernet.h>
#include <ICMPPing.h>

//DEFINE MANUALMENTE O MAC ADDRESS E O IP DO ARDUINO
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
IPAddress ip(192, 168, 1, 155);
 
SOCKET pingSocket = 0;

//VARIAVEIS PARA CONTROLE DOS DISPOSITIVOS
int dispositivosTotal = 0;
int totalUltimoScan = 0;

ICMPPing ping(pingSocket, (uint16_t)random(0, 255));

void setup() {
  Serial.begin(9600); //INICIA O MONITOR SERIAL

  Ethernet.begin(mac, ip); //INICIA O SHIELD ETHERNET
  
  Serial.println("Setup Concluído\n");
}

void loop() {
    escanearIPs(); //CHAMA O MÉTODO DE ESCANEAR
    //delay(1000 * 60 * 5); //ESPERA 5 MINUTOS ANTES DE REALIZAR OUTRO SCAN
    delay(60000);
}

void escanearIPs() {
  Serial.println("ESCANEANDO REDE...\n");
  
  for(int i = 2; i <= 10; i++) { //ESTRUTURA DE REPETIÇÃO DE 2 ATÉ 244
    char buffer [256];
    IPAddress pingAddr(192,168, 1, i); //ENDEREÇO DE IP PARA PINGAR

    Serial.print("Testando o IP: ");
    Serial.print(pingAddr);
    Serial.print(" | ");
    
    ICMPEchoReply respostaPing = ping(pingAddr, 2); //RESPOSTA DO PING AO ENDEREÇO DE IP CRIADO
  
    if(respostaPing.status == SUCCESS) { //CASO O PING RESPONDER
      sprintf(buffer, "%d.%d.%d.%d conectado", respostaPing.addr[0], respostaPing.addr[1], respostaPing.addr[2], respostaPing.addr[3]);
      totalUltimoScan += 1;
    } else { //CASO NÃO RESPONDER
      sprintf(buffer, "%d.%d.%d.%d offline", pingAddr[0], pingAddr[1], pingAddr[2], pingAddr[3]);
    }

    Serial.println(buffer); //ESCREVE A RESPOSTA
  }

  if(totalUltimoScan > dispositivosTotal) { //CHECA SE O NÚMERO DE DISPOSITIVOS ENCONTRADOS É MAIOR QUE OS DISPOSITIVOS ENCONTRADO ANTERIORMENTE
    Serial.println("Encontrado(s) novo(s) dispositivo(s)");
    dispositivosTotal = totalUltimoScan;
  } else { //CASO NÃO FOR MAIOR
    Serial.println("Não há alterações em sua rede.");
  }
  
  Serial.println("");
  Serial.print("Dispositivos conectados atualmentee: ");
  Serial.println(totalUltimoScan);
  Serial.print("Total de dispositivos já conectados: ");
  Serial.println(dispositivosTotal);
  
  totalUltimoScan = 0;
}


