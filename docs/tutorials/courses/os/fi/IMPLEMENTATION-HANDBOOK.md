# VNTOS: toteuta käyttöjärjestelmä alusta loppuun

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

Tämä on käytännön toteutusopas, ei pelkkä käsitteiden sanasto. Tavoite on, että osaat tehdä muutoksen itse: löydät oikean tiedoston, ymmärrät rajapinnan, kirjoitat toteutuksen, käännät, käynnistät virtuaalikoneessa ja todistat tuloksen. Opas kuvaa nykyistä `os/`-hakemistoa. Se ei väitä, että keskeneräinen ominaisuus olisi valmis.

## 0. Työskentelytapa: tee aina yksi todistettava muutos

Käyttöjärjestelmässä virhe voi rikkoa seuraavan vaiheen kauan ennen kuin virhe näkyy. Siksi jokainen harjoitus tehdään pieninä, tarkistettavina muutoksina.

1. Avaa ensin `os/README.md`, `os/build.sh` ja kyseisen osajärjestelmän lähdetiedosto.
2. Etsi kutsuja ja kutsuttavia funktioita. Kirjaa, mitä tietoa funktio saa, mitä se muuttaa ja mitä se palauttaa.
3. Etsi kaikki nykyiset käyttäjät ennen kuin muutat funktion allekirjoitusta tai tilan tallennuspaikkaa.
4. Kirjoita odotettu tulos ennen käännöstä. Esimerkiksi: “Kun käynnistän QEMU:n, sarjaportissa näkyy täsmälleen uusi tunniste.”
5. Tee pienin muutos, joka voi toteuttaa odotuksen.
6. Käännä komennolla `bash os/build.sh` projektin juuressa. Korjaa ensimmäinen virhe ennen kuin tulkitset myöhempiä virheitä.
7. Käynnistä vain eristetyssä QEMU-virtuaalikoneessa. Älä kirjoita keskeneräistä levykuvaa oikealle levylle.
8. Tallenna sarjaportin tuloste ja tarkista, että havainto vastaa ennustetta.
9. Testaa myös virheellinen syöte ja rajatapaus. Onnistunut normaali tapaus ei todista rajojen turvallisuutta.
10. Päivitä dokumentaatio vastaamaan sitä, mikä lähdekoodissa oikeasti toimii.

**Älä kopioi alla olevia C- tai assembly-tyylisiä esimerkkejä suoraan VNT:hen tarkistamatta syntaksia.** VNTOS käyttää VNT:tä politiikkaan ja C:n/GCC:n assembleria käynnistys- ja CPU-rajapintaan. Käytä olemassa olevien `.vnt`-tiedostojen syntaksia mallina.

## 1. Lähdekoodin kartta ja rakentamisen riippuvuudet

| Tiedosto | Vastuu |
|---|---|
| `os/kernel.vnt` | alustusjärjestys ja kernelin pääsilmukka |
| `os/kernel/core.vnt` | jaettu varhainen tila ja matalan tason apurajapinnat |
| `os/serial.vnt` | COM1-sarjaportti |
| `os/console.vnt` | VGA-teksti ja yhdistetty konsoli |
| `os/kernel/shell.vnt` | komentojen jäsennys ja komentotulkin politiikka |
| `os/ps2.vnt` | PS/2-laitteiden polling |
| `os/interrupts.vnt` | VNT-puolen keskeytyspolitiikka |
| `os/arch/x86_64/interrupts.S` | IDT, sisääntulokehys, PIC/PIT ja `iretq` |
| `os/arch/x86_64/boot.S` | Multiboot2, sivutaulut ja long mode |
| `os/linker.ld` | ELF:n muistiasettelu ja symbolit |
| `os/mm/phys.vnt` | Multiboot2-muistikartan lukeminen ja fyysiset sivukehykset |
| `os/mm/virt.vnt` | nykyisten sivutaulujen tutkiminen ja varhaisen kartoituksen politiikka |
| `os/firmware/acpi.vnt` | ACPI-taulujen etsintä ja rajojen tarkistaminen |
| `os/arch/x86_64/apic.vnt` | Local APIC ja rajattu IOAPIC-alustus |
| `os/kernel/smp.vnt` | lisäprosessorien käynnistys ja rajattu IPI-käsittely |
| `os/build.sh` | kääntäjän rakentaminen, VNT:n käännös, linkitys ja ISO |

Build-skripti on riippuvuuksien totuus. Se kääntää VNT-kääntäjän, kääntää `os/kernel.vnt`-tiedoston freestanding-assemblyksi, kokoaa arkkitehtuuritiedostot ja linkittää `build/kernel.elf`-tiedoston. Se ei automaattisesti tarkoita, että jokainen laite toimii oikealla laitteistolla.

### Tee muutos oikein

- Jos lisäät uuden VNT-moduulin, lisää se `import`-rivillä siihen moduuliin, joka omistaa sen käytön.
- Jos moduuli tarvitsee uuden assembly-symbolin, varmista symbolin täsmällinen nimi generoituun assemblyyn ja linkitettyyn ELF:ään.
- Jos lisäät uuden `.S`-tiedoston, lisää sille kokoamisrivi `os/build.sh`-tiedostoon ja objektitiedosto linkitykseen.
- Jos tarvitset kernelin alku- tai loppuosoitetta, käytä linker-symbolia sen sijaan, että kovakoodaisit arvon.
- Älä lisää samaa vastuuta sekä VNT:hen että assemblyyn. Päätä, kumpi kerros omistaa päätöksenteon ja kumpi vain toteuttaa CPU:n vaatiman rajapinnan.

