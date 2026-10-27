# Rakenna ja testaa keskeytyskernel

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Keskeytyksiä käyttävä kernel voi kääntyä mutta kaatua, kun CPU käyttää IDT:tä. Siksi build, käynnistys ja tapahtuman todentaminen ovat eri tarkistuksia. Testin pitää näyttää, että keskeytys saapui ja käsittelijä teki odotetun asian.

## Sanasto

Odotettu tulos kertoo, mistä onnistuminen tunnistetaan. Loki tallentaa tapahtumia. CI-ajo suorittaa automaattisia tarkistuksia palvelimella.

## Käytännön harjoitus

Aja projektin juuresta bash os/build.sh, käynnistä ISO QEMUssa ja seuraa sarjaporttia. Etsi konkreettinen tulos, kuten muuttunut laskuri. Tarkista GitHub Actionsin ajo ja lue epäonnistuneen vaiheen loki.

## Yleinen virhe ja turvallisuus

Kernelin käynnistyminen ei todista, että keskeytys toimitettiin. Tarkista erikseen ohjaimen alustus, IDT-merkintä, keskeytyksen salliminen ja kuittaus.

## Tarkista ymmärtäminen

Mikä havainto todistaisi, että käsittelijä todella suoritettiin?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: PIT, PIC ja ajastimen tikit](23-pit-pic-and-timer-ticks.md) · [Seuraava oppitunti: Fyysinen muisti ja sivukehykset →](26-physical-memory-and-page-frames.md)
