# Alkuvaiheen sivutaulut

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Sivutus muuntaa virtuaaliosoitteita fyysisiksi osoitteiksi ja voi määrittää muistialueiden käyttöoikeuksia. VNTOSin alkuvaiheen identiteettikartoituksessa tietyllä alueella virtuaaliosoite ja fyysinen osoite ovat samat.

## Sanasto

Virtuaaliosoite on koodin käyttämä osoite. Fyysinen osoite viittaa RAM-muistiin. Sivutaulu kuvaa osoitteiden muunnoksia. Identiteettikartoitus tarkoittaa, että osoite A kartoitetaan fyysiseen osoitteeseen A.

## Käytännön harjoitus

Tutki boot.S-tiedoston sivutauluja. Selvitä niiden sijainti, kattama muistialue ja kohta, jossa ylimmän taulun osoite asetetaan CR3-rekisteriin. Piirrä osoitteen muunnos nuolilla.

## Yleinen virhe ja turvallisuus

Identiteettikartoitus ei ole täydellinen virtuaalimuistijärjestelmä eikä takaa prosessieristystä. Myös sivujen rajat ja käyttöoikeudet ovat tärkeitä.

## Tarkista ymmärtäminen

Mitä hyötyä virtuaaliosoitteista on? Miksi identiteettikartoitus auttaa käynnistyksessä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: x86-64 long mode](09-long-mode.md) · [Seuraava oppitunti: ELF ja linkkeriskripti →](11-elf-and-linker.md)