## 2. Miten VNT-moduuli toteutetaan

Nykyinen `os/kernel.vnt` tuo moduuleja esimerkiksi riveillä `import "serial.vnt"` ja `import "mm/phys.vnt"`. Aloita olemassa olevan moduulin kopioimisesta rakenteelliseksi malliksi, älä keksi uutta kieltä tai rakennusjärjestelmää.

1. Luo tiedosto oikeaan alihakemistoon. Laita alkuun projektin tekijänoikeus- ja lisenssihuomautus.
2. Kirjoita kommentti, joka kertoo moduulin vastuun ja rajat. Älä kuvaa siellä koko käyttöjärjestelmää.
3. Tuo vain ne moduulit, joiden funktioita oikeasti käytät.
4. Tee jokaisesta julkisesta funktiosta selvä sopimus: hyväksytyt syötteet, paluuarvot, muutettu tila ja virhetilanne.
5. Tarkista syötteet ennen osoitteen laskemista tai laitteistoon kirjoittamista.
6. Rajaa `unsafe`-lohko minimiin. Tee tarkistukset turvallisessa koodissa ennen lohkoa.
7. Käytä `state_get`/`state_set`-tilaa vain, kun nykyinen tila-API on tarkoituksenmukainen. Dokumentoi käytetyt indeksit ja niiden omistajat.
8. Lisää import kernelin käynnistykseen vasta, kun moduulin riippuvuudet ovat selvät.
9. Lisää käynnistysviesti vain diagnostiseen polkuun; älä tulosta jokaista merkkiä keskeytyskäsittelijästä.
10. Käännä ja testaa ennen kuin lisäät seuraavan ominaisuuden.

**Valmis moduuli** ei ole vain tiedosto, joka kääntyy. Sen vastuu on yksiselitteinen, rajat on tarkistettu, virhetila on määritelty ja sen toiminnasta on havainto tai testi.

## 3. Käynnistyspolku: GRUBista VNT-koodiin

Toteutus kulkee ketjussa: GRUB löytää Multiboot2-otsakkeen → lataa ELF:n → siirtää ohjaimen `boot32_entry`-symbolille → bootstrap säilyttää käynnistystiedot → rakentaa alkuvaiheen sivutaulut → siirtyy 64-bittiseen long modeen → kutsuu käännettyä kernelin aloitusta.

### Toteuta ja tarkista muutos

1. Avaa `os/arch/x86_64/boot.S` ja etsi `.multiboot`, `multiboot2_header` sekä `boot32_entry`.
2. Tarkista otsakkeen magic, pituus ja tarkistussumma. Muuta niitä vain, jos osaat laskea uuden kelvollisen otsakkeen.
3. Säilytä Multiboot2:n antamat rekisteritiedot ennen kuin käytät `EAX)- tai `EBX)-rekisteriä muuhun.
4. Varmista, että väliaikainen pino on käytössä ennen C/VNT-kutsua. Kernel ei voi luottaa GRUBin jättämään pinoon.
5. Tarkista, että sivutaulut ovat osoitteessa, johon bootstrap voi kirjoittaa, ja että CR3 asetetaan ennen pagingin käyttöä.
6. Tarkista, että long modeen siirtymisen edellytykset on asetettu oikeassa järjestyksessä: PAE, EFER.LME, CR3, CR0.PG ja kelvollinen 64-bittinen koodisegmentti.
7. Varmista, että siirtymän jälkeen käytetään 64-bittisiä käskyjä ja kelvollista pinoa.
8. Lisää diagnostinen portti-`0xE9`-merkki ennen riskialtista vaihetta ja toinen sen jälkeen. Näin tiedät, missä käynnistys pysähtyy.
9. Tarkista linkkeriskriptistä, että käynnistyskoodi ja Multiboot2-otsake pysyvät sopivassa ELF:n alkuosassa.
10. Käynnistä QEMU:lla GRUBin kautta; älä testaa vain suoraan ELF:n suorittamista, jos käynnistyssopimus edellyttää GRUBia.

### Tavalliset virheet

- Otsake ei löydy, koska se on väärässä ELF:n kohdassa tai väärin kohdistettu.
- Käynnistystiedot ylikirjoitetaan ennen kuin niiden osoite on tallennettu.
- Sivutaulujen osoitteet tai flag-bitit ovat väärin.
- Pino ei ole oikein kohdistettu ennen ABI-kutsua.
- Debug-tuloste loppuu tiettyyn merkkiin; tämä rajaa vian viimeisen onnistuneen vaiheen ja seuraavan vaiheen väliin.

## 4. ELF ja linkkeri: miten symbolit päätyvät kernel-kuvaan

