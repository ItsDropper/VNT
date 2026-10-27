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
