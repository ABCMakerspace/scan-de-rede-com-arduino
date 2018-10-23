#include <SPI.h>
#include <ICMPPing.h>

// Define o MAC ADDRESS da placa
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0xAD};

SOCKET pingSocket = 0;

byte socketStat[MAX_SOCK_NUM];
byte destMac[6];
IPAddress ipInicial;
IPAddress ipFinal;

//VARIAVEIS PARA CONTROLE DOS DISPOSITIVOS
int dispositivosTotal = 0;
int totalUltimoScan = 0;

// Instancia todos os objetos de comunicação
// (W5100, ETHERNET e ICMPPing)
ICMPPing ping(pingSocket, (uint16_t)random(0, 255));

void setup() {
  Serial.begin(9600); // Inicia o monitor serial
  while (!Serial) {} // Espera pela porta serial. Necessario para modelos com porta USB on-chip.
  Serial.println("\n\nComunicacao serial iniciada.");

  Serial.println("\nObtendo endereco IP do servidor DHCP...");
  while (Ethernet.begin(mac) == 0) {        // Inicializa o shield ethernet e obtem o endereco IP.
    Serial.println("Falha ao obter endereco IP...");
  };
  Serial.print("Endereco IP do Arduino: ");
  printIP(Ethernet.localIP());

  // Determina qual a faixa de enderecos IP da rede.
  defineRange();
  Serial.print("\nIP inicial da rede: ");
  printIP(ipInicial);
  Serial.print("IP final da rede:     ");
  printIP(ipFinal);

  Serial.println("\nSetup concluido.\n");
}

void loop() {
  escanearIPs(); //CHAMA O MÉTODO DE ESCANEAR
  delay(1000 * 60 * 1); //ESPERA 1 MINUTO(S) ANTES DE REALIZAR OUTRO SCAN
}


void escanearIPs() {
  Serial.println("ESCANEANDO REDE...\n");

  byte b0, b1, b2, b3;
  for (b0 = ipInicial[0]; b0 <= ipFinal[0]; b0++) {
    for (b1 = ipInicial[1]; b1 <= ipFinal[1]; b1++) {
      for (b2 = ipInicial[2]; b2 <= ipFinal[2]; b2++) {
        for (b3 = ipInicial[3]; b3 <= ipFinal[3]; b3++) {
          IPAddress pingAddr(b0, b1, b2, b3); //ENDEREÇO DE IP PARA PINGAR

          Serial.print("Testando o IP: ");
          Serial.print(pingAddr);
          Serial.print(" | ");

          ICMPEchoReply respostaPing = ping(pingAddr, 2); //RESPOSTA DO PING AO ENDEREÇO DE IP CRIADO

          if (respostaPing.status == SUCCESS) { //CASO O PING RESPONDER
            Serial.print("conectado | Mac Address: ");
            printMAC();
            totalUltimoScan += 1;
          } else { //CASO NÃO RESPONDER
            Serial.println("offline");
          }
        }
      }
    }
  }

  if (totalUltimoScan > dispositivosTotal) { //CHECA SE O NÚMERO DE DISPOSITIVOS ENCONTRADOS É MAIOR QUE OS DISPOSITIVOS ENCONTRADO ANTERIORMENTE
    Serial.println("Encontrado(s) novo(s) dispositivo(s)");
    dispositivosTotal = totalUltimoScan;
  } else { //CASO NÃO FOR MAIOR
    Serial.println("Nao ha alteracoes em sua rede.");
  }

  Serial.print("\nDispositivos conectados atualmente: ");
  Serial.println(totalUltimoScan);
  Serial.print("Total de dispositivos ja conectados: ");
  Serial.println(dispositivosTotal);

  totalUltimoScan = 0;
}

// "Imprime" no terminal serial um endereco IP.
void printIP(IPAddress ip)
{
  for (byte i = 0; i < 4; i++)
  {
    Serial.print(ip[i], DEC);
    if (i < 3) Serial.print(".");
  }
  Serial.println();
}

// Determina qual a faixa de enderecos IP da rede a partir
// do endereco IP e da mascara de sub-rede do Arduino.
void defineRange()
{
  IPAddress ip = Ethernet.localIP();
  IPAddress mascara = Ethernet.subnetMask();

  for (byte i = 0; i < 4; i++)
  {
    ipInicial[i] = ip[i] & mascara[i];

    if (ipInicial[i] == Ethernet.localIP()[i])
    {
      ipFinal[i] = ipInicial[i];
    }
    else
    {
      ipFinal[i] = ~(Ethernet.subnetMask()[i]);
    }
    if (i == 3) {
      if (ipInicial[3] == 0) {
        ipInicial[3] = 2;
      }
      if (ipFinal[3] == 255) {
        ipFinal[3] = 254;
      }
    }
  }
}

// "Imprime" no terminal serial um endereco MAC.
void printMAC() {
  W5100.readSnDHAR(0, destMac);
  for (byte j = 0; j < 6; j++) {
    if (destMac[j] < 16) Serial.print(F("0"));
    Serial.print(destMac[j], HEX);
    if (j < 5) Serial.print(":");
  }
  Serial.println();
}