`os/linker.ld` määrittää ELF-formaatin, arkkitehtuurin, entry-symbolin, lataussegmentit ja osioiden sijainnit. Symboli, jota VNT:n unsafe-rajapinta hakee nimellä, on käyttökelpoinen vasta kun symboli on olemassa linkitetyssä kuvassa.

1. Etsi `ENTRY(boot32_entry)` ja varmista, että vastaava symboli on määritelty assemblyssä.
2. Tarkista `.multiboot`, `.boot32`, `.text`, `.rodata`, `.data` ja `.bss` -osioiden järjestys.
3. `KEEP` estää tärkeän otsakkeen poistamisen linkityksessä. Älä poista sitä ymmärtämättä vaikutusta.
4. `NOLOAD`-BSS sisältää nollattavan varatun tilan, ei levyltä luettavia alustettuja tavuja.
5. Jos lisäät globaaleja taulukoita, tarkista, mihin osioon ne menevät ja kuinka paljon tilaa ne vaativat.
6. Jos lisäät symbolin, etsi sen käyttö ja tarkista generoitu assembly ennen kuin oletat nimen.
7. Tarkista ELF-otsakkeet ja symbolit työkaluilla `readelf -h -l -s build/kernel.elf`, jos ne ovat asennettuina.
8. Tarkista Multiboot2-tuki komennolla `grub-file --is-x86-multiboot2 build/kernel.elf`, kun työkalu on saatavilla.
9. Muuta yksi linkkerin sääntö kerrallaan. Väärä latausosoite voi rikkoa kaikki myöhemmät osoitteet.
10. Kirjaa odotetut osoitteet ja segmentit, älä päättele niitä pelkästä käännöksen onnistumisesta.

## 5. Freestanding VNT: ei käyttöjärjestelmän palveluja

Freestanding-kernel ei voi olettaa, että käytössä ovat tavallinen prosessi, tiedostot, ympäristömuuttujat tai käyttöjärjestelmän tarjoama muisti. VNT-kääntäjä emittoi kernelille assemblyä komennolla, jota `os/build.sh` käyttää: `build/vnt --emit-freestanding-asm os/kernel.vnt -o build/kernel.s`.

1. Aloita pienestä funktiosta, joka ei käytä laitteistoa. Tarkista VNT:n kääntäjän nykyinen tuki syntaksille.
2. Lisää funktio oikeaan moduuliin ja tuo moduuli tarvittaessa.
3. Käännä assemblyksi ja tarkista generoitu funktiosymboli.
4. Varmista, ettei koodi kutsu libc-funktioita tai muuta ympäristöä, jota kernel ei linkitä.
5. Vältä dynaamista allokointia, kunnes allokaattorin sopimus ja virhepolku ovat olemassa.
6. Älä oleta, että `string` on turvallinen rajaton merkkijono. Tiedä, mistä osoite tulee ja mikä takaa lopetusnollan.
7. Varmista, että kokonaislukujen leveys ja etumerkki sopivat rekisteri- ja muistirajapintaan.
8. Jos tarvitset osoitteen tai rekisterin, käytä olemassa olevaa rajattua apurajapintaa ja tarkista sen semantiikka.
9. Testaa ensin tavallinen arvo, sitten nolla, negatiivinen arvo ja suurin sallittu arvo.
10. Jos kääntäjä tuottaa väärän assemblyn, vika voi olla kääntäjässä eikä kernelin logiikassa. Pienennä esimerkki ja vertaa odotettuun koodiin.

## 6. Sarjaportti ja konsoli

### COM1: `os/serial.vnt`

COM1:n perus-I/O käyttää portteja kuten `0x3F8` (data), `0x3F9` (interrupt enable) ja `0x3FD` (line status). Nykyinen toteutus on polling-pohjainen. `serial_putc` odottaa lähetysrekisterin olevan valmis ennen tavun kirjoittamista.

Toteuta uusi sarjaporttitoiminto näin:

1. Määritä ensin, onko toiminto tulostusta, vastaanottoa vai alustus.
2. Etsi oikea UART-rekisteri ja tarkista rekisterin käyttöehdot laitteistodokumentaatiosta.
3. Rajaa portti-I/O assemblyyn; pidä kutsuvan VNT-funktion sopimus luettavana.
4. Älä jää odottamaan ikuisesti, jos laite ei vastaa. Polling-loopille tarvitaan selkeä rajaus tai tiedostettu rajoite.
5. Vastaanotossa tarkista data-ready-bitti ennen datarekisterin lukua.
6. Palauta erillinen arvo “ei merkkiä saatavilla” -tilanteessa. Älä sekoita sitä tavalliseen merkkiarvoon.
7. Testaa ensin merkkijono `serial_puts`-funktiolla, sitten yksi merkki, rivinvaihto ja vastaanotto.
8. Varmista, että debuggaus toimii myös silloin, kun VGA-tila on rikki.

### VGA ja yhdistetty konsoli: `os/console.vnt`

VGA-tekstimuisti alkaa perinteisessä kartoituksessa osoitteesta `0xB8000`. Tekstiruutu on 80×25 solua; jokaisessa solussa on merkki ja attribuuttitavu. Näyttöohjaimen kursori on erillinen tila.

