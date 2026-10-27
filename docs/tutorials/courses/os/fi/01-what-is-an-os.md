# Mitä käyttöjärjestelmä tekee?

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Käyttöjärjestelmä jakaa CPU-aikaa, muistia ja laitteita ohjelmille. Kernel tarjoaa peruspalvelut ja hallitsee laitteistoa. Muu käyttöjärjestelmä voi sisältää ajureita, palveluita, komentotulkin ja graafisen käyttöliittymän.

## Sanasto

Resurssi on esimerkiksi RAM tai CPU-aika. Ajuri ohjaa laitetta. Komentotulkki lukee käyttäjän komentoja. Kernel toimii etuoikeutetussa suoritustilassa.

## Käytännön harjoitus

Piirrä kolme kerrosta: laitteisto, kernel ja ohjelmat. Piirrä nuoli, joka kuvaa ohjelman pyyntöä kernelille ja vastausta takaisin. Vertaa mallia VNTOSin shelliin, joka on tällä hetkellä kernelin sisällä.

## Yleinen virhe ja turvallisuus

VNTOSissa on käynnistys, konsoli, keskeytysten käsittelyä ja kokeilu usealla CPU-ytimellä. Siinä ei vielä ole yleistä prosessieristystä, käyttäjätilan sovelluksia, tiedostojärjestelmää tai valmista tehtäväajastinta.

## Tarkista ymmärtäminen

Nimeä kolme resurssia, joita käyttöjärjestelmä hallitsee. Miksi ajuri ja kernel eivät ole sama asia?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Ajattele käyttöjärjestelmää resurssien hallitsijana. Jos kaksi ohjelmaa haluaa käyttää samaa laitetta, tarvitaan yhteiset säännöt. Kernel toteuttaa osan näistä säännöistä ja hallitsee laitteistoa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Aloita tästä](00-start-here.md) · [Seuraava oppitunti: Tietokoneen osat kernelin näkökulmasta →](02-computer-basics.md)

## Toteutuslaboratorio: Käyttöjärjestelmän vastuut

Tämä osio täydentää käsitteitä käytännön toteutusmenettelyllä. Lähdekoodiviitteet ovat projektin nykyisiä polkuja; avaa ne ennen muutosta.

**Tutkittavat tiedostot:** `os/kernel.vnt`, `os/kernel/core.vnt`, `os/serial.vnt`

**Tavoite:** Seuraa yhtä vastuuta koko kutsuketjun läpi. Erota politiikka laitteiston yksityiskohdista. Tee pieni ominaisuus, jonka omistaja on yksiselitteinen.

### Tee työ järjestyksessä

1. **Rajaa vastuu:** Valitse yksi tehtävä, esimerkiksi käynnistysviestin kirjoittaminen.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

2. **Etsi nykyinen toteutus:** Etsi kutsuja, jotka päättävät milloin tehtävä suoritetaan.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

3. **Kirjaa sopimus:** Etsi erillinen moduuli, joka toteuttaa varsinaisen toiminnon.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

4. **Tee pienin muutos:** Kirjaa mitä kernel saa olettaa ja mitä sen pitää tarkistaa.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

5. **Tarkista riippuvuudet:** Lisää virhepolku, jossa toiminnon riippuvuus puuttuu.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

6. **Käännä:** Testaa että virhe näkyy diagnostiikassa eikä aiheuta hiljaista jatkamista.
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
