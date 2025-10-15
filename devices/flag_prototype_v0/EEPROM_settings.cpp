/*
 * EEprom mapp
 * [0] -> [29] Device defaults
 * [10 -> 13] - ROUNDMINUTES
 * [14 -> 17] - SCORESECONDS
 * [18 -> 21] - CAPTURESECONDS
 * [16] - SOUND ENABLE
 * [30] -> []Game history
 * [30 - 35] - 1.
 * [36 - 41] - 2.
 * [42 - 47] - 3.
*/
#include "EEPROM_settings.h"


// ------ EEPROM ------
uint32_t             EEPROM_SCORESECONDS; 
uint32_t             EEPROM_ROUNDMINUTES;
uint32_t             EEPROM_CAPTURESECONDS;

uint32_t SCORESECONDS;  
uint32_t CAPTURESECONDS;  

int   RedAddress[] = {30, 36, 42};
int GreenAddress[] = {32, 38, 44};
int BlueAddress[] = {34, 40, 46};

long EEPROMReadlong(long address) {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void writeIntIntoEEPROM(int address, int number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

void readSettingsEEPROM() {
  /*
   * int EEPROM_ROUNDMINUTES -> [180 - 181]; int EEPROM_SCORESECONDS -> [182-183]; int EEPROM_CAPTURESECONDS -> [184-185]; boolean EEPROM_SOUND[186]; boolean EEPROM_PASSWORD[187]; char EEPROM_saved_password[8];
   *       ROUNDMINUTES                             SCORESECONDS                           CAPTURESECONDS
   */
  EEPROM_ROUNDMINUTES = EEPROMReadUint32(10);
  if (EEPROM_ROUNDMINUTES >= 999) EEPROM_ROUNDMINUTES = ROUND_TIME_30M;
  EEPROM_SCORESECONDS = EEPROMReadUint32(14);
  if (EEPROM_SCORESECONDS >= 999) EEPROM_SCORESECONDS = 40;
  EEPROM_CAPTURESECONDS = EEPROMReadUint32(18);
  // if (EEPROM_CAPTURESECONDS >= 999) EEPROM_CAPTURESECONDS = 5;
  // EEPROM_SOUND = EEPROM.read(16); // sound bit

  // Activate new values
  ROUNDMINUTES = EEPROM_ROUNDMINUTES;
  if (ROUNDMINUTES < 1) ROUNDMINUTES = 1;
  SCORESECONDS = EEPROM_SCORESECONDS;
  if (SCORESECONDS < 1) SCORESECONDS = 1;
  CAPTURESECONDS = EEPROM_CAPTURESECONDS;
  if (CAPTURESECONDS < 1) CAPTURESECONDS = 1;
  // soundEnable = EEPROM_SOUND;

  // if (soundEnable == false) {
  //       strncpy(sound_text, string_off, strlen(string_off) + 1);
  // } else {
  //       strncpy(sound_text, string_on, strlen(string_on) + 1);
  // }
  // if (EEPROM_SOUND == false) {
  //       strncpy(EEPROM_sound_text, string_off, strlen(string_off) + 1);
  // } else {
  //       strncpy(EEPROM_sound_text, string_on, strlen(string_on) + 1);
  // }
}

void writeSettingsEEPROM() {
  EEPROMWriteUint32(10, EEPROM_ROUNDMINUTES);
  EEPROMWriteUint32(14, EEPROM_SCORESECONDS);
  EEPROMWriteUint32(18, EEPROM_CAPTURESECONDS);
  // EEPROM.write(16, EEPROM_SOUND);

  // Activate new values
  ROUNDMINUTES = EEPROM_ROUNDMINUTES;
  SCORESECONDS = EEPROM_SCORESECONDS;
  CAPTURESECONDS = EEPROM_CAPTURESECONDS;
  // soundEnable = EEPROM_SOUND;
  // if (soundEnable == false) {
  //       strncpy(sound_text, string_off, strlen(string_off) + 1);
  //   } else {
  //       strncpy(sound_text, string_on, strlen(string_on) + 1);
  //   }
  // if (EEPROM_SOUND == false) {
  //       strncpy(EEPROM_sound_text, string_off, strlen(string_off) + 1);
  //   } else {
  //       strncpy(EEPROM_sound_text, string_on, strlen(string_on) + 1);
  //   }
}

void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }



void EEPROMWriteUint32(long address, uint32_t value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }

uint32_t EEPROMReadUint32(long address) {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

// void GameSaveHistory(long redTime, long greenTime) {
//    int RedHistSize = sizeof(RedAddress) / sizeof(int);
//    int GreenHistSize = sizeof(GreenAddress) / sizeof(int);
//    int BlueHistSize = sizeof(BlueAddress) / sizeof(int);
//    unsigned long RedHistory[RedHistSize];
//    unsigned long GreenHistory[GreenHistSize];
//    unsigned long BlueHistory[BlueHistSize];
//    //Create red history array
//    for (int i = 0; i < RedHistSize; i++) {
//        unsigned long RedActualValue = EEPROMReadlong(RedAddress[i]);
//        unsigned long GreenActualValue = EEPROMReadlong(GreenAddress[i]);
//        unsigned long BlueActualValue = EEPROMReadlong(BlueAddress[i]);
//        RedHistory[i] = RedActualValue;
//        GreenHistory[i] = GreenActualValue;   
//        BlueHistory[i] = BlueActualValue;     
//    }
   
//    // rotate History array
//    for (int i = RedHistSize - 1; i > 0; i--) RedHistory[i] = RedHistory[i - 1];
//    for (int i = GreenHistSize - 1; i > 0; i--) GreenHistory[i] = GreenHistory[i - 1];
//    for (int i = BlueHistSize - 1; i > 0; i--) BlueHistory[i] = BlueHistory[i - 1];
//    //Insert new time values 
//    GreenHistory[0] = greenTime;
//    RedHistory[0] = redTime;
//    BlueHistory[0] = blueTime;
//    //Write back 
//    for (int i = 0; i < RedHistSize; i++)EEPROMWritelong(RedAddress[i], RedHistory[i]);    
//    for (int i = 0; i < GreenHistSize; i++)EEPROMWritelong(GreenAddress[i], GreenHistory[i]);    
//    for (int i = 0; i < BlueHistSize; i++)EEPROMWritelong(BlueAddress[i], BlueHistory[i]);   
// }

// TODO rework for 3 commands
// void GameShowHistory() {
//    cls();
//    int RedHistSize = sizeof(RedAddress) / sizeof(int);
//    int GreenHistSize = sizeof(GreenAddress) / sizeof(int);
//    int BlueHistSize = sizeof(BlueAddress) / sizeof(int);
//    unsigned long RedHistory[RedHistSize];
//    unsigned long GreenHistory[GreenHistSize];
//    unsigned long BlueHistory[BlueHistSize];
//    //Create  history array
//    for (int i = 0; i < RedHistSize; i++) {
//        unsigned long RedActualValue = EEPROMReadlong(RedAddress[i]);
//        unsigned long GreenActualValue = EEPROMReadlong(GreenAddress[i]);
//        unsigned long BluenActualValue = EEPROMReadlong(BlueAddress[i]);
//        RedHistory[i] = RedActualValue;
//        GreenHistory[i] = GreenActualValue;
//        BlueHistory[i] = BlueActualValue;     
//    }
//     //Print history
//    int scrolIndex = 0;
//    cls();
//         lcd.setCursor(0,0);
//         lcd.print(F("=Game History="));
//         lcd.setCursor(0,1);
//         lcd.print(scrolIndex); 
//         lcd.print("  G-");
//         printHistoryTime(GreenHistory[scrolIndex]);
//         lcd.print(" R-");
//         printHistoryTime(RedHistory[scrolIndex]);
//         lcd.setCursor(0,2);
//         lcd.print(scrolIndex + 1);
//         lcd.print("  G-");
//         printHistoryTime(GreenHistory[scrolIndex + 1]);
//         lcd.print(" R-");
//         printHistoryTime(RedHistory[scrolIndex + 1]);
//         lcd.setCursor(0,3);
//         lcd.print(scrolIndex + 2);
//         lcd.print("  G-");
//         printHistoryTime(GreenHistory[scrolIndex + 2]);
//         lcd.print(" R-");
//         printHistoryTime(RedHistory[scrolIndex + 2]);
   
  
//    while(1){
//     var = keypad.waitForKey();
//     if (var == '2') { //up
//       if (scrolIndex > 0) {
//         cls();
//         scrolIndex--;
//         lcd.setCursor(0,0);
//         lcd.print(F("=Game History="));
//         lcd.setCursor(0,1);
//         lcd.print(scrolIndex); 
//         lcd.print(F("  G-"));
//         printHistoryTime(GreenHistory[scrolIndex]);
//         lcd.print(F(" R-"));
//         printHistoryTime(RedHistory[scrolIndex]);
//         lcd.setCursor(0,2);
//         lcd.print(scrolIndex + 1);
//         lcd.print(F("  G-"));
//         printHistoryTime(GreenHistory[scrolIndex + 1]);
//         lcd.print(F(" R-"));
//         printHistoryTime(RedHistory[scrolIndex + 1]);
//         lcd.setCursor(0,3);
//         lcd.print(scrolIndex + 2);
//         lcd.print(F("  G-"));
//         printHistoryTime(GreenHistory[scrolIndex + 2]);
//         lcd.print(F(" R-"));
//         printHistoryTime(RedHistory[scrolIndex + 2]);
//       }
//     }
//     if (var == '8') { //down
//       if (scrolIndex < RedHistSize - 3) {
//         cls();
//         scrolIndex++;
//         lcd.setCursor(0,0);
//         lcd.print(F("=Domination History="));
//         lcd.setCursor(0,1);
//         lcd.print(scrolIndex);
//         lcd.print(F("  G-"));
//         printHistoryTime(GreenHistory[scrolIndex]);
//         lcd.print(F(" R-"));
//         printHistoryTime(RedHistory[scrolIndex]);
//         lcd.setCursor(0,2);
//         lcd.print(scrolIndex + 1);
//         lcd.print(F("  G-"));
//         printHistoryTime(GreenHistory[scrolIndex + 1]);
//         lcd.print(F(" R-"));
//         printHistoryTime(RedHistory[scrolIndex + 1]);
//         lcd.setCursor(0,3);
//         lcd.print(scrolIndex + 2);
//         lcd.print(F("  G-"));
//         printHistoryTime(GreenHistory[scrolIndex + 2]);
//         lcd.print(F(" R-"));
//         printHistoryTime(RedHistory[scrolIndex + 2]);
//       }
//     }
//     if(var == '#' ){
//       Bbipp(30);
//       break;
//     }
//    } 
// }