1. Selvitä, missä kursori ja väri säilytetään. Nykyinen toteutus käyttää kernelin jaettua tilaa.
2. Toteuta merkkien erikoistapaukset erikseen: tavallinen merkki, rivinvaihto, carriage return ja backspace.
3. Ennen muistin kirjoitusta tarkista, että kursori pysyy välillä 0–1999.
4. Kun näyttö täyttyy, siirrä rivit ja tyhjennä viimeinen rivi. Tarkista lähde- ja kohdeosoitteet sekä kopioitavien tavujen määrä.
5. Päivitä kursori sekä muistissa että VGA-ohjaimen rekistereissä.
6. Yhdistetyn konsolin pitää välittää teksti sekä näytölle että sarjaporttiin, mutta älä tee tästä rekursiivista kutsua.
7. Testaa pitkä teksti, monta rivinvaihtoa, backspace tyhjän rivin alussa ja ruudun vieritys.
8. Testaa sarjaportti yksin, jotta konsolin ongelma ei peitä bootin todellista tilaa.

## 7. Komentotulkki: tekstistä rajattuun toimintaan

Komentotulkin politiikka sijaitsee `os/kernel/shell.vnt`-tiedostossa. Konsoli ja sarjaportti ovat I/O-kerroksia; shellin pitäisi päättää, mitä komento tarkoittaa, ei hallita näyttömuistin rekistereitä.

1. Etsi syötteen keräävä silmukka ja tarkista, kuinka monta merkkiä puskuriin mahtuu.
2. Määritä, mitä tapahtuu, kun puskuri täyttyy. Älä kirjoita sen lopun yli.
3. Käsittele backspace, Enter ja tavalliset merkit erillisinä tapauksina.
4. Lisää uusi komento komentojen tunnistuskohtaan; älä tee komentoa, joka suorittaa mielivaltaisen muistiosoitteen.
5. Tarkista kaikki argumentit ennen kuin kutsut laite- tai muistifunktiota.
6. Pidä komennon tuloste erillään sen toiminnosta, jotta toiminto on testattavissa.
7. Virheellisen komennon pitää antaa ymmärrettävä virhe ja palata komentokehotteeseen, ei muuttaa kernelin tilaa sattumanvaraisesti.
8. Testaa tyhjä komento, tuntematon komento, liian pitkä syöte, puuttuva argumentti ja kelvollinen komento.
9. Lisää `help`-teksti vasta, kun komento todella on olemassa.
10. Jos komento voi kirjoittaa laitteistoon tai muistiin, dokumentoi vaikutus ja turvallinen testi.

## 8. Keskeytykset ja IDT: CPU:n sopimus

VNTOS jakaa keskeytyspolun kahteen kerrokseen. `os/arch/x86_64/interrupts.S` rakentaa CPU:n vaatiman sisääntulo-/paluukehyksen. `os/interrupts.vnt` päättää, mitä vektori tarkoittaa.

### Toteutusjärjestys

1. Etsi `ISR_NOERR` ja `ISR_ERROR` makrot. CPU ei työnnä virhekoodia jokaisessa poikkeuksessa; nämä makrot normalisoivat pinon.
2. Tarkista, mitkä vektorit saavat CPU:n työntämän virhekoodin. Väärä luokittelu siirtää kaikkia pinon kenttiä.
3. Etsi IDT-porttien rakentaminen ja varmista, että jokainen käytetty vektori osoittaa kelvolliseen sisääntuloon.
4. Yhteisen sisääntulon pitää tallentaa kaikki VNT-kutsun aikana tarvittavat yleisrekisterit.
5. Ennen VNT-funktion kutsua säilytä ABI:n vaatima pinon kohdistus. Tarkista myös x86-64:n red zone -sääntö; build käyttää `-mno-red-zone`.
6. VNT-dispatcher saa vektorin ja virhekoodin. Pidä sen allekirjoitus synkronoituna assembly-kutsun kanssa.
7. Paluupolun on palautettava rekisterit samassa järjestyksessä ja käytettävä `iretq`, ei tavallista `ret`-käskyä.
8. Poikkeusdiagnostiikka saa lukea virhekoodin ja tarvittaessa CR2:n, mutta virheellinen muistiosoite ei saa johtaa rajattomaan lisäkirjoitukseen.
9. Laite-IRQ:n kuittaus (EOI) on tehtävä oikeassa kerroksessa täsmälleen kerran.
10. Testaa ensin hallittu vektori QEMU:ssa. Älä tarkoituksella laukaise vaarallista poikkeusta fyysisessä koneessa.

### Miksi keskeytyskäsittelijä pitää pitää lyhyenä?

Keskeytys voi tulla kesken muun koodin. Käsittelijä ei saa olettaa, että muu työ on keskeytyskohdassa valmiissa tilassa. Älä tulosta pitkiä viestejä, varaa muistia tai odota laitetta keskeytyskontekstissa. Kirjaa minimaalinen tapahtuma ja siirrä raskaampi työ myöhempään vaiheeseen.

## 9. PIC ja PIT: ajastimen tikki

Nykyinen x86-64-keskeytysassembly remappaa perinteisen PIC:n vektoreihin 32–47 ja alustaa PIT-ajastimen. `os/pic.vnt` hallitsee IRQ-maskien politiikkaa. Älä sekoita maskin muuttamista keskeytyksen kuittaukseen.

