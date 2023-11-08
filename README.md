# Povezivanje I2C i Ethernet interfejsa – projektni zadatak

Tema projektnog zadatka je bila povezivanje I2C i Ethernet interfejsa. Dvije Raspberry Pi platforme međusobno ostvaruju komunikaciju pomoću Ethernet interfejsa, tako da jedan Raspberry Pi posmatramo kao server, a drugi kao klijent. Server je taj koji nudi podatke klijentu ili ukoliko klijent zahtijeva upis podataka, upisuje ih u odgovarajuće registre. Klijent određuje da li želi upisivanje ili čitanje podataka, a server na osnovu I2C komunikacije komunicira sa odgovarajućim modulom, u našem slučaju to je PWM Click modul, a zatim čita tražene ili upisuje zahtijevane vrijednosti. U nastavku slijedi kratak osvrt na implementaciju zadatka.

# Priprema ciljne platforme i kloniranje repozitorijuma

Da bismo realizovali ovaj zadatak potrebne su nam dvije Raspberry Pi platforme, PWM Click modul, te osciloskop koji će nam omogućiti da posmatramo odgovarajuće pravougaone signale te nam time dati potvrdu da smo uspješno riješili zadatak. 
Potrebno je da konfigurišemo Raspberry Pi platforme u smislu instalacije odgovarajućeg operativnog sistema, te formatiranja SD kartice, kao i postavljanja odgovarajućih IP adresa i korisničkog imena. Povezivanje na Raspberry Pi platformu možemo učiniti unošenjem sljedeće komande u terminal, pri čemu username i ip_addr treba da se zamijene stvarnim vrijednostima:
ssh username@ip_addr

Da bismo pokrenuli programe koji su dostupni unutar ovog repozitorijuma potrebno je da kloniramo ovaj repozitorijum, tako što u terminalu unesemo sljedeću komandu:
```
git clone https://github.com/NikolaBabic1213/I2C-Ethernet
```

U nastavku će biti objašnjeno kako su programi realizovani, te kako ih na odgovarajući način kompajlirati i prenijeti na ciljnu platformu.

# Rad sa PWM Click modulom

