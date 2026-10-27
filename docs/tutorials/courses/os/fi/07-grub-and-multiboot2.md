# GRUB ja Multiboot2

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

GRUB lataa kernelin ja välittää käynnistystietoja. Multiboot2 määrittää, miten yhteensopiva kernel tunnistetaan ja mitä tietoja käynnistyslataimen pitää välittää. Kernelin otsakkeen pitää noudattaa formaatin rakennetta.

## Sanasto

Otsake on tunnistettava tietorakenne. Magic-arvo on vakio, jonka avulla formaatti tunnistetaan. Boot information on käynnistyslataimen antamaa tietoa, esimerkiksi muistialueista.

## Käytännön harjoitus

Etsi Multiboot2-otsake tiedostosta os/arch/x86_64/boot.S. Katso sen kenttiä ja vertaa niitä os/build.sh-tiedoston GRUB- ja ISO-vaiheisiin. Älä muuta vakioita ilman, että tarkistat formaatin vaatimukset.

## Yleinen virhe ja turvallisuus

Jos GRUB ei löydä kerneliä, VNT-koodin debuggaus ei ole vielä oikea vaihe. Tarkista ensin, että ISO sisältää oikeat tiedostot.

## Tarkista ymmärtäminen

Mitä GRUB tekee? Miksi oikea kernel-tiedosto voi silti jäädä käynnistymättä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Käynnistysformaatti on sopimus käynnistyslataimen ja kernelin välillä. Molempien pitää tulkita otsake samalla tavalla. Yksi väärä kenttä voi estää koko kernelin lataamisen.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Mitä käynnistyksessä tapahtuu?](06-the-boot-sequence.md) · [Seuraava oppitunti: Lue käynnistys-assemblya →](08-reading-boot-assembly.md)