1. PIC:n master ja slave käyttävät erillisiä maskiportteja. IRQ 0–7 kuuluu masterille, 8–15 slavelle.
2. Slave-IRQ:n avaaminen vaatii myös masterin kaskadi-IRQ:n (IRQ2) avaamisen.
3. Tarkista IRQ-numero ennen bittisiirtoa. Negatiivinen tai yli 15 oleva numero ei saa kirjoittaa porttiin.
4. PIC-remappauksen jälkeen varmista, että IDT:ssä on käsittelijät uusille vektoreille.
5. PIT:n jakaja määrää tikin taajuuden. Laske se dokumentoidulla lähtötaajuudella; älä arvaa lukua.
6. Lisää ajastintikkiä kasvattava toiminto VNT-politiikkaan, mutta älä tee tulostusta IRQ0:n sisällä.
7. Tarkista, että EOI lähetetään myös slave-PIC:n tapauksessa oikeassa järjestyksessä.
8. Testaa maski ensin suljettuna, sitten avaa vain tarvittava IRQ virtuaalikoneessa.
9. Tikin kasvaminen osoittaa IRQ:n kulkeneen, mutta ei vielä todista prosessien ajastusta. VNTOS ei tällä perusteella ole valmis preemptiivinen scheduler.
10. Jos tikit pysähtyvät, tarkista IDT, IF-lippu, PIC-maskit, PIT:n asetukset ja EOI ennen kuin muutat VNT:n laskuria.

## 10. Fyysinen muisti: Multiboot2 ja sivukehykset

`os/mm/phys.vnt` lukee Multiboot2:n tietorakennetta ja muistikarttaa. Tietorakenteen tagit ovat pituudeltaan muuttuvia, joten jokaista askelta pitää rajata.

1. Tarkista boot-magic ennen kuin luotat käynnistystietojen osoitteeseen.
2. Tarkista, ettei osoite ole nolla eikä nykyisen kartoituksen ulkopuolella.
3. Lue kokonaiskoon kenttä ja varmista, että se on vähintään otsakkeen kokoinen.
4. Kulje tagista toiseen tagin oman koon mukaan. Älä oleta, että kaikki tagit ovat samanpituisia.
5. Hylkää tagi, jonka koko on liian pieni tai jonka loppu ylittää koko tietorakenteen.
6. Etsi muistokartta ja tarkista sen entry-koon minimipituus ennen kenttien lukemista.
7. Käytä vain käytettäväksi ilmoitettuja alueita, mutta varaa kernelin, Multiboot2-tietojen, bootstrap-taulujen ja bitmapin käyttämä muisti.
8. Sivukehyksen koko on 4096 tavua. Osoite pitää kohdistaa sivurajaan ennen indeksin laskemista.
9. Bitmapissa yksi bitti kuvaa yhtä kehystä. Dokumentoi, tarkoittaako bitti varattua vai vapaata kehystä.
10. Varaus etsii vapaan bitin, merkitsee sen varatuksi ja palauttaa kehysosoitteen. Vapautus tarkistaa kohdistuksen ja alueen rajat.
11. Älä palauta osoitetta, joka on jo varattu tai jota kernel ei saa käyttää.
12. Testaa tyhjä kartta, vain varatut alueet, yksi vapaa kehys, loppunut muisti, virheellinen koko ja toistuva vapautus.
13. Nykyinen toteutus rajautuu ensimmäiseen 1 GiB:n identiteettikartoitukseen. Älä väitä sen hallitsevan mielivaltaista fyysistä muistia.

## 11. Virtuaalimuisti: osoite ei ole fyysinen kehys

`os/mm/virt.vnt` tutkii nykyistä kartoitusta ja tekee varhaisen vaiheen testejä. Virtuaaliosoite on osoite, jonka CPU:n käsky käyttää; sivutaulut muuntavat sen fyysiseksi osoitteeksi.

1. Aloita x86-64:n nelitasoisen sivutaulun indeksien laskennasta. 4 KiB:n sivussa alimmat 12 bittiä ovat sivun sisäinen offset.
2. PML4-, PDPT-, PD- ja PT-indeksit tulevat osoitteen eri bittialueista. Käytä arkkitehtuurin määrittämiä maskeja ja siirtoja.
3. Tarkista jokaisen tason present-bitti ennen seuraavan osoitteen dereferointia.
4. Muista, että taulumerkinnän osoitekentän lisäksi on flag-bittejä. Maskaa ne pois ennen fyysisen osoitteen käyttämistä.
5. Nykyinen bootstrap käyttää suuria 2 MiB:n sivuja ensimmäisen 1 GiB:n identiteettikartoitukseen; älä tulkitse PD-merkintää PT-osoitteeksi.
6. Kartoitusmuutos vaatii vapaan sivutaulun, oikein asetetut oikeudet ja TLB:n huomioimisen. Pelkkä muistissa olevan bitin muutos ei välttämättä muuta jo välimuistissa olevaa käännöstä.
7. Älä kirjoita sivutauluun ennen kuin tiedät, onko kehys käytettävissä ja kuka omistaa sen.
8. Testaa käännös tunnetulla identiteettiosoitteella ja virheellinen/unmapped-osoite erikseen.
9. Tee kartoituksen testistä palautuva: tallenna alkuperäinen merkintä, tee muutos, tarkista tulos ja palauta alkuperäinen tila.
10. Nykyinen rajattu identiteettikartoitus ei ole täysi käyttäjätilojen virtuaalimuistinhallinta. Käyttäjätilat, copy-on-write ja sivutuslevy eivät seuraa tästä automaattisesti.

