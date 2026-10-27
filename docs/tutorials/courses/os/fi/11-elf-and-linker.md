# ELF ja linkkeriskripti

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

ELF on tiedostomuoto, joka kuvaa käännetyn ohjelman rakenteen. Linkkeri yhdistää objektitiedostot ja sijoittaa niiden osiot. Linkkeriskripti kertoo, mihin muistiosoitteisiin kernelin osat sijoitetaan ja mistä suoritus alkaa.

## Sanasto

.text sisältää yleensä koodia, .data alustettuja muuttujia ja .bss nollattavaa tai alustamatonta dataa. Symboli on funktion tai muuttujan tunniste. Linkkeriskripti määrittää muistiasettelua.

## Käytännön harjoitus

Etsi os/-hakemistosta linkkeriskripti ja avaa se. Vertaa sitä os/build.sh-tiedostoon. Selvitä, miten skripti välitetään linkkerille ja mikä kernel-tiedosto syntyy. Jos työkalut ovat käytössä, tutki ELF-tiedoston osioita readelfillä.

## Yleinen virhe ja turvallisuus

Puuttuva symboli on usein merkki siitä, että tiedosto ei ole mukana buildissa tai funktion nimi/rajapinta ei täsmää. Älä vain poista linkkerin virheilmoitusta.

## Tarkista ymmärtäminen

Mikä ero on objektitiedostolla ja ELF-kernelillä? Mitä linkkeriskripti määrittää?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Linkkeri ratkaisee symbolien sijainnit ja sijoittaa osiot. Jos symboli puuttuu, selvitä missä sen pitäisi olla määritelty ja onko tiedosto mukana buildissa. Älä vain poista virheilmoitusta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Alkuvaiheen sivutaulut](10-initial-paging.md) · [Seuraava oppitunti: Freestanding-ohjelma →](12-freestanding-vnt.md)

## Koodiesimerkki: ELF:n muistiasettelu

```ld
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(boot32_entry)

PHDRS
{
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0x00100000;
    __kernel_start = .;
    .multiboot ALIGN(8) : { KEEP(*(.multiboot)) } :text
    .boot32 ALIGN(16) : { KEEP(*(.boot32)) } :text
    .text ALIGN(0x1000) : { *(.text .text.*) } :text
}
```

Tämä on os/linker.ld:n ote. ENTRY määrittää aloitussymbolin, PT_LOAD-segmentit kertovat lataajalle mitä muistialueita ladataan ja KEEP säilyttää Multiboot2-otsakkeen. `.` on linkkerin nykyinen osoite; ALIGN siirtää seuraavan osion oikeaan kohdistukseen.

**Tee itse:** Käännä kernel ja tarkista `readelf -h -l -s build/kernel.elf`. Etsi entry-symboli, PT_LOAD-segmentit ja __kernel_start. Jos muokkaat linker-skriptiä, vertaa ennen/jälkeen -tulosta sekä GRUB-tarkistusta.


## Toteutuslaboratorio: ELF ja linkkeriskripti

Tässä harjoituksessa toteutat tai jäljität toimivan osan, et vain kuvaile käsitettä.

**Tutkittavat tiedostot:** `os/linker.ld`, `os/build.sh`

**Tavoite:** Tee ELF:n rakenteesta tarkistettava. Entry-symbolin, osioiden ja lataussegmenttien pitää vastata bootstrapin odotuksia. Tarkista symbolit työkalulla, älä arvaa niitä.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Selvitä ENTRY ja PHDRS sekä jokaisen PT_LOAD-segmentin oikeudet.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

2. **Etsi nykyinen toteutus:** Jäljitä .multiboot, .boot32, .text, .rodata, .data ja .bss -osioiden järjestys.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

3. **Kirjaa sopimus:** Tarkista linker-symbolien käyttö VNT:n symbol_address-kutsuissa.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

4. **Tee pienin muutos:** Käännä ja tarkista readelf -h -l -s, jos työkalu on asennettu.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

5. **Tarkista riippuvuudet:** Poista tai muuta linkkerisääntöä vain erillisessä kokeessa ja vertaa ELF:iä.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

6. **Käännä:** Varmista grub-file-tarkistus sekä QEMU-käynnistys.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

### Sopimus ja rajat

- Määritä syötteet, paluuarvot ja virhetilanteet ennen toteutusta.
- Tarkista osoitteet, pituudet, indeksit ja tilan alustaminen ennen käyttöä.
- Erota CPU:n vaatima mekanismi VNT:n päätöksenteosta.
- Älä väitä nykyisen rajoitetun toteutuksen tukevan ominaisuuksia, joita koodi ei sisällä.
- Tarkista, ettei uusi toiminto vaadi alustamatonta moduulia tai linkittämätöntä symbolia.
- Pidä diagnostiikka rajattuna ja vältä odottamista keskeytyskontekstissa.

### Testaa jokainen polku

1. Kirjoita ennuste ennen käännöstä.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut komento.
4. Käynnistä kernel QEMU:ssa ja tallenna sarjaportin tuloste.
5. Testaa normaali tapaus, rajatapaus ja virheellinen syöte.
6. Tarkista ettei virheellinen pyyntö muuta muistia tai laitteiston tilaa.
7. Jos testaus ei ole käytettävissä, merkitse tulos vahvistamattomaksi.

### Tavalliset virheet

- Osoitteen tai pituuden tarkistus tehdään vasta luku-/kirjoitusoperaation jälkeen.
- Uusi funktio ei ole mukana import-ketjussa.
- Paluuarvon merkitys ei ole sama kutsujalle ja toteuttajalle.
- Diagnostiikka peittää alkuperäisen vian.
- Käännöksen onnistumista pidetään toimintatestinä.

### Kirjaa toteutus

Dokumentoi muutetut tiedostot, funktiosopimus, build-tulos, QEMU-havainto, negatiiviset testit ja rajoitukset. Näin toinen kehittäjä pystyy toistamaan tarkistuksen.