Ukoliko smo uspješno pristupili Raspberry Pi platformi, potrebno je da na odgovarajući način povežemo pinove PWM Click modula i Raspberry Pi platforme. Raspored pinova Raspberry Pi platforme  je dostupan na sljedećem [linku](https://pinout.xyz/), a dokumentacija vezana za PWM Click modul [ovdje](https://download.mikroe.com/documents/datasheets/PCA9685_datasheet.pdf).

GPIO 2 (I2C1 SDA) pin povezujemo sa SDA pinom PWM Click modula, a GPIO 3 (I2C1 SCL) povezujemo sa SCL pinom PWM Click modula. Potrebno je da povežemo napajanje od 3.3V, te pin Ground. OE (output enable) pin treba da bude na niskom nivou da bi izlaz bio omogućen, no iz dokumentacije možemo uočiti da je taj pin preko pull-down otpornika već spojen na masu, tako da taj pin možemo držati odspojenim.
Na slici ispod prikazan je način povezivanja.

<img src="https://github.com/NikolaBabic1213/I2C-Ethernet/assets/128309520/b80423d7-bf2f-4486-a1a8-ffe3e2e67961" width="1000" height="780">



<img src="https://github.com/NikolaBabic1213/I2C-Ethernet/assets/128309520/a669770f-4999-473d-bf37-e28bae684a1b" width="780" height="1000">




Da bismo realizovali zadatak potrebno je da proučimo dokumentaciju PWM Click modula. PWM Click modul je razvojna pločica na kojoj se nalazi PCA9685, 16-kanalni kontroler. Potrebno je da na odgovarajući način podesimo I2C adresu uređaja.  Adresa u našem slučaju je sedmobitna, viša 4 bita postavljena su na vrijednost 1000, a vrijednosti nižih bita A2, A1 i A0 su određene položajem odgovarajućih jumper-a, što možemo vidjeti na samoj pločici. U našem slučaju vrijednosti su postavljene na 000. Tako da konačno formirana I2C adresa je 01000000, odnosno u heksadecimalnoj notaciji 0x40. U implementaciji I2C komunikacije koristili smo kombinovanu transakciju. 
Na sljedećoj slici prikazani su registri i njihove adrese. Od interesa su nam MODE1 registar sa adresom 0x00, zatim LED0_ON_L, LED0_ON_H,  LED0_OFF_L, LED0_OFF_H registri koji se odnose na kanal 0, a čije su adrese redom 0x06, 0x07, 0x08, 0x09.

![image](https://github.com/NikolaBabic1213/I2C-Ethernet/assets/128309520/e3d0363b-4eb3-4066-8610-8ac95219543c) 

U registar MODE1 upisujemo podatak 0x20, razlog toga možemo objasniti, ako se pogleda sljedeća slika. 

![image](https://github.com/NikolaBabic1213/I2C-Ethernet/assets/128309520/4ff5d2e3-8e60-446d-80a5-7d4a3bda4ffb)


 
Omogućavamo AUTO-INCREMENT bit, dok ostale postavljamo na nulu. RESTART je onemogućen, EXTCLK je postavljen na nulu, jer se koristi interni clock signal, SLEEP koristimo navedeni normal mode. Kada zapišemo te vrijednosti dobijamo podatak 00100000, odnosno u heksadecimalnoj notaciji 0x20.
Vrijednosti koje se upisuju u gore navedena četiri registra zavise od željenog faktora popune pravougaonog signala. U primjeru koji je naveden u priloženoj dokumentaciji korišten je faktor popune od 20%, a vrijednosti koje se upisuju u registre su: 0x99, 0x01, 0xCC, 0x04 i to respektivno kako su navedeni i registri. 
Za faktor popune od 50% bilo bi potrebno navesti sljedeće heskadecimalne vrijednosti 0x00, 0x00, 0x00, te 0x08 i to navedenim redom.
I2C komunikacija se sastoji od dvije poruke, u prvoj poruci u MODE1 registar upisujemo podatak 0x20, a u drugoj poruci u četiri navedena registra upisujemo odgovarajuće vrijednosti koje zavise od faktora popune kojeg želimo postaviti.

# Testiranje PWM Click modula

Ako pretpostavimo da smo uspješno klonirali  udaljeni repozitorijum, možemo da izlistamo sve fajlove pomoću komande ls. Vidimo da imamo tri fajla koja su napisana u C jeziku. Trenutno nam je od interesa fajl koji se zove i2c.c. Ukoliko smo ispravno povezali Raspberry Pi platformu i PWM Click modul, možemo da kroskompajliramo program i prenesemo na ciljnu platformu tako što ćemo pokrenuti sljedeće komande:
```
arm-linux-gnueabihf-gcc i2c.c -o i2c
scp i2c username@ip_addr:~
```
Nakon druge komande biće potrebno unijeti šifru koju smo postavili na Raspberry Pi platformu. Na taj način uspješno smo prenijeli program na ciljnu platformu gdje ga možemo pokrenuti unosom sljedeće komande:
```
./i2c
```
Prije toga potrebno je da sondu osciloskopa postavimo na kanal 0 PWM Click modula i povežemo ground. Ukoliko smo sve ispravno odradili dobijamo signal prikazan na sljedećoj slici:


<img src="https://github.com/NikolaBabic1213/I2C-Ethernet/assets/128309520/484d5d25-540b-4e52-8f3d-9c18ae2ad53b" width="1000" height="780">


Ovim smo potvrdili ispravnost I2C komunikacije. 

# Ethernet komunikacija

Za Ethernet komunikaciju su potrebne dvije RapberryPi platforme, jedna u funkciji servera, a druga u funkciji klijenta. Server šalje read/write poruku klijentu, a klijent odgovara u zavisnosti od poruke. Ako je u pitanju read poruka, on odgovara podatkom koji je smješten u registar čija je adresa smještena u okviru read poruke. Ako je u pitanju write poruka onda se upisuju podaci u registre koji su navedeni u okviru te poruke. Protokol koji se koristi na transportnom sloju je UDP. Format UDP datagrama je sljedeći: prvi bajt predstavlja fleg bajt čija vrijednost određuje da li je u pitanju read ili write poruka (0 - write, ostalo - write), drugi bajt je adresa slave uređaja (u našem slučaju adresa pwm click-a), treći bajt je adresa registra u koji se piše/iz kojeg se čita, i ostali bajtovi, ako postoje, su podaci koji se upisuju. Ako šalje write poruka tada se unosi broj poruka i poruke koje se šalju, odnosno podaci koji se upisuju, a ako je u pitanju read poruka, onda se ovi podaci ne unose.

# Pokretanje programa

Da bismo pokrenuli programe moramo prvo kompajlirati kod pa prenijeti na ciljnu platformu, u našem slučaju to je RaspberryPi. Kompajliranje se vrši pomoću sljedeće komande:
```
arm-linux-gnueabihf-gcc <source_code.c> - o <name_of_executable_file>
```
Gdje umjesto <source_code.c> stavljamo puni naziv izvornog koda fajla, a umjesto <name_of_executable_file> stavljamo željeno ime programa koji ćemo pokretati.
Imaćemo dva programa, sender, koji šalje read/write poruke, i receiver, koji ih prima i šalje povratne informacije. Pokrećemo ih na 2 RaspberryPi-a tako što ćemo ukucati:
```
./sender
```
na Raspberry-u koji šalje zahtjeve, i
```
./receiver
```
na Raspberry-u koji prima podatke.
Prilikom pokretanja programa potrebno je prvo podesiti mode1 registar (adresa 0x00) tako što ćemo u njega upisati podatak 0x20. U sljedećoj poruci ćemo podešavati parametre vezane za faktor popune tako što ćemo postaviti adresu registra na adresu registra LED0_ON_L (0x06) i poslati par podataka. Potrebno je podesiti vrijednosti 4 registra, no pošto je auto-increment uključen, u sklopu mode1 registra, to znači da ćemo jednom porukom moći podesiti vrijednosti registara koji slijede registru LED0_ON_L. Svaki naredni podatak u i2c transakciji će inkrementovati vrijednost adrese registra. 
Da bismo dobili faktor popune od 20% šaljemo sljedeće podatke: 0x01, 0x99, 0x04, 0xCC.
Za faktor popune od 50% podaci su sljedeći: 0x00, 0x00, 0x00, 0x08.
Na sljedećoj slici prikazan je izlazni signal PWM click-a za faktor popune od 20%: 

<img src="https://github.com/NikolaBabic1213/I2C-Ethernet/assets/128309520/a367beed-ef38-4154-b24d-edadc2904cac" width="1000" height="780">


