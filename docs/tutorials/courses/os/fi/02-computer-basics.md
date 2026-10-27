# Tietokoneen osat kernelin näkökulmasta

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

CPU suorittaa käskyjä. RAM säilyttää aktiiviset tiedot nopeasti saatavilla, kun taas SSD tai HDD säilyttää tiedot virran katketessa. Kernel käyttää osoitteita muistiin ja ohjausrajapintoja laitteisiin. Keskeytys ilmoittaa tapahtumasta, joka vaatii CPU:n huomiota.

## Sanasto

Rekisteri on CPU:n pieni tallennuspaikka. Osoite kertoo, mistä muistista luetaan tai minne kirjoitetaan. Keskeytys siirtää suorituksen käsittelijään. Polling tarkoittaa, että CPU tarkistaa laitteen tilan itse.

## Käytännön harjoitus

Piirrä näppäimistön tapahtumaketju: näppäin, laiteohjain, kernel, komentotulkki. Merkitse, missä kohtaa laite voi ilmoittaa keskeytyksellä ja missä kohtaa kernel voi tarkistaa tilaa kyselyllä.

## Yleinen virhe ja turvallisuus

Fyysinen ja virtuaalinen osoite eivät aina ole sama asia. Ne voivat vastata toisiaan alkuvaiheen identiteettikartoituksessa, mutta sivutus voi muuntaa osoitteen.

## Tarkista ymmärtäminen

Mikä ero on RAM-muistilla ja tallennustilalla? Mikä ero on keskeytyksellä ja pollingilla?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

CPU ei ymmärrä tiedoston tai ikkunan käsitettä itsestään. Se suorittaa käskyjä ja lukee tai kirjoittaa muistia. Korkeamman tason ominaisuudet rakentuvat näiden mekanismien päälle.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Mitä käyttöjärjestelmä tekee?](01-what-is-an-os.md) · [Seuraava oppitunti: Valmistele kehitysympäristö →](03-setup.md)

## Toteutuslaboratorio: CPU, portit, muisti ja osoitteet

Tämä osio täydentää käsitteitä käytännön toteutusmenettelyllä. Lähdekoodiviitteet ovat projektin nykyisiä polkuja; avaa ne ennen muutosta.

**Tutkittavat tiedostot:** `os/arch/x86_64/boot.S`, `os/console.vnt`, `os/serial.vnt`

**Tavoite:** Seuraa osoitteen kulkua käskystä laitteeseen. Erota fyysinen osoite, virtuaaliosoite ja I/O-portti. Tee pieni luku- tai kirjoituspolun jäljitys ennen koodimuutosta.

### Tee työ järjestyksessä

1. **Rajaa vastuu:** Etsi VGA-osoite 0xB8000 ja sarjaportin portit.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

2. **Etsi nykyinen toteutus:** Kirjaa kunkin toiminnon käyttämä osoiteavaruus.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

3. **Kirjaa sopimus:** Tarkista, että muistiin kirjoittaminen tapahtuu vain kartoitettuun alueeseen.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

4. **Tee pienin muutos:** Älä korvaa portti-I/O:ta tavallisella muistikirjoituksella.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

5. **Tarkista riippuvuudet:** Tee taulukko rekisteristä, osoitteesta, leveydestä ja tarkoituksesta.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

6. **Käännä:** Varmista QEMU:ssa erikseen näyttö- ja sarjaporttituloste.
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
