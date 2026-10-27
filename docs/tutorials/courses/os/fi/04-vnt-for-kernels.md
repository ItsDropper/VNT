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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Valmistele kehitysympäristö](03-setup.md) · [Seuraava oppitunti: Miten lähdekoodista syntyy kernel →](05-build-pipeline.md)
