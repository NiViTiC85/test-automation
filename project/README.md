# Projekt – Testsvit för ett inbyggt system

## Översikt

I detta projekt ska ni i grupper om två skapa en komplett testsvit för biblioteket `libatmega`, som är ett C++-bibliotek för inbyggda system baserat på ATmega328P-mikrokontrollern. Projektet fokuserar på olika testnivåer: enhetstester, komponenttester och hårdvaru-/mjukvaruintegrationstester.

## Arkitektur
Biblioteket är portabelt och kan anpassas till olika arkitekturer via användning av så kallade 
[interfaces](../lectures/L01/cpp/05%20-%20Arv%20och%20interfaces.md) (abstrakta basklasser i C++). 
Bibliotekets logik-implementation använder sig enbart av interfaces i stället för specifika hårdvaruimplementationer.

Detta betyder att samma logik kan köras på exempelvis:
   * ATmega328P (Arduino Uno).
   * STM32-mikrokontrollers.
   * ESP32-enheter.
   * Linux-system (för testning).
   * Vilken annan plattform som helst med rätt drivrutinsimplementationer.

När hårdvaran byts ut behöver endast drivrutinerna skrivas om - resten av systemet förblir oförändrat.

Tack vare interfaces kan också stub-implementationer, som simulerar hårdvarubeteende, användas för komponenttester. Detta gör det möjligt att testa logiken utan tillgång till fysisk hårdvara.

## Syfte

Syftet med projektet är att:
* Förstå och tillämpa olika testnivåer i inbyggda system.
* Skriva enhetstester för isolerade drivrutiner och komponenter.
* Skriva komponenttester för system som använder multipla drivrutiner.
* Identifiera och dokumentera vad som kan och inte kan automatiseras.
* Tillämpa virtuella plattformar samt stub-implementationer.
* Använda Google Test-ramverket för C++.
* **Identifiera och korrigera buggar** - observera att biblioteket kan innehålla buggar som ni förväntas hitta och åtgärda under testningens gång.

## Testarkitektur

### Virtuell hårdvaruplatform

För att möjliggöra enhetstester av hårdvarunära kod utan fysisk ATmega328P-mikrokontroller använder projektet en 
[virtuell hårdvaruplatform](../library/include/arch/test/hw_platform.h), som simulerar ATmega328P:s registerstruktur i minnet.

#### Hur det fungerar

När koden kompileras med `TESTSUITE`-definitionen (vilket sker automatiskt vid testkompilering) 
används den virtuella hårdvaruplatformen istället för den riktiga AVR-hårdvaran:

I headerfilen [arch/avr/hw_platform.h](../library/include/arch/avr/hw_platform.h) väljs automatiskt 
rätt implementation utefter om `TESTSUITE` är definierad:

   ```cpp
   #ifndef TESTSUITE
       #include <avr/io.h>                 // Riktig AVR-hårdvara.
   #else
       #include "arch/test/hw_platform.h"  // Virtuell hårdvara för testning.
   #endif
   ```

I headerfilen [arch/test/hw_platform.h](../library/include/arch/test/hw_platform.h) definieras alla 
AVR-register som mappningar till en byte-array, exempelvis:

   ```cpp
   #define DDRB  test::Memory::data.reg8[2U] // Virtuell version av datariktningsregister B.
   #define PORTB test::Memory::data.reg8[5U] // Virtuell version av portregister B.
   #define PINB  test::Memory::data.reg8[8U] // Virtuell version av pinregister B.
   ```

Drivrutinerna använder samma registerdefinitioner oavsett om de körs på riktig hårdvara eller i form av en testsvit. 
Detta betyder att samma källkod kan testas och köras på båda plattformarna.

#### Fördelar med virtuell hårdvaruplattform

* **Snabb exekvering**: Tester körs direkt på en dator utan att behöva flashas till en mikrokontroller.
* **Inspekterbarhet**: Register kan läsas och verifieras direkt i testerna.
* **Isolation**: Varje test kan starta med nollställda register.
* **Determinism**: Tester ger samma resultat varje gång de körs, vilket underlättar felsökning.

#### Exempel: GPIO-test

Exempelvis hade följande kunnat göras för att unit-testa [GPIO-drivern för ATmega328p](../library/include/driver/gpio/atmega328p.h).
I detta fall konfigurerar vi en lysdiod ansluten till pin 8 (PORTB0) och kontrollerar att motsvarande bitar i hårdvaruregistren
har satts korrekt av GPIO-drivern. Här läser vi från `DDRB` samt `PORTB` i den virtuella hårdvaruplattformen. 
Verifikationen sker via den makroliknande funktionen `EXPECT_TRUE` från Google Test:

