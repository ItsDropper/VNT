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


## Syvempi harjoitus: seuraa koodia vaiheittain

Arkkitehtuurin rekistereitä ei pidä muuttaa arvauksella. Tarkista jokaisen bitin tarkoitus ja vaadittu järjestys luotettavasta x86-64-dokumentaatiosta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Lue käynnistys-assemblya](08-reading-boot-assembly.md) · [Seuraava oppitunti: Alkuvaiheen sivutaulut →](10-initial-paging.md)
