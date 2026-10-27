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


## Syvempi harjoitus: seuraa koodia vaiheittain

Build-ketju on riippuvuusketju: jos varhainen vaihe epäonnistuu, myöhemmät virheet voivat olla seurausta siitä. Tallenna onnistuneen buildin tulos ja vertaa ensimmäistä poikkeavaa vaihetta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: VNT-kielen perusteet kernel-koodissa](04-vnt-for-kernels.md) · [Seuraava oppitunti: Mitä käynnistyksessä tapahtuu? →](06-the-boot-sequence.md)