```cpp
// Anslut en lysdiod till pin 8 (PORTB0).
gpio::Atmega328p led{8U, gpio::Direction::Output};

// Verifiera att PORTB0 är satt till utport, dvs. att bit 0 i DDRB är ettställd.
EXPECT_TRUE(utils::read(DDRB, 0U));

// Tänd lysdioden.
led.write(true);

// Verifiera att lysdioden är tänd, dvs. att bit 0 i PORTB är ettställd. 
EXPECT_TRUE(utils::read(PORTB, 0U));
```

#### Implementera tester med virtuell hårdvara

När du skriver nya tester (exempelvis för timer-drivern):

1. **Läs datablad**: Förstå vilka register som används och vilka bitar som ska sättas.
2. **Verifiera registervärden**: Använd `EXPECT_EQ` eller `EXPECT_TRUE` för att kontrollera att rätt bitar är satta. Använd gärna `utils::read` från [library/include/utils/utils.h](../library/include/utils/utils.h) för att läsa enskilda bitar.
3. **Simulera hårdvarabeteende**: För mer komplexa tester (som timer callbacks) kan du behöva simulera hårdvarans beteende manuellt (se GPIO-testernas `simulateToggle()`-funktion).
4. **Reset state**: Kom ihåg att återställa register mellan tester för att undvika sidoeffekter.

## Testnivåer

### 1. Enhetstester (unit tests)

Enhetstester validerar enskilda komponenter isolerat från resten av systemet. Dessa tester 
använder den virtuella hårdvaruplattformen för att simulera ATmega328P:s register. 
I detta projekt ska du skriva enhetstester för följande drivrutiner:

#### GPIO-drivrutin (`driver::gpio::Atmega328p`)
* Testa att pinnar kan konfigureras som in- eller utgångar.
* Verifiera att enbart en instans per pin kan användas samtidigt.
* Verifiera att utgångar kan sättas höga/låga.
* Kontrollera att ingångar kan läsas korrekt.
* Testa toggling av pinnar.
* Validera pull-up resistor-konfiguration.

**Testfil**: [library/test/driver/gpio/atmega328p_test.cpp](../library/test/driver/gpio/atmega328p_test.cpp)

#### Serial-drivrutin (`driver::serial::Atmega328p`)
* Testa serial-initialisering med olika baudhastigheter.
* Verifiera att data kan skickas och tas emot korrekt.
* Kontrollera bufferthantering för in- och utdata.
* Testa att interrupts konfigureras korrekt.
* Validera felhantering vid buffert-överflöde.
* Testa läsning och skrivning av textsträngar.

**Testfil**: [library/test/driver/serial/atmega328p_test.cpp](../library/test/driver/serial/atmega328p_test.cpp)

**Tips**: Simulera UART-register (UDR0, UCSR0A, UCSR0B) och verifiera att rätt data skrivs till buffertarna.

#### Smart temperatursensor (`driver::tempsensor::Smart`)
* Testa temperaturavläsning och konvertering.
* Kontrollera ADC-integration.
* Testa prediktionsfunktionalitet med linjär regression.
* Validera felhantering.

**Testfiler**: 
* [library/test/driver/tempsensor/smart_test.cpp](../library/test/driver/tempsensor/smart_test.cpp)

#### Timer-drivrutin (`driver::timer::Atmega328p`)
* Testa timer-initialisering med olika konfigurationer.
* Verifiera att timers kan startas och stoppas.
* Kontrollera callback-funktionalitet vid timeout.
* Validera interrupt-konfiguration.

**Testfil**: [library/test/driver/timer/atmega328p_test.cpp](../library/test/driver/timer/atmega328p_test.cpp)

**Tips**: Använd befintliga hardware-register-simuleringar för att verifiera timer-konfiguration och callback-funktionalitet.

#### Linjär regression (`ml::lin_reg::Fixed`)
* Testa träning av modellen med känd data.
* Verifiera prediktioner.
* Kontrollera hantering av edge cases (t.ex. för få datapunkter, felaktig lärhastighet).

**Testfil**: [library/test/ml/lin_reg/fixed_test.cpp](../library/test/ml/lin_reg/fixed_test.cpp)

**Tips**: Jämför prediktioner mot manuellt beräknade värden för att verifiera korrekthet.

### 2. Komponenttester (component tests)