## 12. ACPI: firmware-tiedot on tarkistettava ennen käyttöä

`os/firmware/acpi.vnt` etsii Multiboot2:n välittämiä RSDP-tietoja, tarkistaa tunnisteita, pituuksia ja tarkistussummia sekä tutkii MADT-taulua. ACPI on ulkoista firmware-dataa: sitä ei saa käsitellä luotettuna osoitinlistana.

1. Etsi RSDP:n tagi Multiboot2-tietorakenteesta; älä skannaa mielivaltaista muistia ilman rajaa.
2. Tarkista RSDP:n tunniste ja vanhan rakenteen tarkistussumma.
3. Jos revision perusteella käytetään laajennettua rakennetta, tarkista pituus ennen lisäkenttien lukemista.
4. Lue RSDT/XSDT-osoite ja varmista, että se mahtuu käytössä olevaan osoiteikkunaan.
5. Tarkista jokaisen taulun allekirjoitus, pituus ja checksum ennen sen sisällön käsittelyä.
6. MADT sisältää eripituisia merkintöjä. Varmista, että jokainen merkintä on vähintään otsakkeensa kokoinen ja että seuraava offset pysyy taulun sisällä.
7. Tallenna vain tarvittavat tiedot: CPU APIC ID:t, Local APIC -osoite, IOAPIC-tiedot ja interrupt source override -tiedot.
8. Rajaa taulukkojen määrä ja tallennettavien CPU:iden määrä. Rajaus on parempi kuin puskurin ylikirjoitus.
9. Jos osoite tai tyyppi ei ole tuettu, ilmoita virhe ja käytä turvallista fallbackia.
10. Testaa puuttuva RSDP, väärä checksum, lyhyt taulu, tuntematon merkintätyyppi ja useita CPU-merkintöjä.
11. ACPI:n CPU-luettelo kertoo firmwaresta löydetyt prosessorit; se ei yksin todista, että ne ovat käynnissä tai että scheduler käyttää niitä.

## 13. APIC ja MMIO

`os/arch/x86_64/apic.vnt` käsittelee Local APICin ja IOAPICin rekistereitä. MMIO tarkoittaa, että tavallisen muistiluvun/-kirjoituksen osoite viittaa laitteistorekisteriin.

1. Varmista, että kyseinen fyysinen rekisterialue on kartoitettu ennen kuin luet sitä.
2. Tarkista laitteiston ilmoittama osoite ja hyväksy vain tuettu kartoitus. Nykyinen koodi hylkää poikkeavat IOAPIC-osoitteet sen sijaan, että dereferoisi niitä.
3. IOAPIC käyttää valinta- ja datarekisteriä. Rekisteri-indeksin kirjoitus ja datan luku/kirjoitus muodostavat yhden loogisen operaation.
4. Lue IOAPICin version rekisteri ja tarkista redirection-entryjen määrä ennen silmukkaa.
5. Pidä reitit maskattuina, kunnes IDT, kohde-CPU ja keskeytyksen kuittaus ovat valmiita.
6. Local APICin LVT-rekisterien maskaus ja spurious-vector-asetus ovat eri tehtäviä; tarkista jokaisen rekisterin merkitys.
7. APICin EOI ja PICin EOI eivät ole keskenään sama operaatio.
8. Älä ota käyttöön reititystä, jonka kohdevektoria ei ole asennettu IDT:hen.
9. Testaa ensin rekisterien luku ja maskattu tila. Keskeytyksen saapuminen on vasta seuraava testi.
10. QEMU:n emuloitu laitteisto ei todista, että kaikki fyysiset emolevyt käyttävät samoja osoitteita tai firmware-asetuksia.

## 14. SMP: käynnistä lisäprosessori hallitusti

SMP tarkoittaa usean prosessoriytimen käyttöä. Nykyinen `os/kernel/smp.vnt` valmistelee AP-trampoliinin ja käynnistää AP:ita sarjassa. Tämä ei vielä tarkoita, että kernelissä olisi valmis prosessiaikataulutus tai yleinen lukitusjärjestelmä.

