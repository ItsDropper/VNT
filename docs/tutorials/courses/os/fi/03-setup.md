# Valmistele kehitysympäristö

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kernelin build voi tarvita VNT-kääntäjän, assemblerin, linkkerin, GRUB-työkaluja ja QEMUn. Tarkka lista löytyy projektin ohjeista ja build-skriptistä. Selvitä puuttuvat riippuvuudet ennen kuin alat muuttaa lähdekoodia.

## Sanasto

Build on käännös- ja paketointiketju. Riippuvuus on työkalu tai kirjasto, jota ketju tarvitsee. Virtuaalikone jäljittelee laitteistoa ja eristää kokeet oikeasta tietokoneesta.

## Käytännön harjoitus

Lue projektin README ja os/build.sh. Kun työkalut ovat asennettuina, aja projektin juuresta bash os/build.sh. Kirjaa komento, ensimmäinen virhe ja sen ympärillä oleva teksti. Älä korjaa ongelmaa arvaamalla.

## Yleinen virhe ja turvallisuus

Älä suorita build-skriptiä järjestelmänvalvojana ilman syytä. Älä käytä levykomentoja, jotka kirjoittavat oikealle levylle tai USB-tikulle.

## Tarkista ymmärtäminen

Mistä hakemistosta build suoritetaan? Miten erotat puuttuvan riippuvuuden varsinaisesta käännösvirheestä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Kun ympäristö ei toimi, erota työkalun asennusongelma projektin virheestä. Tarkista komento, nykyinen hakemisto ja ensimmäinen virhe. Älä suorita komentoa, jonka vaikutusta et ymmärrä.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---


## Koodiesimerkki: työkalujen tarkistus ja build

```bash
command -v gcc
command -v ld
command -v qemu-system-x86_64
bash os/build.sh
```

command -v kertoo, löytyykö työkalu PATH-ympäristömuuttujasta. Build-skripti itse kääntää VNT-kääntäjän, emittoi freestanding-assemblyn, assembloi bootstrapin ja linkittää ELF:n. ISO-työkalut ovat erillinen, valinnainen vaihe.

**Tee itse:** Suorita tarkistukset ennen buildia. Jos build epäonnistuu, kirjaa ensimmäinen epäonnistunut komento; älä päättele gcc:n tai GRUBin puuttumisesta, että VNT-lähdekoodi on väärin.


## Toteutuslaboratorio: Kehitysympäristön valmistelu

Tämä osio täydentää käsitteitä käytännön toteutusmenettelyllä. Lähdekoodiviitteet ovat projektin nykyisiä polkuja; avaa ne ennen muutosta.

**Tutkittavat tiedostot:** `os/build.sh`, `.github/workflows/vntos-boot.yml`

**Tavoite:** Tee ympäristöstä toistettava. Tarkista työkalujen olemassaolo ennen kuin tulkitset käännösvirhettä. Varmista, että virtuaalitesti ei koske oikeita levyjä.

### Tee työ järjestyksessä

1. **Rajaa vastuu:** Avaa build-skripti ja luettele gcc, ld, VNT-kääntäjä ja valinnaiset GRUB-työkalut.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

2. **Etsi nykyinen toteutus:** Varmista projektin juurihakemisto ja Git-työpuun tila.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

3. **Kirjaa sopimus:** Aja build-komento ja tallenna poistumiskoodi sekä ensimmäinen virhe.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

4. **Tee pienin muutos:** Jos ISO-työkaluja puuttuu, erota ELF:n kääntö ISO:n luomisesta.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

5. **Tarkista riippuvuudet:** Käytä QEMU:n väliaikaista levyä ja sarjaportin ulostuloa.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

6. **Käännä:** Älä tulkitse puuttuvaa valinnaista työkalua kernelin lähdekoodivirheeksi.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

### Selvitä jokainen rajapinta

- Mikä funktio aloittaa toiminnon, ja kuka sitä kutsuu?
- Mitä arvoja funktio lukee, mitä se muuttaa ja mitä se palauttaa?
- Mitkä muistiosoitteet, indeksit tai laitearvot tulevat ulkopuolelta?
- Mikä tarkistus tapahtuu ennen kuin arvoa käytetään?
- Mikä on selkeä virhetilan paluuarvo tai diagnostiikka?
- Mikä osa on VNT:tä ja mikä osa on CPU:n vaatimaa assemblyä?
- Mitä nykyinen toteutus nimenomaisesti ei tue?

### Hyväksymistesti

1. Kirjoita ennen käännöstä odotettu tuloste tai tilamuutos.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Käynnistä syntynyt kernel vain QEMU:ssa.
4. Tarkista sarjaportin tai konsolin tuloste ja vertaa sitä odotukseen.
5. Toista testi rajatapauksella ja virheellisellä syötteellä.
6. Jos et voi suorittaa testiä, ilmoita se avoimesti äläkä merkitse sitä läpäistyksi.

### Yleiset toteutusvirheet

- Uuden funktion lisääminen ilman kutsuja tai importtia: koodi ei ole osa käynnistyspolkua.
- Oletus, että käännös tarkoittaa käynnistymistä.
- Rajojen tarkistaminen vasta sen jälkeen, kun muistia on jo luettu tai kirjoitettu.
- Liian suuri muutos, jonka jälkeen ei tiedetä, mikä rikkoi toiminnan.
- Diagnostiikan lisääminen keskeytyskäsittelijään niin, että alkuperäinen virhe muuttuu uudeksi jumiutumiseksi.

### Muutosraportti

Kirjoita muistiin: (1) tiedostot, joita muutit, (2) uuden toiminnon sopimus, (3) tarkka build-tulos, (4) QEMU-havainto, (5) testatut virhepolut ja (6) rajoitukset, joita et ratkaissut. Tämä raportti on osa toteutusta, ei ylimääräistä paperityötä.

---

[← Edellinen oppitunti](./02-computer-basics.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./04-vnt-for-kernels.md)
