# Freestanding-ohjelma

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Tavallinen sovellus voi pyytää käyttöjärjestelmältä tiedostoja, muistia ja verkkoyhteyksiä. Kernelin alkuvaiheessa näitä palveluja ei ole valmiina. Freestanding-koodi ei voi olettaa tavallisen standardikirjaston tai käyttöjärjestelmän runtime-ympäristön olevan käytössä.

## Sanasto

Hosted-ohjelma toimii käyttöjärjestelmän tarjoamassa ympäristössä. Runtime on suorituksen aikana tarvittavaa tukikoodia. ABI määrittää esimerkiksi argumenttien ja paluuarvojen välityksen.

## Käytännön harjoitus

Avaa src/compiler/freestanding_x86.c ja tutki freestanding-käsittelyä. Vertaa havaintoa os/kernel.vnt-tiedostoon. Kun löydät funktion, selvitä, käyttääkö se käyttöjärjestelmän palvelua tai ajonaikaista kirjastoa.

## Yleinen virhe ja turvallisuus

VNT-kielen syntaksin tuki ei tarkoita, että kaikki kirjastot toimivat kernelissä. Muistiallokointi tai tiedostokutsut vaativat erillisen toteutuksen, jos niitä tarvitaan.

## Tarkista ymmärtäminen

Miksi tavallisessa ohjelmassa toimiva funktio voi epäonnistua freestanding-kernelissä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Kun lisäät uuden funktion kernelin puolelle, tarkista, tarvitseeko se runtime-tukea. Tavallisessa ohjelmassa tuki voi tulla kirjastosta; kernelissä riippuvuus pitää tunnistaa ja ratkaista erikseen.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: ELF ja linkkeriskripti](11-elf-and-linker.md) · [Seuraava oppitunti: Sarjaportti debuggausvälineenä →](13-serial-io.md)
