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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Tietokoneen osat kernelin näkökulmasta](02-computer-basics.md) · [Seuraava oppitunti: VNT-kielen perusteet kernel-koodissa →](04-vnt-for-kernels.md)
