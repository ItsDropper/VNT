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


## Syvempi harjoitus: seuraa koodia vaiheittain

Sarjaportin tuloste on havainto, ei täydellinen totuus. Tulosteen puuttuminen voi tarkoittaa, ettei koodi päässyt kohtaan, konsolia ei alustettu tai laiteasetukset eivät täsmää. Testaa hypoteeseja erikseen.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Freestanding-ohjelma](12-freestanding-vnt.md) · [Seuraava oppitunti: Sarjaportin komentotulkki →](14-the-serial-shell.md)
