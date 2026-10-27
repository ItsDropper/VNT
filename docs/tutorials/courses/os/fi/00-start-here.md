# Aloita tästä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kurssi opettaa lukemaan, rakentamaan, käynnistämään ja muuttamaan VNTOS-käyttöjärjestelmän ydintä. Aloitamme tietokoneen peruskäsitteistä ja etenemme käynnistykseen, konsoliin, keskeytyksiin, muistiin ja useaan CPU-ytimeen.

## Sanasto

Kernel tarkoittaa käyttöjärjestelmän ydintä. Lähdekoodi on ihmisen kirjoittamaa ohjelmatekstiä. Kääntäjä muuttaa lähdekoodin koneen suoritettavaksi koodiksi. QEMU jäljittelee tietokonetta ohjelmallisesti.

## Käytännön harjoitus

Avaa os/-hakemisto GitHubissa. Etsi os/kernel.vnt ja os/build.sh. Lue tiedostojen alut ja etsi, mistä kernelin toiminta näyttää alkavan. Älä vielä muuta koodia; harjoittele ensin koodin seuraamista.

## Yleinen virhe ja turvallisuus

Älä asenna keskeneräistä ydintä oikealle tietokoneelle tai kirjoita levykuvaa USB-tikulle. Käytä QEMUta. VNTOS on oppimisprojekti, ei valmis työpöytäkäyttöjärjestelmä.

## Tarkista ymmärtäminen

Mikä on kernel? Mitä kääntäjä tekee? Miksi käytämme QEMUta?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Tavoitteena ei ole opetella kaikkia tiedostonimiä ulkoa. Opettele kysymään: kuka kutsuu tätä, mitä tämä muuttaa, mitä oletetaan valmiiksi ja miten toiminnan voi todentaa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---


## Koodiesimerkki: rakenna ja käynnistä turvallisesti

```bash
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

Ensimmäinen komento käyttää projektin omaa build-skriptiä. Skripti luo ensin build/kernel.elf:n ja tekee ISO:n vain, jos GRUB- ja xorriso-työkalut löytyvät. Käynnistä vain virtuaalikoneessa. Jos ISO:ta ei syntynyt, lue buildin lopputuloste äläkä suorita QEMU-komentoa sokkona.

**Tee itse:** Harjoitus: kirjoita ylös buildin luoma tiedosto ja se, mitä QEMU:n sarjaportissa näkyy. Käännös onnistui vasta, kun komento päättyi onnistuneesti; käynnistys onnistui vasta, kun kernelin oma käynnistysviesti näkyy.


## Toteutuslaboratorio: Aloita tästä

Tämä osio täydentää käsitteitä käytännön toteutusmenettelyllä. Lähdekoodiviitteet ovat projektin nykyisiä polkuja; avaa ne ennen muutosta.

**Tutkittavat tiedostot:** `os/README.md`, `os/build.sh`, `os/kernel.vnt`

**Tavoite:** Tee ensin rakennusketjun kartta. Kirjoita jokaisesta vaiheesta syöte, tuloste ja epäonnistumisen merkki. Älä muuta bootia ja VNT-koodia samalla kertaa.

### Tee työ järjestyksessä

1. **Rajaa vastuu:** Avaa build-skripti ja nimeä kaikki työkalut niiden todellisessa suoritusjärjestyksessä.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

2. **Etsi nykyinen toteutus:** Etsi kernel_main ja seuraa importteja moduuleihin.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

3. **Kirjaa sopimus:** Merkitse missä syntyy assembly, missä objektitiedostot ja missä ELF.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

4. **Tee pienin muutos:** Aja build-komento ja tallenna koko ensimmäinen virhe.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

5. **Tarkista riippuvuudet:** Käynnistä ISO QEMU:ssa ja vertaa sarjaportin ensimmäistä riviä ennusteeseen.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

6. **Käännä:** Tee yksi dokumentoitu muutos, joka lisää uuden käynnistystunnisteen.
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

[Seuraava oppitunti: what is an os →](./01-what-is-an-os.md) · [Sisällysluettelo](./README.md)
