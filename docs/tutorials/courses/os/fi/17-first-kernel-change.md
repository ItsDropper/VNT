# Tee ensimmäinen pieni muutos

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Ensimmäisen muutoksen pitää olla pieni ja helposti tarkistettava. Selkeämpi shellin virheilmoitus on parempi harjoitus kuin keskeytysten tai muistinhallinnan uudelleenkirjoitus. Ennen muokkausta selvitä nykyinen toiminta ja miten uuden käytöksen voi todistaa.

## Sanasto

Diff näyttää muutokset. Rajattu muutos muuttaa yhtä asiaa. Palautettava tila tarkoittaa, että voit palata tunnettuun versioon, jos kokeilu epäonnistuu.

## Käytännön harjoitus

Lue funktio, kirjoita nykyinen toiminta ylös, tee yksi muutos, tarkista diff, rakenna kernel, käynnistä QEMU ja suorita siihen liittyvä testi. Jos tulos muuttuu odottamattomasti, selvitä syy ennen seuraavaa muutosta.

## Yleinen virhe ja turvallisuus

Älä tee monta riippumatonta muutosta kerralla. Muuten et tiedä, mikä niistä aiheutti virheen. Älä myöskään väitä testanneesi toimintaa, jos tarkistit vain lähdekoodin.

## Tarkista ymmärtäminen

Suunnittele pieni muutos: tiedosto, uusi käytös, testi ja palautussuunnitelma.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Testaa toimintaa, älä vain käännöstä](16-testing.md) · [Seuraava oppitunti: Miten jatkaa kurssin jälkeen →](18-next-steps.md)
