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


## Syvempi harjoitus: seuraa koodia vaiheittain

Ennen muokkausta tallenna lähtötilanne ja määritä hyväksymiskriteeri. Muutoksen jälkeen vertaa toteutunutta käytöstä siihen, mitä kirjoitit etukäteen. Pieni muutos on hyödyllinen vain, jos sen vaikutus voidaan todentaa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Testaa toimintaa, älä vain käännöstä](16-testing.md) · [Seuraava oppitunti: Miten jatkaa kurssin jälkeen →](18-next-steps.md)