Komponenttester validerar att flera komponenter fungerar korrekt tillsammans. I detta projekt ska du skriva komponenttester för:

#### Logic-implementation (`logic::Logic`)
Denna klass orkestrerar flera drivrutiner för att implementera systemets huvudlogik:
* LED-kontroll via GPIO.
* Knapphantering med debounce.
* Temperaturavläsning och prediktion.
* Seriell kommunikation.
* Watchdog-hantering.
* EEPROM-persistens.

**Testansats**:
* Skapa och använd stubklasser för alla drivrutiner (GPIO, Timer, Watchdog, Serial, EEPROM, TempSensor).
* Stubklasserna ska ärva från respektive interface och implementera enkel testbar logik.
* Testa att initialisering fungerar korrekt med både giltiga och ogiltiga konfigurationer.
* Verifiera knapphantering och debounce-logik.
* Kontrollera LED-toggling via timers.
* Testa temperaturavläsning och utskrift.
* Validera EEPROM-läsning och skrivning.
* Testa watchdog reset-funktionalitet.

**Stub-implementationer**: Se befintliga stubklasser i `library/include/driver/*/stub.h` som referens för hur stubklasser ska utformas.

**Viktiga aspekter för stubklasser**:
* Stubklasser ska ärva från respektive interface-klass.
* Implementera all funktionalitet som krävs för att testa logic-klassen.
* Använd enkla interna variabler för att spåra tillstånd (t.ex. pin-värden, timer-status).
* Tillhandahåll metoder för att verifiera att korrekta operationer utfördes.
* Håll implementationen enkel och fokuserad på testbarhet.

**Testfil**: `library/test/logic/logic_test.cpp`

### 3. Hårdvaru-/mjukvaruintegrationstester

Vissa tester kan inte automatiseras fullt ut på grund av hårdvaruberoenden. Dessa kräver manuell verifiering eller hårdvara-i-loop (HIL) testning.

#### Tester som inte kan automatiseras:

1. **Fysisk GPIO-verifiering**
   * Faktisk spänningsnivå på pinnar.

2. **Timer-precision på verklig hårdvara**
   * Faktisk timeout-noggrannhet.

3. **Watchdog-reset på verklig hårdvara**
   * Faktisk systemreset.
   * Reset-tid.
   * Power-on-reset beteende.

5. **Seriell kommunikation**
   * Faktisk UART-kommunikation.

6. **EEPROM uthållighet**
   * Dataretention över tid.

#### Semi-automatiserade tester: Python-baserad seriell kommunikation (VG-nivå)

**OBS!** Se [L09 - SW/HW-integrationstestning (del II)](../lectures/L09/README.md) för detaljerad implementation och exempel.

För att underlätta testning av seriell kommunikation och programflöde på verklig hårdvara ska ni skapa ett **Python-skript** som:

1. **Använder rätt arkitektur**:
   * Skapa en `uart/`-katalog med `Interface`, `Driver`, `Stub` och `Factory`-klasser enligt strukturen från [L09](../lectures/L09/README.md).
   * Använd Factory-pattern för att välja mellan `Driver `(Windows) och `Stub` (Linux/Mac).

2. **Läser och skriver data via den seriella porten**:
   * Tar emot och skriver ut data som mikrokontrollern skickar via UART.
   * Formaterar utskriften på ett läsbart sätt (exempelvis via timestamps och färgkodning).
   * Loggar all kommunikation till en fil för senare analys.

3. **Styr programflödet**:
   * Skickar kommandosträngar till mikrokontrollern för att styra dess beteende.
   * Möjliggör interaktiv eller scriptad kontroll av systemet.

**Testfall att implementera** (se L09 för detaljer):
* Toggle-funktionalitet via kommandot `t`.
* Status-avläsning via kommandot `s`.
* Temperaturavläsning via kommandot `r`.
* Felhantering vid ogiltiga kommandon.

**Krav på Python-skriptet**:
* Använd den skapade UART-drivern för seriell kommunikation.
* Implementera robust felhantering (t.ex. om porten inte kan öppnas).
* Tillhandahåll hjälptext som beskriver tillgängliga kommandon.
* Logga all kommunikation med timestamps för senare analys.
* Dokumentera skriptets användning i projektets README.

**Krav på hårdvaruimplementation**:
* Systemet måste modifieras för att lyssna på och svara på kommandon via den seriella porten.
* Använd den seriella drivrutinens string-läsfunktionalitet för att ta emot kommandon.
* Implementera en enkel kommandotolk som kan tolka inlästa tecken såsom beskrivet i L09.
* Dokumentera vilka kommandon som stöds och deras förväntade svar.

