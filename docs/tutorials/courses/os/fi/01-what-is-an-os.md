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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Aloita tästä](00-start-here.md) · [Seuraava oppitunti: Tietokoneen osat kernelin näkökulmasta →](02-computer-basics.md)
