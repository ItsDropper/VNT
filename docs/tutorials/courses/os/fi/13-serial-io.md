# Sarjaportti debuggausvälineenä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Sarjaportti antaa kernelille yksinkertaisen tekstikanavan. QEMU voi ohjata sen tulosteen terminaaliin, vaikka näytönohjain tai graafinen käyttöliittymä ei toimisi. Siksi sarjaportti on hyödyllinen ensimmäisten käynnistysvaiheiden tutkimiseen.

## Sanasto

UART on sarjaliikenteen laiteosa. Polling tarkistaa laitteen tilaa toistuvasti. Debuggaus tarkoittaa virheen syyn selvittämistä havaintojen avulla.

## Käytännön harjoitus

Etsi os/-hakemistosta sarjaportin alustus- ja kirjoitusfunktiot. Seuraa, missä ensimmäinen viesti tulostetaan. Käynnistä kernel projektin ohjeiden mukaisesti QEMUssa ja vertaa tulostetta lähdekoodin polkuun.

## Yleinen virhe ja turvallisuus

Viimeinen tuloste rajaa ongelmaa mutta ei yksin osoita tarkkaa vikakohtaa. Liiallinen debug-tulostus voi muuttaa ajoitusta, erityisesti keskeytyskoodissa.

## Tarkista ymmärtäminen

Miksi sarjaportti auttaa ennen graafista käyttöliittymää? Mitä viimeinen viesti todistaa ja mitä ei?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Freestanding-ohjelma](12-freestanding-vnt.md) · [Seuraava oppitunti: Sarjaportin komentotulkki →](14-the-serial-shell.md)
