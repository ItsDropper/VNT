# Miten lähdekoodista syntyy kernel

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kääntäjä käsittelee VNT-koodia, assembler kääntää assemblyn objektitiedostoksi, linkkeri yhdistää käännetyt osat ja paketointivaihe luo käynnistettävän ISO-kuvan. Jokainen vaihe voi epäonnistua erikseen.

## Sanasto

Objektitiedosto sisältää käännettyä koodia. Linkkeri ratkaisee symboliviittauksia ja sijoittaa osiot. ISO on levytiedosto, jonka QEMU voi esittää käynnistysmediana.

## Käytännön harjoitus

Avaa os/build.sh. Tee taulukko: komento, syöte, tulos. Etsi kohta, jossa kernel linkitetään, ja kohta, jossa ISO kootaan. Aja build vasta riippuvuuksien valmistuttua.

## Yleinen virhe ja turvallisuus

ISO-tiedoston olemassaolo ei todista, että GRUB tunnistaa kernelin tai että CPU pääsee aloituskohtaan. Linkitysvirhe voi olla seurausta aiemmasta käännösvirheestä.

## Tarkista ymmärtäminen

Mikä ero on kääntäjällä ja linkkerillä? Miksi buildin jokainen vaihe pitää ymmärtää erikseen?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: VNT-kielen perusteet kernel-koodissa](04-vnt-for-kernels.md) · [Seuraava oppitunti: Mitä käynnistyksessä tapahtuu? →](06-the-boot-sequence.md)
