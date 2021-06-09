#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#define LONGITUD_BYTES 18
#define LONGITUD_BYTES_ESCRITURA 16
/*
Pines para conectar el lector
*/
#define RST_PIN D3
#define SS_PIN D4
// Constantes para el ejemplo
#define MODO_LECTURA 1
#define MODO_ESCRITURA 2
#define MODO MODO_ESCRITURA
MFRC522 lector(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key clave;

bool leer(char mensaje[LONGITUD_BYTES])
{
  if (!lector.PICC_IsNewCardPresent())
  {
    return false;
  }
  if (!lector.PICC_ReadCardSerial())
  {
    Serial.println("Error leyendo serial");
    return false;
  }
  byte bloque = 1; // El bloque que leemos
  byte longitud = LONGITUD_BYTES;
  byte buferLectura[LONGITUD_BYTES];

  MFRC522::StatusCode estado;
  estado = lector.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &clave, &(lector.uid));
  if (estado != MFRC522::STATUS_OK)
  {
    Serial.println("Error autenticando");
    Serial.println(lector.GetStatusCodeName(estado));
    return false;
  }
  estado = lector.MIFARE_Read(bloque, buferLectura, &longitud);
  if (estado != MFRC522::STATUS_OK)
  {
    Serial.println("Error leyendo bloque");
    Serial.println(lector.GetStatusCodeName(estado));
    return false;
  }

  for (uint8_t i = 0; i < LONGITUD_BYTES - 2; i++)
  {
    mensaje[i] = buferLectura[i];
  }
  // Ya pueden retirar la tarjeta

  lector.PICC_HaltA();
  lector.PCD_StopCrypto1();
  return true;
}
bool escribir(char cadena[LONGITUD_BYTES_ESCRITURA])
{

  if (!lector.PICC_IsNewCardPresent())
  {
    return false;
  }
  if (!lector.PICC_ReadCardSerial())
  {
    Serial.println("Error leyendo serial");
    return false;
  }

  byte bloque = 1;
  byte buferEscritura[LONGITUD_BYTES_ESCRITURA];
  // Copiar cadena al búfer
  for (uint8_t i = 0; i < LONGITUD_BYTES_ESCRITURA; i++)
  {
    buferEscritura[i] = cadena[i];
  }
  MFRC522::StatusCode estado;
  estado = lector.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &clave, &(lector.uid));
  if (estado != MFRC522::STATUS_OK)
  {
    Serial.println("Error autenticando");
    Serial.println(lector.GetStatusCodeName(estado));
    return false;
  }
  estado = lector.MIFARE_Write(bloque, buferEscritura, LONGITUD_BYTES_ESCRITURA);
  if (estado != MFRC522::STATUS_OK)
  {
    Serial.println("Error escribiendo bloque");
    Serial.println(lector.GetStatusCodeName(estado));
    return false;
  }
  // Ya pueden retirar la tarjeta

  lector.PICC_HaltA();
  lector.PCD_StopCrypto1();
  return true;
}
void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    // Esperar serial. Nota: la tarjeta NO HARÁ NADA hasta que haya comunicación Serial (es decir, que el monitor serial sea abierto)
    // si tú no quieres esto, simplemente elimina todas las llamadas a Serial
  }
  // Iniciar lector
  SPI.begin();
  lector.PCD_Init();
  // Preparar la clave para leer las tarjetas RFID
  for (byte i = 0; i < 6; i++)
  {
    clave.keyByte[i] = 0xFF;
  }
  Serial.println("Iniciado correctamente");
}

void loop()
{
  if (MODO == MODO_LECTURA)
  {

    char contenidoRfid[LONGITUD_BYTES] = "";
    bool lecturaExitosa = leer(contenidoRfid);
    if (lecturaExitosa)
    {
      Serial.println("Lo que hay escrito es:");
      Serial.println(contenidoRfid);
    }
    else
    {
      Serial.println("Error leyendo. Tal vez no hay RFID presente");
    }
  }
  else if (MODO == MODO_ESCRITURA)
  {
    char mensaje[] = "parzibyte";
    bool escrituraExitosa = escribir(mensaje);
    if (escrituraExitosa)
    {
      Serial.println("Escrito ok");
    }
    else
    {
      Serial.println("Error escribiendo. Tal vez no hay RFID presente");
    }
  }
  delay(1000);
}