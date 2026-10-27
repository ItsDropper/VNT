# x86-64 long mode

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

64-bittiseksi käännetty kernel ei voi hypätä suoraan long modeen. Käynnistysvaiheen pitää asettaa CPU:n tilarekisterit, valmistella sivutaulut ja vaihtaa suoritusmuoto arkkitehtuurin vaatimalla tavalla.

## Sanasto

CR0, CR3 ja CR4 ovat CPU:n ohjausrekistereitä. EFER sisältää muun muassa long modeen liittyviä asetuksia. CPU-tila määrittää, miten käskyt ja osoitteet tulkitaan.

## Käytännön harjoitus

Etsi boot.S-tiedostosta kohta, jossa CR3 asetetaan ja CPU:n tila vaihtuu. Kirjoita, mitä tehdään ennen siirtymää ja mitä sen jälkeen. Tarkista käskyjen merkitys arkkitehtuuridokumentaatiosta ennen muutoksia.

## Yleinen virhe ja turvallisuus

Long mode ei tarkoita, että muistinhallinta olisi valmis tai että laiteajurit toimisivat. Se on CPU:n suoritusmuoto, ei kokonainen käyttöjärjestelmän ominaisuus.

## Tarkista ymmärtäminen

Miksi 64-bittinen käännös ei riitä? Miten long mode eroaa muistinhallinnasta?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Lue käynnistys-assemblya](08-reading-boot-assembly.md) · [Seuraava oppitunti: Alkuvaiheen sivutaulut →](10-initial-paging.md)