**Installation av pyserial**:
```bash
pip install pyserial
```

#### Dokumentation av manuella tester

För varje icke-automatiserbar test ska du dokumentera:
* **Testbeskrivning**: Vad testet verifierar.
* **Förutsättningar**: Hårdvara och setup som krävs.
* **Teststeg**: Steg-för-steg instruktioner.
* **Förväntat resultat**: Vad som ska observeras.
* **Faktiskt resultat**: Vad som observerades vid utförande.
* **Status**: Pass/Fail.

## Tekniska krav

### Verktyg och ramverk
* **Google Test**: För enhetstester och komponenttester.
* **Stubklasser**: Egenimplementerade stubklasser (ej Google Mock) för att simulera hårdvara.
* **GCC/G++**: C++17 eller senare.
* **Make**: För build automation.
* **WSL (Windows Subsystem for Linux)**: Utvecklings- och testmiljö (Ubuntu rekommenderas).

### Kodkvalitet
* Tester ska kompilera utan varningar (`-Wall -Wextra -Werror`).
* Följ befintlig kodstil i projektet.
* Använd beskrivande testnamn som tydligt anger vad som testas.
* Inkludera kommentarer där logiken är komplex.
* Varje test ska testa en sak, exempelvis initiering, skrivning, läsning osv.
* Använd EXPECT- och/eller ASSERT-makron korrekt enligt Google Test best practices.

### Testtäckning
* Täck både "happy path" (normala fall med korrekta indata) och felfall (ogiltiga indata, felhantering).
* Inkludera edge cases och boundary conditions.

## Projektstruktur

```
library/test/
├── makefile                          # Byggkonfiguration för testsviten.
├── testsuite.cpp                     # Startfil för testsviten.
├── README.md                         # Denna fil.
├── driver/
│   ├── adc/
│   │   └── atmega328p_test.cpp       # Befintliga ADC-tester.
│   ├── eeprom/
│   │   └── atmega328p_test.cpp       # Befintliga EEPROM-tester.
│   ├── gpio/
│   │   └── atmega328p_test.cpp       # GPIO-tester, ska implementeras.
│   ├── serial/
│   │   └── atmega328p_test.cpp       # Serial-tester, ska implementeras.
│   ├── tempsensor/
│   │   ├── smart_test.cpp            # Tester av smart temperatursensor, ska implementeras.
│   │   └── tmp36_test.cpp            # Befintliga TMP36-tester.
│   ├── timer/
│   │   └── atmega328p_test.cpp       # Timer-tester, ska implementeras.
│   └── watchdog/
│       └── atmega328p_test.cpp       # Befintliga watchdog-tester.
├── logic/
│   └── logic_test.cpp                # Komponenttester, ska implementeras.
├── ml/
│   └── lin_reg/
│       └── fixed_test.cpp            # Regressions-tester, ska implementeras i helklass.
└── scripts/
    └── serial_test.py                # Python-skript för seriell kommunikation, ska implementeras.
```

## Git och versionshantering