1. Selvitä ensin BSP (käynnistyksen aloittava CPU) ja AP (käynnistettävä lisä-CPU).
2. Hae CPU:n APIC ID ACPI:n MADT-tiedoista. Älä oleta, että APIC ID on sama kuin taulukon indeksi.
3. Kopioi trampoliini osoitteeseen, johon AP:n käynnistystila pystyy siirtymään. Nykyinen koodi käyttää osoitetta `0x8000`; tarkista, ettei alue ole muun käytössä.
4. Varmista trampoliinin koko ja symbolien offsetit ennen kopiointia.
5. Päivitä pino- ja entry-slotit ennen kuin lähetät käynnistysviestin.
6. APICin INIT/SIPI-protokolla on arkkitehtuurisidonnainen. Noudata käynnistysjärjestystä ja viive-/tilatarkistuksia; älä lähetä viestejä sokkona.
7. AP:n ensimmäinen koodi tarvitsee oman pinon, kelvollisen CPU-tilan ja selkeän siirtymän VNT-entryyn.
8. AP julkaisee “online”-tilan vasta, kun se on oikeasti päässyt sovittuun valmiuspisteeseen.
9. Jos AP ei vastaa, aikakatkaise ja kirjaa APIC ID. Älä odota ikuisesti.
10. Aloita AP:n toiminta turvallisella pysäytyksellä/idle-polulla. Älä anna sen käsitellä jaettua muistia ennen kuin synkronointisopimus on olemassa.
11. Testaa yksi AP kerrallaan. Jos yksi toimii, lisää useampi ja testaa toistuvasti.
12. CPU:n käynnistyminen, IPI:n vastaanotto ja tehtävien ajastus ovat kolme eri ominaisuutta.

## 15. IPI: viesti CPU:lta toiselle

IPI (Inter-Processor Interrupt) on Local APICin kautta lähetetty prosessorien välinen keskeytys. Nykyisessä koodissa vektori 240 ohjaa rajattuun `smp_handle_ipi()`-polkuun.

1. Varaa vektori, jota ei käytä poikkeus, PIC tai muu APIC-käsittelijä.
2. Lisää assemblyyn tarvittava stub, jos vektori tarvitsee oman sisääntulon; varmista, että se rakentaa saman yhteisen kehyksen.
3. Lisää IDT-portti ennen kuin lähetät IPI:n.
4. Määritä, mitä IPI tarkoittaa ja millä tiedolla käsittelijä toimii. Pelkkä “keskeytys tapahtui” ei kerro pyydettyä työtä.
5. Varmista, että vastaanottaja on online ja että APICin kohde-ID on oikein.
6. Lähetä IPI ja tarkista APICin toimitustila/virhetilat arkkitehtuurin ohjeen mukaan.
7. Käsittelijän pitää olla lyhyt: kuittaa pyyntö, kirjaa tapahtuma tai herätä rajattu toiminto.
8. Älä käytä tavallista jaettua muuttujaa viestinä ilman muistijärjestyksen sopimusta.
9. Testaa vastaanotto yhdellä AP:lla, sitten useammalla. Laske lähetetyt ja vastaanotetut viestit erikseen.
10. Jos IPI ei saavu, tarkista IDT, kohde-ID, APIC enable, vektori, CPU:n IF ja APICin toimitustila.

## 16. Jaettu tila ja synkronointi: älä arvaa kilpailutilannetta

Kaksi CPU:ta voi suorittaa samaa koodia samanaikaisesti. Jos molemmat lukevat ja kirjoittavat samaa muuttujaa ilman määriteltyä synkronointia, tulos voi riippua ajoituksesta. Keskeytysten estäminen yhdellä CPU:lla ei lukitse toista CPU:ta.

1. Luettele jaettu tila: laskurit, allokaattorin bitmap, konsolin kursori, CPU-taulukot ja IPI-pyynnöt.
2. Merkitse jokaiselle tilalle omistaja ja sallitut kontekstit: BSP, AP, keskeytys vai tavallinen kernel-koodi.
3. Valitse mekanismi vasta sopimuksen jälkeen: atominen operaatio, spinlock, per-CPU-tila tai yksittäinen omistaja.
4. Jos käytät spinlockia, määritä mitä tapahtuu, jos sama CPU saa keskeytyksen lukon ollessa pidettynä. Lukon haltija ei saa jäädä keskeytyksen taakse jumiin.
5. Jos lukko suojaa muistiallokointia, älä kutsu lukon sisältä koodia, joka yrittää samaa lukkoa uudelleen.
6. Määritä muistijärjestys: milloin kirjoitetut tiedot ovat vastaanottavan CPU:n nähtävissä.
7. Älä julista jaettua muuttujaa atomiseksi vain siksi, että sen koko on 32 tai 64 bittiä.
8. Testaa toistuvilla rinnakkaisilla päivityksillä ja vertaa odotettua kokonaismäärää toteutuneeseen.
9. Lisää aikakatkaisu tai debug-tunniste lukon odotukseen virtuaalitestissä; ikuinen jumitus on virhe, ei onnistunut synkronointi.
10. Kun synkronointisopimusta ei ole, pidä AP:t rajatussa idle-tilassa. Turvallinen rajoitus on parempi kuin piilevä muistivaurio.

## 17. Miten lisäät uuden kernel-ominaisuuden kokonaan

Käytä tätä menettelyä riippumatta siitä, lisäätkö laitteen, muistipalvelun vai uuden diagnostiikkakomennon.

