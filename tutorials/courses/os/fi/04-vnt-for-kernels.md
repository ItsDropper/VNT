# VNT-kielen perusteet kernel-koodissa

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

VNTOSin tavallinen logiikka pyritään pitämään VNT-kielessä. Assemblya tarvitaan CPU:n tarkkoihin rajapintoihin, kuten käynnistykseen ja keskeytyksen sisääntuloon. Muuttujat säilyttävät arvoja, funktiot nimeävät toimintoja, ehdot valitsevat haaroja ja silmukat toistavat toimintoja.

## Sanasto

Osoitin viittaa muistiosoitteeseen. Freestanding tarkoittaa, ettei ohjelma voi olettaa tavallisen käyttöjärjestelmän palveluita. Funktiokutsu siirtää suorituksen toiseen funktioon ja palaa yleensä takaisin.

## Käytännön harjoitus

Avaa os/kernel.vnt ja etsi alustuksen pääpolku. Seuraa funktiokutsuja. Avaa sitten os/kernel/shell.vnt ja valitse yksi komentoa käsittelevä funktio. Kirjoita ylös, mitä se saa syötteeksi ja mitä se muuttaa.

## Yleinen virhe ja turvallisuus

VNT-kääntäjän tavallisille ohjelmille tarjoamat ominaisuudet eivät automaattisesti toimi freestanding-kernelissä. Virheellinen osoitin voi kaataa koko järjestelmän.

## Tarkista ymmärtäminen

Valitse yksi funktio ja selitä sen syöte, toiminta, kutsumat funktiot ja mahdollinen virhetilanne.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Seuraa tietoa funktiosta toiseen: argumentit ovat sisään tulevaa tietoa ja paluuarvo on tulos. Tarkista myös sivuvaikutukset eli muistipaikat tai laitteet, joita funktio muuttaa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Valmistele kehitysympäristö](03-setup.md) · [Seuraava oppitunti: Miten lähdekoodista syntyy kernel →](05-build-pipeline.md)