### Repository-setup
1. **Forka projektet**: Skapa en privat fork av kursens repository.
2. **Bjud in lärare**: Lägg till din lärare som collaborator på ditt privata repository.
3. **SSH-nycklar**: Konfigurera SSH-nycklar för autentisering mot GitHub enligt instruktionerna 
[här](../README.md#konfigurera-ssh-nycklar-för-github).

### Arbetsflöde

#### I WSL/Linux (för testning)
Använd vanliga Git-kommandon i WSL/Linux när du arbetar med tester:
```bash
git add .
git commit -m "Ditt commit-meddelande"
git push
```

#### I Windows (för hårdvara/Microchip Studio)
Använd Git Bash för versionshantering när du arbetar med hårdvaran och använder Microchip Studio för att bygga och flasha firmware:
* Öppna Git Bash i din projektkatalog (högerklicka > "Open Git Bash Here").
* Använd samma Git-kommandon som ovan.

**OBS!** Använd SSH-URL:er för att klona och pusha till ditt privata repository:

```bash
git clone git@github.com:ditt-användarnamn/ditt-repo.git
```

## Genomförande

### Fas 1: Förberedelser
1. Konfigurera Git och SSH-nycklar enligt ovan.
2. Forka projektet och bjud in din lärare som collaborator.
3. Klona ditt forkade repository.
4. Bekanta dig med projektet och befintlig kod.
5. Installera Google Test (se `library/test/README.md`).
6. Kör befintliga tester för att verifiera setup.
7. Läs testdokumentation och förstå befintliga tester.
8. Studera ATmega328P datablad för relevanta register och funktionalitet.

### Fas 2: Enhetstester
2. Implementera tester enligt kraven ovan.
3. Använd befintliga unittester som referens (ADC, EEPROM, GPIO samt Watchdog).
4. Kör tester kontinuerligt (`make` i `library/test/`).
5. Dokumentera alla testfall och deras syfte.

### Fas 3: Komponenttester
1. Granska befintliga stubklasser i `library/include/driver/*/stub.h`.
2. Granska befintliga komponenttester för logic-implementation.
3. Komplettera med ytterligare testfall om nödvändigt.
4. Verifiera att alla drivrutinsinteraktioner testas.
5. Säkerställ att stub-implementationer används korrekt och simulerar hårdvarabeteende.
6. Testa olika kombinationer av drivrutinsinteraktioner.

### Fas 4: Integrationstestdokumentation
1. Identifiera alla tester som kräver hårdvara.
2. Skapa ett Python-skript för seriell kommunikation med enheten.
3. Implementera funktionalitet för att läsa data från enheten och skriva ut.
4. Implementera funktionalitet för att skicka kommandon till enheten.
5. Skapa en testplan för manuella tester.
6. Dokumentera varje test enligt mallen ovan.
7. Om möjligt, utför tester på verklig hårdvara (Arduino Uno eller liknande) med Python-skriptet.
8. Dokumentera resultat och observationer.
9. Skapa en traceability matrix mellan krav och tester.

### Fas 5: Rapportering
1. Sammanfatta testresultat.
2. Dokumentera identifierade buggar eller problem.
3. Föreslå förbättringar av testbarhet.
4. Reflektera över teststrategier och lärdomar.
5. Skapa en slutrapport enligt inlämningskraven.

## Kompilering och körning

### Bygga och köra alla tester
```bash
cd library/test
make
```

### Endast bygga tester
```bash
make build
```

### Endast köra tester (utan omkompilering)
```bash
make run
```

### Rensa kompilerade filer
```bash
make clean
```

## Betygskriterier

### Godkänd (G)
* Tillägg av enhetstester för angivna komponenter implementerade och fungerande.
* Väl utformade stubklasser som korrekt simulerar hårdvarabeteende.
* Komponenttester för logik-implementationen implementerade och fungerande.
* Dokumentation av fem icke-automatiserbara tester.
* Kod följer projektets stilguide.
* Tester använder Google Test korrekt.
* **Kortfattat:** Följ med under lektionerna och genomför lektionsuppgifterna.

### Väl Godkänd (VG)
* Alla G-krav uppfyllda.
* Minst fem nya (egna) mjukvarutester för edge cases och felhantering.
* Python-skript för seriell kommunikation implementerat enligt specifikationer och fungerande.
* Användning av Python-skriptet för att automatisera flera integrationstest.
* **Kortfattat:** Samma som för G, samt fem egna mjukvarutester och ett Python-skript.

## Inlämning

### Format
Projektet lämnas in som en länk till ditt Git-repository innehållande:
1. All testkod.
2. Dokumentation av manuella integrationstester.
3. Testresultat.
4. En sammanfattande rapport (max 3 sidor) som beskriver:
   * Er testningsstrategi.
   * Resultat och observationer.
   * Identifierade problem och lösningar.
   * Reflektion kring testning av inbyggda system.
5. En README-fil med instruktioner för att köra testerna.
6. **För VG**: Python-skript för seriell kommunikation med dokumentation och användningsexempel.

## Resurser

### Dokumentation
* [Google Test Primer](https://google.github.io/googletest/primer.html).
* [Google Test Advanced Guide](https://google.github.io/googletest/advanced.html).
* [ATmega328P Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf).
* Befintliga stub-implementationer i `library/include/driver/*/stub.h`.

## Reflektion

Efter projektets slutförande, reflektera över:
1. Vilka utmaningar uppstod vid testning av inbyggda system?
2. Hur skiljer sig testning av inbyggda system från konventionell mjukvarutestning (såsom genomfördes för maskininlärningsmodellen)?
3. När är det meningsfullt att automatisera tester vs. utföra manuellt?
4. Vilka designprinciper (såsom användning av interfaces) bidrar till ökad testbarhet i inbyggda system?
5. Vilken roll spelar stub-implementationer för att möjliggöra testning av inbyggda system?

---

Lycka till!