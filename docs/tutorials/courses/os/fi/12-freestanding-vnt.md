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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: ELF ja linkkeriskripti](11-elf-and-linker.md) · [Seuraava oppitunti: Sarjaportti debuggausvälineenä →](13-serial-io.md)
