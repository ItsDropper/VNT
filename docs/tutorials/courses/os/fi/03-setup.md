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

[← Sisällysluettelo](./README.md) · [← Edellinen: Tietokoneen osat kernelin näkökulmasta](02-computer-basics.md) · [Seuraava oppitunti: VNT-kielen perusteet kernel-koodissa →](04-vnt-for-kernels.md)