1. **Vaatimus:** kirjoita yksi mitattava lause. Esimerkki: “Tuntematon shell-komento tulostaa virheen ja palaa kehotteeseen.”
2. **Omistaja:** valitse yksi moduuli, jonka vastuulla toiminto on.
3. **Rajapinta:** määritä funktio, parametrit, paluuarvot ja virhetilat ennen toteutusta.
4. **Riippuvuudet:** piirrä kutsuketju ja tarkista, ettei synny import-sykliä tai rekursiivista laitepolkua.
5. **Turvallisuus:** luettele kaikki osoitteet, pituudet, indeksit ja laitearvot, joita syöte voi vaikuttaa.
6. **Minimitoteutus:** tee ensin vain onnistunut normaali tapaus.
7. **Virhepolut:** lisää nolla, rajat ylittävä arvo, puuttuva laite ja resurssin loppuminen.
8. **Käännös:** aja `bash os/build.sh` ja korjaa ensimmäinen virhe.
9. **Staattinen tarkistus:** tarkista ELF, symbolit ja generoitu assembly, jos muutos koskee ABI:a tai bootia.
10. **Virtuaalitesti:** käynnistä ISO QEMU:ssa ja kerää sarjaportin loki.
11. **Negatiivinen testi:** varmista, että virheellinen pyyntö hylätään hallitusti.
12. **Dokumentaatio:** päivitä lähdekoodikartta, rajoitukset ja kurssin kyseinen oppitunti.
13. **Valmistumiskriteeri:** kirjoita, mikä havainto todistaa toiminnon, ja mikä on vielä tekemättä.

## 18. Virheen jäljitys: etene viimeisestä onnistuneesta vaiheesta

- Jos ISOa ei synny, tarkista buildin ensimmäinen epäonnistunut komento.
- Jos ELF syntyy mutta GRUB ei tunnista sitä, tarkista Multiboot2-otsake ja ELF:n lataussegmentit.
- Jos QEMU pysähtyy ennen VNT:n käynnistystulostetta, lisää merkkejä bootstrapin vaiheiden ympärille.
- Jos sarjaportti toimii mutta VGA ei, tutki konsolin kartoitusta, kursoria ja kirjoitusrajoja erikseen.
- Jos keskeytys kaataa kernelin, tarkista virhekoodillisen vektorin stub, tallennusjärjestys, pinon kohdistus ja `iretq`.
- Jos muistin varaus palauttaa virheellisen osoitteen, tarkista muistikartan rajat, varatut alueet, bitmapin bitin merkitys ja sivukohdistus.
- Jos AP ei käynnisty, tarkista ACPI-tiedot, trampoliinin kopiointi, APIC-ID, SIPI-osoite ja valmiustilan aikakatkaisu.
- Jos ongelma esiintyy vain usealla CPU:lla, epäile ensin jaettua tilaa ja muistijärjestystä, älä lisää viiveitä satunnaisesti.

Jokaisen debuggausmuutoksen pitää vastata yhteen kysymykseen. Poista väliaikainen tuloste tai nimeä se selkeästi, kun vika on rajattu.

## 19. Testausmatriisi: mitä “toimii” tarkoittaa?

| Kerros | Minimitodiste | Mitä se ei todista |
|---|---|---|
| VNT-käännös | kääntäjä tuottaa assemblyn | linkitys tai käynnistys |
| ELF-linkitys | `build/kernel.elf` syntyy | GRUBin hyväksyntä |
| Multiboot2 | `grub-file` hyväksyy otsakkeen | kernelin suorituksen onnistuminen |
| Boot | sarjaportin boot-merkki | muistinhallinnan oikeellisuus |
| Konsoli | teksti näkyy VGA:ssa ja COM1:ssä | syötteen käsittely |
| Shell | tunnettu ja tuntematon komento toimivat | puskurin kaikkien rajojen turvallisuus |
| Keskeytys | testattu vektori saavuttaa dispatcherin | kaikkien vektorien oikeellisuus |
| Muisti | varaus ja vapautus läpäisevät rajatestit | mielivaltaisen fyysisen muistin tuki |
| ACPI | checksum ja MADT rajat hyväksytään | jokaisen firmware-toteutuksen tuki |
| SMP | AP julkaisee online-tilan | scheduler tai turvallinen jaettu muisti |
| IPI | vastaanottaja laskee viestin | yleinen viestijono tai tehtäväajastus |

## 20. Kurssin lopputyö: toteuta, älä vain kuvaile

Valitse yksi rajattu parannus: esimerkiksi shell-komennon turvallinen argumenttiparsinta, muistiallokoinnin rajatesti, ACPI-taulun virhepolun parempi diagnostiikka tai IPI-laskurin todennettava testi.

Palauta seuraavat asiat:

- Muutettu lähdetiedosto ja selkeä vastuu.
- Selitys funktioiden parametreista, paluuarvoista ja virhepoluista.
- Vähintään kolme testitapausta: normaali, raja ja virhe.
- Build-tulos ja virtuaalikoneen havainto; älä väitä testejä tehdyiksi, jos niitä ei suoritettu.
- Dokumentoitu rajoitus, jota toteutus ei vielä ratkaise.
- Linkki seuraavaan oppituntiin kurssin indeksissä.

**Valmistumiskriteeri:** pystyt aloittamaan tyhjästä, nimeämään oikean tiedoston, selittämään jokaisen muutetun rajapinnan, toteuttamaan ominaisuuden, näyttämään miten se epäonnistuu ja todistamaan testillä, että korjaus toimii.
