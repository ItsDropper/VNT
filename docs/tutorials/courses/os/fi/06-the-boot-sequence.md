# Mitä käynnistyksessä tapahtuu?

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Firmware käynnistää tietokoneen, käynnistyslatain etsii kernelin ja lataa sen muistiin, ja käynnistyskoodi valmistelee CPU:n ennen VNT-koodia. Jokainen vaihe olettaa, että edellinen on jättänyt CPU:n ja muistin oikeaan tilaan.

## Sanasto

Entry point on suorituksen aloituskohta. Pino säilyttää muun muassa funktiokutsujen paluuosoitteita. Firmware on emolevyn käynnistyskoodi. GRUB on käynnistyslatain.

## Käytännön harjoitus

Avaa os/arch/x86_64/boot.S. Etsi Multiboot2-otsake, pinon asetus, sivutaulujen valmistelu ja siirtyminen 64-bittiseen tilaan. Seuraa tiedostoa järjestyksessä ja piirrä vaiheet nuolilla.

## Yleinen virhe ja turvallisuus

Jos virhe tapahtuu ennen konsolin alustusta, sarjaportissa ei välttämättä näy mitään. Älä päättele hiljaisuudesta, ettei CPU olisi suorittanut käskyjä.

## Tarkista ymmärtäminen

Kuvaile ketju firmwaresta VNT-funktioon. Miksi kernel tarvitsee alkuvaiheen assemblya?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Käynnistys on sarja järjestettyjä siirtymiä. Jokaisessa vaiheessa on oletuksia rekistereistä, muistista ja pinosta. Jos oletus rikkoutuu, seuraava vaihe voi kaatua ennen virheilmoitusta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Miten lähdekoodista syntyy kernel](05-build-pipeline.md) · [Seuraava oppitunti: GRUB ja Multiboot2 →](07-grub-and-multiboot2.md)
